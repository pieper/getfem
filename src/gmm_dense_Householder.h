/* -*- c++ -*- (enables emacs c++ mode)                                    */
/* *********************************************************************** */
/*                                                                         */
/* Library :  Generic Matrix Methods  (gmm)                                */
/* File    :  gmm_dense_Householder.h : Householder calculus.              */
/*     									   */
/* Date : June 5, 2003.                                                    */
/* Author : Caroline Lecalvez, Caroline.Lecalvez@gmm.insa-tlse.fr          */
/*          Yves Renard, Yves.Renard@gmm.insa-tlse.fr                      */
/*                                                                         */
/* *********************************************************************** */
/*                                                                         */
/* Copyright (C) 2003  Yves Renard.                                        */
/*                                                                         */
/* This file is a part of GMM++                                            */
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

#ifndef GMM_DENSE_HOUSEHOLDER_H
#define GMM_DENSE_HOUSEHOLDER_H

#include "gmm_solvers.h"

namespace gmm {

  /* ********************************************************************* */
  /*    Rank one update  (complex and real version)                        */
  /* ********************************************************************* */

  template <class Matrix, class VecX, class VecY>
  inline void rank_one_update(Matrix &A, const VecX& x,
			      const VecY& y, row_major) {
    typedef typename linalg_traits<Matrix>::value_type value_type;
    size_type N = mat_nrows(A);
    if (N > vect_size(x) || mat_ncols(A) > vect_size(y))
      DAL_THROW(dimension_error,"dimensions mismatch");
    typename linalg_traits<VecX>::const_iterator itx = vect_const_begin(x);
    for (size_type i = 0; i < N; ++i, ++itx) {
      typedef typename linalg_traits<Matrix>::sub_row_type row_type;
      row_type row = mat_row(A, i);
      typename linalg_traits<row_type>::iterator
	it = vect_begin(row), ite = vect_end(row);
      typename linalg_traits<VecY>::const_iterator ity = vect_const_begin(y);
#   ifdef USING_BROKEN_GCC295
      typedef typename linalg_traits<Matrix>::value_type T;
      for (; it != ite; ++it, ++ity)
	const_cast<T &>(*it) += conj_product(*ity, *itx);
#   else
      value_type tx = *itx;
      for (; it != ite; ++it, ++ity) *it += conj_product(*ity, tx);
#   endif
    }
  }

  template <class Matrix, class VecX, class VecY>
  inline void rank_one_update(Matrix &A, const VecX& x,
			      const VecY& y, col_major) {
    typedef typename linalg_traits<Matrix>::value_type value_type;
    size_type M = mat_ncols(A);
    if (mat_nrows(A) > vect_size(x) || M > vect_size(y))
      DAL_THROW(dimension_error,"dimensions mismatch");
    typename linalg_traits<VecY>::const_iterator ity = vect_const_begin(y);
    for (size_type i = 0; i < M; ++i, ++ity) {
      typedef typename linalg_traits<Matrix>::sub_col_type col_type;
      col_type col = mat_col(A, i);
      typename linalg_traits<col_type>::iterator
	it = vect_begin(col), ite = vect_end(col);
      typename linalg_traits<VecX>::const_iterator itx = vect_const_begin(x);
#   ifdef USING_BROKEN_GCC295
      typedef typename linalg_traits<Matrix>::value_type T;
      for (; it != ite; ++it, ++itx)
	const_cast<T &>(*it) += conj_product(*ity, *itx);
#   else
      value_type ty = *ity;
      for (; it != ite; ++it, ++itx) *it += conj_product(ty, *itx); 
#   endif
    }
  }
  
  template <class Matrix, class VecX, class VecY>
  inline void rank_one_update(const Matrix &AA, const VecX& x,
			      const VecY& y) {
    Matrix& A = const_cast<Matrix&>(AA);
    rank_one_update(A, x, y, typename principal_orientation_type<typename
		    linalg_traits<Matrix>::sub_orientation>::potype());
  }

  /* ********************************************************************* */
  /*    Rank two update  (complex and real version)                        */
  /* ********************************************************************* */

  template <class Matrix, class VecX, class VecY>
  inline void rank_two_update(Matrix &A, const VecX& x,
			      const VecY& y, row_major) {
    typedef typename linalg_traits<Matrix>::value_type value_type;
    size_type N = mat_nrows(A);
    if (N > vect_size(x) || mat_ncols(A) > vect_size(y))
      DAL_THROW(dimension_error,"dimensions mismatch");
    typename linalg_traits<VecX>::const_iterator itx1 = vect_const_begin(x);
    typename linalg_traits<VecY>::const_iterator ity2 = vect_const_begin(y);
    for (size_type i = 0; i < N; ++i, ++itx1, ++ity2) {
      typedef typename linalg_traits<Matrix>::sub_row_type row_type;
      row_type row = mat_row(A, i);
      typename linalg_traits<row_type>::iterator
	it = vect_begin(row), ite = vect_end(row);
      typename linalg_traits<VecX>::const_iterator itx2 = vect_const_begin(x);
      typename linalg_traits<VecY>::const_iterator ity1 = vect_const_begin(y);
#   ifdef USING_BROKEN_GCC295
      typedef typename linalg_traits<Matrix>::value_type T;
      for (; it != ite; ++it, ++ity1, ++itx2)
	const_cast<T &>(*it) += conj_product(*ity1, *itx1)
	                      + conj_product(*itx2, *ity2);
#   else
      value_type tx = *itx1, ty = *ity2;
      for (; it != ite; ++it, ++ity1, ++itx2)
	*it += conj_product(*ity1, tx) + conj_product(*itx2, ty);
#   endif
    }
  }

