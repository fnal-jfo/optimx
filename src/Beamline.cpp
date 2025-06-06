//  =================================================================
//
//  Beamline.cpp
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


void      Beamline::append(std::shared_ptr<Element> e)
{
  beamline_.push_back(e);
}  

RMatrix   Beamline::rmatrix( double& alfap,  double& energy, double ms, double& tetaY, double dalfa, int st) const
{
  RMatrix m;
  return m;
}

RMatrix Beamline::rmatrixsc( double& alfap,  double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st ) const
{
  RMatrix m;
  return m;
}
  
void Beamline::preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const
{
  return;
}  
  
int  Beamline::trackOnce( double ms,   double& Enr0,    int n_elem, int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const
{
  return 0;
}  

int  Beamline::track( double ms, double& Enr0,  Coordinates& v, double& tetaY ) const // track trajectory
{
  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void Beamline::updateEdges()
{

  // scan the beamline for edges associated with bends.
  // adjust the edge gradient attribute equal to the
  // neighboring bend gradient attribute. 
  
  std::shared_ptr<CFBend> pcfb;
  
  for (auto it = begin(); it != end(); ++it) {

    if ( (pcfb = std::dynamic_pointer_cast<CFBend>(*it)) &&
	 (it != begin()) ) {

      auto itu = it; --itu; 

      std::shared_ptr<Edge>   pedge;

      if ( (pedge = std::dynamic_pointer_cast<Edge>(*itu)) ) {
	pedge->isupstream        = true;
        pedge->bendGradient      = pcfb->G; 
      }	

      auto itd = it; ++itd;
      if (itd == end())  break;
      
      if ( (pedge = std::dynamic_pointer_cast<Edge>(*itd)) ) {
	pedge->isupstream        = false;
        pedge->bendGradient      = pcfb->G; 
      }	
    }
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void Beamline::setParameters( int np, std::vector<double> const& dat, ... )
{
  std::cout << "**THIS SHOULD NOT BE CALLED*** Beamline::setParameters "<< std::endl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

