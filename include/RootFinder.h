//  =================================================================
//
//  RootFinder.h
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

#ifndef ROOTFINDER_H
#define ROOTFINDER_H
#ifdef  USE_GSL

#include <functional>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_vector.h>

class RootFinder {

 public:

  enum Algorithm { powell_hybrid, generalized_newton, powell_hybrid_unscaled, newton };
  
  RootFinder( int n, std::function<int( double* x, double* fx  )> f, std::function<void( void* info)> update = [](void* ){},  Algorithm algo = generalized_newton, double epsrel=0.0);

 ~RootFinder();

 int operator()(double* x, double eps=1.0e-4, int maxiters=100, int maxsteps=1);

 void print_state (size_t iter, gsl_multiroot_fsolver * s);

 double epsilon() const;  

 private:

  int   step_;      //  cumulative count of converged steps 
  int   n_; 
 
  static int   fgsl_ (const gsl_vector* x, void* params, gsl_vector* f);
  static int  fdgsl_ (const gsl_vector* x, void* params, gsl_matrix* J);
  static int fdfgsl_ (const gsl_vector* x, void* params, gsl_vector* f, gsl_matrix* J);
  

  std::function<int(  double* x, double* y )>                            f_;  // the ( n-dimensional ) function.  x = argument, y = f(x)     
  std::function<void( void* info)>                                  update_;  // callback ( may be used to update gui). 

  gsl_multiroot_fdfsolver*     solver_;
  gsl_multiroot_function_fdf   fdmultiroot_ ;

  double epsrel_;
  
};


#else  // USE_GSL

namespace RootFinder { enum Algorithm { powell_hybrid, generalized_newton, powell_hybrid_unscaled, newton }; }
  
#endif // USE_GSL
#endif // ROOTFINDER_H 
