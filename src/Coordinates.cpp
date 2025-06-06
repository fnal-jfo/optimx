//  =================================================================
//
//  Coordinates.cpp
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

#include <Coordinates.h>

#include <utility> 
#include <algorithm> 
#include <array>
#include <algorithm>
#include <initializer_list>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double& Coordinates::operator[](int i)
{
  return c[i];
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Coordinates::operator[](int i) const
{
  return c[i];
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



Coordinates::Coordinates()
  : c({0.0, 0.0, 0.0, 0.0, 0.0, 0.0}), lost(0),
      pid(-1), nelem(-1), npass(0), weight(1.0) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Coordinates::Coordinates( std::initializer_list<double> lst)
{
  std::copy( lst.begin(), lst.end(), &c[0]);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
Coordinates::Coordinates( Coordinates const& o)
  : c(o.c),
    lost(o.lost),
    pid(o.pid),
    nelem(o.nelem),
    npass(o.npass),
    weight(o.weight) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Coordinates::Coordinates( Coordinates&& o)
  :Coordinates() // initialize via default constructor, C++11 only
{
  c.swap(o.c);
  std::swap(lost,  o.lost);
  std::swap(pid,   o.pid);
  std::swap(nelem, o.nelem);
  std::swap(npass, o.npass);
  std::swap(weight,o.weight);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Coordinates& Coordinates::operator=( Coordinates rhs)
{
  c.swap(rhs.c);
  std::swap(lost,  rhs.lost);
  std::swap(pid,   rhs.pid);
  std::swap(nelem, rhs.nelem);
  std::swap(npass, rhs.npass);
  std::swap(weight,rhs.weight);

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Coordinates::renormalize(double ms, double ein, double eout)
{

  // renormalization after change in energy (acceleration)
  
  if ( ein == eout ) return;
  
  double capa = sqrt(sqrt((2.0*ein*ms + ein*ein)/(2.0*eout*ms+eout*eout)));

  c[0] *= capa;
  c[1] *= capa;
  c[2] *= capa;
  c[3] *= capa;
  c[5] *= capa*capa*(ms+eout)/(ms+ein);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Coordinates operator+( Coordinates const& lhs, Coordinates const& rhs)
{
   // coordinates addition 

  Coordinates result = lhs;
  for ( int i=0; i<6; ++i ) {
    result[i] += rhs[i];
  }
  return result;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Coordinates operator-( Coordinates const& lhs, Coordinates const& rhs)
{
   // coordinates subtraction 

  Coordinates result = lhs;
  for ( int i=0; i<6; ++i ) {
    result[i] -= rhs[i];
  }
  return result;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Coordinates operator*( double lhs, Coordinates const& rhs)
{
  // multiplication by a scalar 

  Coordinates result = rhs;
  for ( int i=0; i<6; ++i ) {
    result[i] *= lhs;
  }
  return result;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Coordinates operator*( Coordinates const& lhs, double rhs)
{
 // multiplication by a scalar 

  return rhs*lhs;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

