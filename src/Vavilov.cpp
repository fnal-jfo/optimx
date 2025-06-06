//  =================================================================
//
//  Vavilov.cpp
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
#include <fstream>
#include <iomanip>
#include <vector>
#include <complex>
#include <cmath>
#include <Constants.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_spline.h>

#include <Vavilov.h>

using Constants::PI;

std::random_device                     Vavilov::dev_;            // random device to initialize seed
std::mt19937                           Vavilov::rng_(dev_());      // rng_(dev());    
std::uniform_real_distribution<double> Vavilov::dist_(0.0, 1.0); // distribution in range [0.0, 1.0]

static const double         me = Constants::ME_MEV*1.0e6; // electron rest mass [eV]
static constexpr double  egma  = 0.5772156649015328606065120900824024310421;

#if 0
std::complex<double> unpack(std::complex<double>* zv, int n, int i)
{
  if (i    <   n/2) {
    return  zv[n/2+i];
  }
  else {  
    return  zv[i-n/2];
  }
}
#endif

double unpack(double* rv, int n, int i)
{
  if (i    <   n/2) {
    return  rv[n/2+i];
  }
  else {  
    return  rv[i-n/2];
  }
}

// ...............................................................................
double Si(double const x) 
{
  double res = gsl_sf_Si(x);
  return res;
}

// ................................................................................
double Ci(double const x) 
{
  return  gsl_sf_Ci(x);
}

//.................................................................................

