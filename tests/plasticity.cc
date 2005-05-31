/* *********************************************************************** */
/*                                                                         */
/* Copyright (C) 2002-2004 Yves Renard, Julien Pommier.                    */
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
 * Small deformation plasticity problem.
 *
 * This program is used to check that getfem++ is working. This is also 
 * a good example of use of Getfem++.
*/

#include <getfem_assembling.h> /* import assembly methods (and norms comp.) */
#include <getfem_regular_meshes.h>
#include <getfem_modeling.h>
#include <getfem_plasticity.h>

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

template<typename VEC> static void vecsave(std::string fname, const VEC& V);

//function to save a vector
template<typename VEC> static void vecsave(std::string fname, const VEC& V) {
  std::ofstream f(fname.c_str()); f.precision(16);
  for (size_type i=0; i < V.size(); ++i) f << V[i] << "\n"; 
}

/*
  structure for the elastoplatic problem
*/
struct plasticity_problem {

  enum { DIRICHLET_BOUNDARY_NUM = 0, NEUMANN_BOUNDARY_NUM = 1};
  getfem::getfem_mesh mesh;  /* the mesh */
  getfem::mesh_im  mim;      /* integration methods.                         */
  getfem::mesh_fem mf_u;     /* main mesh_fem, for the elastostatic solution */
  getfem::mesh_fem mf_rhs;   /* mesh_fem for the right hand side (f(x),..)   */
  getfem::mesh_fem mf_coef;  /* mesh_fem used to represent pde coefficients  */
  scalar_type lambda, mu;    /* Lam� coefficients.                           */

  scalar_type residu;        /* max residu for the iterative solvers         */

  scalar_type stress_threshold;
  size_type flag_hyp;
  std::vector<std::vector<scalar_type> > sigma_b;

  std::string datafilename;
  ftool::md_param PARAM;

  bool solve(plain_vector &U);
  void init(void);
  plasticity_problem(void) : mim(mesh), mf_u(mesh), mf_rhs(mesh),
			     mf_coef(mesh) {}
};

/* Read parameters from the .param file, build the mesh, set finite element
 * and integration methods and selects the boundaries.
 */
void plasticity_problem::init(void)
{
  const char *MESH_TYPE = PARAM.string_value("MESH_TYPE","Mesh type ");
  const char *FEM_TYPE  = PARAM.string_value("FEM_TYPE","FEM name");
  const char *INTEGRATION = PARAM.string_value("INTEGRATION",
					       "Name of integration method");
  cout << "MESH_TYPE=" << MESH_TYPE << "\n";
  cout << "FEM_TYPE="  << FEM_TYPE << "\n";
  cout << "INTEGRATION=" << INTEGRATION << "\n";

  residu = PARAM.real_value("RESIDU", "residu");

  //  file to save the mesh
  datafilename=std::string(PARAM.string_value("ROOTFILENAME","Filename for saving"));
  /* First step : build the mesh */
  size_type N;
  bgeot::pgeometric_trans pgt = 0; 
  if (strcmp(MESH_TYPE, "load")!=0) {
    std ::cout << "created getfem mesh"  << "\n"; 
    pgt = bgeot::geometric_trans_descriptor(MESH_TYPE);
    N = pgt->dim();
    std::vector<size_type> nsubdiv(N);
    nsubdiv[0]=PARAM.int_value("NX", "Nomber of space steps in x direction ");
    nsubdiv[1]=PARAM.int_value("NY", "Nomber of space steps in y direction ");
    if(N==3)
      nsubdiv[2]=PARAM.int_value("NZ", "Nomber of space steps in z direction ");
    getfem::regular_unit_mesh(mesh, nsubdiv, pgt,
                              PARAM.int_value("MESH_NOISED") != 0);
    
    bgeot::base_matrix M(N,N);
    for (size_type i=0; i < N; ++i) {
      static const char *t[] = {"LX","LY","LZ"};
      M(i,i) = (i<3) ? PARAM.real_value(t[i],t[i]) : 1.0;
    }
    if (N>1) { M(0,1) = PARAM.real_value("INCLINE") * PARAM.real_value("LY"); }

    /* scale the unit mesh to [LX,LY,..] and incline it */
    mesh.transformation(M);
  } else {
    std ::cout << "mesh from pdetool"  << "\n"; 
    const char *MESH_FILE = PARAM.string_value("MESH_FILE","Mesh file name");
    mesh.read_from_file(MESH_FILE);
    
    N = mesh.dim();
    pgt = mesh.trans_of_convex(mesh.convex_index().first_true());
  }

  mu = PARAM.real_value("MU", "Lam� coefficient mu");
  lambda = PARAM.real_value("LAMBDA", "Lam� coefficient lambda");
  mf_u.set_qdim(N);

  /* set the finite element on the mf_u */
  getfem::pfem pf_u = 
    getfem::fem_descriptor(FEM_TYPE);
  getfem::pintegration_method ppi = 
    getfem::int_method_descriptor(INTEGRATION);

  mim.set_integration_method(mesh.convex_index(), ppi);
  mf_u.set_finite_element(mesh.convex_index(), pf_u);
  
  /* set the finite element on mf_rhs (same as mf_u is DATA_FEM_TYPE is
     not used in the .param file */
  const char *data_fem_name = PARAM.string_value("DATA_FEM_TYPE");
  if (data_fem_name == 0) {
    if (!pf_u->is_lagrange()) {
      DAL_THROW(dal::failure_error, "You are using a non-lagrange FEM. "
		<< "In that case you need to set "
		<< "DATA_FEM_TYPE in the .param file");
    }
    mf_rhs.set_finite_element(mesh.convex_index(), pf_u);
  } else {
    mf_rhs.set_finite_element(mesh.convex_index(), getfem::fem_descriptor(data_fem_name));
  }

  /* set the finite element on mf_coef. Here we use a very simple element
   *  since the only function that need to be interpolated on the mesh_fem 
   * is f(x)=1 ... */
  mf_coef.set_finite_element(mesh.convex_index(),
			     getfem::classical_fem(pgt,0));

  /* set boundary conditions
   * (Neuman on the upper face, Dirichlet elsewhere) */
  cout << "Selecting Neumann and Dirichlet boundaries\n";
  getfem::mesh_region border_faces;
  getfem::outer_faces_of_mesh(mesh, border_faces);
  for (getfem::mr_visitor it(border_faces); !it.finished(); ++it) {
    assert(it.is_face());
    base_node un = mesh.normal_of_face_of_convex(it.cv(), it.f());
    un /= gmm::vect_norm2(un);

    if (gmm::abs(un[0] - 1.0) < 1.0E-7)
      mesh.region(NEUMANN_BOUNDARY_NUM).add(it.cv(), it.f());
    else if (gmm::abs(un[0] + 1.0) < 1.0E-7) 
      mesh.region(DIRICHLET_BOUNDARY_NUM).add(it.cv(), it.f());
      
  }
 
  //PARTIE RELATIVE A LA PLASTICITE  
  stress_threshold = PARAM.real_value("STRESS_THRESHOLD",
				      "plasticity stress_threshold");
  flag_hyp=PARAM.int_value("FLAG_HYP");
}



