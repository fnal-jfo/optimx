//  =================================================================
//
//  ElementNew.cpp
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

#include <algorithm> 
#include <cmath>
#include <utility> 

#include <Channel.h>
#include <Beamline.h>
#include <Twiss.h>
#include <TrackParam.h>
//#include <Structs.h>
#include <Losses.h>
#include <Element.h>
#include <Medium.h>
#include <Foil.h>
#include <Cavity.h>
#include <Utility.h>
#include <Constants.h>
#include <RMatrix.h>
#include <OptimMessages.h>
#include <Globals.h>

using std::acosh;

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;
using Constants::C_CGS;


bool    Element::use_mad_frame_ = false;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Element::propagate( double hr, double ms, RMatrix_t<3>& W, Vector_t<3>& R ) const
{
  // only bending elements alter the orientation of the local frame axes.
  // For all other elements, there is no change, so the default function is an
  // identity transformation

  double r   =  hr / B;
  R = R + W* Vector_t<3>({0.0, 0.0, L_});

  if (use_mad_frame_) return;
  
  double psi_in = atan2( W[1][0], W[1][1]);

  double si = sin(psi_in);
  double ci = cos(psi_in);
  
  RMatrix_t<3> T= { { ci, -si,  0.0 },
                    { si,  ci,  0.0 },
                    { 0.0, 0.0, 1.0 } };

  // X = Wx
  // X = W S^-1 x  when S is transformation from output  to input

  // transformation from input  to output: T  
  // transformation from output to input:  T^{-1}
 
  
  W =   W*T;  // we need  (T^{-1})^{-1} to invert the rotation 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Beamline* Element::splitnew(int nslices) const  // return a sliced element as a beamline 
{
  Beamline* bml = new Beamline();
  Element* e = this->clone();
  e->L_      /= nslices;
  e->slices_  = nslices;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
RMatrix   Element::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{
  // This is the default. It does NOTHING and returns an identity matrix.
  // THIS IS NOT OK FOR A DRIFT or any element with finite length !
  RMatrix const m = RMatrix().toUnity();
  return m;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Element::preTrack(RMatrix_t<3>& frame, double ms, double Enr0, int n_elem, TrackParam& prm, RMatrix& m1 ) const
{
   prm.p0   = sqrt(2.*ms*Enr0+Enr0*Enr0);
   prm.Hr0  = prm.p0/C_DERV1;
   prm.vp0  = C_CGS*prm.p0/sqrt(prm.p0*prm.p0+ms*ms);

   m1 = rmatrix(frame, Enr0, ms, 3);  // NOTE: this non-virtual function calls the element-specific virtual function
                                      // with alfap as first argument, with alfap=0.0. This is appropriate for all
                                      // NON-BENDING elements. Bending element slices have alfap !=0.0 
                                      // rmatrix(0.0, Enr0, ms, tetaY, 0.0, 3)      
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
