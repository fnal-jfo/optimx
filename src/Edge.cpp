//  =================================================================
//
//  Edge.cpp
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
#include <Beamline.h>
#include <Constants.h>
#include <TrackParam.h>
#include <Coordinates.h>

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;
using Constants::C_CGS;


Edge::Edge(const char* nm, char const* fnm)  // 'G'
  : Element(nm,fnm)
{} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Edge::Edge(Edge const& o) // 'G'
  : Element(o)
{} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix Edge::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{
  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;      // Brho
  
  RMatrix mi;
  mi.toUnity();
  
  if( fabs(B) < 10*std::numeric_limits<double>::epsilon() ){
    return mi;
  }

  double s  = tan(PI*G/180.0);       // G: [deg] pole face angle 
  double ri = fabs(B/hr);            // ri = 1/rho (curvature) NOTE: the absolute value is needed
                                     // if 

  mi[1][0] =     ri*s;               // a positive edge angle is defocussing horizontally and focussing vertically  
  mi[3][2] =   - ri*s + S*ri*ri;     // S = K *gap * (1+ sin^2(beta))/cos^3(beta) [beta = G*180/pi = edge angle] 


  if (T_ != 0.0) mi = mi.similarOrtho( RMatrix::m_tilt(T_*PI/180.0));

  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Edge::setParameters( int np, double dat[], ... )
{
  L_ = 0.0;     // 

  double* rdat[] = { &B, &G, &S, &T_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  //B  = dat[0];  // Bend Field [kG/cm] 
  //G  = dat[1];  // Pole face angle [deg]
  //S  = dat[2];  // edge field effective length 
  //T_ = dat[3];  // tilt [deg]
  //           = dat[4]; // Tilt error [deg]    
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Edge::setParameters( int np, std::vector<double> const& dat, ... )
{
  L_ = 0.0;     // 

  double* rdat[] = { &B, &G, &S, &T_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  //B  = dat[0];  // Bend Field [kG/cm] 
  //G  = dat[1];  // Pole face angle [deg]
  //S  = dat[2];  // edge field effective length 
  //T_ = dat[3];  // tilt [deg]
  //           = dat[4]; // Tilt error [deg]    
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Edge::toString(char* buf) const
{
  sprintf(buf,"%g  Angle[deg]=%g  Eff.Length[cm]=%g  Tilt[deg]=%g",
			       B, G, S, T_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Edge::preTrack(double ms, double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1 ) const
{
   prm.p0   = sqrt(2.*ms*Enr0+Enr0*Enr0);
   prm.Hr0  = prm.p0/C_DERV1;
   prm.vp0  = C_CGS*prm.p0/sqrt(prm.p0*prm.p0+ms*ms);

   
   if( fabs(B) < std::numeric_limits<double>::epsilon() )  return;

   prm.ri0 = B/prm.Hr0;   // negative sign allowed ????  
   prm.R0  = 1.0/prm.ri0;

   m1  = Element::rmatrix( Enr0, ms, tetaY, 0.0, 3);

   prm.sgn = (B >= 0.0)  ? 1.0 : -1.0;

   // cos and sin for rotation

   double alfa = T_/180.*PI;
   prm.cb1    =  prm.cb2 = cos(alfa);
   prm.sb1    = -sin(alfa);
   prm.sb2    = -prm.sb1;
   prm.phi    =  PI*G/180.;
   prm.sfi    =  sin(prm.phi);
   prm.cfi    =  cos(prm.phi);

   if(prm.cfi !=0 ) prm.tfi=prm.sfi / prm.cfi;

   // determine where is the dipole

   if(n_elem == 0) {
     prm.EdgeBefore = true;
   }
   else {
     prm.EdgeBefore = isupstream;
     prm.G          = bendGradient;
   }
    
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Edge::trackOnce( double ms, double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		     RMatrix const& m1, Coordinates& v ) const
{

  double x, xp, y, yp;
  double xf, xpf, yf, ypf;
  double ss,s,c;
  double xc, yc;
  double phi, deltaL, vp;

  int status = 0; 
  if ( (status = backwardTest(prm, n_elem, n_turn, v)) ) return status;

  double p  = prm.p0*(1.0+v[5]);  // p = p0 (1 + dp/p)  
  double Hr = p/C_DERV1; // brho  =  p/ec   p is in MeV/c
  
  if( fabs(B) < std::numeric_limits<double>::epsilon() ) return 0;

  if ( fabs(prm.G) < 10*std::numeric_limits<double>::epsilon() ) {   // G = edge field effective length

    x  = prm.cb1*v[0] - prm.sb1*v[2];
    y  = prm.sb1*v[0] + prm.cb1*v[2];
    xp = prm.cb1*v[1] - prm.sb1*v[3];
    yp = prm.sb1*v[1] + prm.cb1*v[3];

    double Rp = prm.R0*p/prm.p0;

    s  = sin(xp);
    c  = cos(xp);

    if ( prm.EdgeBefore) { // upstream edge

      double Ltilda=x*prm.tfi*prm.sgn/(c-prm.sgn*prm.tfi*s);

      ypf = yp+ (y+Ltilda*yp)*(S/(Rp*Rp)-prm.tfi/fabs(Rp));
      yf  = y + Ltilda*(yp-ypf);

      double R  = Rp*cos(ypf);

      yc = R*c - Ltilda*s;
      ss = Ltilda/yc; ss=1.-ss*ss;

      if( ss < 0.0 ) {
	v.lost  = 2;
      	v.nelem = n_elem+1;
        v.npass = n_turn;
        goto done; 
      }

      xf     = x - Ltilda*Ltilda/(yc*(1.+sqrt(ss)));
      xc     = Ltilda*c+R*s;
      yc     = x-yc;
      xpf    = atan(xc/(xf-yc));
      deltaL = Ltilda-R*(xpf-xp);
    }
    else { //  downstream edge
      
      double R  = Rp*cos(yp);

      xc = R*s;
      yc = x - R*c;

      double ytilda = x/R;
      double xtilda = 2.*c-ytilda;
      double Rtilda = cos(prm.phi*prm.sgn-xp) - ytilda*prm.cfi;

      ss = Rtilda*Rtilda+ytilda*xtilda;

      if( ss < 0.0){
        v.lost  = 2;
        v.nelem = n_elem+1;
        v.npass = n_turn;
        goto done; 
      }

      ytilda = x*prm.cfi*xtilda/(sqrt(ss)+Rtilda);
      xpf    = atan((prm.sgn*ytilda*prm.tfi+xc)/(ytilda-yc));
      xf     = ytilda*(1.+prm.sgn*prm.tfi*tan(xpf));

      double Ltilda = ytilda*prm.sgn*prm.tfi/cos(xpf);

      ypf    = yp+(y-Ltilda*yp)*(S/(Rp*Rp)-prm.tfi/fabs(Rp));
      yf     = y + Ltilda*(ypf-yp);
      deltaL = R*(xpf-xp)-Ltilda;
    }


    v[3] += S*v[2]/(Rp*Rp); // edge focusing correction for finite edge length

    v[0]  = prm.cb2*xf  - prm.sb2*yf;
    v[2]  = prm.sb2*xf  + prm.cb2*yf;
    v[1]  = prm.cb2*xpf - prm.sb2*ypf;
    v[3]  = prm.sb2*xpf + prm.cb2*ypf;

    vp    = C_CGS*p/sqrt(p*p+ms*ms)*cos(yp);
    v[4] += (vp/prm.vp0)*deltaL;

    // Attempt to fix long trajectory problem.
   
    v[4] = m1[4][0]*v[0] + m1[4][1]*v[1] + m1[4][2]*v[2] +
           m1[4][3]*v[3] + m1[4][4]*v[4] + m1[4][5]*v[5];
  }

  else {  // regular thin edge 

    v.c = m1*v.c;
  }
  
 done:	

  if ( (status = transAmpTest(prm, n_elem, n_turn, v)) ) return status; 

  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

