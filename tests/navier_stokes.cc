/* *********************************************************************** */
/*                                                                         */
/* Copyright (C) 2002-2005 Yves Renard, Michel Fourni�.                    */
/*                                                                         */
/* This program is free software; you can redistribute it and/or modify    */
/* it under the terms of the GNU Lesser General Public License as          */
/* published by the Free Software Foundation; version 2.1 of the License.  */
/*                                                                         */
/* This program is distributed in the hope that it will be useful,         */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/* GNU Lesser General Public License for more details.                     */
/*                                                                         */
/* You should have received a copy of the GNU Lesser General Public        */
/* License along with this program; if not, write to the Free Software     */
/* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,  */
/* USA.                                                                    */
/*                                                                         */
/* *********************************************************************** */

/**
 * Navier_Stokes problem.
 *
 * This program is used to check that getfem++ is working. This is also 
 * a good example of use of Getfem++.
*/

#include <getfem_assembling.h> /* import assembly methods (and norms comp.) */
#include <getfem_export.h>   /* export functions (save solution in a file)  */
#include <getfem_regular_meshes.h>
#include <getfem_modeling.h>
#include <gmm.h>

/* try to enable the SIGFPE if something evaluates to a Not-a-number
 * of infinity during computations
 */
#ifdef GETFEM_HAVE_FEENABLEEXCEPT
#  include <fenv.h>
#endif

/* some Getfem++ types that we will be using */
using bgeot::base_small_vector; /* special class for small (dim<16) vectors */
using bgeot::base_node;  /* geometrical nodes(derived from base_small_vector)*/
using bgeot::scalar_type; /* = double */
using bgeot::size_type;   /* = unsigned long */
using bgeot::base_matrix; /* small dense matrix. */

/* definition of some matrix/vector types. These ones are built
 * using the predefined types in Gmm++
 */
typedef getfem::modeling_standard_sparse_vector sparse_vector;
typedef getfem::modeling_standard_sparse_matrix sparse_matrix;
typedef getfem::modeling_standard_plain_vector  plain_vector;

/*
 * structure for the navier_stokes problem
 */
struct navier_stokes_problem {

  enum { DIRICHLET_BOUNDARY_NUM = 0, NEUMANN_BOUNDARY_NUM = 1};
  getfem::getfem_mesh mesh;  /* the mesh */
  getfem::mesh_im  mim;      /* integration methods.                         */
  getfem::mesh_fem mf_u;     /* main mesh_fem, for the velocity              */
  getfem::mesh_fem mf_p;     /* mesh_fem for the pressure                    */
  getfem::mesh_fem mf_rhs;   /* mesh_fem for the right hand side (f(x),..)   */
  getfem::mesh_fem mf_coef;  /* mesh_fem used to represent pde coefficients  */
  scalar_type nu, dt, T, dtexport;

  scalar_type residu;        /* max residu for the iterative solvers         */
  int noisy, dxexport;

  std::string datafilename;
  ftool::md_param PARAM;

  bool solve(void);
  void init(void);
  navier_stokes_problem(void) : mim(mesh), mf_u(mesh), mf_p(mesh),
				mf_rhs(mesh), mf_coef(mesh) {}
};

/* Read parameters from the .param file, build the mesh, set finite element
 * and integration methods and selects the boundaries.
 */
