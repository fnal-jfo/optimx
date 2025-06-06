//  =================================================================
//
//  QuadrupoleNew.cpp
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

#include <Constants.h>
#include <Structs.h>
#include <Losses.h>
#include <Element.h>
#include <RMatrix.h>
#include <limits>

using Constants::C_DERV1;
using Constants::C_CGS;
using Constants::PI;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix  Quadrupole::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{
  
  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  double ks;
  double fi;
  
  RMatrix mi;
  mi.toUnity();

  mi[4][5]=L_/(gamma1*gamma1);

  if( fabs(G) < std::numeric_limits<double>::epsilon() ) {
    mi[0][1] = mi[2][3]= L_;
    return mi;
  }

  if(G >0){ // hor focusing, ver defocusing 
    ks  = sqrt( G / hr );
    fi = ks * L_;
    mi[0][0] = mi[1][1] = cos(fi);
    mi[0][1] = sin(fi)/ks;
    mi[1][0] = -ks*ks * mi[0][1];
    mi[2][2] = mi[3][3] = cosh(fi);
    mi[2][3] = sinh(fi)/ks;
    mi[3][2] = ks*ks * mi[2][3];
  }
  else   { // ver focusing, hor defocusing 
    ks  = sqrt( - G / hr );
    fi = ks*L_;

    mi[0][0] = mi[1][1] = cosh(fi);
    mi[0][1] = sinh(fi)/ks;
    mi[1][0] = ks*ks * mi[0][1];
    mi[2][2] = mi[3][3] = cos(fi);
    mi[2][3] = sin(fi)/ks;
    mi[3][2] = -ks*ks* mi[2][3];
  }

  if(T_ != 0) {
    double psi = T_*(PI/180);
    double c = cos(psi);
    double s = sin(psi);  
    RMatrix_t<6> T =   {
                         { c, 0, -s,  0, 0, 0 },
                         { 0, c,  0, -s, 0, 0 },
                         { s, 0,  c,  0, 0, 0 },
                         { 0, s,  0,  c, 0, 0 },
                         { 0, 0,  0,  0, 1, 0 },
                         { 0, 0,  0,  0, 0, 1 }
                       };
        
    mi = T.transpose()*mi*T;
  }
  return mi;
}
