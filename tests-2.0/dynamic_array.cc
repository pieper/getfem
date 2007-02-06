// -*- c++ -*- (enables emacs c++ mode)
//========================================================================
//
// Copyright (C) 2002-2006 Yves Renard.
//
// This file is a part of GETFEM++
//
// Getfem++ is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301,
// USA.
//
//========================================================================
#include <getfem/dal_basic.h>
#include <deque>
#include <complex>

typedef std::deque<int>::size_type size_type;
template<typename T> struct dyndeque : public std::deque<T> {
  T &operator[](unsigned i) { 
    if (i >= this->size()) 
      this->resize(i+1); 
    return std::deque<T>::operator[](i); 
  }
};

template<typename T> struct dynarray : public dal::dynamic_array<T> {
  void push_back(const T& t) { (*this)[this->size()] = t; }
};

template <typename DA> void bench_da(unsigned N1, unsigned N2) {
  double t = dal::uclock_sec();
  DA v;
  for (unsigned n=0; n < N1; ++n) {
    v.clear();
    for (unsigned i=0; i < N2; ++i) {
      v.push_back(i);
    }
  }
  cout << "  push_back         : " << dal::uclock_sec()-t << " sec\n";

  t = dal::uclock_sec();
  v.clear();
  v.resize(N2);
  for (unsigned n=0; n < N1*2; ++n) {
    for (unsigned i=0; i < N2; ++i) {
      v[i] = i+n;
    }
  }
  cout << "  random access fill: " << dal::uclock_sec()-t << " sec\n";
  
  t = dal::uclock_sec();
  v.clear();
  v.resize(N2);
  for (unsigned n=0; n < N1*2; ++n) {
    typename DA::iterator it = v.begin(), ite = v.end();
    for (; it != ite; ++it) {
      *it += n;
    }
  }
  cout << "  iterator fill     : " << dal::uclock_sec()-t << " sec\n";

  DA v2; v2.resize(N2);
  { typename DA::iterator it = v2.begin(), ite = v2.end();
    for (; it != ite; ++it) *it = rand(); }

  t = dal::uclock_sec(); 
  for (unsigned n=0; n < N1/10; ++n) {
    v = v2; std::sort(v.begin(), v.end());
  }
  cout << "  sort              : " << dal::uclock_sec()-t << " sec\n";
  
}

void bench() {
  unsigned N1=1000, N2 = 10000;
  cout << "dynamic_array<long long> performances: \n";
  bench_da<dynarray<size_type> >(N1, N2);
  cout << "std::deque<long long> performances:\n";
  bench_da<dyndeque<size_type> >(N1, N2);
  cout << "dynamic_array<int> performances: \n";
  bench_da<dynarray<int> >(N1, N2);
  cout << "std::deque<int> performances:\n";
  bench_da<dyndeque<int> >(N1, N2);
}