void navier_stokes_problem::init(void) {
  const char *MESH_TYPE = PARAM.string_value("MESH_TYPE","Mesh type ");
  const char *FEM_TYPE  = PARAM.string_value("FEM_TYPE","FEM name");
  const char *FEM_TYPE_P  = PARAM.string_value("FEM_TYPE_P","FEM name P");
  const char *INTEGRATION = PARAM.string_value("INTEGRATION",
					       "Name of integration method");
  cout << "MESH_TYPE=" << MESH_TYPE << "\n";
  cout << "FEM_TYPE="  << FEM_TYPE << "\n";
  cout << "INTEGRATION=" << INTEGRATION << "\n";

  /* First step : build the mesh */
  bgeot::pgeometric_trans pgt = 
    bgeot::geometric_trans_descriptor(MESH_TYPE);
  size_type N = pgt->dim();
  std::vector<size_type> nsubdiv(N);
  std::fill(nsubdiv.begin(),nsubdiv.end(),
	    PARAM.int_value("NX", "Nomber of space steps "));
  getfem::regular_unit_mesh(mesh, nsubdiv, pgt,
			    PARAM.int_value("MESH_NOISED") != 0);
  
  /* scale the unit mesh to [LX,LY,..] and incline it */
   bgeot::base_matrix M(N,N);
  for (size_type i=0; i < N; ++i) {
    static const char *t[] = {"LX","LY","LZ"};
    M(i,i) = (i<3) ? PARAM.real_value(t[i],t[i]) : 1.0;
  }
  mesh.transformation(M);

  datafilename = PARAM.string_value("ROOTFILENAME","Base name of data files.");
  residu = PARAM.real_value("RESIDU"); if (residu == 0.) residu = 1e-10;

  nu = PARAM.real_value("NU", "Viscosit�");
  dt = PARAM.real_value("DT", "Time step");
  T = PARAM.real_value("T", "Final time");
  dtexport = PARAM.real_value("DT_EXPORT", "Final time");
  noisy = PARAM.int_value("NOISY", "");
  dxexport = PARAM.int_value("DX_EXPORT", "");
  mf_u.set_qdim(N);

  /* set the finite element on the mf_u */
  getfem::pfem pf_u = 
    getfem::fem_descriptor(FEM_TYPE);
  getfem::pintegration_method ppi = 
    getfem::int_method_descriptor(INTEGRATION); 

  mim.set_integration_method(mesh.convex_index(), ppi);
  mf_u.set_finite_element(mesh.convex_index(), pf_u);
  mf_p.set_finite_element(mesh.convex_index(),
			  getfem::fem_descriptor(FEM_TYPE_P));

  /* set the finite element on mf_rhs (same as mf_u is DATA_FEM_TYPE is
     not used in the .param file */
  const char *data_fem_name = PARAM.string_value("DATA_FEM_TYPE");
  if (data_fem_name == 0) {
    if (!pf_u->is_lagrange()) {
      DAL_THROW(dal::failure_error, "You are using a non-lagrange FEM "
		<< data_fem_name << ". In that case you need to set "
		<< "DATA_FEM_TYPE in the .param file");
    }
    mf_rhs.set_finite_element(mesh.convex_index(), pf_u);
  } else {
    mf_rhs.set_finite_element(mesh.convex_index(), 
			      getfem::fem_descriptor(data_fem_name));
  }
  
  /* set the finite element on mf_coef. Here we use a very simple element
   *  since the only function that need to be interpolated on the mesh_fem 
   * is f(x)=1 ... */
  mf_coef.set_finite_element(mesh.convex_index(),
			     getfem::classical_fem(pgt,0));

  /* set boundary conditions
   * (Neuman on the upper face, Dirichlet elsewhere) */
  cout << "Selecting Neumann and Dirichlet boundaries\n";
  getfem::convex_face_ct border_faces;
  getfem::outer_faces_of_mesh(mesh, border_faces);
  for (getfem::convex_face_ct::const_iterator it = border_faces.begin();
       it != border_faces.end(); ++it) {
    assert(it->f != size_type(-1));
    base_node un = mesh.normal_of_face_of_convex(it->cv, it->f);
    un /= gmm::vect_norm2(un);
    if (0) {
    // if (dal::abs(un[N-1] - 1.0) < 1.0E-7) { // new Neumann face
      mesh.add_face_to_set(NEUMANN_BOUNDARY_NUM, it->cv, it->f);
    } else {
      mesh.add_face_to_set(DIRICHLET_BOUNDARY_NUM, it->cv, it->f);
    }
  }
}

/**************************************************************************/
/*  Model.                                                                */
/**************************************************************************/

base_small_vector sol_f(const base_small_vector &P) {
  base_small_vector res(P.size());
  res[P.size()-1] = -1.0;
  return res;
}

base_small_vector Dir_cond(const base_small_vector &P) {
  base_small_vector res(P.size());
  // res[P.size()-1] = 0.0;
  return res;
}

