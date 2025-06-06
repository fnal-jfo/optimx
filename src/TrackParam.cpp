//  =================================================================
//
//  TrackParam.cpp
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

#include <TrackParam.h>

TrackParam::TrackParam()
  :
  EdgeBefore(false),
  p0(0.0),
  vp0(0.0),
  Hr0(0.0),
  ri0(0.0),
  R0(0.0),
  cb1(0.0),
  sb1(0.0),
  cb2(0.0),
  sb2(0.0),
  phi(0.0),
  cfi(0.0),
  sfi(0.0),
  tfi(0.0),
  sgn(1.0),
  G(0.0),
  Efin(0.0)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::ostream& operator<<(std::ostream& os, TrackParam const& tp)
{

  os << "EdgeBefore = " << tp.EdgeBefore << "\n" 
    << "p0   = " << tp.p0   << "\n"
    << "vp0  = " << tp.vp0  << "\n"
    << "Hr0  = " << tp.Hr0  << "\n"
    << "ri0  = " << tp.ri0  << "\n"
    << "R0   = " << tp.R0   << "\n"
    << "cb1  = " << tp.cb1  << "\n"
    << "sb1  = " << tp.sb1  << "\n"
    << "cb2  = " << tp.cb2  << "\n"
    << "sb2  = " << tp.sb2  << "\n"
    << "phi  = " << tp.phi  << "\n"
    << "cfi  = " << tp.cfi  << "\n"
    << "sfi  = " << tp.sfi  << "\n"
    << "tfi  = " << tp.tfi  << "\n"
    << "sgn  = " << tp.sgn  << "\n" 
    << "G    = " << tp.G    << "\n"
    << "Efin = " << tp.Efin;

  return os;
}