double Cin(double const x) 
{
  double y   = fabs(x);
  double res = 0;

  if (y < 1.0e-10) {
    double x2 = y*y;
    res = -x2/4 + (x2*x2)/(4*3*2*4); 
  }    
  else {   
    res = log(y) + egma - Ci(y);  // definition of Cin 
  }

  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vavilov::Vavilov(Vavilov::Parameters const& parms)
  : gma_(parms.gma), I0_(parms.I0), dE_(parms.dE), n_(parms.nfft)
{
  // parameters struct:
  //p.gma;        // incident beam gamma (energy) 
  //p.I0;         // Bethe ionization potential         [eV] 
  //p.dE;         // average energy loss for the foil   [MeV]   
  //p.weps       = 2.5;       // width of FFT  window in normalized energy space  
  //p.nfft       = 8192;      // number of fft samples 
  // ..........................................................

  std::cerr << "Vavilov::Vavilov() "<< std::endl;

  static const double   me = Constants::ME_MEV*1.0e6;      // electron rest mass [eV];

  bta_       = sqrt( 1.0 - 1.0/(gma_*gma_));               // 0.713;
  Tmax_      = 2*me*(gma_*gma_-1.0);                       // Tmax = 1.057e6; maximum allowed energy transfer
  A_         = dE_/(2*Tmax_*(log(Tmax_/I0_)-(bta_*bta_))); // A = 2.268e-3; Vavilov amplitude  
  weps_      = 2*20*dE_/Tmax_;                             // window in normalized energy space
  delta_eps_ = weps_/n_;                                   // sample spacing in energy space
  wx_        = 1.0/delta_eps_;                             // width of window in normalized "frequency" space  
  delta_x_   = wx_/n_;                                     // sample spacing in normalized "frequency" space  


   data_        = std::vector<double>(n_,0.0);           // FFT data in half complex format

   // compute the Vavilov distribution samples; 

  vavsamples();

  // initialize the spline interpolators; 

  spline_ = gsl_spline_alloc(gsl_interp_steffen, n_+1);
  acc_    = gsl_interp_accel_alloc();

  xa_ = new double[n_+1];
  ya_ = new double[n_+1];

  double emin = epsmin(); 


  for (int i=0; i<n_; ++i) {
    xa_[i]      =  emin + i*delta_eps_; 
    double y    =  unpack(&data_[0], n_,i )*delta_x_*n_; 
    ya_[i]      =  y; 

    if (i>0) {
     if  ( xa_[i] <=  xa_[i-1] ) {
       std::cerr << "pdf interpolation error" << std::endl;
       std::cerr << " i =  " << i-n_/2  << std::endl;
       std::cerr << " xa_[i]     = " <<  xa_[i-1] << std::endl;
       std::cerr << " xa_[i-1]   = " <<  xa_[i]  << std::endl;
       exit(1);
     }
   }
  }

  xa_[n_] =   epsmax();
  ya_[n_] =   ya_[0];
 
  std::cerr << "Initializing the pdf spline interpolator..."<< std::endl;  
  gsl_spline_init(spline_,       xa_, ya_,  n_+1);
  std::cerr << "Done. [pdf spline interpolator]"<< std::endl;  

  // ................

  int ninvcdf = n_+1;
  
  invcdfxa_ = new double[ninvcdf];
  invcdfya_ = new double[ninvcdf];


  // we look only at the positive energy values.
  
  emin        = 0.0;
  double deps = (epsmax()-emin)/(ninvcdf-1);

  int ilast = 0;
  for (int i=0; i<ninvcdf; ++i) {
    double eps = emin + i*deps;  
    invcdfya_[i] = eps; 
    invcdfxa_[i] = cdf(eps);

    if  ( (i>0) && (invcdfxa_[i] <=  invcdfxa_[i-1]) ) {
       std::cerr << "cdf interpolation error" << std::endl;
       std::cerr << " i =  " << i  << std::endl;
       std::cerr << " invcdfxa_[i] = " <<  invcdfxa_[i] << std::endl;
       std::cerr << " invcdfxa_[i-1]   = " <<  invcdfxa_[i-1] << std::endl;
       ilast = i+1;
    }
  }


  if (ilast > 0) {
    for (int i = 0; i<ilast; ++i ) {
      invcdfxa_[i] =   invcdfxa_[ilast] * double(i)/double(ilast);
    }
  }
    
  
  invcdfspline_ = gsl_spline_alloc(gsl_interp_steffen, ninvcdf);
  invcdfacc_    = gsl_interp_accel_alloc();

  std::cerr << "Initializing the invcdf spline interpolator..."<< std::endl;  
  gsl_spline_init(invcdfspline_, invcdfxa_, invcdfya_,  ninvcdf);
  std::cerr << "Done. [invcdf spline interpolator] " << std::endl;  

#if 0
  //....................................................... 
  // reinterpolate at equal probability intervals

  ninvcdf = 20000; 
  int dn  = 5000;
  
  double* tmpxa = new double[ninvcdf];
  double* tmpya = new double[ninvcdf];

  gsl_spline*       tmpspline = gsl_spline_alloc(gsl_interp_steffen, ninvcdf);
  gsl_interp_accel* tmpacc    = gsl_interp_accel_alloc(); 

  int nmin    = 0;
  int nmax    = dn;
  double pmin = 0.0;
  double pmax = 0.7;
  double dp   = (pmax-pmin)/(nmax-nmin);

  double  p = 0;
  
  std::cout << " pmin = " << pmin << std::endl;
  std::cout << " pmax = " << pmax << std::endl;
  std::cout << " p = " << p << std::endl;
  for (int i=nmin; i<nmax; ++i) {
   p = pmin + i*dp;
   tmpxa[i] = p;
   tmpya[i] = invcdf(p);
  }

  nmin    = dn;
  nmax    = ninvcdf-dn;
  pmin = 0.2;
  pmax = 0.8;
  dp   = (pmax-pmin)/(nmax-nmin);
    
  std::cout << " pmin = " << pmin << std::endl;
  std::cout << " pmax = " << pmax << std::endl;
  std::cout << " p = " << p << std::endl;
  for (int i=nmin; i<nmax; ++i) {
   p = pmin + (i-nmin)*dp;
   tmpxa[i] = p;
   tmpya[i] = invcdf(p);
  }

  
  nmin    = dn;
  nmax    = ninvcdf;
  pmin = 0.7;
  pmax = invcdfxa_[n_];
  dp   = (pmax-pmin)/(nmax-nmin);
    
  std::cout << " pmin = " << pmin << std::endl;
  std::cout << " pmax = " << pmax << std::endl;
  std::cout << " p = " << p << std::endl;
  for (int i=nmin; i<nmax; ++i) {
   p = pmin + (i-nmin)*dp;
   tmpxa[i] = p;
   tmpya[i] = invcdf(p);
  }
  
  std::cout << " p = " << p << std::endl;

  tmpxa[ninvcdf-1] = invcdfxa_[n_];
  tmpya[ninvcdf-1] = invcdfya_[n_];

  gsl_spline_init( tmpspline, tmpxa, tmpya,  ninvcdf);


  //std::swap(invcdfspline_, tmpspline);
  //std::swap(invcdfacc_,    tmpacc);
  //std::swap(invcdfxa_,     tmpxa);
  //std::swap(invcdfya_,     tmpya);

  gsl_spline_free(tmpspline);
  gsl_interp_accel_free(tmpacc);

#endif
  
  dist_ =  std::uniform_real_distribution<double>(0.0, cdf(epsmax())); 

  }

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vavilov::Vavilov(Vavilov const& o)
  :
  A_(o.A_),
  bta_(o.bta_),
  gma_(o.gma_),
  Tmax_(o.Tmax_),
  I0_(o.I0_),
  dE_(o.dE_),
  weps_(o.weps_),
  delta_eps_(o.delta_eps_),
  wx_(o.wx_),
  delta_x_(o.delta_x_),
  data_(o.data_),
  n_(o.n_),
  spline_(o.spline_),
  invcdfspline_(o.invcdfspline_),
  xa_(o.xa_),
  ya_(o.ya_),
  invcdfxa_(o.invcdfxa_),
  invcdfya_(o.invcdfya_),
  acc_(o.acc_),
  invcdfacc_(o.invcdfacc_)
{
  std::cerr << "*************** Vavilov::Vavilov(Vavilov const& o)" << std::endl; 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Vavilov::~Vavilov()
{
  std::cerr << "Vavilov::~Vavilov() "<< std::endl;

  gsl_spline_free(spline_);
  gsl_interp_accel_free(acc_);

  gsl_spline_free(invcdfspline_);
  gsl_interp_accel_free(invcdfacc_);

  if (xa_) delete [] xa_; 
  if (ya_) delete [] ya_; 

  if (invcdfxa_) delete [] invcdfxa_; 
  if (invcdfya_) delete [] invcdfya_; 

}  

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::complex<double> Vavilov::vavft(double x) const
{
  // this function evaluates the Fourier transform of the Vavilov distribution
  // at "frequency" x 

  // We transform from frequency to angular frequency  because FFT sampling uses frequency
  // Distribution function is expressed in angular frequency

  x *= (2*PI);
    
  static constexpr std::complex<double> J   = {0.0,1.0};

  // note: the factor 2*pi in the denominator of the scaling constant is omitted
  // since omega = 2*pi*f and  d omega = 2 pi * df

  std::complex<double> result = exp(A_)*exp( J * x* A_ *(2*log(Tmax_/I0_) + 1.0 - bta_*bta_)) * exp( A_*((J*x - bta_*bta_) * (-Cin(x) + J*Si(x) ) - exp(J*x)));   

  return result;
}


// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Vavilov::vavsamples() const
{

  std::cerr << "Vavilov::vavsamples()" << std::endl;
  std::cerr << "Vavilov::vavsamples() delta_x_ = " << delta_x_  << std::endl;

  // Sample the transform of the Vavilov distribution.
  // The energy probability distribution is then obtained by inversion. 
  
  for (int i=1; i<n_/2; ++i) {
    double x = -i*delta_x_; // normally, positive frequencies only
                            // we use negative frequencies only -
                            // because we need to use -i inverse
                            // while FFT inverse is defined as +i)   
    
    std::complex<double> val =  vavft(x);
    data_[i]        =  std::real(val);
    data_[n_-i]     =  std::imag(val);
  }

  // special points 
  std::complex<double> val =  vavft(0.0);  
  data_[0]    = val.real();
  val =  vavft(-(n_/2)*delta_x_); 
  data_[n_/2] = val.real();


  int istatus = gsl_fft_halfcomplex_radix2_inverse(&data_[0], 1, n_);

  if (istatus != GSL_SUCCESS)
    { std::cerr << "Error in Vavilov::vavsamples()" << std::endl;}

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Vavilov::operator()() const
{
     double r = dist_(rng_);
     double val = invcdf( r );
     return val;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 double Vavilov::pdf(double x) const
{
  if ((x < 0.0) || (x > epsmax() )) return 0.0;
  return gsl_spline_eval(spline_, x, acc_);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Vavilov::cdf(double eps) const
{
  double result = 0.0;
  int istatus = gsl_spline_eval_integ_e(spline_, 0.0,  eps, acc_, &result); 
  return result;  
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Vavilov::invcdf(double p) const
{
  static double pmax =  cdf(epsmax()); 
  if ( p <= 0.0 )  return 0.0;
  if ( p >= pmax)  return epsmax();

  return gsl_spline_eval(invcdfspline_, p, invcdfacc_);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


