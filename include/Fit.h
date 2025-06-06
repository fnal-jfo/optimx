//  =================================================================
//
//  Fit.h
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

#ifndef FIT_H
#define FIT_H

#include <iostream>


struct FitElem {
   static int    constexpr N_ELM_FIT = 50; //Nunber of Elements in a fitting group
   static int    constexpr NPOINT    = 21;
   static int    constexpr NGROUP    = 51;
   static double constexpr STEP_MULT = 0.001;
   static double constexpr STEP_MIN  = 1.0e-5; 
   int n;                 // n - Number of Elements in the group,
   int param; 	          // parameter - which parameter (L,G,B) should be adjusted (L=0), (G=2)
   int el[N_ELM_FIT];     // el - numbers of Elements of the group
   double step;           // step size for the group
   double min;            // minimum allowed value
   double max;            // maximum allow value
}; 

struct FitStep {

  double dL; 
  double dB; 
  double dG; 
  double btx; 
  double dbtx; 
  double bty; 
  double dbty; 
  double alfa; 
  double dalfa;

  FitStep();
  FitStep( FitStep const& o);
  FitStep& operator=( FitStep rhs);

};

std::ostream& operator<<( std::ostream& os, FitElem  const& o);
std::ostream& operator<<( std::ostream& os, FitStep  const& o);

#endif