  template <class Matrix, class VecX, class VecY>
  inline void rank_two_update(Matrix &A, const VecX& x,
			      const VecY& y, col_major) {
    typedef typename linalg_traits<Matrix>::value_type value_type;
    size_type M = mat_ncols(A);
    if (mat_nrows(A) > vect_size(x) || M > vect_size(y))
      DAL_THROW(dimension_error,"dimensions mismatch");
    typename linalg_traits<VecX>::const_iterator itx2 = vect_const_begin(x);
    typename linalg_traits<VecY>::const_iterator ity1 = vect_const_begin(y);
    for (size_type i = 0; i < M; ++i, ++ity1, ++itx2) {
      typedef typename linalg_traits<Matrix>::sub_col_type col_type;
      col_type col = mat_col(A, i);
      typename linalg_traits<col_type>::iterator
	it = vect_begin(col), ite = vect_end(col);
      typename linalg_traits<VecX>::const_iterator itx1 = vect_const_begin(x);
      typename linalg_traits<VecY>::const_iterator ity2 = vect_const_begin(y);
#   ifdef USING_BROKEN_GCC295
      typedef typename linalg_traits<Matrix>::value_type T;
      for (; it != ite; ++it, ++itx1, ++ity2)
	const_cast<T &>(*it) += conj_product(*ity1, *itx1)
	                      + conj_product(*itx2, *ity2);
#   else
      value_type ty = *ity1, tx = *itx2;
      for (; it != ite; ++it, ++itx1, ++ity2)
	*it += conj_product(ty, *itx1) + conj_product(tx, *ity2); 
#   endif
    }
  }
  
  template <class Matrix, class VecX, class VecY>
  inline void rank_two_update(const Matrix &AA, const VecX& x,
			      const VecY& y) {
    Matrix& A = const_cast<Matrix&>(AA);
    rank_two_update(A, x, y, typename principal_orientation_type<typename
		    linalg_traits<Matrix>::sub_orientation>::potype());
  }

  /* ********************************************************************* */
  /*    Householder vector computation (complex and real version)          */
  /* ********************************************************************* */

  template <class VECT> void house_vector(const VECT &VV) {
    VECT &V = const_cast<VECT &>(VV);
    typedef typename linalg_traits<VECT>::value_type T;
    typedef typename number_traits<T>::magnitude_type R;
    
    R mu = vect_norm2(V), abs_v0 = dal::abs(V[0]);
    if (mu != R(0)) {
      T beta;
      if (abs_v0 != R(0)) beta = abs_v0 / (V[0] * (abs_v0 + mu));
      else beta = T(R(1) / mu);
      gmm::scale(V, beta);
    }
    V[0] = T(1);
  }
  
  /* ********************************************************************* */
  /*    Householder updates  (complex and real version)                    */
  /* ********************************************************************* */

  template <class MAT, class VECT1, class VECT2> inline
    void row_house_update(const MAT &AA, const VECT1 &V, const VECT2 &WW) {
    VECT2 &W = const_cast<VECT2 &>(WW); MAT &A = const_cast<MAT &>(AA);
    typedef typename linalg_traits<MAT>::value_type value_type;
    typedef typename number_traits<value_type>::magnitude_type magnitude_type;

    gmm::mult(conjugated(A),
	      scaled(V, magnitude_type(-2.0)/vect_norm2_sqr(V)), W);
    rank_one_update(A, V, W);
  }

  template <class MAT, class VECT1, class VECT2> inline
    void col_house_update(const MAT &AA, const VECT1 &V, const VECT2 &WW) {
    VECT2 &W = const_cast<VECT2 &>(WW); MAT &A = const_cast<MAT &>(AA);
    typedef typename linalg_traits<MAT>::value_type value_type;
    typedef typename number_traits<value_type>::magnitude_type magnitude_type;
    
    gmm::mult(A, scaled(V, magnitude_type(-2.0) / vect_norm2_sqr(V)), W);
    rank_one_update(A, W, V);
  }

  /* ********************************************************************* */
  /*    Hessemberg reduction with Householder.                             */
  /* ********************************************************************* */

