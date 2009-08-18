// -*- c++ -*- (enables emacs c++ mode)
//===========================================================================
//
// Copyright (C) 2006-2009 Yves Renard, Julien Pommier, Jeremie Lasry.
//
// This file is a part of GETFEM++
//
// Getfem++  is  free software;  you  can  redistribute  it  and/or modify it
// under  the  terms  of the  GNU  Lesser General Public License as published
// by  the  Free Software Foundation;  either version 2.1 of the License,  or
// (at your option) any later version.
// This program  is  distributed  in  the  hope  that it will be useful,  but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or  FITNESS  FOR  A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
// You  should  have received a copy of the GNU Lesser General Public License
// along  with  this program;  if not, write to the Free Software Foundation,
// Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.
//
//===========================================================================

#include "crack_bilaplacian.h"
#include "getfem/getfem_export.h"   /* export functions (save solution in a file)  */
#include "getfem/getfem_derivatives.h"
#include "getfem/getfem_regular_meshes.h"
#include "getfem/getfem_assembling.h" /* import assembly methods (and norms comp.) */
#include "getfem/getfem_fourth_order.h"
#include "getfem/getfem_model_solvers.h"
#include "getfem/getfem_superlu.h"
  
/* some Getfem++ types that we will be using */
using bgeot::base_small_vector; /* special class for small (dim<16) vectors */
using bgeot::base_node;  /* geometrical nodes(derived from base_small_vector)*/
using bgeot::base_vector; /* dense vector. */
using bgeot::scalar_type; /* = double */
using bgeot::size_type;   /* = unsigned long */
using bgeot::base_matrix; /* dense matrix. */

/* definition of some matrix/vector types. 
 * default types of getfem_model_solvers.h
 */
typedef getfem::modeling_standard_sparse_vector sparse_vector;
typedef getfem::modeling_standard_sparse_matrix sparse_matrix;
typedef getfem::modeling_standard_plain_vector  plain_vector;

size_type is_global_dof_type(getfem::pdof_description dof){
size_type global_dof = 0 ;
   for (dim_type d = 0; d < 4 ; ++d){
       if (dof == getfem::global_dof(d)) {
          global_dof = 1;
	      }
   }
return global_dof ;
}
 
int main(int argc, char *argv[]) {

   try {
    bilaplacian_crack_problem p;
    p.PARAM.read_command_line(argc, argv);
    if (p.PARAM.int_value("MIXED_ELEMENTS"))
       p.init_mixed_elements();
    else
       p.init();
    plain_vector U;
    p.mesh.write_to_file("mesh.m") ;
    scalar_type ring_radius = p.PARAM.real_value("RING_RADIUS");
    cout.precision(16);
    if (p.PARAM.int_value("SOL_REF") == 0) {
       if (!p.solve(U)) GMM_ASSERT1(false, "Solve has failed");
       p.compute_sif(U, ring_radius);
       if (p.PARAM.int_value("FIC_SERIE") ){
           scalar_type R ;
           for (unsigned i = 0 ; i < 26 ; i++){
              R = 0.05 + i * 0.01 ;
              cout << "R = " << R << "\n" ;
              p.compute_sif(U, R);
           }    
       }
       if (p.PARAM.int_value("COMPUTE_ERROR") == 1)  
          p.compute_error(U) ;
    }
    if (p.PARAM.int_value("SOL_REF") == 1) {
       if (!p.solve_moment(U)) GMM_ASSERT1(false, "Solve has failed");
        p.compute_sif(U, ring_radius);
    }
    if (p.PARAM.int_value("SOL_REF") == 2) {
       if (!p.solve(U)) GMM_ASSERT1(false, "Solve has failed");
       if (p.PARAM.int_value("MIXED_ELEMENTS") == 0)
           p.compute_sif(U, ring_radius);
    }
    if (p.PARAM.int_value("ENRICHMENT_OPTION") > 2){
        p.sif_direct_estimation(U) ;
    }
    scalar_type K1_exact = 0. ;
    scalar_type K2_exact = 0. ;
    p.exact_sif(K1_exact, K2_exact) ;
    cout << "K1_exact = " << K1_exact << " ; K2_exact = " << K2_exact << "\n" ;

    //p.compute_H2_error_field(U) ;
    //

    int VTK_EXPORT = int(p.PARAM.int_value("VTK_EXPORT"));
    int MATLAB_EXPORT = int(p.PARAM.int_value("MATLAB_EXPORT"));
    int DX_EXPORT = int(p.PARAM.int_value("DX_EXPORT"));

    if (VTK_EXPORT || MATLAB_EXPORT || DX_EXPORT)
       p.export_solution(U) ;

   //getchar();
   cout << "End of program reached\n"; 
   } GMM_STANDARD_CATCH_ERROR;
  return 0; 
}
























