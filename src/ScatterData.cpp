//  =================================================================
//
//  ScatterData.cpp
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

#include <Optim.h> // ViewType
#include <ScatterData.h>
#include <algorithm>
#include <tuple>
#include <iostream>

constexpr double  AbstractScatterData::scaling_[];  

AbstractScatterData::~AbstractScatterData()
{}

void AbstractScatterData::setView(ViewType view)
{
  view_ = view;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ViewType AbstractScatterData::view() const
{
  return view_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

size_t AbstractScatterData::size() const
{
  return size_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void AbstractScatterData::computeSize() 
{
  int size = 0;
  start();
  while (!done()) {
    ++size;
  };
  size_ = size;
    
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AbstractScatterData::Limits AbstractScatterData::limits() 
{
  // NOTE: the momentum variables have a different scale 

  AbstractScatterData::State s = start();
  double xmin   = s.state[0];
  double xmax   = xmin;

  double xpmin  = s.state[1];
  double xpmax  = xpmin;

  double ymin   = s.state[2];
  double ymax   = ymin;

  double ypmin  = s.state[3];
  double ypmax  = ypmin;

  double smin   = s.state[4];
  double smax   = smin;

  double dppmin = s.state[5];
  double dppmax = dppmin;

  while (!done()) { 
        xmin    = std::min(s.state[0],  xmin);
        xmax    = std::max(s.state[0],  xmax);
        xpmin   = std::min(s.state[1], xpmin);
        xpmax   = std::max(s.state[1], xpmax);
        ymin    = std::min(s.state[2],  ymin);
        ymax    = std::max(s.state[2],  ymax);
        ypmin   = std::min(s.state[3], ypmin);
        ypmax   = std::max(s.state[3], ypmax);
        smin    = std::min(s.state[4],  smin);
        smax    = std::max(s.state[4],  smax);
        dppmin  = std::min(s.state[5], dppmin);
        dppmax  = std::max(s.state[5], dppmax);
        s = next();
  };

  return Limits{ xmin,xmax, xpmin,xpmax, ymin,ymax, ypmin,ypmax,
		 smin,smax, dppmin,dppmax};
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PoincareScatterData::PoincareScatterData( std::shared_ptr<FILE>& fp, ViewType view ) 
  : fp_(fp)
{
  view_ = view;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
size_t      PoincareScatterData::size() const
{
    return 0; // FIXME
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QPointF     PoincareScatterData::sample(size_t i) const
{
  return QPointF(); // FIXME
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QRectF    PoincareScatterData::boundingRect ()  const
{
  return QRectF() ; // FIXME
			
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 	 PoincareScatterData::setRectOfInterest (QRectF const &rect)
{
  // FIXME
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PoincareScatterData::State  PoincareScatterData::start() const
{
  idx_    = 0;
  eof_    = false;
  auto fp = fp_.get();
  rewind(fp);
  auto& state = state_.state; 
  int id;
  int lost;
  int status = fscanf(fp, " %lg %lg %lg %lg %lg %lg %d %d", &state[0], &state[1], &state[2], &state[3], &state[4], &state[5], &id, &lost); 
  state_.lost = lost;
  state_.id   = id;
  std::transform(&state_.state[0], &state_.state[6], &scaling_[0], &state_.state[0], std::multiplies<double>() );
  return state_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PoincareScatterData::State  PoincareScatterData::next()  const
{
  auto fp = fp_.get();
  auto& state = state_.state; 
  int id;
  int lost;
  int status = fscanf(fp, " %lg %lg %lg %lg %lg %lg %d %d", &state[0], &state[1], &state[2], &state[3], &state[4], &state[5], &id, &lost); 
  state_.lost = lost;
  state_.id   = id;
  std::transform(&state_.state[0], &state_.state[6], &scaling_[0], &state_.state[0], std::multiplies<double>() );
 
  return state_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool PoincareScatterData::done()  const
{
  char c = fgetc(fp_.get());
  if  ( c == EOF ) return true;
  fseek(fp_.get(), -1, SEEK_CUR);
  return false;
}


//.................................................................................................................

TrackingScatterData::TrackingScatterData(Bunch& bunch)
  : bunch_(bunch) 
{}

TrackingScatterData::TrackingScatterData(Coordinates const* coords, int N, ViewType view )
  : coords_(coords), N_(N) 
{
  setView(view);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


size_t      TrackingScatterData::size() const
{
    return N_; // FIXME
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QPointF     TrackingScatterData::sample(size_t i) const
{
  return QPointF(); // FIXME
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QRectF    TrackingScatterData::boundingRect ()  const
{
  return QRectF() ; // FIXME
			
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 	 TrackingScatterData::setRectOfInterest (QRectF const &rect)
{
  // FIXME
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackingScatterData::State TrackingScatterData::start() const
{
  idx_ = 0;
  std::copy(&coords_[idx_].c[0], &coords_[idx_].c[6], &state_.state[0]);
  // ... apply scale transformation 
  std::transform(&state_.state[0], &state_.state[6], &scaling_[0], &state_.state[0], std::multiplies<double>() );
  state_.lost = coords_[idx_].lost;
  return state_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackingScatterData::State     TrackingScatterData::next()  const
{
  ++idx_;
  std::copy(&coords_[idx_].c[0], &coords_[idx_].c[6], &state_.state[0]);
  // ... apply scale transformation 
  std::transform(&state_.state[0], &state_.state[6], &scaling_[0], &state_.state[0], std::multiplies<double>() );
  state_.lost = coords_[idx_].lost;
  return state_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool     TrackingScatterData::done()  const
{
  return idx_ >= N_;
}

