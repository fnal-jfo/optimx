//  =================================================================
//
//  Quadrupole.cpp
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

#include <Constants.h>
#include <Coordinates.h>
#include <Structs.h>
#include <Element.h>
#include <RMatrix.h>
#include <TrackParam.h>
#include <limits>

using Constants::C_DERV1;
using Constants::C_CGS;
using Constants::PI;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Quadrupole::Quadrupole(const char* nm, char const* fnm) : Element(nm,fnm) {}

Quadrupole::Quadrupole(Quadrupole const& o) : Element(o) {}

Quadrupole::~Quadrupole()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Quadrupole::toString( char* buf) const
{
  Element::toString(buf);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Quadrupole* Quadrupole::split(int nslices) const
{
  Quadrupole* e = this->clone();
  e->L_ /= nslices;
  return e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix Quadrupole::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{

  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  double ks;
  double fi;
  
  RMatrix mi;
  mi.toUnity();

  mi[4][5]=L_/(gamma1*gamma1);

  if( fabs(G) < std::numeric_limits<double>::epsilon() ) {
    mi[0][1] = mi[2][3]= L_;
    return mi;
  }

  if(G >0){ // hor focusing, ver defocusing 
    ks  = sqrt( G / hr );
    fi = ks * L_;
    mi[0][0] = mi[1][1] = cos(fi);
    mi[0][1] = sin(fi)/ks;
    mi[1][0] = -ks*ks * mi[0][1];
    mi[2][2] = mi[3][3] = cosh(fi);
    mi[2][3] = sinh(fi)/ks;
    mi[3][2] = ks*ks * mi[2][3];
  }
  else   { // ver focusing, hor defocusing 
    ks  = sqrt( - G / hr );
    fi = ks*L_;

    mi[0][0] = mi[1][1] = cosh(fi);
    mi[0][1] = sinh(fi)/ks;
    mi[1][0] = ks*ks * mi[0][1];
    mi[2][2] = mi[3][3] = cos(fi);
    mi[2][3] = sin(fi)/ks;
    mi[3][2] = -ks*ks* mi[2][3];
  }

  //if(T != 0) tilt(T, &mi);
  if(T_ != 0)  mi = mi.similarOrtho(RMatrix::m_tilt(T_*PI/180.0)); // VERIFY SIGN OF ROTATION

  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix Quadrupole::rmatrixsc( double& alfap, double& Enr, double ms, double current, BeamSize& bs, double& tetaY, double dalfa, int st ) const
{
  
  // quad matrix with space/charge 

  if( fabs(current) < 1000.0 * std::numeric_limits<double>::epsilon() ) return rmatrix( alfap, Enr, ms, tetaY, dalfa, st );
   
  RMatrix mi;
  mi.toUnity();

  double P  = Enr+ms;
         P  = sqrt(P*P-ms*ms);
  double hr = P/C_DERV1;
  double bt = P/(Enr+ms);

  double gx = 0.0;
  double gy = 0.0;
  double gs = 0.0;

  GetBeamFocusing(current, bt, bs, gx, gy, gs);

  double dg = G*cos(T_*PI/90.);

  double Gx    = 0.0;
  double Gy    = 0.0;
  double rotat = 0.0;

  GetNormalizedGradientsNew(gx+dg, gy-dg, gs-G*sin(T_*PI/90.), Gx, Gy, rotat); // rotat in rd

  get2DMatrix(hr, Gx, L_, mi, Horizontal);
  get2DMatrix(hr, Gy, L_, mi, Vertical);

  mi = mi.similarOrtho(RMatrix::m_tilt(-rotat));

  return mi;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Quadrupole::setParameters( int np, double dat[], ... )
{
  double* rdat[] = { &L_, &G, &T_, &ofsX_, &ofsY_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  //L_    = dat[0]; // length in cm 
  //G     = dat[1]; // gradient in kG/cm
  //T_    = dat[2]; // roll angle in deg
  //ofsX_ = dat[3]; // h offset in cm 
  //ofsY_ = dat[4]; // v offset in cm

  B = S = 0.0;
  ofsX_ = dat[3];
  ofsY_ = dat[4];
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Quadrupole::setParameters( int np, std::vector<double> const& dat, ... )
{
  double* rdat[] = { &L_, &G, &T_, &ofsX_, &ofsY_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  //L_ = dat[0]; // length in cm 
  //G  = dat[1]; // gradient in kG/cm
  //T_ = dat[2]; // roll angle in deg
  // ofsX_ = dat[3] // h offset in cm
  // ofsY_ = dat[4] // v offset in cm

  B = S = 0.0;
  ofsX_ = dat[3];
  ofsY_ = dat[4];
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Quadrupole::trackOnce( double ms, double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		           RMatrix const& m1, Coordinates& v ) const
{

  double m00, m01, m10, m22, m23, m32, s,c, ss, cc, sc;
  double n00, n01, n02, n03, n10, n12, n22, n23, n32;
  double phi;

  double p     = prm.p0*(1.0+v[5]);  // p = p0 (1 + dp/p)  
  double delta = v[5];
   
  int status = 0; 
  if (status = backwardTest(prm, n_elem, n_turn, v )) return status;


  double Hr = p/C_DERV1; // brho  in kG-cm  
  double k1 = fabs(L_) < 1.0e-3 ? 0.0 : G/Hr;  // no fringe for very thin quads ***FIXME***    
  
  
  if ( fabs(G) < std::numeric_limits<double>::epsilon() )  {  // treat as a drift

    v[0] += L_*tan(v[1]);
    v[2] += L_*tan(v[3]);

    double vp  = C_CGS*p/sqrt( p*p*(1.0 + v[1]*v[1] + v[3]*v[3]) + ms*ms );
    v[4] += (vp - prm.vp0) * L_ / prm.vp0;

    if (status = transAmpTest(prm, n_elem, n_turn, v )) return status;
    return 0;
   }

  //------------------------------------------------------------------------------------------------------
  // Nonlinear upstream edge correction. The correction is a limit for a sharp field edge.
  // For more details, see 
  // G.E. Lee-Whiting, “Third Order Aberrations of a Magnetic Quadrupole Lens”, NIM-83, pp.232-244 (1970)
  // E. Forest, Beam Dynamics: A New Attitude and Framework  Harwood Academic Publishers (1988)
  // .....................................................................................................

  double x  = v[0];
  double xp = v[1];
  double y  = v[2];
  double yp = v[3];
  
  if (fringe_on) {

   v[0] +=  k1/(1.0+delta) * (1.0/12.0) * (x*x*x    + 3*y*y*x);
   v[1] +=  k1/(1.0+delta) * (1.0/4.0)  * (2*x*y*yp - (x*x+y*y)*xp);
   v[2] -=  k1/(1.0+delta) * (1.0/12.0) * (y*y*y    + 3*x*x*y);
   v[3] -=  k1/(1.0+delta) * (1.0/4.0)  * (2*y*x*xp - (y*y+x*x)*yp);
  }
  
  //------------------------------------------------------------------------------------------------------

  double ks;
  
  if( G > 0) { // hor focusing, ver defocusing 
  	ks   = sqrt( G / Hr );
  	phi = ks*L_;
  	m00 = cos(phi);
  	m01 = sin(phi)/ks;
  	m10 = -ks*ks*m01;
  	m22 = cosh(phi);
  	m23 = sinh(phi)/ks;
  	m32 = ks*ks*m23;
  }
  else { // ver focusing, hor defocusing 
  	ks  = sqrt( - G / Hr );
  	phi = ks*L_;
        m00 = cosh(phi);
  	m01 = sinh(phi)/ks;
  	m10 = ks*ks*m01;
  	m22 = cos(phi);
  	m23 = sin(phi)/ks;
  	m32 = -ks*ks*m23;
  }

  // apply the X,Y offset 

  v[0] -= ofsX_;
  v[2] -= ofsY_;

   
  // perform element rotation and compute
  // the output vector using a matrix product
  // this is equivalent to 
  // x_o = [R^T M R] x_i

  if( fabs(T_) < 100*std::numeric_limits<double>::epsilon() ){
      x    =  m00*v[0]+m01*v[1];
      v[1] =  m10*v[0]+m00*v[1];
      v[0] =  x;
      x    =  m22*v[2]+m23*v[3];
      v[3] =  m32*v[2]+m22*v[3];
      v[2] =  x;
  } 
  else {
      phi = T_/180.*PI;
      s   = sin(-phi);
      c   = cos(phi);
      cc  = c*c;
      ss  = s*s;
      sc  = s*c;
      n00 = cc*m00 + ss*m22;
      n01 = cc*m01 + ss*m23;
      n02 = sc*(m22 - m00);
      n03 = sc*(m23 - m01);
      n10 = cc*m10 + ss*m32;
      n12 = sc*(m32-m10);
      n22 = cc*m22 + ss*m00;
      n23 = cc*m23 + ss*m01;
      n32 = cc*m32 + ss*m10;
      x   = v[0]; 		
      xp  = v[1];
      y   = v[2]; 		
      yp  = v[3];

      v[0] =  n00*x + n01*xp + n02*y + n03*yp;
      v[1] =  n10*x + n00*xp + n12*y + n02*yp;
      v[2] =  n02*x + n03*xp + n22*y + n23*yp;
      v[3] =  n12*x + n02*xp + n32*y + n22*yp;
  }

  //------------------------------------------------------------------------------------------------------
  // Nonlinear downstream edge correction. The correction is a limit for a sharp field edge.
  // For more details, see 
  // G.E. Lee-Whiting, “Third Order Aberrations of a Magnetic Quadrupole Lens”, NIM-83, pp.232-244 (1970)
  // E. Forest, Beam Dynamics: A New Attitude and Framework  Harwood Academic Publishers (1988)
  // .....................................................................................................

  if (fringe_on) {

    x  = v[0];
    xp = v[1];
    y  = v[2];
    yp = v[3];

    v[0] -=  k1/(1.0+delta) * (1.0/12.0) * (x*x*x    + 3*y*y*x);
    v[1] -=  k1/(1.0+delta) * (1.0/4.0)  * (2*x*y*yp - (x*x+y*y)*xp);
    v[2] +=  k1/(1.0+delta) * (1.0/12.0) * (y*y*y    + 3*x*x*y);
    v[3] +=  k1/(1.0+delta) * (1.0/4.0)  * (2*y*x*xp - (y*y+x*x)*yp);

  }

  //------------------------------------------------------------------------------------------------------

  v[0] += ofsX_;
  v[2] += ofsY_;

  double vp  = C_CGS*p/sqrt( p*p*(1.0 + v[1]*v[1] + v[3]*v[3]) + ms*ms );  // velocity
  v[4] += (vp/prm.vp0 - 1.0) * L_;
  
 done:	

   if (status = transAmpTest(prm, n_elem, n_turn, v )) return status;

  return 0;
}


