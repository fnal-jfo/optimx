//  =================================================================
//
//  RMatrix.h
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

#ifndef RMATRIX_H
#define RMATRIX_H

#include <ostream>
#include <stdexcept>
#include <iomanip>
#include <cmath>
#include <complex>
#include <type_traits>
#include <utility>
#include <initializer_list>
		
#include <Structs.h>
#include <MatrixUtility.h>
#include <Utility.h>
#include <Vector.h>

#ifndef RMATRIXFWD_H
#define RMATRIXFWD_H
#include <RMatrixFwd.h>
#endif

template <int dim, typename T>
struct matrix_private_accessor {
  static T* data( RMatrix_t<dim,T>& matrix) {return matrix.m_;}
}; // to access private member

template <int, typename T> class RMatrix_t;

template <int dim, typename T>
std::ostream& operator<< ( std::ostream& out, RMatrix_t<dim,T> const& rhs);

template<int dim, typename T>
T symplecticNorm( Vector_t<dim,T> const& lhs);

template<int dim, typename T, typename U>
Vector_t<dim,U> operator*(  Vector_t<dim,U> const& lhs, RMatrix_t<dim,T> const& rhs  );

template<int dim, typename T, typename U>
Vector_t<dim,U>  operator*(  RMatrix_t<dim,T> const& lhs, Vector_t<dim,U> const& rhs);

template<int dim, typename T, typename U>
RMatrix_t<dim,T> operator*(  RMatrix_t<dim,T> const& lhs,  U const& rhs );

template<int dim, typename T, typename U>
RMatrix_t<dim,T> operator*(  U const& lhs,   RMatrix_t<dim,T> const& rhs  );

template<int dim, typename T>
RMatrix_t<dim,T> operator*(  RMatrix_t<dim,T> const& lhs,  RMatrix_t<dim,T> const& rhs  );


// ........................................................................................

template <int dim, typename T>  
class RMatrix_t {

  friend struct matrix_private_accessor<dim,T>;
  friend std::ostream& operator<<<> ( std::ostream& out, RMatrix_t<dim,T> const& rhs);

  struct RMatrixRow_t {

  public:

  RMatrixRow_t( T const m[] ): row_( const_cast<T*>(m)) {}; // proxy class for [][] notation

  T&        operator[](int i)       { return row_[i];} 
  T const&  operator[](int i) const { return row_[i];} 

  private:
     T* row_; 
  };
  
 public:

  RMatrix_t();

  template<typename U>
  RMatrix_t(U const& m, int n=dim);

  RMatrix_t(T m[][dim]);

  RMatrix_t(std::initializer_list<std::initializer_list<T>> lst);
  RMatrix_t(char name);

  template <int dim1, typename U>
  RMatrix_t(RMatrix_t<dim1,U> const& o);           

  static RMatrix_t  reflection(int diag[dim]);
  static RMatrix_t  m_tilt(double deg);
  static RMatrix_t  localToGlobal(double psi, double phi, double theta); // local to global coordinate transformation (rotation) 

  RMatrix_t& operator=(RMatrix_t const& rhs);

  ~RMatrix_t();
  
  RMatrixRow_t        operator[](int i);
  RMatrixRow_t const  operator[](int i) const;

  std::array<double,6> operator*( std::array<double,6> const& rhs  )      const;  // multiply by a vector  

  Vector_t<dim,T>  operator*(  T rhs[]  )              const; // kludge

  RMatrix_t      operator-(  RMatrix_t const& rhs  )   const;
  RMatrix_t      operator+(  RMatrix_t const& rhs  )   const;
  
  RMatrix_t      operator-() const;   
  
  RMatrix_t&     toUnity();
  RMatrix_t&     toZero();

  double         norm()      const;
  RMatrix_t      transpose() const; 
  RMatrix_t           similar(RMatrix_t const& U ) const; // general similarity transformation                returns U^{-1} M U   
  RMatrix_t      similarOrtho(RMatrix_t const& U ) const; // similarity transformation for orthogonal matrix  returns U^T    M U 

  RMatrix_t      tilt(double alpha) const;

  int   findEigenValues(std::complex<double> lambda[], std::complex<double> v[][dim], bool checkstab = true)    const;  // returns eigenvalues (lambda) & eigenvectors  eigenvectors are returned as ROWS !
  int   findEigenVector(std::complex<double> const& lambda,  std::complex<double> v[], bool checkstab = true)  const;  // returns eigenvector corresponding to lambda

