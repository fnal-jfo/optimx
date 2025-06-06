//  =================================================================
//
//  LiLens.cpp
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
#include <Coordinates.h>
#include <TrackParam.h>

using Constants::C_DERV1;
using Constants::PI;
using Constants::C_CGS;


LiLens::LiLens(const char* nm, char const* fnm) : Element(nm,fnm) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LiLens::LiLens(LiLens const& o) : Element(o) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LiLens::toString(char* buf) const
{
  sprintf(buf,"   j[kA/cm**2]=%g ",G);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix LiLens::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{
  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  
  RMatrix mi;
  mi.toUnity();
  
  mi[4][5] = L_/(gamma1*gamma1);

  double kx2      = 0.2*PI*G/hr;            //  G   - current in kA
  double fi;

  double s;
  double c;
  
  if( fabs(kx2) < std::numeric_limits<double>::epsilon() ){
    mi[0][1] = mi[2][3] = L_;
    return std::move(mi);
  }
 if(kx2 >0){
   s  = sqrt( kx2 );
   fi = L_ * s;
   mi[0][0] = mi[1][1] = mi[2][2] = mi[3][3] = cos(fi);
   mi[0][1] = mi[2][3] = sin(fi)/s;
   mi[1][0] = mi[3][2]= - s * s * mi[0][1];
  }
  else {
    s=sqrt( - kx2 );
    fi=L_ * s;

    mi[0][0] = mi[1][1] = mi[2][2] = mi[3][3] = cosh(fi);
    mi[0][1] = mi[2][3] = sinh(fi)/s;
    mi[1][0] = mi[3][2] = s * s * mi[0][1];
  }

    // if(T != 0)tilt(T, mi); // FIXME !

    return std::move(mi);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix LiLens::rmatrixsc( double& alphap, double& Enr,    double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st ) const
{
  // li lens matrix with space/charge 

  RMatrix mi;
  mi.toUnity();

  double P  = Enr+ms;
         P  = sqrt(P*P-ms*ms);
  double hr = P/C_DERV1;
  double bt = P/(Enr+ms);
  double alfap = 0.;

  double gx = 0.0;
  double gy = 0.0;
  double gs = 0.0;

  double dg = 0.2*PI*G;  // 2*pi*G/100.0   el->G = current in kA

  double Gx    = 0.0;
  double Gy    = 0.0;
  double rotat = 0.0;

  GetNormalizedGradientsNew(gx+dg, gy+dg, gs, Gx, Gy, rotat); // rotat in rd 

  get2DMatrix(hr, Gx, L_, mi, Horizontal);
  get2DMatrix(hr, Gy, L_, mi, Vertical);

  mi = mi.similarOrtho(RMatrix::m_tilt(-rotat));

  return mi;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LiLens::setParameters( int np, double dat[], ... )
{
  double* rdat[] = { &L_, &G};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  //L_ = dat[0];
  //G = dat[1];
  B = S = T_ = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LiLens::setParameters( int np, std::vector<double> const& dat, ... )
{
  double* rdat[] = { &L_, &G};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  //L_ = dat[0];
  //G = dat[1];
  B = S = T_ = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int LiLens::trackOnce( double ms,   double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		      RMatrix const& m1, Coordinates& v) const
{
 
  double  m00, m01, m10, phi;
  double p = prm.p0*(1.0+v[5]);  // p = p0 (1 + dp/p)  
  double Hr = p/C_DERV1; // brho  =  p/ec   p is in MeV/c   

  int status = 0; 
  if (status = backwardTest(prm, n_elem, n_turn, v)) return status;

  if (fabs(G) < std::numeric_limits<double>::epsilon() ) { // a drift
   return Element::trackOnce(ms, Enr0, n_elem, n_turn, prm, m1, v); 
  }
	
  double s = 0.2*PI*G/Hr;    //  el->G   - current in kA

  if(s >0.0){
    s = sqrt(s);
    phi=L_ * s;
    m00 = cos(phi);
    m01 = sin(phi)/s;
    m10 = -s*s*m01;
  }
  else {
    s   = sqrt(-s);
    phi = L_ * s;
    m00 = cosh(phi);
    m01 = sinh(phi)/s;
    m10 = s*s*m01;
  }

  double x = m00*v[0]+m01*v[1];
  v[1]     = m10*v[0] + m00*v[1];
  v[0]     = x;
  x 	   = m00*v[2]+m01*v[3];
  v[3]     = m10*v[2]+m00*v[3];
  v[2]     = x;

 done:	

 if (status = transAmpTest(prm, n_elem, n_turn, v)) return status;

 return 0;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Beamline* LiLens::splitnew(int nslices) const  // return a sliced element as a beamline 
{
 // LiLens cannot be split so we return a cloned element. 
  Beamline* bml = new Beamline();
  LiLens* e      = this->clone();
  e->L_      /= 1;
  e->slices_  = 1;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix   LiLens::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{

  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  
  RMatrix mi;
  mi.toUnity();
  
  mi[4][5] = L_/(gamma1*gamma1);

  double kx2      = 0.2*PI*G/hr;            //  G   - current in kA
  double fi;

  double s;
  double c;
  
  if( fabs(kx2) < std::numeric_limits<double>::epsilon() ){
    mi[0][1] = mi[2][3] = L_;
    return mi;
  }
 if(kx2 >0){
   s  = sqrt( kx2 );
   fi = L_ * s;
   mi[0][0] = mi[1][1] = mi[2][2] = mi[3][3] = cos(fi);
   mi[0][1] = mi[2][3] = sin(fi)/s;
   mi[1][0] = mi[3][2]= - s * s * mi[0][1];
  }
  else {
    s=sqrt( - kx2 );
    fi=L_ * s;

    mi[0][0] = mi[1][1] = mi[2][2] = mi[3][3] = cosh(fi);
    mi[0][1] = mi[2][3] = sinh(fi)/s;
    mi[1][0] = mi[3][2] = s * s * mi[0][1];
  }

    // if(T != 0)tilt(T, mi); // FIXME !

    return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