bool navier_stokes_problem::solve() {
  size_type nb_dof_rhs = mf_rhs.nb_dof();
  size_type N = mesh.dim();

  cout << "Number of dof for u : " << mf_u.nb_dof() << endl;
  cout << "Number of dof for p : " << mf_p.nb_dof() << endl;

  // 
  // definition of the Laplacian problem
  //

  // Laplacian brick.
  // getfem::mdbrick_scalar_elliptic<> laplacian(mf_u, mf_coef, nu, true);
  getfem::mdbrick_isotropic_linearized_elasticity<>
    laplacian(mim, mf_u, mf_coef, -nu, nu, true);

  // Volumic source term
  plain_vector F(nb_dof_rhs * N);
  for (size_type i = 0; i < nb_dof_rhs; ++i)
    gmm::copy(sol_f(mf_rhs.point_of_dof(i)),
	      gmm::sub_vector(F, gmm::sub_interval(i*N, N)));
  getfem::mdbrick_source_term<> laplacian_f(laplacian, mf_rhs, F);


  // Dirichlet condition brick.
  for (size_type i = 0; i < nb_dof_rhs; ++i)
    gmm::copy(Dir_cond(mf_rhs.point_of_dof(i)),
	      gmm::sub_vector(F, gmm::sub_interval(i*N, N)));
  getfem::mdbrick_Dirichlet<> laplacian_dir(laplacian_f, mf_rhs,
					  F, DIRICHLET_BOUNDARY_NUM);
  
  // Dynamic brick.
  getfem::mdbrick_dynamic<> laplacian_dyn(laplacian_dir, mf_coef, 1.);
  laplacian_dyn.set_dynamic_coeff(1.0/dt, 1.0);

  // 
  // definition of the mixed problem
  //

  getfem::mdbrick_mass_matrix<> mixed(mim, mf_u, mf_coef, 1./dt, true);
  
  // Pressure term
  getfem::mdbrick_linear_incomp<> mixed_p(mixed, mf_p);
  
  // Dirichlet condition brick.
  for (size_type i = 0; i < nb_dof_rhs; ++i)
    gmm::copy(Dir_cond(mf_rhs.point_of_dof(i)),
	      gmm::sub_vector(F, gmm::sub_interval(i*N, N)));
  getfem::mdbrick_Dirichlet<> mixed_dir(mixed_p, mf_rhs,
					F, DIRICHLET_BOUNDARY_NUM);

  // Dynamic brick.
  getfem::mdbrick_dynamic<> mixed_dyn(mixed_dir, mf_coef, 1.);
  laplacian_dyn.set_dynamic_coeff(0.0, 1.0);


  // 
  // dynamic problem
  //

  plain_vector U0(mf_u.nb_dof()), USTAR(mf_u.nb_dof());
  
  gmm::iteration iter(residu, noisy);
  getfem::standard_model_state MSL(laplacian_dir);
  getfem::standard_model_state MSM(mixed_dir);
  
  std::auto_ptr<getfem::dx_export> exp;
  getfem::stored_mesh_slice sl;
  if (dxexport) {
    exp.reset(new getfem::dx_export(datafilename + ".dx", false));
    if (N <= 2)
      sl.build(mesh, getfem::slicer_none(),4);
    else
      sl.build(mesh, getfem::slicer_boundary(mesh),4);
    exp->exporting(sl,true);
    exp->exporting_mesh_edges();
    exp->write_point_data(mf_u, U0, "stepinit"); 
    exp->serie_add_object("deformationsteps");
  }

  scalar_type t_export(dtexport);
  for (scalar_type t = 0; t <= T; t += dt) {

    iter.init();
    laplacian_dyn.set_DF(gmm::scaled(U0, 1./dt));
    getfem::standard_solve(MSL, laplacian_dyn, iter);
    gmm::copy(laplacian.get_solution(MSL), USTAR);
    
    cout << "norm de USTAR : " << gmm::vect_norm2(USTAR) << endl;
    
    iter.init();
    mixed_dyn.set_DF(gmm::scaled(USTAR, 1./dt));
    getfem::standard_solve(MSM, mixed_dyn, iter);
    gmm::copy(mixed.get_solution(MSM), U0);
    
    cout << "norm de U0 : " << gmm::vect_norm2(U0) << endl;

    if (dxexport && t >= t_export-dt/20.0) {
      exp->write_point_data(mf_u, U0);
      exp->serie_add_object("deformationsteps");
      t_export += dtexport;
    }

  }
  return 1;
}

/**************************************************************************/
/*  main program.                                                         */
/**************************************************************************/

int main(int argc, char *argv[]) {
  dal::exception_callback_debug cb;
  dal::exception_callback::set_exception_callback(&cb); // to debug ...

#ifdef GETFEM_HAVE_FEENABLEEXCEPT /* trap SIGFPE */
  feenableexcept(FE_DIVBYZERO | FE_INVALID);
#endif

  try {    
    navier_stokes_problem p;
    p.PARAM.read_command_line(argc, argv);
    p.init();
    p.mesh.write_to_file(p.datafilename + ".mesh");
    if (!p.solve()) DAL_THROW(dal::failure_error,"Solve has failed");
  }
  DAL_STANDARD_CATCH_ERROR;

  return 0; 
}
