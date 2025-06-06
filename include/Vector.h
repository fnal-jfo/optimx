//  =================================================================
//
//  Vector.h
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

#ifndef VECTOR_H
#define VECTOR_H

#include <initializer_list>
#include <iostream>
#include <iomanip>
#include <complex>

template <typename T, typename U> 
T convertTo( U value)
{  
  T retvalue = value;
  return  retvalue;
}

template<> 
std::complex<double> convertTo<std::complex<double>,double>( double value);

// ............................................................................

template <int, typename T> class Vector_t;

template <int dim, typename T>
std::ostream& operator<< ( std::ostream& out, Vector_t<dim,T> const& rhs);

template<int dim, typename T>
T operator*( Vector_t<dim,T> const& lhs, Vector_t<dim,T> const& rhs  );

template<int dim, typename T>
Vector_t<dim,T> operator*(  Vector_t<dim,T> const& lhs, T const& rhs );

template<int dim, typename T>
Vector_t<dim,T> operator*(  T const& lhs, Vector_t<dim,T> const& rhs  );

template<int dim, typename T>
Vector_t<dim,T> operator/(  Vector_t<dim,T> const& lhs, T const& rhs );

template<int dim, typename T>
Vector_t<dim,T> operator/(  T const& lhs, Vector_t<dim,T> const& rhs  );

template<int dim, typename T>
Vector_t<dim,std::complex<double>> conj( Vector_t<dim,std::complex<double>> const& arg);


template <int dim, typename T=double>  
class Vector_t {

  friend std::ostream& operator<<<> ( std::ostream& out, Vector_t<dim,T> const& rhs);

  friend T operator*<>( Vector_t<dim,T> const& lhs, Vector_t<dim,T> const& rhs  );

  friend Vector_t<dim,T> operator*<>(  Vector_t<dim,T> const& lhs, T const& rhs );

  friend Vector_t<dim,T> operator*<>(  T const& lhs,   Vector_t<dim,T> const& rhs  );

  friend Vector_t<dim,T> operator/<>(  Vector_t<dim,T> const& lhs, T const& rhs );

  friend Vector_t<dim,T> operator/<>(  T const& lhs,   Vector_t<dim,T> const& rhs  );

 public:

  Vector_t();

  template <typename U>
  Vector_t(U* m, int n=dim);

  template <typename U>
  Vector_t(std::initializer_list<U> l );

  template <int dim1, typename U>
  Vector_t(Vector_t<dim1,U> const& o); // converter ??          

  Vector_t& operator=(Vector_t const& rhs);

  ~Vector_t();
  
  T &           operator[](int i);
  T const&      operator[](int i) const;

  Vector_t      operator-(  Vector_t const& rhs  )   const;
  Vector_t      operator+(  Vector_t const& rhs  )   const;
  Vector_t&     operator/=(T val);
  Vector_t&     operator*=(T val);
  Vector_t&     normalize();
  
  Vector_t      operator-() const;   
  
  Vector_t&    toUnity();
  Vector_t&    toZero();

  double        norm()      const;
  int           size()      const { return dim_;}

  private:
  

  int      dim_; 
  T     v_[dim]; 

};


// ---------------
// Specializations
// ---------------

template<int dim>
Vector_t<dim,std::complex<double>> conj( Vector_t<dim,std::complex<double>> const& arg);


// ---------------
// Implementation
// ---------------

