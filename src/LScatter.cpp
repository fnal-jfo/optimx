//  =================================================================
//
//  LScatter.cpp
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

#include <Element.h>
#include <Beamline.h>
#include <Constants.h>
#include <Coordinates.h>
#include <Utility.h>
#include <TrackParam.h>

using Constants::PI;
using Utility::gauss;


LScatter::LScatter(const char* nm, char const* fnm)
  : Element(nm,fnm)
{} // 'U'

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LScatter::LScatter(LScatter const& o)
  : Element(o)
{} // 'U'

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//RMatrix LScatter::rmatrix()
//{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LScatter::setParameters( int np, double dat[], ... )
{
  L_ = 0.0;
  double* rdat[] = { &B, &G, &T_};

  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  //B  = dat[0];
  //G  = dat[1];
  //T_ = dat[2];
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LScatter::setParameters( int np, std::vector<double> const& dat, ... )
{
  L_ = 0.0;
  double* rdat[] = { &B, &G, &T_};

  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  //B  = dat[0];
  //G  = dat[1];
  //T_ = dat[2];
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LScatter::toString(char* buf) const
{
  sprintf(buf, "Rms Loss Spread[MeV]=%lg  1/L*dL/dx[1/cm]=%lg Tilt[deg]=%lg", B, G, T_);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LScatter::trackOnce(double ms, double& Enr0, int n_elem, int n_turn,
	                       TrackParam& prm, RMatrix const& m1, Coordinates& v) const
{

  int status = 0; 
  if (status = backwardTest(prm, n_elem, n_turn, v)) return status;


  double phi = T_/180.*PI;
  double s   = B * B *(1. + G * (v[0]*cos(phi) + v[2]*sin(phi)));

  s = ( s>0.0) ? sqrt(s) : 0.0;

  v[5] += s*gauss()*(ms+ Enr0)/(prm.p0 * prm.p0);

 done:	

  if (status = transAmpTest(prm, n_elem, n_turn, v)) return status; 

  return 0;
  
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Beamline* LScatter::splitnew(int nslices) const  // return a sliced element as a beamline 
{
 // LScatter cannot be split so we return a cloned element with slices_ = 1. 
  Beamline* bml = new Beamline();
  LScatter* e      = this->clone();
  e->L_      /= 1;
  e->slices_  = 1;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


RMatrix   LScatter::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{
  return Element::rmatrix( frame, energy, ms, st);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
