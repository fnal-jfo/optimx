//  =================================================================
//
//  FoilNew.h
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

#ifndef FOILNEW_H
#define FOILNEW_H


#include <Element.h>
#include <memory>

class Vavilov;
class Landau;
class Moliere;

struct FoilNew: public Element
{
  FoilNew(); 
  FoilNew( char const* nm, char const* fnm="");
  FoilNew( FoilNew const& o); 
  FoilNew& operator = (FoilNew const& rhs); 
 ~FoilNew(); 

 FoilNew*  clone() const { return new FoilNew(*this);}

  //void virtual  preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const;
  void preTrack( RMatrix_t<3>& frame, double ms,    double Enr0, int n_elem, TrackParam& prm, RMatrix& m1) const;

  int  virtual trackOnce( double ms,   double& Enr0,    int n_elem, int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v ) const;
  //int  virtual track( double ms, double& Enr0,  Coordinates& v, double& tetaY ) const; // track trajectory

  //  virtual RMatrix rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const;
  void toString( char* buf) const;
  void setParameters( int np, double attributes[], ... );


 private:

  double Tmax(double gma) const;                                   // maximum kinetic energy transfer to an electron 
  static double radLength(int Z, double A);
  //static double radLength( OptimX::MaterialType );
  void initialize(double bg, double gma);

  double A_;      // atomic mass  
  int    Z_;      // atomic number  
  double X0_;     // radiation length  
  double rho_;    // material density g/cm**3  
  double na_;     // atom density [atoms/cm**3] 
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


#endif // FOILNEW_H 

