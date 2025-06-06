//  =================================================================
//
//  WakeField.cpp
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

using Constants::C_DERV1;

WakeField::WakeField(const char* nm, char const* fnm)
  : Element(nm,fnm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

WakeField::WakeField(WakeField const& o)
  : Element(o)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void WakeField::toString(char* buf) const
{
  return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix WakeField::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{
  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  
  RMatrix mi;
  mi.toUnity();
  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void WakeField::setParameters( int np, double dat[], ... )
{
  L_ = 0.0; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void WakeField::setParameters( int np, std::vector<double> const& dat, ... )
{
  L_ = 0.0; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Beamline* WakeField::splitnew(int nslices) const  // return a sliced element as a beamline 
{
  Beamline* bml = new Beamline();
  WakeField* e = this->clone();
  e->L_      /= nslices;
  e->slices_  = nslices;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
RMatrix   WakeField::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{  
  return Element::rmatrix( frame, energy, ms, st);
}
  
