//  =================================================================
//
//  matrinv.cpp
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

#include <iostream>
#include <limits>
#include <complex>
#include <cmath>
#include <cstring>
#include <Utility.h>
#include <vector>


static double const tiny = std::numeric_limits<double>::epsilon(); 

// Array indices in all routines start from 1;  like a[1 ... n]
// solves a set of linear equations a*x=b

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// LU backsubstitution
//

namespace Utility {

using std::abs;
  
template  < typename Matrix_t >
void ludcmp( Matrix_t a, int n, int* indx, double& d, char* err)
{
  int i,imax,j,k;
  using T = decltype( a[0][0] );

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
void lubksb( Matrix_t a, int n, int* indx, T* b)
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

  template<> void lubksb<std::complex<double>,std::complex<double>**>(std::complex<double> **a, int n, int* indx, std::complex<double> b[]);
  template<> void lubksb<double,double**>(double **a, int n, int indx[], double b[]);

  template<> void lubksb<std::complex<double>,std::complex<double>(*)[4]>(std::complex<double> (*a)[4], int n, int* indx, std::complex<double> b[]);
  template<> void lubksb<double, double (*)[4]>(double (*a)[4], int n, int* indx, double b[]);

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename Matrix_t >
void matr_invert(int n, Matrix_t m, Matrix_t minv, char *err)
{

  double d = 0.0;
  using T = decltype( m[0][0] );
  
  std::vector<int> indx(n);
  std::vector<T>   col (n);

  ludcmp(m, n, &indx[0], d, err);

   for( int j=0; j<n; ++j){
     for(int i=0; i<n; ++i) { 
       col[i] = T();
     }
     col[j] = 1.0;
     lubksb(m, n, &indx[0], &col[0]);
     for(int i=0; i<n; ++i) { 
       minv[i][j] = col[i];
     }
   }

}

} // namespace Utility
