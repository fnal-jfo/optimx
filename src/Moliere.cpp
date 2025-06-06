//  =================================================================
//
//  Moliere.cpp
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
#include <cmath>
#include <functional>
#include <random>
#include <fstream>
#include <iomanip>
#include <Constants.h>
#include <Moliere.h>
#if __clang__
#include <boost/math/special_functions/bessel.hpp>
#else
#include <tr1/cmath>
#endif

#if __clang__
using boost::math::cyl_bessel_j;
#else
using std::tr1::cyl_bessel_j;
#endif

using Constants::PI;

Moliere::Moliere(Moliere::Parameters const& data)
  : generator_(),  rnorm_(0.0,1.0), rnd_(0.0, 1.0)
{
  Lc_       =  data.Lc;        // 3.2    
  theta0_   =  data.theta0;    // 3.585e-5;
  s_        =  data.s;         // 1.762e-6; // [m] foil thickness

  epsabs_    = 1.0e-8;  // integration absolute error 
  epsrel_    = 1.0e-6;  // integration relative error
  limit_     = 10000;   // maximum number of iteration allowed for integration   

  workspace_ = gsl_integration_romberg_alloc(limit_);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Moliere::~Moliere()
{
  gsl_integration_romberg_free(workspace_);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Moliere::Fluctuation Moliere::operator()()
{
     static const double q      = 1.25; 
     static const double Theta0 = 1.0;
     
     double z1    = (1.0-q/Lc_) * rnorm_(generator_); 
     double z2    = (1-0-q/Lc_) * rnorm_(generator_); 

     double x     = z1*s_*theta0_/sqrt(12.0) + z2*s_*theta0_/sqrt(2);  
     double tx    = z2*theta0_; 
     //.......

     z1    =  (1.0-q/Lc_)*rnorm_(generator_); 
     z2    =  (1.0-q/Lc_)*rnorm_(generator_); 

     double y     = z1*s_*theta0_/sqrt(12.0) + z2*s_*theta0_/sqrt(2);  
     double ty    = z2*theta0_; 

     //.......................................................
     // large angle (single scattering) contribution
     // this contribution is significant only in the far tail
     //.......................................................

     double z     = rnd_(generator_);
     double Theta  = Theta0/Lc_;
     double psi    = ( z > 1.0/((Theta*Theta)*Lc_) ) ? 0.0 : sqrt( 1.0/(Lc_*z) - (Theta*Theta) );
     double alpha  = 2*PI*rnd_(generator_);

     double dtx    = theta0_*psi*cos(alpha); 
     double dty    = theta0_*psi*sin(alpha); 

     double dx    =  0.5*s_*dtx;
     double dy    =  0.5*s_*dty;

     Fluctuation fluctuation; 

     fluctuation.x  = (x+dx) *100;  // convert to cm
     fluctuation.y  = (y+dy) *100;  // convert to cm
     fluctuation.xp = tx+dtx;
     fluctuation.yp = ty+dty; 
 
     return fluctuation;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Moliere::Distribution Moliere::operator()(int N) // random generation 
{
   static const double q      = 1.25;
   static const double Theta0 = 1.0;
 
   Distribution dist;
   dist.x.resize(N);
   dist.y.resize(N);
   dist.tx.resize(N);
   dist.ty.resize(N);

   for (int i=0; i<N; ++i ) { 

     double z1    = (1.0-q/Lc_) * rnorm_(generator_); 
     double z2    = (1-0-q/Lc_) * rnorm_(generator_); 

     double x     = z1*s_*theta0_/sqrt(12.0) + z2*s_*theta0_/sqrt(2);  
     double tx    = z2*theta0_; 

     //.......

     z1    = (1.0-q/Lc_)*rnorm_(generator_); 
     z2    = (1.0-q/Lc_)*rnorm_(generator_); 

     double y     = z1*s_*theta0_/sqrt(12.0) + z2*s_*theta0_/sqrt(2);  
     double ty    = z2*theta0_; 

     double z     = rnd_(generator_);
     double Theta  = Theta0/Lc_;
     double psi    = ( z > 1.0/((Theta*Theta)*Lc_) ) ? 0.0 : sqrt( 1.0/(Lc_*z) - (Theta*Theta) );
     double alpha  = 2*PI*rnd_(generator_);
     double dtx    = theta0_*psi*cos(alpha); 
     double dty    = theta0_*psi*sin(alpha); 

     x += 0.5*s_*dtx; // factor 100 for cm 
     y += 0.5*s_*dty; // factor 100 for cm 

     tx += dtx;
     ty += dty;

     dist.x[i]  = x*100;  // convert to cm
     dist.y[i]  = y*100;  // convert to cm
     dist.tx[i] = tx;
     dist.ty[i] = ty;
   }
   return dist;
}   

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Moliere::pdf( double theta, double Lc)
{

  // analytic probability density
  
  struct Wrapper {
    std::function<double(double)> integrand;
    static double f(double x, void* p) { double res = ((Wrapper*) p)->integrand(x); return res;}
    gsl_function_struct gslf;
  } wrapper;

  wrapper.integrand = [&theta, Lc](double x){ return x*cyl_bessel_j(0.0, theta*x) * exp( -0.5*(x*x)*(1- log(x)/3.2) );};  
  wrapper.gslf.function = &Wrapper::f; 
  wrapper.gslf.params   = &wrapper; 

  double result;   // integration result 

  size_t neval = 0; // number of eval performed if the integration fails
  int status   = gsl_integration_romberg(&wrapper.gslf, epsabs_, 20.0, epsabs_, epsrel_, &result, &neval, workspace_);

  return 1.0/(2*PI)*result;
}
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if  0

int main(int argc, char* argv[])
{
  static double pi = 4*atan(1.0);
  
  Moliere ts(3.2);

  //double dtheta = 10.0/99;
  //for (int i=0; i<100; ++i) {
  //  std::cout << i*dtheta << "  " << ts.moliere(  i*dtheta, 3.2) << std::endl; 
  //}

  int N = 10000000;

  Moliere::Distribution d = ts(N);

  std::fstream fs;
  fs.open( "transverse_distribution.dat", std::fstream::out);

  using std::setw;
  int nbins = 500;
  double theta0 = ts.theta0();
  
  double dtheta = 15 * theta0/(nbins-1); 
  std::vector<int> thetahisto( nbins, 0);
   
  for (int i=0; i<N; ++i) {
     fs << setw(16) << d.x[i]
       << setw(16) << d.y[i]
       << setw(16) << d.tx[i]
       << setw(16) << d.ty[i] << std::endl;  
       double theta = sqrt( d.tx[i]* d.tx[i] +  d.ty[i]* d.ty[i] ); 
       int k = std::nearbyint((-0.5*dtheta + theta)/dtheta);
       if (k<0)       continue;
       if (k>nbins-1) continue;
       //int k = (theta/dtheta);
       ++thetahisto[k];
  }
  fs.close();

  double sum = 0.0;
  fs.open( "transverse_histo.dat", std::fstream::out);
  for (int i=0; i<nbins; ++i) {
    double valmol  =   2*pi*(i*dtheta/theta0)*ts.moliere((i*dtheta)/theta0, ts.Lc())/theta0; 
    double valhist =   ((i == 0) ? 0 : double(thetahisto[i])/(dtheta*double(N))); 
    sum += double(thetahisto[i])/double(N); 
    fs << setw(16) << i*dtheta
       << setw(16) <<  valhist
        << setw(16) << valmol
       << setw(16) << (( i==0) ?  1.0 :  valhist/valmol) << std::endl; 
  }

  std::cout << " sum = " << sum << std::endl;

  fs.close();

  return 0;
}
#endif
