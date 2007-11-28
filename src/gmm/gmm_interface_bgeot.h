// -*- c++ -*- (enables emacs c++ mode)
//===========================================================================
//
// Copyright (C) 2002-2008 Yves Renard
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

/**@file gmm_interface_bgeot.h
   @author  Yves Renard <Yves.Renard@insa-lyon.fr>
   @date October 13, 2002.
   @brief interface for bgeot::small_vector
*/
#ifndef GMM_INTERFACE_BGEOT_H__
#define GMM_INTERFACE_BGEOT_H__


namespace gmm {

  /* ********************************************************************* */
  /*		                                         	 	   */
  /*		Traits for bgeot objects                     		   */
  /*		                                         		   */
  /* ********************************************************************* */

  template <typename T> struct linalg_traits<bgeot::small_vector<T> > {
    typedef bgeot::small_vector<T> this_type;
    typedef this_type origin_type;
    typedef linalg_false is_reference;
    typedef abstract_vector linalg_type;
    typedef T value_type;
    typedef T& reference;
    typedef typename this_type::iterator iterator;
    typedef typename this_type::const_iterator const_iterator;
    typedef abstract_dense storage_type;
    typedef linalg_true index_sorted;
    static size_type size(const this_type &v) { return v.size(); }
    static iterator begin(this_type &v) { return v.begin(); }
    static const_iterator begin(const this_type &v) { return v.begin(); }
    static iterator end(this_type &v) { return v.end(); }
    static const_iterator end(const this_type &v) { return v.end(); }
    static origin_type* origin(this_type &v) { return &v; }
    static const origin_type* origin(const this_type &v) { return &v; }
    static void clear(origin_type* o, const iterator &it, const iterator &ite)
    { std::fill(it, ite, value_type(0)); }
    static void do_clear(this_type &v)
    { std::fill(v.begin(), v.end(), value_type(0)); }
    static value_type access(const origin_type *, const const_iterator &it,
			     const const_iterator &, size_type i)
    { return it[i]; }
    static reference access(origin_type *, const iterator &it,
			    const iterator &, size_type i)
    { return it[i]; }
    static void resize(this_type &v, size_type n) { v.resize(n); }
  };

}


#endif //  GMM_INTERFACE_BGEOT_H__
