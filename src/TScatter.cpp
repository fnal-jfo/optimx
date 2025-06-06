//  =================================================================
//
//  TScatter.cpp
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
#include <Constants.h>
#include <Utility.h>
#include <TrackParam.h>
#include <Coordinates.h>

using Constants::PI;
using Utility::gauss;


TScatter::TScatter(const char* nm, char const* fnm) // 'T'
  : Element(nm,fnm)
{} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TScatter::TScatter(TScatter const& o) // 'T'
  : Element(o)
{}

//RMatrix TScatter::rmatrix()
//{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TScatter::setParameters( int np, double dat[], ... )
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

void TScatter::setParameters( int np, std::vector<double> const& dat, ... )
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

void TScatter::toString(char* buf) const
{
 sprintf(buf, "Rms angle[mrad]=%lg  1/L*dL/dx[1/cm]=%lg Tilt[deg]=%lg", B, G, T_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int TScatter::trackOnce(double ms, double& Enr0, int n_elem, int n_turn,
	                TrackParam& prm, RMatrix const& m1, Coordinates& v ) const
{
  int status = 0; 
  if (status = backwardTest(prm, n_elem, n_turn, v)) return status;
  
  double phi = T_/180.*PI;

  double s = B * B *(1. + G * (v[0]*cos(phi) + v[2]*sin(phi)));

  s = ( s>0.0) ? 0.001*sqrt(s) : 0.0;

  v[1] += s*gauss();
  v[3] += s*gauss();

 done:	

  if (status = transAmpTest(prm, n_elem, n_turn, v)) return status; 

  return 0;
}

