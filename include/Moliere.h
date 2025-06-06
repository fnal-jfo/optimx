//  =================================================================
//
//  Moliere.h
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

#ifndef MOLIERE_H
#define MOLIERE_H

#include <gsl/gsl_integration.h>

class Moliere {

public:

  struct Distribution {
    std::vector<double> x;    
    std::vector<double> y;    
    std::vector<double> tx;    
    std::vector<double> ty;    
  };

  struct Fluctuation {
    double x;    
    double y;    
    double xp;    
    double yp;    
  };

  struct Parameters {  // distribution parameters 
    double Lc       =  3.2;
    double theta0   =  3.585e-5;
    double thetamin =  1.745e-6;
    double s        =  1.762e-6; // [m] foil thickness
  };

  Moliere(Parameters const& data);
 ~Moliere();

  Fluctuation   operator()();
  Distribution  operator()(int N); // random generation 
  double pdf( double theta, double Lc);
  double theta0() const {return theta0_;}
  double Lc()     const {return Lc_;}

 private:

  double Lc_;
  double theta0_;
  double thetamin_;
  double s_;        // [m] foil thickness
  
  double epsabs_;   // integration absolute error 
  double epsrel_;   // integration relative error
  size_t limit_;    // maximum number of iteration allowed for integration   
  gsl_integration_romberg_workspace* workspace_; // integration workspace  

  std::default_random_engine              generator_;
  std::normal_distribution<double>        rnorm_;
  std::uniform_real_distribution<double>  rnd_;
  
};

#endif // MOLIERE_H
