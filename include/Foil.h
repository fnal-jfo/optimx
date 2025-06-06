//  =================================================================
//
//  Foil.h
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

#ifndef FOIL_H
#define FOIL_H


#include <Element.h>
#include <memory>

class Vavilov;
class Landau;
class Moliere;

struct Foil: public Element
{
  Foil(); 
  Foil( char const* nm, char const* fnm="");
  Foil( Foil const& o); 
  Foil& operator = (Foil const& rhs); 
 ~Foil(); 

 Foil*  clone() const { return new Foil(*this);}

  //void virtual  preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const;
  void  preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const;

  int  virtual trackOnce( double ms,   double& Enr0,    int n_elem, int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v ) const;
  //int  virtual track( double ms, double& Enr0,  Coordinates& v, double& tetaY ) const; // track trajectory

  //  virtual RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const;
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );


 private:

  double radiation_length( int Z, double A);   
		 
  double Tmax(double gma) const;                                   // maximum kinetic energy transfer to an electron 
  static double radLength(int Z, double A, double rho);
  //static double radLength( OptimX::MaterialType );
  void initialize(double bg, double gma);

  double dm_;     // foil thickness in g/cm**2  
  double s_;      // foil thickness in cm 
  double Theta0_; // is this correct ??? 
  double Atheta_; // MeV  What is this  ??? 
  double a0_;     // cm   What is this  ??? 
  double I0_;     // ionization energy
  double dE_;     // average energy loss (WARNING: particle energy dependent)

  mutable std::shared_ptr<Vavilov> vavilov_;
  mutable std::shared_ptr<Moliere> moliere_;
  mutable std::shared_ptr<Landau>  landau_;

  //  static std::random_device                     dev_;           // random device to initialize seed
  //static std::mt19937                           rng_;           // rng_(dev());    
  //static std::uniform_real_distribution<double> dist_;          // dist_(0.0, 1.0) distribution in range [0.0, 1.0]

};


#endif // FOIL_H 

#if 0 
//----------------------------------------------
// Radiation length data
//----------------------------------------------

Material     Z    A     X0ρg/cm2  X0 [cm]  Ec [MeV]
Hydrogen     1    1.01    63      700000    350
Helium       2    4.00    94      530000    250
Lithium      3    6.94    83      156       180
Carbon       6   12.01    43       18.8      90
Nitrogen     7   14.01    38      30500     85
Oxygen       8   16.00    34      24000     75
Aluminium   13   26.98    24        8.9     40
Silicion    14   28.09    22        9.4      9
Iron        26   55.85    13.9      1.7     20.7
Copper      29   63.55    12.9      1.4     18.8
Silver      47   109.9     9.3      0.89    11.9
Tungsten    74  183.9      6.8      0.35    8.0
Lead        82  207.2      6.4      0.46    7.4
Air         7.3  14.4     37      30000     84
SiO2       11.2  21.7     27        12      57
Water       7.5  14.2     36        36      83
Kapton                    33.0      19.4
Concrete                  26.7      10.7
Beryllium  4     9.01               35.3
//---------------------------------------------------
#endif