/**************************************************************************/
/*  Model.                                                                */
/**************************************************************************/

bool plasticity_problem::solve(plain_vector &U) {
  size_type nb_dof_rhs = mf_rhs.nb_dof();
  size_type N = mesh.dim();


  plain_vector F(nb_dof_rhs * N);
  getfem::VM_projection proj(flag_hyp);
  getfem::mdbrick_plasticity<> PLAS(mim, mf_u, mf_coef, lambda, mu,
				    stress_threshold, proj);
  
  // Neumann condition brick
  getfem::mdbrick_source_term<> NEUMANN(PLAS, mf_rhs, F,NEUMANN_BOUNDARY_NUM);
  // Dirichlet condition brick.
  getfem::mdbrick_Dirichlet<> final_model(NEUMANN, mf_rhs,
					  F, DIRICHLET_BOUNDARY_NUM, false);
  getfem::standard_model_state MS(final_model);

  const size_type Nb_t=1;
  scalar_type t[Nb_t]={0.5};

  std::string uname(datafilename+".U");
  std::ofstream f0(uname.c_str()); f0.precision(16);
  f0 << "\n";
  f0.close();

  std::string sname(datafilename+".sigmabar");
  std::ofstream s0(sname.c_str()); f0.precision(16);
  s0 << "\n";
  s0.close();

  for (size_type nb = 0; nb < Nb_t;++nb) {

    // Defining the Neumann condition right hand side.
    base_small_vector v(N);
    v[N-1] = -PARAM.real_value("FORCE");
    gmm::scale(v,t[nb]);
    
    for (size_type i = 0; i < nb_dof_rhs; ++i)
      gmm::copy(v, gmm::sub_vector(F, gmm::sub_interval(i*N, N)));

    NEUMANN.set_rhs(F);
    
    // Generic solve.
    cout << "Number of variables : " << final_model.nb_dof() << endl;

    gmm::iteration iter(residu, 2, 40000);
    getfem::standard_solve(MS, final_model, iter);

    PLAS.compute_constraints(MS);
    
    // Get the solution and save it
    gmm::copy(PLAS.get_solution(MS), U);
    std::ofstream f(uname.c_str(),std::ios_base::app); f.precision(16);
    f << t[nb] << "\n";
    for(size_type i=0;i<gmm::vect_size(U);++i) 
      f <<U[i] <<" " ;  
    f<<"\n";

    //Get sigma_bar (remaining constraints) and save it
    PLAS.get_proj(sigma_b);
    
    std::ofstream s(sname.c_str(),std::ios_base::app); s.precision(16);
    size_type nb_elts;
    size_type nb_cv = gmm::vect_size(sigma_b);
    s << "\n";
    for (size_type cv=0;cv<nb_cv;++cv){
      nb_elts = gmm::vect_size(sigma_b[cv]);
      for(size_type i=0;i<nb_elts;++i) s <<sigma_b[cv][i] <<" ";
    }      
  }
  return true;
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
    plasticity_problem p;
    p.PARAM.read_command_line(argc, argv);
    p.init();
    p.mesh.write_to_file(p.datafilename + ".mesh");
    plain_vector U(p.mf_u.nb_dof());
    if (!p.solve(U)) DAL_THROW(dal::failure_error,"Solve has failed");

    cout << "Resultats dans fichier : "<<p.datafilename<<".* \n";
    p.mf_u.write_to_file(p.datafilename + ".meshfem",true);
    scalar_type t[2]={p.mu,p.lambda};
    vecsave(p.datafilename+".coef", std::vector<scalar_type>(t, t+2));    
  }
  DAL_STANDARD_CATCH_ERROR;
  
  return 0; 
}
