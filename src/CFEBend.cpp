//  =================================================================
//
//  CFEBend.cpp
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
#include <limits>
#include <Element.h>
#include <Constants.h>
#include <TrackParam.h>
#include <Coordinates.h>

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;
using Constants::C_CGS;

CFEBend::CFEBend(const char* nm, char const* fnm) // 'R'
  : Element(nm,fnm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CFEBend::CFEBend(CFEBend const& o) // 'R'
  : Element(o)
{} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CFEBend::toString(char* buf) const
{
  sprintf(buf,"%g %g  E[kV/cm]=%g  Ge[kV/cm**2]=%g", B, G, S, T_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CFEBend::setParameters( int np, double dat[], ... )
{

  double* rdat[] = { &L_, &B, &G, &S, &T_};

  for (int i=0; i<np; ++i) {
    *(rdat[i]) = dat[i];
  }
  
  //L_ = dat[0]; // length [cm]
  //B  = dat[1]; // magnetic field [kG]
  //G  = dat[2]; // magnetic gradient [kG/cm]
  //S  = dat[3]; // electric field [kV/cm]
  //T_ = dat[4]; // electric gradient [kV/cm^2]
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CFEBend::setParameters( int np, std::vector<double> const& dat, ... )
{

  double* rdat[] = { &L_, &B, &G, &S, &T_};

  for (int i=0; i<np; ++i) {
    *(rdat[i]) = dat[i];
  }
  
  //L_ = dat[0]; // length [cm]
  //B  = dat[1]; // magnetic field [kG]
  //G  = dat[2]; // magnetic gradient [kG/cm]
  //S  = dat[3]; // electric field [kV/cm]
  //T_ = dat[4]; // electric gradient [kV/cm^2]
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix CFEBend::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{
  // For reference see: R. Baartman,
  //                    Generalized Wien Filter
  //                    Triumf report TRI-DN-07-06
  // ---------------------------------------------------------------- 

  double P      = sqrt(energy * (energy + 2*ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  
  RMatrix mi;
  mi.toUnity();

  // combined functions dipole + electrostatic bend
  // S : electric field(kV/cm);
  // T : gradient(kV/cm**2)
  // bt    : relativistic beta
  // hr    : momentum ? brho
  // G : magnetic gradient ?
  // ri    :
  // P     :
            
    
  double  ky2  = -( G + T_/(C_DERV3*bt) )/hr;  // eta^2
  double  ri   =    0.001*S/(P*bt) + B / hr; // h
  double  s    =    0.001*S*ms / (P*P);      // h_E
  double  kx2  =    (ri*ri) + s*s -ky2;      // xi^2
  double  kp   =    ri + 0.001*S * ms*ms / (P*P*(energy+ms)); // zeta   is this correct ????

  double ks;
  double fi;
     
  // no bending // THIS MAY NOT BE CORRECT IF ELECTRIC AND MAGNETIC BENDING CANCEL OUT !!!! FIXME
   
#if 0
  if ( fabs(ri) < 100* std::numeric_limits<double>::epsilon() ) {
      mi[0][1] = L_;
      mi[2][3] = L_;
      mi[4][5] = L_/(gamma1*gamma1);
      return mi;
  };
#endif
  
  // vertical motion

  if ( fabs(ky2) < 100*std::numeric_limits<double>::epsilon() ) {
      mi[2][3] = L_;
  }
  else {
    if(ky2 < 0.0 ){
      ks  = sqrt( -ky2 );
      fi = L_ * ks;
      mi[2][2] = mi[3][3] = cosh(fi);
      mi[2][3] = sinh(fi)/ks;
      mi[3][2] = (ks*ks) * mi[2][3];
    }
    else {
      ks  = sqrt( ky2 );
      fi = L_ * ks;
      mi[2][2] = mi[3][3] = cos(fi);
      mi[2][3] = sin(fi)/ks;
      mi[3][2] = -(ks*ks) * mi[2][3];
    }
  }

  // horizontal motion

  if( fabs(kx2) < 100*std::numeric_limits<double>::epsilon() ) {
    mi[0][1] = L_;
    mi[0][5] = 0.5 * (L_*L_) * kp;
    mi[4][1] = -mi[0][5];
    mi[1][5] = L_ * kp;
    mi[4][0] = -mi[1][5]; 
    mi[4][5] = kp*kp *( L_*L_*L_) / 6.;
  }
  else {
    if(kx2 > 0.0){
      ks  = sqrt(kx2);
      fi = L_ * ks;
      mi[0][0] =  mi[1][1] = cos(fi);
      mi[0][1] =  sin(fi)/ks;
      mi[1][0] = -mi[0][1]*(ks*ks);
    }
    else {
      ks  = sqrt(-kx2);
      fi = L_ * ks;
      mi[0][0] = mi[1][1] = cosh(fi);
      mi[0][1] = sinh(fi)/ks;
      mi[1][0] = (ks*ks)*mi[0][1];
    }

    mi[0][5] =  (1.0 - mi[0][0])*kp/kx2;
    mi[4][1] = -mi[0][5];

    mi[1][5]  = kp*mi[0][1];
    mi[4][0] = -mi[1][5];
    mi[4][5] = L_*(kp*kp)/kx2*(1.0-mi[0][0]);

  }

   mi[4][5] = L_/(gamma1*gamma1) - mi[4][5]; 

  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix CFEBend::rmatrixsc(double& alfap, double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st ) const
{
  // combined functions dipole + electrostatic bend
  // el->S - electric field(kV/cm);
  // el->T_  gradient(kV/cm**2)

  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;

  RMatrix mi;
  mi.toUnity();

  double gx   = 0.0;
  double gy   = 0.0;
  double gs   = 0.0;

  GetBeamFocusing(current, bt, bs, gx, gy, gs);

  // Verify this. I belive optim32 was wrong.
  // double Gy0  = - (G + T_/(C_DERV1*bt)); // ORIGINAL in optim32
  double Gy0  = - (G + T_/(C_DERV3*bt));  // eta^2 from regular matrix
  double ri   =  0.001*S/(P*bt) + B/hr;  // h
  double s    =  0.001*S * ms / (P*P);   // h_E
  double Gx0  = (ri*ri + s*s)*hr-Gy0;    // xi^2

  double Gx    = 0.0;
  double Gy    = 0.0;
  double ks    = 0.0;
  double rotat = 0.0;

  GetNormalizedGradientsNew(gx+Gx0, gy+Gy0, gs, Gx, Gy, rotat); // rotat in rd

  get2DMatrix(hr, Gx, L_, mi, Horizontal);
  get2DMatrix(hr, Gy, L_, mi, Vertical);

  double kp  = ri + 0.001*S * ms*ms / (P*P*(energy+ms));

  double cs     =  cos(rotat);
  double sn     = -sin(rotat);

  // horizontal motion ...

  double kx2 = Gx/hr;

  if( fabs(kx2) < 100*std::numeric_limits<double>::epsilon() ) {
    mi[0][5] = 0.5 * L_*L_ * kp * cs;
    mi[4][1] = -mi[0][5];
    mi[1][5] = L_ * kp * cs;
    mi[4][0] = - mi[1][5]; 
    mi[4][5] = kp*kp * L_*L_*L_ * cs*cs/ 6.;
  }
  else {
    mi[0][5] = (kp/kx2)*(1.0 - mi[0][0])* cs;
    mi[4][1] = -mi[0][5];
    mi[1][5] =  kp*mi[1][0]* cs;
    mi[4][0] = -mi[1][5];

    mi[4][5] = L_*kp*kp/kx2 *(1.0-mi[0][1]/L_) *cs*cs;
  }

  // vertical motion ... 

  double ky2 = Gy/hr;

  if( fabs(ky2) < 100*std::numeric_limits<double>::epsilon()) {

   mi[2][5] = 0.5 * L_*L_ * kp * sn;
   mi[4][3] = -mi[2][5];
   
   mi[3][5] = L_ * kp * sn;
   mi[4][2] = -mi[3][5];

   mi[4][5] += kp*kp * L_*L_*L_ * sn*sn/ 6.0;
  }
  else {

    mi[2][5] = kp/ky2 * (1.0-mi[2][2]) * sn;
    mi[4][3] = -mi[2][5];
    mi[3][5] = kp*mi[2][3] * sn;
    mi[4][2] = -mi[3][5];

    mi[4][5] += L_*kp*kp/ky2 *(1.0-mi[0][1]/L_) *sn*sn;
  }

  mi[4][5] = L_/(gamma1*gamma1) - mi[4][5];

  mi = mi.similarOrtho(RMatrix::m_tilt(-rotat));  //  rotat is from sc ?  

  return mi;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CFEBend::propagate( double hr, double ms, RMatrix_t<3>& W, Vector_t<3>& V ) const
{
  // NOTE: only bending elements alter the orientation of the local (Frenet-Serret) axes.
  //       When an element is rotated around the s-axis, a transformation is applied before
  //       and **undone** after exiting the element.    

   double P  = hr*C_DERV1;
   double En = sqrt(P*P+ms*ms);
   double bt = P/En;    // v/c
   double ri = 0.001*S/(P*bt) + B / hr;

  if( fabs(ri) < std::numeric_limits<double>::epsilon() ) {
    return Element::propagate(hr, ms, W, V); // No bending: call the generic element version of the propagate function
  }

  double r   = 1./ri;
  double alf = 0.0; // FIXME !!! this should be set to just as for CFBend (T_/ 180.0) * PI but here,  T_ is used for the eletric gradient
  double fi  = L_*ri;

  double sf = sin(fi);
  double cf = cos(fi);

  double sa = sin(alf);
  double ca = cos(alf);

  RMatrix_t<3>  S   = { {  cf,    0.0,    -sf },
                        { 0.0,    1.0,    0.0 },
                        {  sf,    0.0,    cf  } };

  RMatrix_t<3> T   = { {  ca,    -sa,     0.0  },
                       {  sa,     ca,     0.0  },
                       {  0.0,    0.0,    1.0  } };
         
  Vector_t<3>  R   =   {  r*(cf-1),  0.0,   r*sf  };
         
  
  V = V +  W * (T * R);
  W = W * (T*S*T.transpose());

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix  CFEBend::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{
  // For reference see: R. Baartman,
  //                    Generalized Wien Filter
  //                    Triumf report TRI-DN-07-06
  // ---------------------------------------------------------------- 

  double P      = sqrt(energy * (energy + 2*ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  
  RMatrix mi;
  mi.toUnity();

  // combined functions dipole + electrostatic bend
  // S : electric field(kV/cm);
  // T : gradient(kV/cm**2)
  // bt    : relativistic beta
  // hr    : momentum ? brho
  // G : magnetic gradient ?
  // ri    :
  // P     :
            
    
  double  ky2  = -( G + T_/(C_DERV3*bt) )/hr;  // eta^2
  double  ri   =    0.001*S/(P*bt) + B / hr; // h
  double  s    =    0.001*S*ms / (P*P);      // h_E
  double  kx2  =    (ri*ri) + s*s -ky2;      // xi^2
  double  kp   =    ri + 0.001*S * ms*ms / (P*P*(energy+ms)); // zeta   is this correct ????

  double ks;
  double fi;
     
  // no bending // THIS MAY NOT BE CORRECT IF ELECTRIC AND MAGNETIC BENDING CANCEL OUT !!!! FIXME
   
#if 0
  if ( fabs(ri) < 100* std::numeric_limits<double>::epsilon() ) {
      mi[0][1] = L_;
      mi[2][3] = L_;
      mi[4][5] = L_/(gamma1*gamma1);
      return mi;
  };
#endif
  
  // vertical motion

  if ( fabs(ky2) < 100*std::numeric_limits<double>::epsilon() ) {
      mi[2][3] = L_;
  }
  else {
    if(ky2 < 0.0 ){
      ks  = sqrt( -ky2 );
      fi = L_ * ks;
      mi[2][2] = mi[3][3] = cosh(fi);
      mi[2][3] = sinh(fi)/ks;
      mi[3][2] = (ks*ks) * mi[2][3];
    }
    else {
      ks  = sqrt( ky2 );
      fi = L_ * ks;
      mi[2][2] = mi[3][3] = cos(fi);
      mi[2][3] = sin(fi)/ks;
      mi[3][2] = -(ks*ks) * mi[2][3];
    }
  }

  // horizontal motion

  if( fabs(kx2) < 100*std::numeric_limits<double>::epsilon() ) {
    mi[0][1] = L_;
    mi[0][5] = 0.5 * (L_*L_) * kp;
    mi[4][1] = -mi[0][5];
    mi[1][5] = L_ * kp;
    mi[4][0] = -mi[1][5]; 
    mi[4][5] = kp*kp *( L_*L_*L_) / 6.;
  }
  else {
    if(kx2 > 0.0){
      ks  = sqrt(kx2);
      fi = L_ * ks;
      mi[0][0] =  mi[1][1] = cos(fi);
      mi[0][1] =  sin(fi)/ks;
      mi[1][0] = -mi[0][1]*(ks*ks);
    }
    else {
      ks  = sqrt(-kx2);
      fi = L_ * ks;
      mi[0][0] = mi[1][1] = cosh(fi);
      mi[0][1] = sinh(fi)/ks;
      mi[1][0] = (ks*ks)*mi[0][1];
    }

    mi[0][5] =  (1.0 - mi[0][0])*kp/kx2;
    mi[4][1] = -mi[0][5];

    mi[1][5]  = kp*mi[0][1];
    mi[4][0] = -mi[1][5];
    mi[4][5] = L_*(kp*kp)/kx2*(1.0-mi[0][0]);

  }

   mi[4][5] = L_/(gamma1*gamma1) - mi[4][5]; 

  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CFEBend::trackOnce(double ms, double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		       RMatrix const& m1, Coordinates& v) const
{
  return Element::trackOnce(ms, Enr0, n_elem, n_turn, prm, m1, v);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CFEBend::preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const
{
  return Element::preTrack(ms, Enr0,  tetaY, n_elem, prm, m1 );
   
}
