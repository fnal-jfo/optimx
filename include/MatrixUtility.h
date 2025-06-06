//  =================================================================
//
//  MatrixUtility.h
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

#ifndef MATRIXUTILITY_H
#define MATRIXUTILITY_H

#include <cstring>
#include <limits>
#include <type_traits>

namespace Utility {

  template <typename U>
    struct One { static constexpr U value = U(1.0);};

  template <>
  struct One<std::complex<double>> {static constexpr std::complex<double> value = {1.0,0.0};};    

  template <>
  struct One<double> {static constexpr double value = 1.0;};   

  // ...................................................................
  
  template <typename Matrix_t>
  void    hqr(Matrix_t& a, int n, std::complex<double> wri[], char* err);

  template <typename Matrix_t>
  void    balanc( Matrix_t& a, int n);
 
  template <typename Matrix_t> 
  void    elmhes(Matrix_t& a, int n);

  template < class Matrix_t >
  void matr_invert(int n, Matrix_t& m, Matrix_t& minv, char *err);

// .................................................................
// implementations
//..................................................................

template  < typename Matrix_t >
void ludcmp( Matrix_t& a, int n, int* indx, double& d, char* err)
{
  using std::abs;
  
  static double const tiny = std::numeric_limits<double>::epsilon(); 
  using T = typename std::decay<decltype( a[0][0] )>::type;

  int i,imax,j,k;

  T big;
  T temp;

  std::vector<T> vv(n);
  
  d = 1.0;
  for (i=0;i<n; ++i) {
    big = 0.0;
    for (j=0;j<n;++j) {
      temp = a[i][j]; 
      big = ( abs(temp) > abs(big) ) ? temp : big;
    }
    if (abs(big) == 0.0) {
      strcat(err,"Singular matrix in routine ludcmp. ");
      return;
    }
    vv[i]=1.0/big;
  }


  for (k=0; k<n; ++k) {
    big = 0.0;
    for (i=k; i<n; ++i) {
      temp = vv[i] * abs(a[i][k]);
      if (abs(temp) > abs(big) ) {
	big = temp;
	imax=i;
      }
    }

    if (k != imax) {
      for (j=0; j<n; ++j) {
	temp = a[imax][j];
	a[imax][j]= a[k][j];
	a[k][j]=temp;
      }

      d = -d;
      vv[imax]=vv[k];
    }

    indx[k] = imax;
  
    if ( abs(a[k][k]) < tiny) a[k][k] = tiny;

    for (i =k+1;i<n;++i) { 
      temp = (a[i][k] /= a[k][k]);
      for (j=k+1; j<n; ++j) a[i][j] -= temp*a[k][j];     
    }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template  <typename T, typename Matrix_t >
void lubksb( Matrix_t& a, int n, int* indx, T* b)
{
  int i,ii=0,ip,j;
  T sum;

  for (i=0; i<n; ++i) {
    ip    = indx[i];
    sum   = b[ip];
    b[ip] = b[i];
    if (ii != 0 ) { 
      for (j =ii-1; j<i; ++j) sum -= a[i][j]*b[j];
    }
    else if (sum != T(0.0)) {
      ii=i+1;
    }
    b[i]=sum;
  }

  for (i=n-1; i>=0; --i) {
    sum = b[i];
    for (j=i+1; j<n; ++j) sum -= a[i][j]*b[j];
    b[i]=sum/a[i][i];
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename Matrix_t >
void matr_invert(int n, Matrix_t& m, Matrix_t& minv, char *err)
{
  
  double d = 0.0;
  using T = typename std::decay<decltype( m[0][0] )>::type;
  
  std::vector<int> indx(n);
  std::vector<T>   col (n);

  ludcmp(m, n, &indx[0], d, err);

   for( int j=0; j<n; ++j){
     for(int i=0; i<n; ++i) { 
       col[i] = T();
     }
     col[j] = One<T>::value;
     lubksb(m, n, &indx[0], &col[0]);
     for(int i=0; i<n; ++i) { 
       minv[i][j] = col[i];
     }
   }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Matrix_t> 
void elmhes(Matrix_t& a, int n)
{
  //------------------------------------------------------------------------------------ 
  // reduction to Hessenberg form by elimination method. Replaces the real non-symmetric
  // matrix a[0..n-1][0..n-a] by an upper Hessenberg matrix with identical eigenvalues.
  // Recommended, but not required: this routine should be preceeded by balance().   
  //------------------------------------------------------------------------------------
  
  for (int m=1; m<n-1; ++m) {

    auto  x = a[0][0] * 0.0; // so that the function works for complex arrays   

    int i = m;
    for (int j=m;j<n; ++j) {
      if(std::abs(a[j][m-1]) > std::abs(x)) {
	x = a[j][m-1];
	i = j;
      }
    }

    // perm[m] = i;

    if (i != m) {
      for (int j=m-1;j<n; ++j) std::swap(a[i][j],a[m][j]);
      for (int j=0;  j<n; ++j) std::swap(a[j][i],a[j][m]);
    }
    if ( std::abs(x) != 0.0) {
      for (int i=m+1;i<n; ++i) {
        auto y =a[i][m-1];
        if ( std::abs(y) != 0.0) {
   	  y /= x;
	  a[i][m-1]=y;
	  for (int j=m; j<n; ++j) a[i][j] -= y*a[m][j];
	  for (int j=0; j<n; ++j) a[j][m] += y*a[j][i];
	 }
       }
    }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename Matrix_t> 
void balanc( Matrix_t& a, int n)
{
  //--------------------------------------------------
  // Balancing of matrix for achieving better accuracy
  //--------------------------------------------------

  const double RADIX = std::numeric_limits<double>::radix;
  
  bool    done = false;
  double sqrdx = RADIX*RADIX;
  
  while (!done) {
   done  = true;

   for (int i=0; i<n; ++i) {
     double r = 0.0;
     double c = 0.0;
     for (int j=0; j<n; ++j)
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

template <typename Matrix_t> 
void hqr(Matrix_t& a, int n, std::complex<double> wri[], char* err)
{
  //--------------------------------------------------------------------
  // Finds all eigen-values of an upper Hessenberg matrix a[0..n-1][0..n-1].
  // wr[0..n-1] and wi[0..n-1] are arrays which will hold the result.
  // err[0] = 0 if no errors, string with error caption otherwise.
  //--------------------------------------------------------------------

  using std::abs;

  int nn,m,l,k,j,its,i,mmin;

  auto   z = 0.0*a[0][0];
  auto   y = 0.0*a[0][0];
  double x = 0.0;
  auto   w = 0.0*a[0][0];
  double v = 0.0;
  double u = 0.0;
  double t = 0.0;
  auto   s = 0.0*a[0][0];
  auto   r = 0.0*a[0][0];
  auto   q = 0.0*a[0][0];
  auto   p = 0.0*a[0][0];

  double anorm = 0.0;
  double const EPS = std::numeric_limits<double>::epsilon();
  
  for (i=0; i<n; ++i)
    for ( j = std::max((i-1), 0); j<n; ++j)
     anorm += abs(a[i][j]);
     nn     = n-1;
     t      = 0.0;
     while (nn >= 0) {
       its = 0;
       do {
	  for ( l=nn; l>0; l--) {
	    s = abs(a[l-1][l-1])+abs(a[l][l]);
	    if (s == 0.0) s=anorm;
	    if ( abs(a[l][l-1]) < EPS*s ) {
	      a[l][l-1] = 0.0;
	      break;
	    }
	  }
	  x =a[nn][nn];
	  if (l == nn) {
             wri[nn--] = x+t;
	   } 
           else {
	     y = a[nn-1][nn-1];
	     w = a[nn][nn-1]*a[nn-1][nn];
	     if (l == (nn-1)) {
	       p = 0.5*(y-x);
	       q = p*p+w;
	       z = sqrt(abs(q));
	       x += t;
	       if (q >= 0.0) {
		 z        = p+copysign(z,p);
		 wri[nn-1] = wri[nn]=x+z;
		 if (z) wri[nn]=x-w/z;
	       } 
               else {
		 wri[nn]   = std::complex<double>(x+p,-z);
		 wri[nn-1] = std::conj(wri[nn]);
	       }
	       nn -= 2;
	     } 
             else {
	       if (its == 100 ){strcat(err,"Too many iterations in function hqr during eigenvalue computation. "); return;
             }
	     
             if (its == 10 || its == 20) {
	       t += x;
	       for (i=0; i<nn+1; ++i) a[i][i] -= x;
	       s = abs(a[nn][nn-1]) +abs(a[nn-1][nn-2]);
	       y  = x = 0.75*s;
	       w  = -0.4375*s*s;
	     }
	     ++its;
	     for (m=(nn-2);m>=l; --m) {
	       z = a[m][m];
	       r = x-z;
	       s = y-z;
	       p = (r*s-w)/a[m+1][m]+a[m][m+1];
	       q = a[m+1][m+1]-z-r-s;
	       r = a[m+2][m+1];
	       s = abs(p)+abs(q)+abs(r);
	       p /= s;
	       q /= s;
	       r /= s;
	       if (m == l) break;
	       u = abs(a[m][m-1])*(abs(q)+abs(r));
	       v = abs(p)*(abs(a[m-1][m-1])+abs(z)+abs(a[m+1][m+1]));
	       if ( u <= EPS*v ) break;
	     }
	     for (i=m; i<nn-1; ++i) {
	       a[i+2][i] = 0.0;
	       if (i != m) a[i+2][i-1] = 0.0;
	     }
	     for (k=m; k<nn; ++k) {
	       if (k != m) {
		 p=a[k][k-1];
		 q=a[k+1][k-1];
		 r=0.0;
		 if (k+1 != nn ) r=a[k+2][k-1];
		 if ((x=abs(p)+abs(q)+abs(r)) != 0.0) {
		   p /= x;
		   q /= x;
		   r /= x;
		 }
	       }
	       if ( abs(s=copysign(sqrt(p*p+q*q+r*r),p)) != 0.0) {
		 if (k == m) {
		   if (l != m)
		     a[k][k-1] = -a[k][k-1];
		   } 
                   else
		     a[k][k-1] = -s*x;
		   p += s;
		   x = p/s;
		   y = q/s;
		   z = r/s;
		   q /= p;
		   r /= p;
		   for (j=k;j<n; ++j) {
		     p=a[k][j]+q*a[k+1][j];
		     if (k+1 != nn) {
		       p += r*a[k+2][j];
		       a[k+2][j] -= p*z;
		     }
		     a[k+1][j] -= p*y;
		     a[k][j] -= p*x;
		   }
		   mmin = nn<k+3 ? nn : k+3;
		   for (i=0;i<mmin+1; ++i) {
		     p=x*a[i][k]+y*a[i][k+1];
		     if (k != (nn-1)) {
		       p += z*a[i][k+2];
		       a[i][k+2] -= p*r;
		     }
		     a[i][k+1] -= p*q;
		     a[i][k] -= p;
		   }
		 }
	       }
	     }
	   }
	 } while (l < nn-1);
      }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


} // namespace Utility

#endif //MATRIXUTILITY_H
