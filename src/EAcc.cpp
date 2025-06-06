//  =================================================================
//
//  EAcc.cpp
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
#include <RMatrix.h>
#include <TrackParam.h>
#include <Coordinates.h>

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;
using Constants::C_CGS;

EAcc::EAcc(const char* nm, char const* fnm) // 'E'
  : Element(nm,fnm)
{} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

EAcc::EAcc(EAcc const& o) // 'E'
  : Element(o)
{} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EAcc::toString(char* buf) const
{
  // ***FIXME***
  double energy = 0.0;
  sprintf(buf,"  D_E[MeV]=%g  E_out[MeV]=%g", B, energy);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

EAcc* EAcc::split( int nslices) const
{
  EAcc* e = this->clone();
  e->L_ /= nslices;
  e->B /= nslices;
  return e;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EAcc::setParameters( int np, double dat[], ... )
{

  double* rdat[] = {&S, &B}; 
  for (int i=0; i<np; ++i) {
    *(rdat[i]) = dat[i];
  }

  //L_ = dat[0];
  //B =  dat[1];
  //G = S = T_ = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EAcc::setParameters( int np, std::vector<double> const& dat, ... )
{

  double* rdat[] = {&S, &B}; 
  for (int i=0; i<np; ++i) {
    *(rdat[i]) = dat[i];
  }

  //L_ = dat[0];
  //B =  dat[1];
  //G = S = T_ = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 int EAcc::trackOnce( double ms,   double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		      RMatrix const& m1, Coordinates& v ) const
{

  int status = 0; 
  if ( (status = backwardTest(prm, n_elem, n_turn, v)) ) return status;

  double EnrNew = Enr0;
  double tetaY  = 0.0;
  
  RMatrix tm   = Element::rmatrix(EnrNew, ms, tetaY, 0.0, 3); // IS IT NECESSARY TO RECOMPUTE THIS MATRIX HERE ?  
  v.c    = tm*v.c;

  double capa = sqrt(sqrt( (2*(Enr0)*ms + Enr0 * Enr0)/(2.*EnrNew*ms+EnrNew*EnrNew)));

  v[0] *= capa;
  v[1] *= capa;
  v[2] *= capa;
  v[3] *= capa;
  v[5] *= capa*capa*(ms+EnrNew)/(ms+ Enr0);

  Enr0  = EnrNew;

 done:	

  if ( (status = transAmpTest(prm, n_elem, n_turn, v)) ) return status; 

  return 0;
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix EAcc::rmatrix( double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{

  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  RMatrix mi;
  mi.toUnity();

  double E1 = energy;
  double E2 = energy+B;
  double P1 = sqrt(E1*(E1+2.*ms));
  double P2 = sqrt(E2*(E2+2.*ms));
  double beta1 = P1/(E1+ms);
  double beta2 = P2/(E2+ms);
  double fi1  = (E2-E1)/(2.*L_)*(ms+E1)/(P1*P1);
  double fi2  = (E2-E1)/(2.*L_)*(ms+E2)/(P2*P2);
  double kapa = sqrt(P2/P1);
  double lef  = L_*sqrt(2.*ms*E1+E1*E1)/(E2-E1)*(acosh((ms+E2)/ms)-acosh((ms+E1)/ms));

  mi[0][0] = mi[2][2] = kapa*(1.-lef*fi1);
  mi[0][1] = mi[2][3] = lef*kapa;
  mi[1][0] = mi[3][2] = (fi2*kapa*kapa-fi1*fi2*lef*kapa*kapa-fi1)/kapa;
  mi[1][1] = mi[3][3] = (fi2*lef*kapa*kapa+1.)/kapa;
  mi[4][4] = kapa*beta2/beta1;
  mi[4][4] = kapa*beta1*P1/(beta2*P2);
  mi[4][5] = (B==0.0) ?  L_/gamma1*gamma1 : kapa*L_/B*P1*beta1/(beta1/beta2-1.0);
  energy = E2;

  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix EAcc::rmatrixsc( double& alfap,  double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st ) const
{
  // electrostatic accelerator section

  RMatrix mi;
  mi.toUnity();

  double P  = energy+ms;
         P  = sqrt(P*P-ms*ms);
  double hr = P/C_DERV1;
  double bt = P/(energy+ms);

  alfap = 0.0; // is this ok ? 

  double gx = 0.0;
  double gy = 0.0;
  double gs = 0.0;

  GetBeamFocusing(current, bt, bs, gx, gy, gs);

  double E1 = energy;
  double E2 = energy+B;
  double P1 = sqrt(2.*ms*E1+E1*E1);
  double P2 = sqrt(2.*ms*E2+E2*E2);
  double gamma1 = 1.0 + E1 / ms;
  double gamma2 = 1.0 + E2 / ms;
  double beta1  = sqrt(E1*(E1+2.*ms))/(E1+ms);
  double beta2  = sqrt(E2*(E2+2.*ms))/(E2+ms);
  double kapa   = sqrt(P2/P1);
  double ksi    = L_/(gamma2-gamma1)*log( (gamma2+sqrt(gamma2*gamma2-1.))/ (gamma1+sqrt(gamma1*gamma1-1.)));
  double fi1    = (E2-E1)/(2.*L_)*(ms+E1)/(2.*ms*E1+E1*E1);
  double fi2    = (E2-E1)/(2.*L_)*(ms+E2)/(2.*ms*E2+E2*E2);

  double Gx    = 0.0;
  double Gy    = 0.0;
  double rotat = 0.0;
  
  GetNormalizedGradientsNew(gx, gy, gs, Gx, Gy, rotat); // rotat in rd
  double s = C_DERV1/ms*gamma1*beta1*(1.+gamma1*beta1/(gamma2*beta2))/2.;

  double ks;
  double cs;
  double sn;
  
  // x matrix

  if (Gx>0.0) {
    ks  = sqrt( s*Gx);
    cs = cos(ks*ksi);
    sn = sin(ks*ksi);
    ks  = ks/sqrt(gamma1*gamma2*beta1*beta2);
  } 
  else {
    ks  = sqrt( -s*Gx);
    cs = cosh(ks*ksi);
    sn = sinh(ks*ksi);
    ks  = ks/sqrt(gamma1*gamma2*beta1*beta2);
  }

  mi[0][0] = cs*kapa-fi1*sn/ks;
  mi[0][1] = sn/ks;
  mi[1][0] = (kapa*fi2-fi1/kapa)*cs+(ks-fi1*fi2/ks)*sn;
  mi[1][1] = cs/kapa+fi2*sn/ks;
  
  // y matrix

  if (Gy>0.0) {
    ks  = sqrt( s*Gy);
    cs = cos(ks*ksi);
    sn = sin(ks*ksi);
    ks  = ks/sqrt(gamma1*gamma2*beta1*beta2);
  } 
  else {
    ks  = sqrt( -s*Gy);
    cs = cosh(ks*ksi);
    sn = sinh(ks*ksi);
    ks = ks/sqrt(gamma1*gamma2*beta1*beta2);
  }
  mi[2][2] = cs*kapa-fi1*sn/ks;
  mi[2][3] = sn/ks;
  mi[3][2] = (kapa*fi2-fi1/kapa)*cs+(ks-fi1*fi2/ks)*sn;
  mi[3][3] = cs/kapa+fi2*sn/ks;
 
  mi = mi.similarOrtho(RMatrix::m_tilt(-rotat));
  energy = E2;

  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Beamline* EAcc::splitnew(int nslices) const  // return a sliced element as a beamline 
{
 // EAcc cannot be split so we return a cloned element. 
  Beamline* bml = new Beamline();
  EAcc* e      = this->clone();
  e->L_      /= 1;
  e->slices_  = 1;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix EAcc::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{

  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  RMatrix mi;
  mi.toUnity();

  double E1 = energy;
  double E2 = energy+B;
  double P1 = sqrt(E1*(E1+2.*ms));
  double P2 = sqrt(E2*(E2+2.*ms));
  double beta1 = P1/(E1+ms);
  double beta2 = P2/(E2+ms);
  double fi1  = (E2-E1)/(2.*L_)*(ms+E1)/(P1*P1);
  double fi2  = (E2-E1)/(2.*L_)*(ms+E2)/(P2*P2);
  double kapa = sqrt(P2/P1);
  double lef  = L_*sqrt(2.*ms*E1+E1*E1)/(E2-E1)*(acosh((ms+E2)/ms)-acosh((ms+E1)/ms));

  mi[0][0] = mi[2][2] = kapa*(1.-lef*fi1);
  mi[0][1] = mi[2][3] = lef*kapa;
  mi[1][0] = mi[3][2] = (fi2*kapa*kapa-fi1*fi2*lef*kapa*kapa-fi1)/kapa;
  mi[1][1] = mi[3][3] = (fi2*lef*kapa*kapa+1.)/kapa;
  mi[4][4] = kapa*beta2/beta1;
  mi[4][4] = kapa*beta1*P1/(beta2*P2);
  mi[4][5] = (B==0.0) ?  L_/gamma1*gamma1 : kapa*L_/B*P1*beta1/(beta1/beta2-1.0);
  energy = E2;

  return mi;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EAcc::preTrack(double ms, double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1 ) const
{
  
   prm.p0   = sqrt(2.*ms*Enr0+Enr0*Enr0);
   prm.Hr0  = prm.p0/C_DERV1;
   prm.vp0  = C_CGS*prm.p0/sqrt(prm.p0*prm.p0+ms*ms);
   
   prm.Efin = Enr0;
   m1       = Element::rmatrix( prm.Efin, ms, tetaY, 0.0, 3);
}
