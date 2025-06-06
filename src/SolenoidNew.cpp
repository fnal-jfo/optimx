//  =================================================================
//
//  SolenoidNew.cpp
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

#include <Beamline.h>
#include <Element.h>
#include <Losses.h>
#include <Constants.h>
#include <memory>
#include <limits>

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;

  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Beamline* Solenoid::splitnew(int nslices) const  // return a sliced element as a beamline 
{
  Beamline* bml = new Beamline();
  Solenoid* e = this->clone();
  e->L_      /= nslices;
  e->slices_  = nslices;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix Solenoid::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{
  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  double s;
  double c;
  
  RMatrix mi;
  mi.toUnity();

  RMatrix me;
  me.toUnity();
  
  mi[4][5]  = L_/(gamma1*gamma1);

  double k =  B / hr;
  double fi = L_ * k;

  if (fabs(k) < std::numeric_limits<double>::epsilon() ) {
    mi[0][1] = mi[2][3] = L_;
    return std::move(mi);
  }

  if ( toupper(name()[1]) == 'C'){     // focusing part only
    s = sin(fi/2.);
    c = cos(fi/2.);
    mi[0][0] = mi[1][1] =  mi[2][2]=mi[3][3] = c;
    mi[0][1] = mi[2][3] =  2.*s/k;
    mi[1][0] = mi[3][2] = -k*s/2.;
  }
  else {                              // 4-dimensional transfer matrix
    s = sin(fi);
    c = cos(fi);
    mi[0][0] =  mi[1][1] 
             =  mi[2][2]
             =  mi[3][3] = (1+c)/2.;
    mi[0][1] =  mi[2][3] =  s/k;
    mi[1][0] =  mi[3][2] = -k*s/4.;
    mi[0][2] =  mi[1][3] =  s/2.;
    mi[2][0] =  mi[3][1] = -s/2.;
    mi[0][3] =  (1-c)/k;
    mi[2][1] = -mi[0][3];
    mi[1][2] = -(1-c)*k/4.;
    mi[3][0] = -mi[1][2];
  }

  if( A>0.0 ) { // effective aperture != 0 implies soft edge focusing 
                //                    == 0 hard edge

    if (st == 1){ // ADD Entrance focusing (typically this is the FIRST slice)
      SolLinearEdge(me, 0.5*k, A, 1);
      mi = mi*me;
    }
    if(st == 2){ // ADD Exit focusing (typically this is the LAST slice)
      SolLinearEdge(me, 0.5*k, A, 2);
      mi =  me*mi;
    }
    if(st == 3){ // TOTAL transfer matrix
      SolLinearEdge(me, 0.5*k, A, 1);
      mi = mi*me;
      SolLinearEdge(me, 0.5*k, A, 2);
      mi = me*mi;
    }
  }

   return mi;
}
