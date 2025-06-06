//  =================================================================
//
//  Tracker3DSeriesData.cpp
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

#include <Tracker3DSeriesData.h>
#include <Bunch.h>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Tracker3DSeriesData::Tracker3DSeriesData( Bunch const& v, int n, PlaneType view) 
  : v_(&v[0]), n_(n), ix_(0), iy_(2), ax_(1.0), ay_(1.0)
{
  setView(view);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Tracker3DSeriesData::~Tracker3DSeriesData()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

size_t Tracker3DSeriesData::size() const
{
   return n_;
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QwtPoint3D Tracker3DSeriesData::sample( size_t i) const
{
  return QwtPoint3D( ax_*v_[i].c[ix_], ay_*v_[i].c[iy_], (v_[i].lost > 0 ? 1.0: 0.0));
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QRectF Tracker3DSeriesData::boundingRect() const
{
  return br_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  Tracker3DSeriesData::setScales( double ax, double ay) const
{
  ax_ = ax;  
  ay_ = ay;

  return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  Tracker3DSeriesData::setView( PlaneType view, double ax, double ay) const
{
  
  ax_ = ax;  
  ay_ = ay;

  switch (view) { 
    case PlaneType::X_Y  : ix_ = 0;  iy_ = 2; break; 
    case PlaneType::X_Xp : ix_ = 0;  iy_ = 1; break; 
    case PlaneType::Y_Yp : ix_ = 2;  iy_ = 3; break; 
    case PlaneType::S_P  : ix_ = 4;  iy_ = 5; break;
  };

  br_ = qwtBoundingRect(*this);
}

