//  =================================================================
//
//  SplineInterpolator.cpp
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

#include <SplineInterpolator.h>

SplineInterpolator::SplineInterpolator()
{}

SplineInterpolator::SplineInterpolator(double const xv[], double const yv[], int n)
  : x_( &xv[0], &xv[n]), y_( &yv[0], &yv[n])
{
  double p;
  double sig;
  
  std::vector<double> u(n);
  
  y2_[0] = u[0] = 0.0; // force natural boundary condition 

  for (int i=1; i<n-1; ++i) {
    sig    = (x_[i]-x_[i-1])/(x_[i+1]-x_[i-1]);
    p      = sig*y2_[i-1]+2.0;
    y2_[i] = (sig-1.0)/p;
    u[i]   = (y_[i+1]-y_[i])/(x_[i+1]-x_[i]) - (y_[i]-y_[i-1])/(x_[i]-x_[i-1]);
    u[i]   = (6.0*u[i]/(x_[i+1]-x_[i-1])-sig*u[i-1])/p;
  }

  y2_[n-1] = 0.0;  // force natural bounday condition

  for (int i=n-2; i>=0; i--) y2_[i] = u[i]+y2_[i]*y2_[i+1];

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double SplineInterpolator::operator()(double x) const
{

 int k;
 int klo = 0;
 int khi = x_.size();

 while (khi-klo > 1) {
    k = (khi+klo) >> 1;
    if (x_[k] > x) khi=k;
    else klo=k;
 }

 double h=x_[khi]-x_[klo];

 // if (h <= 0.0) return 1;

 double a = (x_[khi]- x       )/h;
 double b = (x      - x_[klo] )/h;

 return a*y_[klo]+b*y_[khi]+((a*a*a-a)*y2_[klo]+(b*b*b-b)*y2_[khi])*(h*h)/6.0;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double SplineInterpolator::operator()(double x, int order)  const
{

  if (order == 0)  return (*this)(x);
  
  int klo = 0;
  int khi = x_.size();;

  while (khi-klo > 1) {
    int k = (khi+klo) >> 1;
    if (x_[k] > x) khi = k;
    else klo = k;
  }
  double h =x_[khi]-x_[klo];

  // if (h == 0.0) return 1;

  double a = (x_[khi]- x      )/h;
  double b = (x      - x_[klo])/h;

  return (y_[khi]-y_[klo])/h + ( (3*b*b-1)*y2_[khi] - (3.0*a*a-1)*y2_[klo] )*h/6.0;
}

