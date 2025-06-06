//  =================================================================
//
//  Coordinates.h
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

#ifndef COORDINATES_H
#define COORDINATES_H

#include <complex>
#include <array>
#include <initializer_list>


struct Coordinates {
  
  Coordinates();
  Coordinates( std::initializer_list<double> lst);
  Coordinates( Coordinates const& o);
  Coordinates( Coordinates&& o);
  Coordinates& operator=( Coordinates rhs);

  void renormalize(double ms, double ein, double eout);   // normalize after energy gain

  double& operator[](int);  
  double  operator[](int) const;  

  std::array<double,6> c;    // state vector 

  short int     lost   = 0;   // lost = 0 - no loss; lost=1 coordinate more than 10 m; lost=2 lost on aperture
  int           pid    = 0;   // particle id
  int           nelem  = 0;   // element index 
  int           npass  = 0;   // turn number
  double        weight = 1.0; 
};

Coordinates operator*(  double lhs,              Coordinates const& rhs); 
Coordinates operator*(  Coordinates const& lhs,  double rhs); 
Coordinates operator+(  Coordinates const& lhs,  Coordinates const& rhs); 
Coordinates operator-(  Coordinates const& lhs,  Coordinates const& rhs); 

#endif // COORDINATES_H
