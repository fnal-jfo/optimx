//  =================================================================
//
//  RootFinder.cpp
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

#include <RootFinder.h>
#include <OptimExceptions.h>
#include <iostream>
#include <memory>
#ifdef USE_GSL

RootFinder::RootFinder( int n, std::function<int( double* x, double* fx )> f,
			std::function <void( void*)> update, Algorithm algo, double epsrel  )
  
  : step_(1),  n_(n), f_ (f), fdmultiroot_ { &fgsl_, &fdgsl_,  &fdfgsl_,  n, this },
    update_(update), epsrel_( epsrel < GSL_SQRT_DBL_EPSILON ? GSL_SQRT_DBL_EPSILON : epsrel) 
 {
   gsl_multiroot_fdfsolver_type const*  solver_type = 0;
   
   switch (algo) {
   case powell_hybrid:
     solver_type =  gsl_multiroot_fdfsolver_hybridsj;
     break;
   case powell_hybrid_unscaled:
     solver_type =  gsl_multiroot_fdfsolver_hybridj;
     break;
   case  generalized_newton:
     solver_type =  gsl_multiroot_fdfsolver_gnewton;
     break;
   case newton:
     solver_type =   gsl_multiroot_fdfsolver_newton;
     break;
   default:
     solver_type =  gsl_multiroot_fdfsolver_hybridsj;

   };

   solver_  = gsl_multiroot_fdfsolver_alloc (solver_type, n_);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RootFinder::~RootFinder()
{
  gsl_multiroot_fdfsolver_free (solver_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int RootFinder::fgsl_ (const gsl_vector* x, void* params, gsl_vector * f)
{
  reinterpret_cast<RootFinder*>( params)->f_( x->data, f->data);

  return GSL_SUCCESS;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int RootFinder::fdgsl_ (const gsl_vector * x, void * params, gsl_matrix * J)
{
  
   RootFinder* p =  reinterpret_cast<RootFinder*>( params);

   gsl_vector* fv = gsl_vector_alloc (p->n_);

   std::unique_ptr<gsl_vector, decltype(&gsl_vector_free) > fvunique( fv, gsl_vector_free );

   p->f_( x->data, fv->data);

   gsl_multiroot_function fmulti { &fgsl_,  p->n_, p };
   gsl_multiroot_fdjacobian( &fmulti, x, fv,  p->epsrel_,  J);   
 
   return GSL_SUCCESS;
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int RootFinder::fdfgsl_ (const gsl_vector* x, void* params, gsl_vector *f,  gsl_matrix * J)
{

   RootFinder* p =  reinterpret_cast<RootFinder*>( params);

   p->f_( x->data, f->data);

   gsl_multiroot_function fmulti { &fgsl_,  p->n_, p };
   gsl_multiroot_fdjacobian( &fmulti, x, f,  p->epsrel_,  J);   
 
   return GSL_SUCCESS;
   
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  RootFinder::epsilon() const {

  double eps = 0.0;
  //double sum = 0.0;

  for (int i=0; i<n_; ++i ) {

    double dx =  gsl_vector_get(gsl_multiroot_fdfsolver_dx(solver_), i);
    double f  =  gsl_vector_get(gsl_multiroot_fdfsolver_f(solver_), i);

    if ( dx != dx ) { throw OptimRuntimeException("dx is NaN" ); }
    if ( f  != f  ) { throw OptimRuntimeException("f  is NaN" ); }

    eps  =  std::max(eps, fabs(f));
    //sum +=  fabs(f);
  }

  return eps;
}  
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int RootFinder::operator()(double* x, double eps, int maxiters, int maxsteps )
{
  
  gsl_vector* p = gsl_vector_alloc(n_);

  std::unique_ptr<gsl_vector, decltype(&gsl_vector_free) >  xv ( p, gsl_vector_free );

  
  for (int i=0; i<n_; ++i) {
    gsl_vector_set (p, i,  x[i]);
  }

  gsl_multiroot_fdfsolver_set (solver_, &fdmultiroot_, p);
  
  int iter   = 0;
  int status = 0;

  if ( (status = gsl_multiroot_test_residual(solver_->f, eps ))  ==  GSL_CONTINUE ) {

    epsilon();  

    do
      {
        status = gsl_multiroot_fdfsolver_iterate (solver_);

        switch (status) {

	case GSL_SUCCESS : 
	  break;
	case GSL_EBADFUNC:
	  throw OptimRuntimeException("NaN returned during function or gradient evaluation." );  
	  break;
	case GSL_ENOPROG:
	  throw OptimRuntimeException("No more progess." );
	  break;
	case GSL_ESING:
	  throw OptimRuntimeException("Jacobian is singular." );   
	  break;
	default: 
	  throw OptimRuntimeException("Generic solver failure." );
	};

        double residual = epsilon();
        struct { int iter; int maxiters;  int step_; int maxsteps; double residual; } info = { ++iter, maxiters,  step_,  maxsteps, residual };
        update_( &info );

        status = gsl_multiroot_test_residual(solver_->f, eps);

	epsilon();
	
    } while (status == GSL_CONTINUE && iter < maxiters);

  }

  ++step_; 
    
  gsl_vector* root =   gsl_multiroot_fdfsolver_root(solver_);

  for (int i=0; i<n_; ++i) {
    x[i] = gsl_vector_get(root, i);
  }

  return status;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RootFinder::print_state (size_t iter, gsl_multiroot_fsolver * s)
{

  // this function is meant to be used for debugging
  
  std::cout << "iter = " << iter << std::endl;

  for (int i=0; i<n_; ++i) {
     
    std::cout << "x[" << i << "] =" << gsl_vector_get (s->x, i) << " f[" << i << "] =" << gsl_vector_get (s->f, i) << std::endl;

  }
  
}

#endif // USE_GSL
