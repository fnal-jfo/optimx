//  =================================================================
//
//  EdgeNew.cpp
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

#include <limits> 
#include <Element.h>
#include <Beamline.h>
#include <Losses.h>
#include <Constants.h>
#include <TrackParam.h>

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;
using Constants::C_CGS;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Beamline* Edge::splitnew(int nslices) const  // return a sliced element as a beamline 
{
 // Edge cannot be split so we return a cloned element with slices_= 1. 
  Beamline* bml = new Beamline();
  Edge* e      = this->clone();
  e->L_      /= 1;
  e->slices_  = 1;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix Edge::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{
  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  
  RMatrix mi;
  mi.toUnity();
  
  if( fabs(B) < 10*std::numeric_limits<double>::epsilon() ){
    return mi;
  }

  double s  = tan(PI*G/180.0);
  double ri = fabs(B / hr);   // 1/rho could be negative; this looks like a mistake   
                              // ri = "inverse radius" (curvature)
  
  mi[1][0] =  ri * s;
  mi[3][2] = -ri * s + S *(ri*ri);

  
  if (T_ != 0.0) {

    double psi = T_*PI/180.0;

    double si = sin(psi);
    double ci = cos(psi);
 
    RMatrix S = {  {  ci,   0.0,   si,  0.0,  0.0,   0.0 },
                   { 0.0,    ci,  0.0,   si,  0.0,   0.0,},
                   { -si,   0.0,   ci,  0.0,  0.0,   0.0 },
                   { 0.0,   -si,  0.0,   ci,  0.0,   0.0 },
                   { 0.0,   0.0,  0.0,  0.0,  1.0,   0.0 },
                   { 0.0,   0.0,  0.0,  0.0,  0.0,   1.0 }
                };

                   
    mi = S.transpose()*mi*S;
  }

  return mi;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Edge::preTrack( RMatrix_t<3>& frame, double ms,    double Enr0, int n_elem, TrackParam& prm, RMatrix& m1) const
{
   prm.p0   = sqrt(2.*ms*Enr0+Enr0*Enr0);
   prm.Hr0  = prm.p0/C_DERV1;
   prm.vp0  = C_CGS*prm.p0/sqrt(prm.p0*prm.p0+ms*ms);

   
   if( fabs(B) < std::numeric_limits<double>::epsilon() )  return;

   prm.ri0 = B/prm.Hr0;   // negative sign allowed ????  
   prm.R0  = 1.0/prm.ri0;

   m1  = rmatrix( frame, Enr0, ms);

   prm.sgn = (B >= 0.0)  ? 1.0 : -1.0;

   // cos and sin for rotation

   double alfa = T_/180.*PI;
   prm.cb1    =  prm.cb2 = cos(alfa);
   prm.sb1    = -sin(alfa);
   prm.sb2    = -prm.sb1;
   prm.phi    =  PI*G/180.;
   prm.sfi    =  sin(prm.phi);
   prm.cfi    =  cos(prm.phi);

   if(prm.cfi !=0 ) prm.tfi=prm.sfi / prm.cfi;

   // determine where is the dipole

   if(n_elem == 0) {
     prm.EdgeBefore = true;
   }
   else {
     prm.EdgeBefore = isupstream;
     prm.G          = bendGradient;
   }
    
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
