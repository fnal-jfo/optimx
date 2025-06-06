//  =================================================================
//
//  Aperture.cpp
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
#include <TrackParam.h>
#include <Coordinates.h>

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;


Aperture::Aperture(const char* nm, char const* fnm)
  : Element(nm,fnm)
{} // 'H'

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Aperture::Aperture(Aperture const& o)
 : Element(o)
{} // 'H'

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Aperture::toString(char* buf) const
{
   switch(N) {
     case 1:
       sprintf(buf," Ellipse:   Ax[cm]=%g  Ay[cm]=%g  OffsetX[cm]=%g OffsetY[cm]=%g  Tilt[deg]=%g",
                  			S, A, B, G, T_);
       break;
     case 2:
       sprintf(buf," Hyperbole: Ax[cm]=%g  Ay[cm]=%g  OffsetX[cm]=%g OffsetY[cm]=%g  Tilt[deg]=%g",
                  			S, A, B, G, T_);
       break;
     default:
       sprintf(buf," Rectang.   Ax[cm]=%g  Ay[cm]=%g  OffsetX[cm]=%g OffsetY[cm]=%g  Tilt[deg]=%g",
                  			S, A, B, G, T_);
       break;
    }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix Aperture::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
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

void Aperture::setParameters( int np, double dat[], ... )
{

  L_ = 0.0;

  double Ndum = 0.0;
  double* rdat[]  = { &S, &A, &Ndum, &B, &G, &T_ }; 
  for (int i=0; i<np; ++i) {
    *(rdat[i]) = dat[i];
  }
  if ( Ndum != 0.0 ) N = Ndum; 
  
  //S  = dat[0];
  //A  = dat[1];
  //N  = dat[2];
  //B  = dat[3];
  //G  = dat[4];
  //T_ = dat[5];


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Aperture::setParameters( int np, std::vector<double> const& dat, ... )
{

  L_ = 0.0;

  double Ndum = 0.0;
  double* rdat[]  = { &S, &A, &Ndum, &B, &G, &T_ }; 
  for (int i=0; i<np; ++i) {
    *(rdat[i]) = dat[i];
  }
  if ( Ndum != 0.0 ) N = Ndum; 
  
  //S  = dat[0];
  //A  = dat[1];
  //N  = dat[2];
  //B  = dat[3];
  //G  = dat[4];
  //T_ = dat[5];


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Aperture::trackOnce( double ms, double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		         RMatrix const& m1, Coordinates& v) const
{


  double p = prm.p0*(1.0 + v.c[5]);

  if( p < 0.0) {
    v.lost  = 2;
    v.nelem = n_elem+1;
    v.npass = n_turn;
    return 1;
  }

  double Hr = p/C_DERV1; // brho  =  p/ec   p is in MeV/c
  double s = 0.0;
  double c = 0.0;
  double x = 0.0;
  double y = 0.0;
   
  int ret = 0;

  if( (S <= 0.0) || (A <= 0.0) )  goto done;
  
  s = sin(PI*T_/180.);
  c = cos(PI*T_/180.);
  x =  (v[0] - B)*c + (v[2] - G)*s;
  y = -(v[0] - B)*s + (v[2] - G)*c;

  switch (N) {
    case 1:  // Ellipse
      x /= S;
      y /= A;
      if( (x*x+y*y) > 1.0 )  ret=1;
      break;
    case 2:  // Hyperbole
      if( (fabs(2.*x*y)>(S*S)) || (x*x+y*y>A*A)) ret=1;
      break;
    default: // Rectangle
      if( (fabs(x) > S) || (fabs(y) > A)) ret=1; 
      break;
  }

  if(ret){

     v.lost  = 2;
     v.nelem = n_elem+1;
     v.npass = n_turn;

     return 2;
  }

done:	

  if( fabs(v[0])>1000.0 || fabs(v[2])>1000.0) {
    v.lost  = 1;
    v.nelem = n_elem+1;
    v.npass = n_turn;
    return 2;
  }
   
   return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Beamline* Aperture::splitnew(int nslices) const  // return a sliced element as a beamline 
{
  Beamline* bml = new Beamline();
  Aperture* e = this->clone();
  e->L_      /= nslices;
  e->slices_  = nslices;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix   Aperture::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{
  return Element::rmatrix( frame, energy, ms, st);
}
