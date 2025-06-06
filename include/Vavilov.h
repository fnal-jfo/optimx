//  =================================================================
//
//  Vavilov.h
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

#include <vector>
#include <complex>
#include <cmath>
#include <random>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_spline.h>
#include <vector>

#ifndef VAVILOV_H
#define VAVILOV_H

class Vavilov {

 public:
  struct Parameters {
    double gma;        // incident beam gamma (energy) 
    double I0;         // Bethe ionization potential
    double dE;         // average energy loss for the foil      
    int    nfft       = 8192;      // number of fft samples 
  };
  
  Vavilov(Parameters const&p );
  Vavilov(Vavilov const& o);
  // Vavilov();
 ~Vavilov();

  double             operator()() const;   // random generator 

  double          pdf(double eps) const;
  double          cdf(double eps) const;
  double       invcdf(double eps) const;

  double Tmax()   const  {return  Tmax_;    }   
  double epsmin() const  {return -0.5*weps_;} 
  double epsmax() const  {return  0.5*weps_;}  

private:
 
  std::complex<double>   vavft(double x) const;  
  void                   vavsamples() const;  

  double A_;
  double bta_;        // incident beam beta  (velocity)
  double gma_;        // incident beam gamma (energy) 
  double Tmax_;       // [eV] max kinetic energy transfer (collision with free electron) 
  double I0_;         // [eV] Bethe ionization potential
  double dE_;         // [eV] average energy loss in the foil
  
  double weps_;       // width of FFT  window in normalized energy space  
  double delta_eps_;  // sample spacing in normalized energy space

  double wx_;         // witdh of window in "frequency" space  
  double delta_x_;    // sample spacing  in "frequency" space  
  
    
  // static constexpr std::complex<double> J_  = std::complex<double>(0.0,1.0);
  static constexpr double               egma_  = 0.5772156649015328606065120900824024310421; // Euler-Mascheroni constant  

  mutable std::vector<double>           data_ ;          // FFT data in half-complex format 
  size_t                                n_;              // no of samples in FFT 
  gsl_spline*                           spline_;         // spline interpolation object for pdf  
  gsl_spline*                           invcdfspline_;   // spline interpolation object for inverse cdf 
  double*                               xa_;             // pdf independent variable samples
  double*                               ya_;             // pdf dependent   variable samples
  double*                               invcdfxa_;       // inverse cdf independent variable samples 
  double*                               invcdfya_;       // inverse cdf dependent   variable samples
  gsl_interp_accel*                     acc_;            // spline accelerator object for pdf  
  gsl_interp_accel*                     invcdfacc_;      // spline accelerator object for inverse cdf

  static std::random_device                     dev_;   // random device to initialize seed
  static std::mt19937                           rng_;   // rng_(dev());    
  static std::uniform_real_distribution<double> dist_;  // dist_(0.0, 1.0) distribution in range [0.0, 1.0]


};
#endif // VAVILOV_H

