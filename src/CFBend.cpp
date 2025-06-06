//  =================================================================
//
//  CFBend.cpp
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

#include <Element.h>
#include <Constants.h>
#include <cmath>
#include <memory>
#include <limits>
#include <TrackParam.h>
#include <Coordinates.h>

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;
using Constants::C_CGS;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CFBend::CFBend( char const* nm, char const* fnm) // 'B', 'D'
  : Element(nm,fnm)
{} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CFBend::CFBend( CFBend const& o) // 'B', 'D'
  : Element(o)
{} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix CFBend::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{
 
  //--------------------------------------------------
  // For reference see:
  // ------------------
  // 
  // Handbook of Accelerator Physics and Engineering
  // Section 2.2 Optics and Lattices by K. Brown 
  //
  // NOTE:
  // ----
  // There is  a misktake in the handbook !!!
  // We should have 
  //  mi[0][5] =  - mi[4][1]  [ mi[0][5] > 0 for k_x = 0  
  //  mi[1][5] =  - mi[4][0]  [ mi[1][5] > 0 for k_x = 0
  
  // -----
  // Note:
  // -----
  //
  // K. Brown matrices use the path length difference
  // (delta l) as coordinate . This is a good choice for
  // ultra-relativistic  particles. In general, one must
  // take into account the relative difference in velocity for a particle
  // with momentum dp/p. This difference is dv/v = 1/gma**2
  // OptiM uses (Delta s), the distance s-s0 between a particle and
  // the reference particle, measured along the reference orbit as
  // coordinate. This means that if the path length difference for
  // a particle with dp/p >0  is positive, at ultra-relativistic
  // energy gma --> infty and (delta s) = - (delta l) i.e.
  // the off-momentum particle is **behind** the reference
  //
  //--------------------------------------------------

  double P      = sqrt(energy * (energy + 2*ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1.0 + energy/ms;

  RMatrix mi;
  mi.toUnity();

  double fi;    // phase advance 
  // alfap = 0.0; 
  double ks;


  // zero field and zero gradient -- behaves as a drift 
 
  if( fabs(B) < 100.0*std::numeric_limits<double>::epsilon() &&
      fabs(G) < 100.0*std::numeric_limits<double>::epsilon() ) {

      mi[0][1] = L_;
      mi[2][3] = L_;
      mi[4][5] = L_/(gamma1*gamma1);

      return mi;
  }	

  // vertical motion

  double ky2  = -(G/hr);
  
  if( fabs(ky2) < 100.0 *std::numeric_limits<double>::epsilon()) { // no ver focusing
      mi[2][3] = L_;
  }
  else if (ky2 >= 0.0) { // G > 0  vertical defocusing 
    ks  = sqrt(ky2);
    fi = L_*ks;
    mi[2][2] =  mi[3][3] = cos(fi);
    mi[2][3] =  (fabs(ks) < 1.0e-12 ? L_ : sin(fi)/ks);
    mi[3][2] = -ks*ks * mi[2][3];
  }
  else {
     ks  = sqrt( -ky2 );
     fi = L_*ks;
     mi[2][2] =  mi[3][3] = cosh(fi);
     mi[2][3] =  (fabs(ks) < 1.0e-12 ? L_ : sinh(fi)/ks);
     mi[3][2] =  ks*ks * mi[2][3];
  }
    
  // horizontal motion

  double ri = B / hr; // inverse curvature radius
  double kx2  = (G/hr) +  ri*ri;

  // *** some bending but no net hor focusing (note that this includes bending contribution)
  if (fabs(kx2) < 100.0 * std::numeric_limits<double>::epsilon() )  {
    mi[0][1] = L_;

    // longitudinal 

    mi[0][5] = 0.5 * L_ * L_ * ri;
    mi[4][1] = -mi[0][5];

    mi[1][5] = L_ * ri;
    mi[4][0] = -mi[1][5];

    mi[4][5] = mi[1][5] * mi[1][5] * L_ /6.;
    mi[4][5] = L_/(gamma1*gamma1) -mi[4][5];

  }

  else {

    if (kx2 >= 0.0) {  // G>0 ==> hor focusing
      ks  = sqrt(kx2);
      fi  = L_ * ks;
      mi[0][0] =  mi[1][1] = cos(fi);
      mi[0][1] = (fabs(ks) < 1.0e-12 ? L_ : sin(fi)/ks);
      mi[1][0] = -ks*ks*mi[0][1];
    }
    else {
      ks = sqrt(-kx2);
      fi = L_ * ks;
      mi[0][0] = mi[1][1] = cosh(fi);
      mi[0][1] = (fabs(ks) < 1.0e-12 ? L_: sinh(fi)/ks);
      mi[1][0] = ks*ks*mi[0][1];
    }

    // longitudinal motion
    //---------------------
  
    mi[0][5] =   (ri/kx2) * (1.0 - mi[0][0]);
    mi[1][5] =   ri*mi[0][1];

    mi[4][0] =  -mi[1][5];
    mi[4][1] =  -mi[0][5];
    mi[4][5] =  L_*(ri*ri)/kx2* ( 1.0- mi[0][1]/L_ );
    mi[4][5] =  L_/(gamma1*gamma1) - mi[4][5];
  }


  if( (T_ != 0) || (tetaY != 0) ) {  // alfa[deg], fi[rad], tetaY[deg]

  // NOTE: tiltb expects angles in degrees, **except for the second one** ([rd])
  // this is very confusing and needs to be fixed.
  // std::cerr << " T_     = " << T_ << std::endl;
  // std::cerr << " dalfa = " << dalfa << std::endl;

    // tiltb :  b stands for magnetic field orientation  
    alfap = tiltb(T_ + dalfa, (B*L_)/hr, tetaY, &mi);
  }

  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix CFBend::rmatrixsc(double& alfap, double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st ) const
{
  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;

  double c;
  double fi;
  
  RMatrix mi;
  mi.toUnity();

  double gx = 0.0;
  double gy = 0.0;
  double gs = 0.0;
  
  GetBeamFocusing(current, bt, bs, gx, gy, gs);

  double ri =  B / hr;
  double s  = -(T_ + dalfa)*(PI/180.0); // in rd 
  double dg = ( G + 0.5*B*ri)*cos(2.0*s);

  double Gx0 = 0.5*(B*B)/hr + dg;
  double Gy0 = 0.5*(B*B)/hr - dg;
  double Gs0 = ( G + 0.5*B*ri )*sin(2.0*s);

  double Gx = 0.0;
  double Gy = 0.0;
  double rotat = 0.0;
  
  // rotat is in rad 
  GetNormalizedGradientsNew(gx+Gx0, gy+Gy0, gs+Gs0, Gx, Gy, rotat);

  // set up the hor and ver 2D Matrices 

  get2DMatrix(hr, Gx, L_, mi, Horizontal);
  get2DMatrix(hr, Gy, L_, mi, Vertical);

  s -= rotat;
  
  double cs = cos(s);
  double sn = sin(s);

  // horizontal motion ...

  double kx2 = Gx/hr;
  double ks;

  if( fabs(kx2) < 100.0*std::numeric_limits<double>::epsilon() ) {
    mi[0][5] = 0.5 *(L_*L_)* ri*cs;
    mi[4][1] = -mi[0][5];

    mi[1][5] = L_ * ri * cs;
    mi[4][0] = -mi[1][5];
  
    mi[4][5] = (L_*L_*L_) * (ri*ri) * (cs*cs) /6.0;
  }

 else { 

    mi[0][5] = (ri/kx2)*(1.0-mi[0][0]) * cs;
    mi[4][1] = -mi[0][5];

    mi[1][5] = ri*mi[0][1] *cs;
    mi[4][0] = -mi[1][5];
      
    mi[4][5] =  L_*(ri*ri)/kx2 *(1.0 - mi[0][1]/L_) * (cs*cs) ;
  }

 // vertical motion ...

  double ky2 = Gy/hr;
 
 if( fabs(ky2) < 100 * std::numeric_limits<double>::epsilon() ) {
    mi[2][5] = 0.5 * (L_*L_) * ri * sn;
    mi[4][3] = -mi[2][5];
    mi[3][5] = L_ * ri * sn;
    mi[4][2] = -mi[3][5];
    mi[4][5] += (L_*L_*L_) * (ri*ri) * (sn*sn) / 6.0;
 }
 else {
    mi[2][5]  =  (ri/ky2)*(1.0-mi[2][2]) * sn;
    mi[4][3]  = -mi[2][5];
    mi[3][5]  =  ri*mi[2][3] * sn;
    mi[4][2]  = -mi[3][5]; 
    mi[4][5] +=  L_*(ri*ri)/ky2 *(1.0 - mi[0][1]/L_) * (sn*sn);
 }

  mi[4][5] = L_/(gamma1*gamma1) - mi[4][5];

  // NOTE: tiltsp expects arg 1 in deg
  //                      arg 2 in rad
  //                      arg 3 in deg
  //                      arg 4 in deg
  // this is very confusing and needs to be addressed.
  alfap = tiltsp(T_ + dalfa,  B*L_ / hr, -rotat*(180/PI), &tetaY,  &mi);  

  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CFBend::setParameters( int np, double dat[], ... )
{

  double* rdat[] = { &L_, &B, &G, &T_, &S};
  for (int i=0; i<np; ++i) {
    *(rdat[i]) = dat[i];
  };
   
  //L_ = dat[0]; // length [cm]
  //B  = dat[1]; // magnetic field [kG]
  //G  = dat[2]; // magnetic gradient [kG/cm]
  //T_ = dat[3]; // tilt angle [deg] 
  //S  = dat[4]; // tilt error [deg]
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CFBend::setParameters( int np, std::vector<double> const& dat, ... )
{

  double* rdat[] = { &L_, &B, &G, &T_, &S};
  for (int i=0; i<np; ++i) {
    *(rdat[i]) = dat[i];
  };
   
  //L_ = dat[0]; // length [cm]
  //B  = dat[1]; // magnetic field [kG]
  //G  = dat[2]; // magnetic gradient [kG/cm]
  //T_ = dat[3]; // tilt angle [deg] 
  //S  = dat[4]; // tilt error [deg]
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CFBend::toString(char* buf) const
{
  //***FIXME***
  double fi = 0.0; //L_*B/Hrt*180./PI;
  double alfa_exit = 0.0;
  
  sprintf(buf, "%g %g %g %g %g %g", B, G, S, T_, alfa_exit, fi);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CFBend::preTrack(double ms, double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1 ) const
{

   prm.p0   = sqrt(2.*ms*Enr0+Enr0*Enr0);
   prm.Hr0  = prm.p0/C_DERV1;
   prm.vp0  = C_CGS*prm.p0/sqrt(prm.p0*prm.p0+ms*ms);
   
   prm.ri0  = B / prm.Hr0;                    // ri0 = 1/rho is the curvature  [cm]

   if(B !=0) prm.R0 = 1.0/prm.ri0;            //  Ri0 = 1/ri0 i the curvature radius [cm] 

   prm.phi          = L_*prm.ri0;             // bend angle in the local hor plane [rad] 

   // calculation of the exit tilt angle ( angle w/r to global Y axis ?)  

   double alfa = T_/180.*PI;    // magnet tilt around s-axis
   double teta = tetaY/180.*PI; // angle w/respect to global Y (around X-axis) 
   
   // (nx, ny, nz) :  unit vector along the trajectory 
   //----------------------
   	
   double nx  = -cos(alfa)*sin(prm.phi);
   double ny  =  sin(teta)*cos(prm.phi) - cos(teta)*sin(alfa)*sin(prm.phi);
   double nz  =  cos(teta)*cos(prm.phi) + sin(teta)*sin(alfa)*sin(prm.phi);

   double ns  =  sqrt(nx*nx + nz*nz);  // component in Hor-plane ?

   // (bx by bz)   magnetic field orientation ????  
   //-----------------------------------------------
   	
   double bx  = -sin(alfa);            
   double by  =  cos(alfa)*cos(teta);
   double bz  = -cos(alfa)*sin(teta);

   double c   = -bx*nx*ny/ns + by*ns - bz*ny*nz/ns;  
   double s   = -bx*nz/ns    + bz*nx/ns;

   double alfap = atan2(s,c);

   // cos and sin for rotation
   //-------------------------
   	
   prm.cb1 =  cos(alfa);   // input rotation
   prm.sb1 = -sin(alfa);
   	
   prm.cb2 =  cos(alfap);  // output rotation
   prm.sb2 =  sin(alfap);

   prm.sgn = (B >= 0.0) ? 1.0 : -1.0;
   prm.sfi = sin(prm.phi);
   prm.cfi = cos(prm.phi);

   m1  = rmatrix(alfap, Enr0, ms, tetaY, 0.0, 3);
   
}
       
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CFBend::trackOnce( double ms, double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		       RMatrix const& m1, Coordinates& v) const
{

  int status = 0; 
  if ( (status = backwardTest(prm, n_elem, n_turn, v)) ) return status;

  double p = prm.p0*(1.0+v[5]);  // p = p0 (1 + dp/p)  
  double Hr = p/C_DERV1; // brho  =  p/ec   p is in MeV/c   

  if( fabs(B) < std::numeric_limits<double>::epsilon() )  { // no bending, just a quadrupole 
    std::unique_ptr<Quadrupole> q(new Quadrupole("qdummy") );
    q->length(L_);
    q->G = G;
    return q->trackOnce( ms, Enr0, n_elem, n_turn, prm, m1, v);
  }

  // upstream rotation of the coordinate system about the propagation axis 

  double x  = prm.cb1*v[0] - prm.sb1*v[2];
  double y  = prm.sb1*v[0] + prm.cb1*v[2];
  double xp = prm.cb1*v[1] - prm.sb1*v[3];
  double yp = prm.sb1*v[1] + prm.cb1*v[3];
 
 
 // THIS CODE NEEDS SOME WORK ... IT IS LIKELY BROKEN  !!!!
 //........................................................................................................

  char* envoverride = std::getenv("OPTIMX_CFBEND_TRACK_WITH_MATRIX");
  if (envoverride) {  // env override: always use matrix  
  
      v.c = m1*v.c;
  }
  else { 
    
    if( fabs(G) < 10*std::numeric_limits<double>::epsilon() ) {    // an ideal dipole with zero gradient
   
     double R  = prm.R0*(1.+v[5])*cos(yp);
     double s  = sin(xp);
     double c  = cos(xp);

     double xc = R*s;
     double yc = (prm.R0 + x) - R*c;
     double sc = xc*prm.cfi-yc*prm.sfi;

     double Rtilda = R*R-sc*sc;

     if(Rtilda < 0.0) {
       v.lost  = 2;
       v.nelem = n_elem+1;
       v.npass = n_turn;
       goto done;
     }

     Rtilda = prm.sgn*sqrt(Rtilda) + xc*prm.sfi +yc*prm.cfi;
     double xf     =  Rtilda -prm.R0;
     double xpf    =  prm.phi - atan((Rtilda*prm.sfi-xc)/(Rtilda*prm.cfi-yc));

     double Lloc   =  R*(prm.phi + xp - xpf); // Local L_ 

     double  yf     =  y + Lloc*sin(yp);
         //  ypf is unchanged.   


     // downstream rotation of the coordinate system about the reference trajectory axis
     
     v[0]  = prm.cb2*xf  - prm.sb2*yf;  // x
     v[2]  = prm.sb2*xf  + prm.cb2*yf;  // y
     v[1]  = prm.cb2*xpf - prm.sb2*yp;  // xp
     v[3]  = prm.sb2*xpf + prm.cb2*yp;  // yp

     // This cannot be right. It fails to account for the change in path length
     double vp    = C_CGS*p/sqrt(p*p+ms*ms)*cos(yp);
     // v[4] += (vp/prm.vp0 - 1.0 )*Lloc;
     // probably should be  ??? 
     v[4] += (vp/prm.vp0 - 1.0 )*L_ - (Lloc-L_); // [(dv/v) - dL_/L_ ] * L_  = dv/v*L_ - dL_ 
  }

   else {          // combined functions dipole
     v.c = m1*v.c;
   }
 } // if (envoverride) 

 // .....................................................................................
    
   if (TiltErr_ != 0.0) {  // simple formulas to describe the kick due to dipole tilt

     double c = -cos(T_/180.*PI)*PI*TiltErr_/180.*L_/prm.R0;
     double s  = sin(T_/180.*PI)*PI*TiltErr_/180.*L_/prm.R0;

     v[0] += 0.5*L_*s;
     v[1] += s;
     v[2] += 0.5*L_*c;
     v[3] += c;
   }

 done:

   if ( (status = transAmpTest(prm, n_elem, n_turn, v)) ) return status;

   return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  CFBend::track( double ms, double& Enr0,  Coordinates& v, double& tetaY ) const // track trajectory
{

  // NOT USED FOR NOW 
  return 0; // error code 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

