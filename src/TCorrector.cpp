//  =================================================================
//
//  TCorrector.cpp
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
#include <Coordinates.h>
#include <TrackParam.h>

using Constants::C_DERV1;
using Constants::C_CGS;
using Constants::PI;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TCorrector::TCorrector(const char* nm, char const* fnm)
  : Element(nm,fnm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TCorrector::TCorrector(TCorrector const& o)
  : Element(o)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//RMatrix TCorrector::rmatrix()
//{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TCorrector::setParameters( int np, double dat[], ... )
{
  double* rdat[] = { &L_, &B, &T_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  
  //L_ = dat[0];  // length in cm
  //B  = dat[1];  // field in kG
  //T_ = dat[2];  // rotation angle ("tilt")
  //S  = G = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TCorrector::setParameters( int np, std::vector<double> const& dat, ... )
{
  double* rdat[] = { &L_, &B, &T_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  
  //L_ = dat[0];  // length in cm
  //B  = dat[1];  // field in kG
  //T_ = dat[2];  // rotation angle ("tilt")
  //S  = G = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TCorrector::toString(char* buf) const
{

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TCorrector::preTrack(double ms, double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1 ) const
{
   prm.p0   = sqrt(2.*ms*Enr0+Enr0*Enr0);
   prm.Hr0  = prm.p0/C_DERV1;
   prm.vp0  = C_CGS*prm.p0/sqrt(prm.p0*prm.p0+ms*ms);
   
   prm.cfi = L_*B*cos(PI*T_/180.);
   prm.sfi = L_*B*sin(PI*T_/180.);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  TCorrector::trackOnce( double ms,   double& Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v ) const

{
  double p = prm.p0*(1.0+v[5]);  // p = p0 (1 + dp/p)  

  if(p<0.0) {
    v.lost  = 2;
    v.nelem = n_elem+1;
    v.npass = n_turn;
    return 1;
  }

  double Hr = p/C_DERV1; // brho  =  p/ec   p is in MeV/c   

  double xp = prm.cfi/Hr;
  double yp = prm.sfi/Hr;

  v[0] += L_ * ( v[1] + 0.5 *xp);
  v[1] += xp;
  v[2] += L_ * ( v[3] + 0.5 *yp);
  v[3] += yp;
     
  double vp    = C_CGS*p/sqrt( p*p*(1.0 + v[1]*v[1] + v[3]*v[3]) + ms*ms );  
  v[4] += (vp/prm.vp0 - 1.0) * L_;


 done:	

  if( fabs(v[0])>1000.0 || fabs(v[2])>1000.0) {
    v.lost  = 1;
    v.nelem = n_elem+1;
    v.npass = n_turn;
    return 2;
  }

  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix TCorrector::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{
  
  double P      = sqrt(energy *(energy + 2.0 * ms)); //  pc in MeV
  double hr     = P/C_DERV1; // in kG-cm   
  double bt     = P/(energy+ms);
  double gamma1 = 1.0 + energy/ms;

  RMatrix mi_d;
  mi_d.toUnity();

  mi_d[0][1] =  L_/2.0;
  mi_d[2][3] =  L_/2.0;
  mi_d[4][5] = (L_/2.0)/(gamma1*gamma1);

  RMatrix mi_k;
  mi_k.toUnity();
  mi_k[1][5] =  (B*L_)/hr;
  mi_k[4][0] = -mi_k[1][5];

  RMatrix mi = (mi_d * mi_k * mi_d); 

  //if(T != 0.0)  tilt( T, &mi);
  if(T_ != 0.0)  mi = mi.similarOrtho(RMatrix::m_tilt(T_*(PI/180.0)));

  return  mi; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix TCorrector::rmatrixsc(double& alfap, double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st ) const
{

  //    case 'O': // drift  
  //    case 'I': // instrument
  //    case 'K': // transverse corrector 
  //    case 'Z': // long corrector
  //    case 'S': // sextupole

  
  double P  = energy+ms;
         P  = sqrt(P*P-ms*ms);
  double hr = P/C_DERV1;
  double bt = P/(energy+ms);

  alfap = 0.0;

  RMatrix mi;
  mi.toUnity();

  // Zero current case

  if(fabs(current)<1.e-12){
   return rmatrix(alfap, energy, ms, tetaY, dalfa, st);
  }

  double gx = 0.0;
  double gy = 0.0;
  double gs = 0.0;

  GetBeamFocusing(current, bt, bs, gx, gy, gs);

  double rotat = 0.0;
  double Gx    = 0.0;
  double Gy    = 0.0;

  GetNormalizedGradientsNew(gx, gy, gs, Gx, Gy, rotat); // rotat in rd !
	
  get2DMatrix(hr, Gx, L_, mi, Horizontal);
  get2DMatrix(hr, Gy, L_, mi, Vertical);

  mi = mi.similarOrtho(RMatrix::m_tilt(-rotat));
 
  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix   TCorrector::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{
  double P      = sqrt(energy *(energy + 2.0 * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1.0 + energy/ms;

  RMatrix mi;
  mi.toUnity();
  
  mi[0][1] = mi[2][3] = L_;
  mi[4][5] = L_/(gamma1*gamma1);

  return mi;
}
