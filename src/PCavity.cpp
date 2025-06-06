//  =================================================================
//
//  PCavity.cpp
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
#include <Beamline.h>
#include <Constants.h>
#include <Coordinates.h>
#include <TrackParam.h>

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;
using Constants::C_CGS;

PCavity::PCavity(const char* nm, char const* fnm)
  : Element(nm,fnm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PCavity::PCavity(PCavity const& o)
  : Element(o)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PCavity::toString(char* buf) const
{
  // ***FIXME*** need a way to pass energy
  double energy = 0.0;
  sprintf(buf,"  D_E[MeV]=%g  E_out[MeV]=%g", G*cos(PI/180.*S), energy);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PCavity* PCavity::split(int nslices) const // return a sliced element
{
  // PCavity cannot be split so we return a cloned element. 
  return this->clone();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Beamline* PCavity::splitnew(int nslices) const  // return a sliced element as a beamline 
{
 // PCavity cannot be split so we return a cloned element. 
  Beamline* bml = new Beamline();
  PCavity* e      = this->clone();
  e->L_      /= 1;
  e->slices_  = 1;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix PCavity::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{

  alfap  = 0.0;
  
  double P      = sqrt(energy * (energy + 2*ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1 + energy/ms;

  RMatrix mi;
  mi.toUnity();

  RMatrix2 t,  me, L, F; 
  RMatrix2 tL, mL, LL;

  int    Ncell   = N;
  double leff    = B;
  double DEn     = G/Ncell;
  double phase   = S*PI/180;
  double wavelen = T_;
  double Pin     = P;
  double En1     = energy;
  double c       = cos(phase);
  double s       = sin(phase);

  t.toUnity();

  L.toUnity();
  L[0][1]  =  0.5*(this->L_/Ncell - leff);

  F.toUnity();
  tL.toUnity();
  LL.toUnity();


  double P2;

  for ( int i=0; i<Ncell; ++i) {
    double P1     = sqrt((En1*En1) + 2.0*En1*ms);
    double En2    = En1+DEn*cos(phase);
           P2     = sqrt((En2*En2) + 2.0*En2*ms);
    double gamma1 = 1.0 + En1/ms;
    double beta1  = sqrt(1.0 - 1.0/(gamma1*gamma1));
    double gamma2 = 1.0 + En2/ms;
    double beta2  = sqrt(1.0 - 1.0/(gamma2*gamma2));

    // Transverse motion
    double psi       = 2.0*PI*leff/(wavelen*beta1);
    double cp        = cos(0.5*psi);
    double E0        = (leff!=0.0) ? DEn*psi/(2.*leff*sin(0.5*psi)) : 0.0;
    double a  = E0*wavelen*beta1*beta1/(2.*PI*sqrt(P1*P2));

    me[0][0] = 1.0 + 0.5*a*(cos(0.5*psi-phase)*psi-2.*sin(0.5*psi)*c);
    me[0][1] = leff*sqrt(P1/P2)-a*beta1*wavelen/(2.*PI)*(cp*psi-2.*sin(.5*psi))*s;
    me[1][0] = E0*beta1/(4.*P2)*(0.5*a*( (2.*c*c+1.)*sin(psi) -
                   psi*(c*c-s*s+2*cp*cp ) ) +4*s*sin(0.5*psi));
    me[1][1] = (P1/P2+me[0][1]*me[1][0])/me[0][0];

    t = L*t;  // drift 
    
    F[1][0]  = -E0*cos(phase-0.5*psi)/(2.*P1*beta1);

    t = F*t;  // edge focusing
    t = me*t; // body 
    F[1][0]  = E0*cos(phase+0.5*psi)/(2.*P2*beta2);
    t = F*t;  // edge focusing
    t = L*t;  // drift
    
    // Longitudinal motion
    LL[0][1] = 0.5*this->L_/Ncell/gamma1/gamma1;

    tL = LL*tL; // drift 
    mL[0][0] = beta2/beta1;   
    mL[0][1] = 0;
    mL[1][0] = 2.*PI*DEn*s/(wavelen*beta1*beta2*beta2*gamma2*ms);
    mL[1][1] = beta1*beta1*gamma1/(beta2*beta2*gamma2);
    tL = mL*tL; // body
    LL[0][1] = 0.5*this->L_/Ncell/gamma2/gamma2;
    tL = LL*tL; // drift 

    En1   = En2;
  }

  s = sqrt(P2/Pin);

  mi[0][0] = mi[2][2] = t[0][0]*s;  
  mi[0][1] = mi[2][3] = t[0][1]*s;
  mi[1][0] = mi[3][2] = t[1][0]*s;
  mi[1][1] = mi[3][3] = t[1][1]*s;

  mi[4][4] = tL[0][0]*s;
  mi[4][5] = tL[0][1]*s;
  mi[5][4] = tL[1][0]*s;
  mi[5][5] = tL[1][1]*s;

  energy    += G*cos(phase);
 
  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PCavity::setParameters( int np, double dat[], ... )
{

  double Ndum = 0.0;
  double* rdat[] = { &L_, &Ndum,  &B, &G, &S, &T_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  if (Ndum != 0.0) { N  = floor(Ndum); }
    
  //L_ = dat[0];           // length 
  //N  = floor(dat[1]);    // NCell
  //B  = dat[2];           // effective length
  //G  = dat[3];           // accelerating gradient 
  //S  = dat[4];           // phase in deg 
  //T_ = dat[5];           // wavelength

  if (N < 1.0) N = 1.0;

  // if (T < 0.00001)  wavelength must be positive 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PCavity::setParameters( int np, std::vector<double> const& dat, ... )
{

  double Ndum = 0.0;
  double* rdat[] = { &L_, &Ndum,  &B, &G, &S, &T_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  if (Ndum != 0.0) { N  = floor(Ndum); }
    
  //L_ = dat[0];           // length 
  //N  = floor(dat[1]);    // NCell
  //B  = dat[2];           // effective length
  //G  = dat[3];           // accelerating gradient 
  //S  = dat[4];           // phase in deg 
  //T_ = dat[5];           // wavelength

  if (N < 1.0) N = 1.0;

  // if (T < 0.00001)  wavelength must be positive 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int PCavity::trackOnce( double ms, double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		       RMatrix const& m1, Coordinates& v ) const
{
  int status = 0; 
  if (status = backwardTest(prm, n_elem, n_turn, v)) return status;

  double p = prm.p0*(1.0+v[5]);  // p = p0 (1 + dp/p)  
  double Hr = p/C_DERV1; // brho  =  p/ec   p is in MeV/c
  
  RMatrix2 me;
  int    Ncell   = N;
  double leff    = B;
  double DEn     = G/Ncell;
  double phase0  = S*PI/180.0;
  double wavelen = T_;
  double En1     = sqrt(p*p+ms*ms)-ms;
  
  //....................................................................
  char* envoverride = std::getenv("OPTIMX_PCAVITY_TRACK_WITH_MATRIX");
  if (envoverride) {  // env override: always use matrix  

     std::cout << "OPTIMX_PCAVITY_TRACK_WITH_MATRIX" << std::endl;

     v.c = m1 *v.c;
     double EnrC = Enr0;

     for (int i=0; i<Ncell; ++i) {

     double P1     =  sqrt(En1*En1+2.*En1*ms);
     double gamma1 = 1.+En1/ms;
     double beta1  = sqrt(1.-1./(gamma1*gamma1));
     double phase  = phase0-2.*PI*v[4]/(wavelen*beta1);
     double En2    = En1+DEn*cos(phase);

     if ( En2<0.0 ){ 
      v.lost  = 1; 
      v.nelem = n_elem+1;
      v.npass = n_turn;	
      return 2;
    }

    EnrC += DEn*cos(phase0);

    if( EnrC < 0.0){
      v.lost  = 1;
      v.nelem = n_elem+1;
      v.npass = n_turn;	
      return 2;
    }

     En1 = En2;
   }

    if (status = transAmpTest(prm, n_elem, n_turn, v )) return status; 

    Enr0 = EnrC;
   
    return 0;
  } 
  //.....................................................................
  

  RMatrix2 t;
  RMatrix2 L;
  RMatrix2 F;

  t.toUnity();
  L.toUnity();
  L[0][1] = 0.5*(this->L_/Ncell - leff);
  F.toUnity();

  double EnrC	= Enr0;;

  for (int i=0; i<Ncell; ++i) {

   //transverse motion

   double P1     =  sqrt(En1*En1+2.*En1*ms);
   double gamma1 = 1.+En1/ms;
   double beta1  = sqrt(1.-1./(gamma1*gamma1));
   double psi    = 2.*PI*leff/(wavelen*beta1);
   double cp     = cos(0.5*psi);
   double phase  = phase0-2.*PI*v[4]/(wavelen*beta1);
   double c      = cos(phase);
   double s      = sin(phase);
   double En2    = En1+DEn*cos(phase);

   if ( En2<0.0 ){ 
     v.lost  = 1; 
     v.nelem = n_elem+1;
     v.npass = n_turn;	
     return 1; // goto done;
   }

   double P2     = sqrt(En2*En2+2.*En2*ms);
   double gamma2 = 1.+En2/ms;
   double beta2  = sqrt(1.-1./(gamma2*gamma2));

   double E0 = ( leff != 0.0) ? DEn*psi/(2.*leff*sin(0.5*psi)) : 0.0;

   double a = E0*wavelen*beta1*beta1/(2.*PI*sqrt(P1*P2));

   me[0][0] = 1.+0.5*a*(cos(0.5*psi-phase)*psi-2.*sin(0.5*psi)*c);
   me[0][1] = leff*sqrt(P1/P2)-a*beta1*wavelen/(2.*PI)*(cp*psi-2.*sin(.5*psi))*s;
   me[1][0] = E0*beta1/(4.*P2)*(0.5*a*( (2.*c*c+1.)*sin(psi) -
	  	                           psi*(c*c-s*s+2*cp*cp ) ) +4.*s*sin(.5*psi));
   me[1][1] = (P1/P2+me[0][1]*me[1][0])/me[0][0];

   t = L*t;
   F[1][0] = -E0*cos(phase-0.5*psi)/(2.*P1*beta1);
   t = F*t;
   t = me*t;
   F[1][0] = E0*cos(phase+0.5*psi)/(2.*P2*beta2);
   t = F*t;
   t = L*t;

   // Longitudinal motion

   double vp1   = C_CGS*sqrt(2.*ms*EnrC+EnrC*EnrC)/(ms+EnrC);
   double vp    = C_CGS*beta1;
   v[4] += (vp-vp1)*(this->L_/Ncell)/vp1;

   EnrC += DEn*cos(phase0);

    if( EnrC < 0.0){
      v.lost  = 1;
      v.nelem = n_elem+1;
      v.npass = n_turn;	
      return 1; // goto done;
    }

    En1 = En2;
  }

  double x  = t[0][0]*v[0] + t[0][1]*v[1];
  v[1] = t[1][0]*v[0] + t[1][1]*v[1];
  v[0] = x;

  x    = t[0][0]*v[2]+t[0][1]*v[3];
  v[3] = t[1][0]*v[2]+t[1][1]*v[3];
  v[2] = x;

  v[5] = sqrt((2*ms*En1+En1*En1)/(2*ms*EnrC+EnrC*EnrC))-1.;
  Enr0 += G*cos(phase0);

 done:	

  if( fabs(v[0])>1000.0 || fabs(v[2])>1000.0) {
    v.lost  = 1;
    v.nelem = n_elem+1;
    v.npass = n_turn;
    return 2;
  }

  return 0; // all ok 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void PCavity::preTrack(double ms, double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1 ) const
{
  
   prm.p0   = sqrt(2.*ms*Enr0+Enr0*Enr0);
   prm.Hr0  = prm.p0/C_DERV1;
   prm.vp0  = C_CGS*prm.p0/sqrt(prm.p0*prm.p0+ms*ms);
   
   prm.Efin = Enr0;
   m1       = Element::rmatrix( prm.Efin, ms, tetaY, 0.0, 3);
}
