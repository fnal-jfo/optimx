//  =================================================================
//
//  CFBendNew.cpp
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
#include <Losses.h>
#include <Constants.h>
#include <cmath>
#include <memory>
#include <limits>
#include <TrackParam.h>

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;
using Constants::C_CGS;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CFBend::propagate( double hr, double ms, RMatrix_t<3>& W ) const
{

  //std::cout << "CFBend::propagate( double hr, double ms, RMatrix_t<3>& W ) const" << std::endl;
  //................................................................................
  // This function only propagates the coordinate transformation (the local
  // coordinate system). OPTIM needs this to determine the rotation required at the
  // bend exit to bring the local x axis in back to the X-Z plane.   
  // ...............................................................................

  // NOTE: only bending elements alter the orientation of the local (Frenet-Serret) axes.
  //       If an element is rotated around the s-axis, a transformation is applied upon entering
  //       and **undone** upon exiting the element.    

  if( fabs(B) < std::numeric_limits<double>::epsilon() ) {
   // No bending; do nothing.
   // Note that we do not update position vector  
   return;  
  }

  double r   =  hr / B;
  double psi =  T_ * (PI/180.0);   // rotation around s 
  double fi  =  L_ * (B / hr);     // bend angle

  double sf = sin(fi);
  double cf = cos(fi);

  double si = sin(psi);
  double ci = cos(psi);


  RMatrix_t<3>  S   = { {   cf,     0.0,     sf    },                   
                        {  0.0,     1.0,     0.0   },
                        {  -sf,     0.0,      cf   } };

  
         
  // if the element is not rotated about the local z-axis, just apply
  // the bend transformation and return.

  if ( fabs(psi) < 1.0e-9 ) {
    W = W *S.transpose();
    //W = W *S;
    return;
  }
  
  RMatrix_t<3> T   = { {   ci,    -si,     0.0  },          // rotation around bend angle 
                       {   si,     ci,     0.0  },
                       {  0.0,     0.0,    1.0  } };



  W = W *T*S.transpose();
  //W = W *T*S;


  return;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CFBend::propagate( double hr, double ms, RMatrix_t<3>& W, Vector_t<3>& V ) const
{
  //std::cout << "CFBend::propagate( double hr, double ms, RMatrix_t<3>& W, Vector_t<3>& V ) const" << std::endl;
  // NOTE: only bending elements alter the orientation of the local (Frenet-Serret) axes.
  //       When an element is rotated around the s-axis, a transformation is applied before
  //       and **undone** after exiting the element.    


  if( fabs(B) < std::numeric_limits<double>::epsilon() ) {
    // If there is no bending: we call the generic element version of the propagate
    // function. This is necessary to avoid having to compute the bending radius
    // which then becomes infinite. 
    Element::propagate(hr, ms, W, V);
    return;
  }

  //double psi_in = atan2( W[1][0], W[1][1]);
  
  double r   =  hr / B;
  //double psi =  -psi_in + T_ * (PI/180.0);   // rotation around s  
  double psi =  T_ * (PI/180.0);             // rotation around s  

  double fi  =  L_ * (B / hr);               // bend angle

  double sf = sin(fi);
  double cf = cos(fi);

  double si = sin(psi);
  double ci = cos(psi);


  RMatrix_t<3>  S   = { {   cf,     0.0,     sf    },      
                        {  0.0,     1.0,     0.0   },
                        {  -sf,     0.0,      cf   } };

  
         
  Vector_t<3>  R   =   {  r*(cf-1),  0.0,   r*sf  };



  RMatrix_t<3> T   = { {   ci,    -si,     0.0  },
                       {   si,     ci,     0.0  },
                       {  0.0,     0.0,    1.0  } };


  R = T*R;
  V = V +  W * R;

 // To get the MADX coordinate system, exit here
 //................................................................................................

  // if (use_mad_frame_) {
        std::cout << "use_mad_frame_ = " << use_mad_frame_ << std::endl;
        W = W *T*S*T.transpose();
       return;
       //}
       //else {
       // W = W *T*S.transpose();
}
 
 //..............................................................................................
#if 0    
   
  //................................................................................................
  // What follows is specific to OPTIMX. 
  // OPTIMX defines the local x-axis so that it always lie in the global X-Z plane 
  
  // To bring x back in the global X-Z plane,  we determine and then "undo" the angle psi betweeen
  // the local x axis and the X-Z plane at the element exit.

  // we assume that cos(phi) ( aka cos(theta_y) is always positive
  // this equivalent to restrict  theta_y to the interval  -pi/2 < theta_y <pi/2 
  //................................................................................................


  double sp = W[1][2];          // sin(phi)
  double cp = sqrt(1.0 -sp*sp); // cos(phi) assumed always positive

  si =  W[1][0]/cp;                           // sin(psi)
  ci =  copysign(sqrt(1.0 -si*si), W[1][1]);  // cos(psi) has the sign of W[1][1]

  
  T[0][0] =  ci;
  T[0][1] = -si;   
  T[1][0] =  si;
  T[1][1] =  ci;
 

  W =   W*T; // apply inverse rotation upon exit
#endif 
  //.....................................................................................................
#if 0
  si =  sin(T_ * (PI/180.0)); 
  ci =  cos(T_ * (PI/180.0)); 

  T[0][0] = ci;
  T[0][1] = -si;
  
  T[1][0] = si;
  T[1][1] = ci;
  
  W =   W*T.transpose(); // apply inverse rotation upon exit

}
#endif
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix CFBend::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
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


  // zero field -- behaves as a drift 
  // BUG : this does not work when G != 0.0.0 

  if( fabs(B) < 100.0 * std::numeric_limits<double>::epsilon() &&
      fabs(G) < 100.0 * std::numeric_limits<double>::epsilon() )
    {
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
     mi[2][3] =  (fabs(ks) < 1.0e-12 ? L_: sinh(fi)/ks);
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
      mi[0][1] =  (fabs(ks) < 1.0e-12 ? L_: sin(fi)/ks);
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


     double psi       = T_*(PI/180);

   
     double si  =  sin(psi);
     double ci  =  cos(psi);


  
     RMatrix T      = { { ci,    0.0,  si,   0.0,  0.0,  0.0 },
                        { 0.0,   ci,   0.0,  si,   0.0,  0.0 },
                        {-si,    0.0,  ci,   0.0,  0.0,  0.0 },
                        { 0.0,   -si,  0.0,  ci,   0.0,  0.0 },
                        { 0.0,   0.0,  0.0,  0.0,  1.0,  0.0 },
                        { 0.0,   0.0,  0.0,  0.0,  0.0,  1.0 } };

     propagate( hr, ms, frame);

     double psi_out   =  -atan2( frame[1][0], frame[1][1] );// << NOTE SIGN CHANGE
     double so        =   sin(psi_out);
     double co        =   cos(psi_out);

     RMatrix Tout     = { { co,  0.0,   so,   0.0,   0.0,  0.0 },
                          { 0.0,  co,  0.0,    so,   0.0,  0.0 },
                          {-so,  0.0,   co,   0.0,   0.0,  0.0 },
                          { 0.0, -so,  0.0,    co,   0.0,  0.0 },
                          { 0.0,   0.0,  0.0,  0.0,  1.0,  0.0 },
                          { 0.0,   0.0,  0.0,  0.0,  0.0,  1.0 } };
            
     RMatrix_t<3> T3out  = { { co,     so,   0.0 },
                             {-so,     co,   0.0 },
                             { 0.0,   0.0,   1.0 } };
              
    mi     = Tout*mi*T; 
    frame  = frame * T3out.transpose(); 

    return mi;
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CFBend::preTrack(RMatrix_t<3>& frame, double ms, double Enr0, int n_elem, TrackParam& prm, RMatrix& m1 ) const
{

   prm.p0   = sqrt(2.*ms*Enr0+Enr0*Enr0);
   prm.Hr0  = prm.p0/C_DERV1;
   prm.vp0  = C_CGS*prm.p0/sqrt(prm.p0*prm.p0+ms*ms);
   
   prm.ri0  = B / prm.Hr0;                     // rho 

   if(B !=0) prm.R0 = 1.0/prm.ri0;             // R0 = 1 / rho = curvature

   double alfa  = prm.phi = L_*prm.ri0;        // bend angle in the local hor plane  

   // calculation of the exit tilt angle ( angle w/r to global Y axis ?)  

   double psi =     T_/180.*PI;    // magnet tilt around s-axis
   

   double sa = sin(alfa);
   double ca = cos(alfa);
  
   double psi_in =  psi - atan2( frame[1][0],frame[1][1]); // check sign 
   double si = sin(psi_in);
   double ci = cos(psi_in);
   
   RMatrix_t<3>  T   = { {  ci,     -si,      0.0    },      
                         {  si,      ci,      0.0    },
                         {  0.0,    0.0,      1.0    } };

   RMatrix_t<3>  S   = { {  ca,     0.0,      sa    },      
                         {  0.0,     1.0,     0.0   },
                         { -sa,     0.0,      ca    } };


   frame = frame * T * S.transpose();
   
   double psip = atan2( frame[1][0],frame[1][1]);

   // cos and sin for rotation
   //-------------------------
   	
   prm.sb1 = -si; // check sign 
   prm.cb1 =  ci;
   	
   prm.sb2 =  sin(psip); // check sign 
   prm.cb2 =  cos(psip); 

   prm.sgn = (B >= 0.0) ? 1.0 : -1.0;
   prm.sfi = sin(alfa);
   prm.cfi = cos(alfa);

   m1  = rmatrix(frame, Enr0, ms, 3);
}
       
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0
int CFBend::trackOnce( double ms, double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		       RMatrix const& m1, Coordinates& v, LossStruct *loss) const
{

  int status = 0; 
  if (status = backwardTest(prm, n_elem, n_turn, v, loss)) return status;

  double p = prm.p0*(1.0+v[5]);  // p = p0 (1 + dp/p)  
  double Hr = p/C_DERV1; // brho  =  p/ec   p is in MeV/c   

  if( fabs(B) < std::numeric_limits<double>::epsilon() )  { // no bending, just a quadrupole 
    std::unique_ptr<Quadrupole> q(new Quadrupole("qdummy") );
    q->length(L_);
    q->G = G;
    return q->trackOnce( ms, Enr0, n_elem, n_turn, prm, m1, v, loss);
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
  
      v = m1*v;
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
       loss->lost=2;
       loss->nelem=n_elem+1;
       loss->npass=n_turn;
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
     v = m1*v;
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

   if (status = transAmpTest(prm, n_elem, n_turn, v, loss)) return status;

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
#endif