  std::pair<Vector_t<dim,std::complex<double>>, RMatrix_t<dim, std::complex<double>>> findEigenValues() const; // returns eigenvalues (lambda) & eigenvectors  eigenvectors are returned as ROWS !

  Vector_t<dim,std::complex<double>> findEigenVector(std::complex<double> const& lambda) const;

  RMatrix_t&  symplectify();
  RMatrix_t&  balanc();
  RMatrix_t&  elmhes();
  RMatrix_t&  hqr();
  RMatrix_t   inverse() const;

  private:
  

  int      dim_; 
  T        m_[dim][dim];  //  dim x dim  square matrix

};


// ---------------
// specializations
// ---------------

template<>
RMatrix_t<3,double>  RMatrix_t<3>::localToGlobal(double psi, double phi, double theta);

template<>
RMatrix_t<6,double>  RMatrix_t<6>::localToGlobal(double psi, double phi, double theta);

template<>
RMatrix_t<6,double>  RMatrix_t<6>::m_tilt(double deg);

template<>
RMatrix_t<6,double>  RMatrix_t<6>::tilt(double alpha) const;

template<int dim>
Vector_t<dim,std::complex<double>> operator*(  Vector_t<dim,std::complex<double>> const& lhs, RMatrix_t<dim,double> const& rhs  );

template<int dim>
Vector_t<dim,std::complex<double>>  operator*(  RMatrix_t<dim,double> const& lhs, Vector_t<dim,std::complex<double>> const& rhs);

template<int dim>
RMatrix_t<dim,std::complex<double>> operator*(  RMatrix_t<dim,std::complex<double>> const& lhs, double const& rhs ); 

template<int dim>
RMatrix_t<dim,std::complex<double>> operator*(  double const& lhs, RMatrix_t<dim,std::complex<double>> const& rhs ); 

template<typename T>
struct to_unity_impl;

template<>
struct to_unity_impl<std::complex<double>>;

// ---------------
// Implementation
// ---------------