  template <class MAT1, class MAT2>
    void Hessenberg_reduction(const MAT1& AA, const MAT2 &QQ, bool compute_Q){
    MAT1& A = const_cast<MAT1&>(AA); MAT2& Q = const_cast<MAT2&>(QQ);
    typedef typename linalg_traits<MAT1>::value_type value_type;
    size_type n = mat_nrows(A);
    std::vector<value_type> v(n), w(n);
    if (compute_Q) gmm::copy(identity_matrix(), Q);
    sub_interval SUBK(0,n);
    for (size_type k = 1; k < n-1; ++k) {
      sub_interval SUBI(k, n-k), SUBJ(k-1,n-k+1);
      v.resize(n-k);
      for (size_type j = k; j < n; ++j) v[j-k] = A(j, k-1);
      house_vector(v);
      row_house_update(sub_matrix(A, SUBI, SUBJ), v, sub_vector(w, SUBJ));
      col_house_update(sub_matrix(A, SUBK, SUBI), v, w);
      // is it possible to "unified" the two on the common part of the matrix?
      if (compute_Q) col_house_update(sub_matrix(Q, SUBK, SUBI), v, w);
    }
  }

  /* ********************************************************************* */
  /*    Householder tridiagonalization for symmetric matrices              */
  /* ********************************************************************* */

  template <class MAT1, class MAT2> 
  void Householder_tridiagonalization(const MAT1 &AA, const MAT2 &QQ,
				     bool compute_q) {
    MAT1 &A = const_cast<MAT1 &>(AA); MAT2 &Q = const_cast<MAT2 &>(QQ);
    typedef typename linalg_traits<MAT1>::value_type T;
    typedef typename number_traits<T>::magnitude_type R;
    // to be optimized

    size_type n = mat_nrows(A); 
    std::vector<T> v(n), p(n), w(n), ww(n);
    sub_interval SUBK(0,n);

    if (compute_q) gmm::copy(identity_matrix(), Q);

    for (size_type k = 1; k < n-1; ++k) { // not optimized ...
      sub_interval SUBI(k, n-k);
      v.resize(n-k); p.resize(n-k); w.resize(n-k); 
      for (size_type l = k; l < n; ++l) 
	{ v[l-k] = w[l-k] = A(l, k-1); A(l, k-1) = A(k-1, l) = T(0); }
      house_vector(v);
      R norm = vect_norm2_sqr(v);
      A(k-1, k) = dal::conj(A(k, k-1) = w[0] - T(2)*v[0]*vect_hp(v, w)/norm);

      gmm::mult(sub_matrix(A, SUBI), gmm::scaled(v, T(-2) / norm), p);
      gmm::add(p, gmm::scaled(v, -vect_hp(p, v) / norm), w);
      rank_two_update(sub_matrix(A, SUBI), v, w);
      // it should be possible to compute only the upper or lower part

      if (compute_q) col_house_update(sub_matrix(Q, SUBK, SUBI), v, ww);
    }
  }

  /* ********************************************************************* */
  /*    Real and complex Givens rotations                                  */
  /* ********************************************************************* */

  template <class T> void Givens_rotation(T a, T b, T &c, T &s) {
    typedef typename number_traits<T>::magnitude_type R;
    R aa = dal::abs(a), bb = dal::abs(b);
    if (bb == R(0)) { c = T(1); s = T(0);   return; }
    if (aa == R(0)) { c = T(0); s = b / bb; return; }
    if (bb > aa)
      { T t = -a/b; s = T(R(1) / (sqrt(R(1)+dal::abs_sqr(t)))); c = s * t; }
    else
      { T t = -b/a; c = T(R(1) / (sqrt(R(1)+dal::abs_sqr(t)))); s = c * t; }
  }

  // Apply Q* v
  template <class T> inline
  void Apply_Givens_rotation_left(T &x, T &y, T c, T s)
  { T t1=x, t2=y; x = dal::conj(c)*t1 - dal::conj(s)*t2; y = c*t2 + s*t1; }

  // Apply v^T Q
  template <class T> inline
  void Apply_Givens_rotation_right(T &x, T &y, T c, T s)
  { T t1=x, t2=y; x = c*t1 - s*t2; y = dal::conj(c)*t2 + dal::conj(s)*t1; }

  template <class MAT, class T>
  void row_rot(const MAT &AA, T c, T s, size_type i, size_type k) {
    MAT &A = const_cast<MAT &>(AA); // can be specialized for row matrices
    for (size_type j = 0; j < mat_ncols(A); ++j)
      Apply_Givens_rotation_left(A(i,j), A(k,j), c, s);
  }

  template <class MAT, class T>
  void col_rot(const MAT &AA, T c, T s, size_type i, size_type k) {
    MAT &A = const_cast<MAT &>(AA); // can be specialized for column matrices
    for (size_type j = 0; j < mat_nrows(A); ++j)
      Apply_Givens_rotation_right(A(j,i), A(j,k), c, s);
  }

}

#endif