template<int dim, typename T>
  Vector_t<dim,T>::Vector_t()
  : dim_(dim)
{
  toZero();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
Vector_t<dim,T> operator*(  Vector_t<dim,T> const& lhs, T const& rhs ) 
{
  Vector_t<dim,T> result(lhs);
  
  for (int i=0; i<dim;  ++i ) {
      result.v_[i] *= rhs;
  }

  return result;
}  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
Vector_t<dim,T> operator*(  T const& lhs,   Vector_t<dim,T> const& rhs)
{
  return rhs*lhs;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
Vector_t<dim,T> operator/(  Vector_t<dim,T> const& lhs, T const& rhs ) 
{
  Vector_t<dim,T> result(lhs);
  
  for (int i=0; i<dim;  ++i ) {
      result.v_[i] /= rhs;
  }

  return result;
}  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
Vector_t<dim,T> operator/(  T const& lhs,   Vector_t<dim,T> const& rhs)
{
  return rhs/lhs;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
template <typename U>
Vector_t<dim,T>::Vector_t( U* v, int n)
  : dim_(dim)
{
  for (int i=0; i<dim_; ++i) {
    v_[i] = convertTo<T>(v[i]);
  }
  if ( n > dim_ )  return;
  
  for (int i=dim_; i<n; ++i) {
    v_[i] = convertTo<T>(U()); // fill remaining entries with "zeros".
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
template<typename U>
Vector_t<dim,T>::Vector_t(std::initializer_list<U> lst )
: dim_(dim)
{
  int i=0;
  for( auto it = lst.begin(); it != lst.end(); ++it, ++i) {
    if (i == dim) continue; 
    v_[i] = convertTo<T>(*it);
  }
  
  if ( i > dim)  return;

  for (int j=i+1; j<dim; ++j) {
    v_[j] = convertTo<T>(U()); // fill remaining entries with "zeros".
  }  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <int dim, typename T>
Vector_t<dim,T>::~Vector_t()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
template<int dim1, typename U>
Vector_t<dim,T>::Vector_t(Vector_t<dim1,U> const& o)
: dim_(dim)
{
  if (dim1 <= dim) {
    for (int i=0; i<dim1; ++i) {
      v_[i] = convertTo<T,U>(o[i]);
    }
    for (int i=dim1; i<dim; ++i) {
      v_[i] =  convertTo<T,U>(o[i]);
    }
  }
  else {
    for (int i=0; i<dim_; ++i) {
      v_[i] = convertTo<T,U>(o[i]);
    }
  }
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
Vector_t<dim,T>& Vector_t<dim,T>::operator=(Vector_t const& rhs)
{
  if (&rhs == this) return *this;

  dim_ = rhs.dim_;

  for (int i=0; i<dim_; ++i) {
	v_[i] = rhs.v_[i];
  }

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <int dim,typename T>
T& Vector_t<dim,T>::operator[](int idx)
{
    return v_[idx];
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim,typename T>
T const& Vector_t<dim,T>::operator[](int idx) const
{
    return v_[idx];
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
template <int dim,typename T>
Vector_t<dim,T>& Vector_t<dim,T>::toZero()
{
    for (int i=0; i<dim_; ++i) {
      v_[i] = T();
    }
    return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <int dim, typename T>
double Vector_t<dim,T>::norm() const
{
  // FIXME: does nort work for complex vector
  double nrm = 0.0;
  for (int i=0; i<dim; ++i ) {
    nrm = v_[i]*v_[i];
  }

  return sqrt(nrm);
}	 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
Vector_t<dim,T> Vector_t<dim,T>::operator+(Vector_t<dim,T> const& rhs) const  
{
  Vector_t<dim,T> lhs;

  for (int i=0; i < dim_; ++i) {
     lhs.v_[i] = v_[i] + rhs.v_[i];
  }		
  return lhs;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
Vector_t<dim,T>&  Vector_t<dim,T>::operator/=(T val)
{
  for (int i=0; i<dim; ++i) {
    (*this)[i] /=val;
  }    
  return *this;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
Vector_t<dim,T>&  Vector_t<dim,T>::operator*=(T val)
{
  for (int i=0; i<dim; ++i) {
    (*this)[i] *=val;
  }
  return *this;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
Vector_t<dim,T> Vector_t<dim,T>::operator-(Vector_t<dim,T> const& rhs) const  
{

  Vector_t<dim,T> lhs;

  for (int i=0; i < dim_; ++i) {
     lhs.v_[i] = v_[i] - rhs.v_[i];
    }		

  return lhs;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
Vector_t<dim,T> Vector_t<dim,T>::operator-() const
{

  Vector_t<dim,T> lhs;

  for (int i=0; i < dim_; ++i) {
     lhs.v_[i] = -v_[i];
   }		
  return lhs;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
std::ostream& operator<< ( std::ostream& out, Vector_t<dim,T> const& rhs)
{

    out << "-----------------" << std::endl;
    for (int i=0; i<dim; ++i) {
	out << std::setw(16) << rhs[i] << "  ";
       out << std::endl;
    }

    out << "-----------------" << std::endl;

    return out;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
T operator*( Vector_t<dim,T> const& lhs, Vector_t<dim,T> const& rhs  )
{
  T result = T();

  for (int i=0; i<dim; ++i) {
    result += lhs[i]*rhs[i];
  }

  return result;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<int dim>
Vector_t<dim,std::complex<double>> conj( Vector_t<dim,std::complex<double>> const& arg)
{
  Vector_t<dim,std::complex<double>> result;
  for (int i=0; i<dim; ++i) {
    result[i] = std::conj(arg[i]);
  }
  return result;  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
Vector_t<dim,T> conj( Vector_t<dim,T> const& arg)
{
  return Vector_t<dim,T>(arg); // conjugaison does nothing on non-complex. Return a copy of the argument.  

}


#endif // VECTOR_H