template<int dim, typename T>
  RMatrix_t<dim,T>::RMatrix_t()
  : dim_(dim)
{
  toZero();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T, typename U>
RMatrix_t<dim,T> operator*(  RMatrix_t<dim,T> const& lhs, U const& rhs ) 
{
  RMatrix_t<dim,T> result(lhs);
  
  for (int i=0; i<dim;  ++i ) {
    for (int j=0; j<dim;  ++j ) {
      result[i][j] *= rhs;
    }
  }
  return result;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T, typename U>
RMatrix_t<dim,T> operator*(  U const& lhs,   RMatrix_t<dim,T> const& rhs)
{
  return rhs*lhs;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<int dim>
RMatrix_t<dim,std::complex<double>> operator*( double const& lhs, RMatrix_t<dim,std::complex<double>> const& rhs)
{
  return rhs*lhs;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
RMatrix_t<dim,T>::RMatrix_t(char name)
  : dim_(dim)
{
  using one = typename Utility::One<T>;
  
  if (name == 'I') { toUnity(); return;} 

  toZero();
  if (name == 'J') { // symplectic matrix 
    for (int i=0; i<dim_; ++i ) {
      for (int j=0; j<dim_; ++j ) {
	if ((j   - i) ==  1 &&  (j%2) ) (*this)[i][j] =  one::value;
	if ((j   - i) == -1 && !(j%2) ) (*this)[i][j] = -one::value;
      }
    }
  }   
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
template<int dim, typename T>
template<typename U>
  RMatrix_t<dim,T>::RMatrix_t( U const& m, int n)
  :dim_(dim)
{
  using one = typename Utility::One<T>;

  if ( dim_ <=n ) {  
    for (int i=0; i<dim_; ++i) {
      for (int j=0; j<dim_; ++j) {
        (*this)[i][j] = m[i][j];
      }
    }
    return;
  }

  for (int i=0; i<n; ++i) {
      for (int j=0; j<n; ++j) {
        if ((i>= dim) || (j >= dim))  { // extend and fill diagonal with ones 
          (*this)[i][j] = (i == j ? one::value : T());
          continue;
	}
	else {
	  (*this)[i][j] = m[i][j];
        }
   }
  }
  return;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
  RMatrix_t<dim,T>::RMatrix_t( T m[][dim])
  :dim_(dim)
{
    for (int i=0; i<dim_; ++i) {
      for (int j=0; j<dim_; ++j) {
        (*this)[i][j] = m[i][j];
      }
    }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
RMatrix_t<dim,T>::RMatrix_t(std::initializer_list<std::initializer_list<T>> lst)
  : dim_(dim)
{
  int i=0;
  for (auto it=lst.begin(); it != lst.end(); ++it, ++i) {
   int j=0;
   for (auto it2=it->begin(); it2 !=it->end(); ++it2, ++j) {
     (*this)[i][j] = *it2;
    }
  }
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
RMatrix_t<dim,T>::~RMatrix_t()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
template <int dim1,typename U>
RMatrix_t<dim,T>::RMatrix_t(RMatrix_t<dim1,U> const& o)
: dim_(dim)
{
  using one = typename Utility::One<T>;
  
  if (dim_ <= dim1) {
    for (int i=0; i<dim_; ++i) {
      for (int j=0; j<dim_; ++j) {
	(*this)[i][j] = o[i][j];
      }
    }
  }
  else { // dim_ > dim1
    for (int i=0; i<dim_; ++i) {
      for (int j=0; j<dim_; ++j) {
        if ((i>= dim1) || (j >= dim1))  { // extend and fill diagonal with ones 
	   (*this)[i][j] = ((i == j) ? one::value : T()); 
	   continue;
	}
	(*this)[i][j] = o[i][j];
      }
    } 
  } // else 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
RMatrix_t<dim,T>& RMatrix_t<dim,T>::operator=(RMatrix_t const& rhs)
{
  if (&rhs == this) return *this;

  dim_ = rhs.dim_;

  for (int i=0; i<dim_; ++i) {
      for (int j=0; j<dim_; ++j) {
	(*this)[i][j] = rhs.m_[i][j];
      }
  }

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
RMatrix_t<dim,T> operator*(  RMatrix_t<dim,T> const& lhs,  RMatrix_t<dim,T> const& rhs  )
{
  RMatrix_t<dim,T> mi;

   for(int i=0; i<dim; ++i) {
        for(int j=0; j< dim; ++j) {
         mi[i][j] = T();
	 for(int k=0; k< dim; ++k) {
	   mi[i][j] += lhs[i][k]* rhs[k][j];
         }		
       }
    }

   return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
    RMatrix_t<dim,T>& RMatrix_t<dim,T>::toUnity()
{

  using one = typename Utility::One<T>;
  
  for (int i=0; i<dim_; ++i) {
    for (int j=0; j<dim_; ++j) {
      (*this)[i][j] = T();
     }
   }
   for (int i=0; i<dim_; ++i) { 
     (*this)[i][i] = one::value;
   }

    return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim,typename T>
RMatrix_t<dim,T>& RMatrix_t<dim,T>::toZero()
{
    for (int i=0; i<dim_; ++i) {
      for (int j=0; j<dim_; ++j) {
	(*this)[i][j] = T();
      }
    }
    return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <int dim, typename T>
  typename RMatrix_t<dim,T>::RMatrixRow_t RMatrix_t<dim,T>::operator[](int i)
{
  return RMatrix_t::RMatrixRow_t( m_[i] );
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <int dim, typename T>
typename RMatrix_t<dim,T>::RMatrixRow_t const RMatrix_t<dim,T>::operator[](int i) const
{
  return RMatrix_t::RMatrixRow_t( m_[i] );
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
double RMatrix_t<dim,T>::norm() const
{
  // Frobenius norm
  
  double nrm = 0.0;
  for (int i=0; i<dim; ++i ) {
     for (int j=0; j<dim; ++j ) {
       nrm = std::abs( (*this)[i][j]);
       nrm *= nrm;
     }
  }
  return sqrt(nrm);
}	 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T>
std::array<double,6> RMatrix_t<dim, T>::operator*( std::array<double,6> const&  rhs ) const  // multiply by a vector  
{
  std::array<double,6> c; 

  for(int i=0; i < dim_; ++i) {
    c[i] = 0.0;
    for(int j=0; j< dim_; ++j) {
      c[i]  += m_[i][j]*rhs[j];
    }		
  }

  return c;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
RMatrix_t<dim,T> RMatrix_t<dim,T>::transpose() const  
{

  RMatrix_t<dim,T> ttm;

  for(int i=0; i < dim_; ++i) {
    for(int j=0; j< dim_; ++j) {
     ttm[i][j] = (*this)[j][i];
    }		
  }

  return ttm;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <int dim, typename T>
RMatrix_t<dim,T> RMatrix_t<dim,T>::similarOrtho(RMatrix_t<dim,T> const& p ) const
{
  return p.transpose()*(*this)*p;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
RMatrix_t<dim,T> RMatrix_t<dim,T>::similar(RMatrix_t<dim,T> const& p ) const
{
  char msg[256];
  auto res = p.inverse()*(*this)*p;
  return res;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
RMatrix_t<dim,T> RMatrix_t<dim,T>::reflection( int* diag )  // reflection matrix  
{

  RMatrix_t<dim,T> ttm;
  
  for(int i=0; i < dim; ++i) {
     ttm[i][i] = diag[i]; 
  }  

  return ttm;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
RMatrix_t<dim,T> RMatrix_t<dim,T>::operator+(RMatrix_t<dim,T> const& rhs ) const  
{

  RMatrix_t<dim,T> ttm;

  for(int i=0; i < dim_; ++i) {
    for(int j=0; j< dim_; ++j) {
     ttm[i][j] = (*this)[i][j] + rhs[i][j];
    }		
  }

  return ttm;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
RMatrix_t<dim,T> RMatrix_t<dim,T>::operator-(RMatrix_t<dim,T> const& rhs) const  
{
  return (*this) + (-rhs);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <int dim, typename T>
  RMatrix_t<dim,T> RMatrix_t<dim,T>::operator-() const // unary minus
{

  RMatrix_t<dim,T> ttm;

  for (int i=0; i < dim_; ++i) {
    for (int j=0; j< dim_; ++j) {
     ttm[i][j] = -(*this)[i][j];
    }		
  }
  return ttm;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <int dim, typename T>
std::ostream& operator<< ( std::ostream& out, RMatrix_t<dim,T> const& rhs)
{

    out << "-----------------" << std::endl;
    for (int i=0; i<dim; ++i) {
      for (int j=0; j<dim; ++j) {
	out << std::setw(16) << rhs[i][j] << "  ";
       }
       out << std::endl;
    }

    out << "-----------------" << std::endl;

    return out;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
RMatrix_t<dim,T>&  RMatrix_t<dim,T>::symplectify()
{
  // Symplectify the matrix using Healy's algorithm 

  RMatrix_t<dim,T> S('J');  // symplectic matrix  
  RMatrix_t<dim,T> I('I');  // identity matrix  

  //............................................................
  // The algorithm below obviously fails if det( I + M) = 0
  // In that case, we do nothing (and possibly issue a warning)
  //.............................................................

  std::complex<double> lambda[dim];
  std::complex<double> v[dim][dim];
  (I + (*this)).findEigenValues(lambda,v);

  for (int i=0; i<dim; ++i ) {
    if ( std::abs(lambda[i]) < 1.0e-12 ) { return *this;}  
  }

  // Ok, proceed with symplectification
 
  RMatrix W  = S * ( I + (*this) ).inverse() * (I - (*this) );
  RMatrix W1 = 0.5 * ( W + W.transpose());
  RMatrix M1 = (I + S*W1)*(I - S*W1).inverse();

  *this = M1;
  return *this; 
}    

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
RMatrix_t<dim,T>&  RMatrix_t<dim,T>::balanc()
{
  Utility::balanc(m_,dim);
  return *this;
}    

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <int dim, typename T>
RMatrix_t<dim,T>&  RMatrix_t<dim,T>::elmhes()
{
  Utility::elmhes(m_,dim);
  return *this;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
RMatrix_t<dim,T>&  RMatrix_t<dim,T>::hqr()
{
  std::complex<double> wri[dim];
  char errmsg[256];
  errmsg[0] = 0;
  Utility::hqr(m_, dim, wri, errmsg);
  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
RMatrix_t<dim, T>  RMatrix_t<dim,T>::inverse() const
{
  // NOTE: matr_invert modifies its second arg,
  //       we make a mutable tmp copy (mtmp) and operate on it
  char errmsg[128]; errmsg[0]=0;
  RMatrix_t<dim,T> minv;
  RMatrix_t<dim,T> mtmp(*this);
  Utility::matr_invert(dim, mtmp, minv, errmsg); // minv = inverse matrix

  if(errmsg[0]) {
     throw std::runtime_error(errmsg);
  }

  return minv; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
int RMatrix_t<dim,T>::findEigenVector(std::complex<double> const& lambda, std::complex<double> v[], bool checkstab) const
{

  using Utility::matr_invert;
  using one = typename Utility::One<T>;

  static constexpr std::complex<double> I(0.0,1.0);         // imaginary i
  static const RMatrix_t<dim,std::complex<double>> Uc('J'); // symplectic matrix

  double const V_ACCURACY = 1.e-6;  // FIXME

  // converts the matrix to a complex if its elements are real 
  // otherwise, this just performs a copy 

  RMatrix_t<dim,std::complex<double>> m(*this);
   

  RMatrix_t<dim, std::complex<double>> minv; // will be holding the actual inverse

  char err[256];
  int ret=0;

  //......................................................................................
  // Use the inverse iteration method to find the eigenvector of the matrix M corresponding
  // to the eigenvalue lambda. Assuming lambda is the exact eigenvalue,
  // let mu = (lambda + delta)  be a close approximation to the eigenvalue. 
  // The eigenvalue gamma of (M - I mu) is the non-trivial solution of
  // ((M -I mu) -I gamma)  x = 0
  // (M - I lambda -I delta -Igamma ) x = 0
  // But, by definition, (M - I lambda) x = 0, so gamma = delta i.e. the eigenvalue of (M - Imu) is delta.
  // The matrix M - Imu is invertible since mu is not an exact eigenvalue of M. 
  // The eigenvalue of the inverse matrix is 1/delta. 
  // Principle of the inverse method: start from a generic vector and apply the
  // inverse matrix iteratively. The component along the dominant eigenvector
  // grows by a factot 1/delta at each step. 

  std::complex<double> constexpr delta   = {1.0e-8,0.0};
  int                  constexpr maxiter = 5;

  // NOTE: we use a relative delta value. 
  for(int i=0; i<dim; ++i) {m[i][i] -= (lambda*(1.0 + delta) ); }   // (M - I*lambda) x = 0
                                                                    // (M - I*lambda - I*delta) x = 0

  err[0]=0;
  matr_invert(dim, m, minv, err); // minv = inverse matrix  v =  (M - I*lambda(1+delta))^{-1}

  if(err[0]) {
    throw std::runtime_error(err);
     return 1;
  }

  for(int i=0; i<dim; ++i) {
    for( int j=0; j<dim; ++j) {
      minv[i][j] *= std::abs(delta*lambda);
    }
  }

  //........................................................................................
  // starting with v (1,1,1,1) (a random linear combination of all eigenvectors)
  // recursively: compute v_i+1 = M v_i and normalize  
  // the vector that is left is the (normalized) eigenvector corresponding to the eigenvalue  
  //.........................................................................................
  
  Vector_t<dim,std::complex<double>> vtmp;
  for (int i=0; i<dim; ++i) { vtmp[i] = one::value;};  

  for (int i=0; i<maxiter; ++i) {
    vtmp  = minv * vtmp;

    // normalize
    auto c = (conj(vtmp)*Uc*vtmp); 
    ret |= (( fabs(c.imag()) > 1.0e-20 ) ? 0 : 1); // the squared norm of an eigenvector is pure imaginary
                                                   // so the imaginary part should never be zero
    vtmp  *= sqrt((2.0*I)/c); // normalize 

  };      

  // checkfor stability 
  if (checkstab) { 
     for (int i=0; i<dim; ++i) {
       ret |=  (fabs(std::abs(lambda)-1.0) > 1.0e-6) ? 2 : 0; // stability implies that the norm of the eigenvalue is < 1.0  
     }   
  }
  
  // copy eigenvector for return
  // WARNING: the eigenvector is returned through argument v !
  for (int i=0; i<dim; ++i) { v[i] = vtmp[i];}   
 
  return ret; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
Vector_t<dim,T> RMatrix_t<dim,T>::operator*(  T rhs[]  ) const
{
  Vector_t<dim,T> result;
  for (int i=0; i<dim; ++i) {
     for (int j=0; j<dim; ++j) {
       result[i] = (*this)[i][j] * rhs[j];
     }
  }
  return result;  
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T>
int RMatrix_t<dim,T>::findEigenValues(std::complex<double> lambda[], std::complex<double> v[][dim], bool checkstab) const
{ 

  double const V_ACCURACY = 10.e-8; 

  auto const& tm = *this;

  using Utility::balanc;
  using Utility::elmhes;
  using Utility::hqr;

  T m[dim][dim];
   
  std::complex<double> wri[dim];
  std::complex<double> la2[dim];
  std::complex<double>  v2[dim][dim];

  int used[dim];
  char err[256];

  for (int i=0; i<dim; ++i)    used[i] = 0;

  for (int i=0; i<dim; ++i) {
    for(int j=0; j<dim; ++j) {
      m[i][j] = tm[i][j];
    }
  }

   // Find eigenvalues 

   balanc(m, dim);
   elmhes(m, dim);
   err[0] = 0;
   hqr(m,dim,wri,err);
 
  if(err[0]){
    throw std::runtime_error(err);  //OptimWarningMessage(this, "4D view", err, QMessageBox::Ok);
    return 1;
  }

  // Find eigenvectors 

  for(int i=0; i<dim; ++i) { la2[i] = wri[i]; }
  int ret =0;
  for(int i=0; i<dim; ++i) {  ret |=  tm.findEigenVector(la2[i], v2[i], checkstab);   } // ***NOTE**: v2[i] is a ROW of v2, not a column!

  if(ret) {
    for(int i=0; i<dim; ++i){
       lambda[i] = la2[i];
       for(int k=0; k<dim; ++k) {v[i][k]=v2[i][k];}
    }
    return 1;
   }
  
 
 // ... sorting  does not belong here  FIXME ...
 // sort eigen-values and eigen-vectors. Result - (v1, v1*, v2, v2*)
 // condition for v1: (1 - u) ~ -imag(v2[i][1]/v2[i][0]) < 0.;
 // pointer to the first x vector

  int pxy[] = {0,1,2,3,4,5};
  
  double d = std::abs(v2[0][0]);

  // find eigenvector with the largest x component 

  int j=0;
   for (int i=0; i<dim; ++i) {
     if ( std::abs(v2[i][0]) > d ) {
        d = std::abs(v2[i][0]);
	j = i;
     }
   }
   pxy[0] = j;
   
   // determine the second x vector (conjugate)

   for (int i=0; i<4; ++i) {
     if(   (fabs( std::abs(v2[i][0]) - std::abs(v2[pxy[0]][0]) ) <d ) && (i !=pxy[0]) ){
        d = fabs( std::abs(v2[i][0]) - std::abs(v2[pxy[0]][0]));
        j = i;
     }
   }
   pxy[1] = j;

   // rotation in imaginary space for x-vector

   for(int i=0; i<2; ++i) {
     auto s = std::abs(v2[pxy[i]][0])/v2[pxy[i]][0];
     for(int j=0; j<dim; ++j) {
       v2[pxy[i]][j] *= s;
     }  
   }

   //sort x vectors according to sign of imag part

   if(imag(v2[pxy[0]][1]) >0.0 ) {
     j      = pxy[0];
     pxy[0] = pxy[1];
     pxy[1] = j;
   }

   //pointers to the y vectors

   j=2;
   for (int i=0; i<4; ++i) {
     if( (i!=pxy[0]) && (i!=pxy[1]) ) {
       pxy[j] =i;
       j++;
     }
   }

   //rotation in imaginary space for y-vectors
   for(int i=2; i<4; ++i) {
   	auto s = abs(v2[pxy[i]][2])/v2[pxy[i]][2];
   	for(int j=0; j<dim; ++j) {
	  v2[pxy[i]][j] *= s;
	}
   }
   //sort y-vectors

   if(imag(v2[pxy[2]][3])>0.0){
     j      = pxy[2];
     pxy[2] = pxy[3];
     pxy[3] = j;
   }

   //Copy

   for(int i=0; i<dim; ++i) {
      lambda[i] = la2[pxy[i]];
      for( int k=0; k<dim; ++k) {
	v[i][k] = v2[pxy[i]][k];
      }
   }

   for (int i=0; i<dim; ++i) { 
     Vector_t<dim,std::complex<double>> vtmp(v[i]);
    }

   return 0;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int n, typename T, template<int, typename U> typename RMatrix_t>
void balanc( RMatrix_t<n,T> const& a)
{

  double const RADIX = std::numeric_limits<double>::radix;
  
  bool    done = false;
  double sqrdx = RADIX*RADIX;
  
  while (!done) {
   done  = true;

   for (int i=0; i<n; ++i) {

     double r = 0.0;
     double c = 0.0;

      for (int j=0;j<n;++j)
       if (j != i) {
	 c += std::abs(a[j][i]);
	 r += std::abs(a[i][j]);
       }
     if ( (c != 0.0) && (r != 0.0) ) {
       double g = r/RADIX;
       double f = 1.0;
       double s = c+r;
       while (c<g) {
	 f *= RADIX;
	 c *= sqrdx;
       }
       g=r*RADIX;
       while (c>g) {
	 f /= RADIX;
	 c /= sqrdx;
       }
       if ((c+r)/f < 0.95*s) {
	 done = false;;
	 g = 1.0/f;

	 //scale[i] *= f; // this is present in latest versions of this routine 

         for (int j=0; j<n; ++j) a[i][j] *= g;
         for (int j=0; j<n; ++j) a[j][i] *= f;

       }
     }
   }
 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <int dim, typename T, typename U>
Vector_t<dim, U>  operator*(  RMatrix_t<dim,T> const& lhs, Vector_t<dim, U> const& rhs ) 
{
  Vector_t<dim,U> result;
  result.toZero();
  
  for (int i=0; i<dim; ++i) {
    for (int j=0; j<dim; ++j) {

      result[i] += lhs[i][j]*rhs[j];  
      
    }
  }

  return result;
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim>
Vector_t<dim,std::complex<double>>  operator*(  RMatrix_t<dim,double> const& lhs, Vector_t<dim,std::complex<double>> const& rhs)
{
  Vector_t<dim,std::complex<double>> result;
  result.toZero();
  
  for (int i=0; i<dim; ++i) {
    for (int j=0; j<dim; ++j) {

      result[i] += std::complex<double>(lhs[i][j],0.0)*rhs[j];  
      
    }
  }

  return result;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim>
Vector_t<dim,std::complex<double>>  operator*(  RMatrix_t<dim,std::complex<double>> const& lhs, Vector_t<dim,double> const& rhs)
{
  Vector_t<dim,std::complex<double>> result;
  result.toZero();
  
  for (int i=0; i<dim; ++i) {
    for (int j=0; j<dim; ++j) {

      result[i] += lhs[i][j]*std::complex<double>(rhs[j], 0.0);  
      
    }
  }

  return result;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim, typename T, typename U>
Vector_t<dim,U> operator*(  Vector_t<dim,U> const& lhs, RMatrix_t<dim,T> const& rhs  )
{
  
  // left vector matrix product

  Vector_t<dim,U> result;
  result.toZero();
  
  for (int j=0; j<dim; ++j) {
    for (int i=0; i<dim; ++i) {
      result[j] += lhs[i]*rhs[i][j];  
    }
  }

  return result;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim>
Vector_t<dim,std::complex<double>> operator*(  Vector_t<dim,std::complex<double>> const& lhs, RMatrix_t<dim,double> const& rhs  )
{

  Vector_t<dim,std::complex<double>> result;
  result.toZero();

  for (int j=0; j<dim; ++j) {
    for (int i=0; i<dim; ++i) {
      result[j] += lhs[i]* std::complex<double>(rhs[i][j], 0.0);  
    }
  }

  return result;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<int dim>
Vector_t<dim,std::complex<double>> operator*(  Vector_t<dim,double> const& lhs, RMatrix_t<dim,std::complex<double>> const& rhs  )
{

  Vector_t<dim,std::complex<double>> result;
  result.toZero();

  for (int j=0; j<dim; ++j) {
    for (int i=0; i<dim; ++i) {
      result[j] += std::complex<double>(lhs[i], 0.0)* rhs[i][j];  
    }
  }

  return result;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#endif // RMatrix_H
