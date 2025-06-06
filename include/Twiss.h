//  =================================================================
//
//  Twiss.h
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

#ifndef TWISS_H
#define TWISS_H

#include <complex>
#include <array>
#include <RMatrixFwd.h>
#include <Vector.h>
#ifdef _MSC_VER
#include <Structs.h>
#else
class Coordinates;
#endif

class Twiss;
std::ostream& operator<<( std::ostream& os, Twiss const& t);

class Twiss {

    friend  std::ostream& operator<<( std::ostream& os, Twiss const& t);

 public:   

  Twiss();
  Twiss( Twiss const& o);
  Twiss& operator=( Twiss              rhs);

  void eigenvectors(std::complex<double> ev[][4]) const;               // returns eigenvectors

  Twiss&   operator=( Vector_t<6,double> const& rhs);                         // sets dispersion component only
  explicit operator Vector_t<6,double>() const;                               // conversion operator returns dispersion as coordinate vector. 
     

  void btfTransform( RMatrix const& tm, std::complex<double> ev[][4]); // propagate lattice functions

  double BtX;
  double AlX; 
  double BtY; 
  double AlY; 

  double DsX; 
  double DsXp; 
  double DsY; 
  double DsYp; 

  double nuX; 
  double nuY;


};

//..........................................................................................................................


class Twiss4D;
std::ostream& operator<<( std::ostream& os, Twiss4D const& t4d);

Twiss4D operator+(Twiss4D const& lhs, Twiss4D const& rhs); 
Twiss4D operator-(Twiss4D const& lhs, Twiss4D const& rhs); 
Twiss4D operator*(double lhs, Twiss4D const& rhs); 

class Twiss4D {

   friend  std::ostream& operator<<( std::ostream& os, Twiss4D const& t4d);

 public:
   
  
   Twiss4D(); 
   Twiss4D(Twiss4D const& o);
   Twiss4D& operator=(Twiss4D rhs);

  Twiss4D&   operator=( Vector_t<6,double> const& rhs); // sets dispersion component only
  explicit   operator Vector_t<6,double>() const;       // conversion operator; returns dispersion as array. 

   double const norm();                           // norm for iteration convergence; 
   
   void betatronFromEV(std::complex<double> const ev[][4]);
   void eigenvectors(std::complex<double> ev[4][4]) const;

   double getU4Dp(double phase, int& ret) const;
   double getU4Dm(double phase, int& ret) const;

   //......................................................
   // NOTE: ORDERING OF THE MEMBERS BELOW MUST BE PRESERVED   
   // .....................................................

   // ***********
   double btx1;     
   double alfx1;
   double bty1;
   double alfy1; 
   double btx2;
   double alfx2;
   double bty2;
   double alfy2;
   double dx;         // dispersion
   double dxp;
   double dy;
   double dyp;
   double teta1;      // nu1 first  phase shift [rad]
   double teta2;      // nu2 second phase shift [rad]
   double u;          // complex phase factor   

   // ***********

   double qx;         // eigentune 1 [dimensionless] 
   double qy;         // eigentune 2 [dimensionless]

   double e1;
   double e2;         // emittances 
};

 

class Twiss6D {

 public:

   Twiss6D(); 
   Twiss6D(Twiss4D const& o);
   Twiss6D& operator=(Twiss6D rhs);
};



#endif
  