int main(void) {
  try {

    cout << "size of int           : " << sizeof(int)           << endl;
    cout << "size of size_t        : " << sizeof(size_t)        << endl;
    cout << "size of (int *)       : " << sizeof(int *)         << endl;
    cout << "size of short int     : " << sizeof(short int)     << endl;
    cout << "size of long int      : " << sizeof(long int)      << endl;
    cout << "size of long long int : " << sizeof(long long int) << endl;
    cout << "size of char          : " << sizeof(char)          << endl;
    cout << "size of float         : " << sizeof(float)         << endl;
    cout << "size of double        : " << sizeof(double)        << endl;
    cout << "size of long double   : " << sizeof(long double)   << endl;
    cout << "size of complex<float>: " << sizeof(std::complex<float>)
	 << endl;
    cout << "size of complex<double>: " << sizeof(std::complex<double>)
	 << endl;
    cout << "size of complex<long double>: "
	 << sizeof(std::complex<long double>) << endl;

    assert(sizeof(dal::int8_type)   == 1);
    assert(sizeof(dal::uint8_type)  == 1);
    assert(sizeof(dal::int16_type)  == 2);
    assert(sizeof(dal::uint16_type) == 2);
    assert(sizeof(dal::int32_type)  == 4);
    assert(sizeof(dal::uint32_type) == 4);
    assert(sizeof(dal::int64_type)  == 8);
    assert(sizeof(dal::uint64_type) == 8);

    // from stl_config.h
#   ifdef __GNUC__
    cout << "Gnu compiler " << __GNUC__ << "." << __GNUC_MINOR__ << endl;
#   endif

#   if defined(__sgi) && !defined(__GNUC__)
    cout << "Sgi compiler " << _COMPILER_VERSION << endl;
#   endif

#   if defined(__SUNPRO_CC)
    cout << "Sun pro compiler\n";
#   endif

#   if defined(__BORLANDC__)
    cout << "Borland compiler\n";
#   endif

    // std::complex<float> x(1.0,0.0);
    // cout << "A complex : " << x << endl;

    bench();
    
    size_t ee = 1, f = 2;
    ptrdiff_t g = ee - f;
    cout << "1 - 2 = " << g << endl;
    if (g != -1) GMM_THROW(dal::internal_error, "Basic operation error");

    dal::dynamic_array<int, 4> t;

    try {
      t[(unsigned)(-5)] = 8;
      GMM_THROW(dal::internal_error,
		"negative index does not produce an error");
    }
    catch(std::logic_error e) {
      cout << "Out of range error successfully catched, ok\n";
    }

    t[64] = 13;
    // cout << "capacity : (should be 80) " << t.capacity() << endl;
    if (t.capacity() != 80)
      GMM_THROW(dal::internal_error, " bad capacity");
    
    dal::dynamic_array<int, 4>::iterator itb = t.begin(), ite = t.end();
    dal::dynamic_array<int, 4>::iterator ita;
    ita = itb++;
    // cout << "range : " << (ita - t.begin()) << endl;
    
    while (itb != ite)  *itb++ = int(3);
    
    
    // std::fill(t.begin(), t.end(), int(3));
    
    // cout << "capacity : (should be 80) " << t.capacity() << endl;
    if (t.capacity() != 80)
      GMM_THROW(dal::internal_error, " bad capacity");
    // cout << "t[64] = (should be 3) " << t[64] << endl;
    if (t[64] != 3) GMM_THROW(dal::internal_error, "iterators don't work");
    
    t.clear();
    // cout << "capacity : (should be 0) " << t.capacity() << endl;
    if (t.capacity() != 0) 
      GMM_THROW(dal::internal_error, " clear does not work");
   
    std::fill(t.begin(), t.end(), int(3));
    // cout << "capacity : (should be 0) " << t.capacity() << endl;
    if (t.capacity() != 0)   
      DAL_THROW(dal::internal_error, " clear does not work");
    t[64] = 6;
    
    dal::dynamic_array<int, 4> t2, t3;
    
    t2[64] = 12;
    t3 = t2 = t;
    
    // cout << "capacity : (should be 80) " << t.capacity() << endl;
    if (t.capacity() != 80)
      GMM_THROW(dal::internal_error, " bad capacity");

    {
      dal::dynamic_array<int, 4>::const_iterator
	it1 = ((const dal::dynamic_array<int, 4> *)(&t))->begin(),
	it2 = ((const dal::dynamic_array<int, 4> *)(&t2))->begin(),
	it3 = ((const dal::dynamic_array<int, 4> *)(&t3))->begin(),
	ite2 = ((const dal::dynamic_array<int, 4> *)(&t))->end(),
	itb2 = ((const dal::dynamic_array<int, 4> *)(&t))->begin();
      
      for ( ; it1 != ite2; it1++, it2++, it3++)
      {
	size_t ind = it1 - itb2;
	if 
	( ( (&(*it1)) != &(t[ind]) ) ||
	  ( (&(*it2)) != &(t2[ind]) ) ||
	  ( (&(*it3)) != &(t3[ind]) ) ||
	  ( (&(*it1)) == (&(*it2)) ) ||
	  ( (&(*it2)) == (&(*it3)) ) ||
	  ( (&(*it1)) == (&(*it3)) ) )
	  GMM_THROW(dal::internal_error, " copy does not work");
      }
    }
    
    {
      dal::dynamic_array<int, 4>::iterator
	it1 = t.begin(),
	it2 = t2.begin(),
	it3 = t3.begin(),
	ite2 = t.end(),
	itb2 = t.begin();
      
      for ( ; it1 != ite2; it1++, it2++, it3++)
	{
	  size_t ind = it1 - itb2;
	  
	  if 
	  ( ( (&(*it1)) != &(t[ind]) ) ||
	    ( (&(*it2)) != &(t2[ind]) ) ||
	    ( (&(*it3)) != &(t3[ind]) ) ||
	    ( (&(*it1)) == (&(*it2)) ) ||
	    ( (&(*it2)) == (&(*it3)) ) ||
	    ( (&(*it1)) == (&(*it3)) ) )
	    GMM_THROW(dal::internal_error, " copy does not work");
	}
    }
    
    
    t[64] = 11;
    
    // cout << "t2[64] = (should be 6 6) " << t2[64] << " " << t3[64]<< endl;
    if (t2[64] != 6)
       GMM_THROW(dal::internal_error, " copy does not work");
    // cout << "capacity : (should be 80) " << t3.capacity() << endl;
    if (t.capacity() != 80)
      GMM_THROW(dal::internal_error, " bad capacity");

  }
  DAL_STANDARD_CATCH_ERROR;

  return 0;
}
