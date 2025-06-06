//  =================================================================
//
//  SymMatrix.h
//
//  This file is part of OptiMX, an interactive tool  
//  for beam optics design and analysis. 
//
//  Copyright (c) 2025 Fermi Forward Discovery Group, LLC.
//  This material was produced under U.S. Government contract
//  89243024CSC000002 for Fermi National Accelerator Laboratory (Fermilab),
//  which is operated by Fermi Forward Discovery Group, LLC for the
//  U.S. Department of Energy. The U.S. Government has rights to use,
//  reproduce, and distribute this software.
//
//  NEITHER THE GOVERNMENT NOR FERMI FORWARD DISCOVERY GROUP, LLC
//  MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
//  LIABILITY FOR THE USE OF THIS SOFTWARE.
//
//  If software is modified to produce derivative works, such modified
//  software should be clearly marked, so as not to confuse it with the
//  version available from Fermilab.
//
//  Additionally, this program is free software; you can redistribute
//  it and/or modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version. Accordingly,
//  this program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  See the GNU General Public License for more details.
//
//  https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
//  https://www.gnu.org/licenses/gpl-3.0.html
//
//  =================================================================
//

#ifndef SYMMATRIX_H
#define SYMMATRIX_H

#include <ostream>
#include <iterator>
#include <stdexcept>
#include <iomanip>
#include <ios>
#include <cmath>
#include <complex>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <initializer_list>

// .....................................................................
// custom ios manipulators

namespace SymMatrix {
  extern int const ios_binary_flag_index;
  std::ostream& setAsciiMode( std::ostream& os); 
  std::ostream& setBinaryMode( std::ostream& os); 
} // namespace

// .....................................................................

template <int dim, typename T>  
class SymMatrix_t {


 struct SymMatrixRow_t {
  
  public:

   SymMatrixRow_t( T** m, int irow, T const mr[] ): m_(m), irow_(irow), row_( const_cast<T*>(mr)) {}; // proxy class for [][] notation

   T&        operator[](int j)       { return  j<= irow_ ? row_[j] : m_[j][irow_]; } 
   T const&  operator[](int j) const { return  j<= irow_ ? row_[j] : m_[j][irow_]; } 

 private:
     int  irow_;
     T*   row_; 
     T**  m_;  
 };
  
 public:

   
  SymMatrix_t();
  SymMatrix_t( T const* data);
  SymMatrix_t( std::initializer_list<T> lst);
  SymMatrix_t( SymMatrix_t const& o);
  SymMatrix_t& operator=(SymMatrix_t const& rhs);

  ~SymMatrix_t();
  
  SymMatrixRow_t        operator[](int i);
  SymMatrixRow_t const  operator[](int i) const;

  private:
  

  int      dim_; 
  T        data_[(dim*(dim+1))/2 ];
  T**      m_;
};



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>  
SymMatrix_t<dim,T>::SymMatrix_t() 
  : dim_(dim), m_(new T*[dim])
{ 
  int k=0; 
  for (int i=0; i<dim_; ++i) { 
    m_[i] = &data_[k]; 
    k += (i+1); 
  } 
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

template <int dim, typename T> 
SymMatrix_t<dim,T>::SymMatrix_t( SymMatrix_t const& o) 
 :  dim_(dim),  m_(new T*[dim])
{ 
  int k=0; 
  for (int i=0; i<dim_; ++i) { 
    m_[i] = &data_[k]; 
    k += (i+1); 
  } 
  std::copy(&o.data_[0], &o.data_[(dim_*(dim_+1))/2], &data_[0]); 
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

template <int dim, typename T> 
SymMatrix_t<dim,T>::SymMatrix_t( T const* data) 
 :  dim_(dim),  m_(new T*[dim])
{ 
  int k=0; 
  for (int i=0; i<dim_; ++i) { 
    m_[i] = &data_[k]; 
    k += (i+1); 
  } 
  std::copy(&data[0], &data[(dim_*(dim_+1))/2], &data_[0]); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

template <int dim, typename T> 
SymMatrix_t<dim,T>::SymMatrix_t( std::initializer_list<T> lst)
  : dim_(dim),  m_(new T*[dim])
{
  int k=0; 
  for (int i=0; i<dim_; ++i) { 
    m_[i] = &data_[k]; 
    k += (i+1); 
  } 
  std::copy(lst.begin(), lst.end(), &data_[0]); 
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

template <int dim, typename T> 
SymMatrix_t<dim,T>& SymMatrix_t<dim,T>::operator=( SymMatrix_t const& rhs) 
{ 
  if (this == &rhs) return *this; 

  std::copy(&rhs.data_[0], &rhs.data_[(dim_*(dim_+1))/2], &data_[0]); 

  return *this; 
} 


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>  
SymMatrix_t<dim,T>::~SymMatrix_t()
{
  delete[] m_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>  
typename SymMatrix_t<dim,T>::SymMatrixRow_t SymMatrix_t<dim,T>::operator[](int i)
{

  return  SymMatrixRow_t( m_, i, m_[i] );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>  
typename SymMatrix_t<dim,T>::SymMatrixRow_t const  SymMatrix_t<dim,T>::operator[](int i) const
{
  return SymMatrixRow_t(m_, i, m_[i] );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

  
template <int dim, typename T>  
std::ostream& operator<<( std::ostream& os, SymMatrix_t<dim,T> const& m)
{
  using  SymMatrix::ios_binary_flag_index;
  
  if ( os.iword(ios_binary_flag_index)  == 1 ) { // binary  stream 
    std::ostream_iterator<char> it(os);
    char const* p   =  reinterpret_cast<char const*>(&m[0][0]);
    unsigned int sz = (dim*(dim+1))/2*sizeof(T);				      
    std::copy( p, p+sz, it); 
  }
  else { // ASCII stream
    std::ostream_iterator<T> it(os," "); 
    T const* p      =  &m[0][0];
    unsigned int sz = (dim*(dim+1))/2;				      
    std::copy( p, p+sz, it); 
  }
  
  return os;
}

#endif // SYMMATRIX_H
