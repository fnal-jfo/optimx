//  =================================================================
//
//  Cavity.cpp
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

#include <OptimMainWindow.h>
#include <Utility.h>
#include <OptimCalc.h>
#include <Cavity.h>
#include <Constants.h>
#include <RMatrix.h>
#include <vector>
#include <cmath>
#include <numeric>
#include <limits>

using Constants::PI;
using Utility::matr_invert;
using std::copysign;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void spline(double const x[], double const y[], int n, double y2[])
{
  double p,sig;
  std::vector<double> u(n);
  
  y2[0] = u[0] = 0.0;
  for (int i=1;i<=n-2;i++) {
    sig   = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
    p     = sig*y2[i-1]+2.0;
    y2[i] = (sig-1.0)/p;
    u[i]  = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
    u[i]  = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
  }
  y2[n-1]=0.;
  for (int i=n-2;i>=0;i--) y2[i]=u[i]+y2[i]*y2[i+1];
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//======================================================
// Return 0 if OK, 1 if wrong input
int splint(double const xa[], double const ya[], double const y2a[], int n, double x, double *y)
{
 int k;
 int klo=0;
 int khi=n-1;
 while (khi-klo > 1) {
    k=(khi+klo) >> 1;
    if (xa[k] > x) khi=k;
    else klo=k;
 }
 double h=xa[khi]-xa[klo];
 if (h <= 0.0) return 1;
 double a=(xa[khi]-x)/h;
 double b=(x-xa[klo])/h;
 *y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
 return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//======================================================
// Return 0 if OK, 1 if wrong input
// computes derivatives for the spline

int dsplint(double const xa[], double const ya[], double const y2a[], int n, double x, double *y)
{
  int klo=0;
  int khi=n-1;

  while (khi-klo > 1) {
    int k=(khi+klo) >> 1;
    if (xa[k] > x) khi=k;
    else klo=k;
  }
  double h =xa[khi]-xa[klo];

  if ( fabs(h) < std::numeric_limits<double>::epsilon() ) return 1;

  double a=(xa[khi]-x)/h;
  double b=(x-xa[klo])/h;
  *y= (ya[khi]-ya[klo])/h + ( (3*b*b-1)*y2a[khi] - (3.*a*a-1)*y2a[klo] )*h/6.0;
  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//======================================================

INTEGRERR rk3(double y[], double dydx[], int n, double x, double h, double yout[],
	INTEGRERR (*derivs)(double, double *, double *, ODEparam *),ODEparam *p)
{

  INTEGRERR ret;
  std::vector<double> ak1(n);
  std::vector<double> ak2(n);
  std::vector<double> ak3(n);
  
  std::vector<double> ytemp(n);
  
  for (int i=0;i<n;i++){
    ak1[i]=h*dydx[i];
    ytemp[i]=y[i]+0.5*ak1[i];
  }
  ret=(*derivs)(x+0.5*h,&ytemp[0],&ak2[0],p);
  
  if(ret){ return ret;}
  for (int i=0;i<n;i++){
    ak2[i]=h*ak2[i];
    ytemp[i]=y[i]+2.*ak2[i]-ak1[i];
  }
  ret=(*derivs)(x+h,&ytemp[0],&ak3[0],p);
  
  if(ret){ return ret;}
  for (int i=0;i<n;i++){
     ak3[i]=h*ak3[i];
     yout[i]=y[i]+(ak1[i]+4.*ak2[i]+ak3[i])/6.;
  }

  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Integrate an ordinary diff. equation with fixed step - (x2-x1)/n
// Initial coordinate array ystart[] is overwritten with final coordinates upon exit.

INTEGRERR odeintfs(double  x1,   double   x2,        // integration interval
	           int   nvar,   double   ystart[],  // number of variables, initial y - values which will be replaced by the final ones
	           int      n,   int*     kount,     // number of steps for integration and actual number of stored points
	           int  kstep,                       // nstep for output(output after every kstep points), <=0 - no output
	           double *xp,   double** yp,        // arrays for output

		   INTEGRERR (*derivs)(double, double *, double *, ODEparam *), //function to compute right-hand part
                   ODEparam *p)
{
  
  INTEGRERR ret;
 
  std::vector<double>    y(nvar); 
  std::vector<double> yout(nvar); 
  std::vector<double> dydx(nvar);

  double  h = (x2-x1)/n;
 
  double x = x1;
  *kount = 0;
  int j  = 0;
  
  for ( int i=0; i<nvar; ++i) { y[i] = ystart[i]; } 

  for ( int nstp=0; nstp<n; nstp++) {
    if ((kstep > 0) && (j <= 0) ) {
      xp[*kount] = x;
      for (int i=0; i<nvar; ++i) { yp[i][*kount] = y[i]; }
      (*kount)++;
      j = kstep;
    }
    --j;
    ret = (*derivs)(x,&y[0],&dydx[0],p);

    if(ret) { return ret;}

    ret = rk3(&y[0], &dydx[0], nvar, x, h, &yout[0], derivs, p);

    if(ret){ return ret;}
    x += h;
    for (int i=0; i<nvar; ++i) { y[i] = yout[i]; } 
  }

  for (int i=0; i<nvar; ++i) { ystart[i] = y[i]; }

  if (kstep>0) {
    xp[*kount] = x;
    for (int i=0; i<nvar; ++i) yp[i][*kount] = y[i];
    (*kount)++;
  }

  return NO_ERR;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

