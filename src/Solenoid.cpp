//  =================================================================
//
//  Solenoid.cpp
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
#include <memory>
#include <limits>

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Solenoid::Solenoid(const char* nm, char const* fnm)
  : Element(nm,fnm)
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Solenoid::Solenoid(Solenoid const& o)
  : Element(o)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::toString(char* buf) const
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix Solenoid::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{
  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  double s;
  double c;
  
  RMatrix mi;
  mi.toUnity();

  RMatrix me;
  me.toUnity();
  
  mi[4][5]  = L_/(gamma1*gamma1);

  double k =  B / hr;
  double fi = L_ * k;

  if (fabs(k) < std::numeric_limits<double>::epsilon() ) {
    mi[0][1] = mi[2][3] = L_;
    return std::move(mi);
  }

  if ( toupper(name()[1]) == 'C'){     // focusing part only
    s = sin(fi/2.);
    c = cos(fi/2.);
    mi[0][0] = mi[1][1] =  mi[2][2]=mi[3][3] = c;
    mi[0][1] = mi[2][3] =  2.*s/k;
    mi[1][0] = mi[3][2] = -k*s/2.;
  }
  else {                              // 4-dimensional transfer matrix
    s = sin(fi);
    c = cos(fi);
    mi[0][0] =  mi[1][1] 
             =  mi[2][2]
             =  mi[3][3] = (1+c)/2.;
    mi[0][1] =  mi[2][3] =  s/k;
    mi[1][0] =  mi[3][2] = -k*s/4.;
    mi[0][2] =  mi[1][3] =  s/2.;
    mi[2][0] =  mi[3][1] = -s/2.;
    mi[0][3] =  (1-c)/k;
    mi[2][1] = -mi[0][3];
    mi[1][2] = -(1-c)*k/4.;
    mi[3][0] = -mi[1][2];
  }

  if( A>0.0 ) { // effective aperture != 0 implies soft edge focusing 
                //                    == 0 hard edge

    if (st == 1){ // ADD Entrance focusing (typically this is the FIRST slice)
      SolLinearEdge(me, 0.5*k, A, 1);
      mi = mi*me;
    }
    if(st == 2){ // ADD Exit focusing (typically this is the LAST slice)
      SolLinearEdge(me, 0.5*k, A, 2);
      mi =  me*mi;
    }
    if(st == 3){ // TOTAL transfer matrix
      SolLinearEdge(me, 0.5*k, A, 1);
      mi = mi*me;
      SolLinearEdge(me, 0.5*k, A, 2);
      mi = me*mi;
    }
  }

   return std::move(mi);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix Solenoid::rmatrixsc(double& alfap, double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st ) const
{

  // solenoid matrix with space/charge 

  RMatrix mi;
  mi.toUnity();

  double P  = energy+ms;
         P  = sqrt(P*P-ms*ms);
  double hr = P/C_DERV1;
  double bt = P/(energy+ms);
  alfap = 0.0; // IS THIS RIGHT ???? FIXME 

  double gx = 0.0;
  double gy = 0.0;
  double gs = 0.0;

  GetBeamFocusing(current, bt, bs, gx, gy, gs);

  double Gx = 0.0;
  double Gy = 0.0;
  double rotat = 0.0;

  GetNormalizedGradientsNew(gx, gy, gs, Gx, Gy, rotat);

  if( fabs( B) < std::numeric_limits<double>::epsilon() ) { // B = 0.0 treat as a drift and return IS THIS RIGHT ??? FIXME  
     get2DMatrix(hr, Gx, L_, mi, Horizontal);
     get2DMatrix(hr, Gy, L_, mi, Vertical);
     mi = mi.similarOrtho(RMatrix::m_tilt(-rotat));
     return mi;
  }   

  if ( toupper(name()[1]) == 'C') {  // pseudo-solenoid (focusing only, no rotation)
    double dg = 0.25 * (B*B) / hr; 
    GetNormalizedGradientsNew(gx+dg, gy+dg, gs, Gx, Gy, rotat); // rotat in rd
    get2DMatrix(hr, Gx, L_, mi, Horizontal );
    get2DMatrix(hr, Gy, L_, mi, Vertical);
    mi = mi.similarOrtho(RMatrix::m_tilt(-rotat));
  }
  else { // regular (full 4D, including rotation) 
    GetNormalizedGradientsNew(gx, gy, gs, Gx, Gy, rotat); // rotat in rd
    std::complex<double> Kx = sqrt( -std::complex<double>(Gx, 0.0) / hr);
    std::complex<double> Ky = sqrt( -std::complex<double>(Gy, 0.0) / hr);
    GetSolenoidMatrix(B/hr, Kx, Ky, L_, &mi);
    mi = mi.similarOrtho(RMatrix::m_tilt(-rotat));
  }
  
  if(st) {
    RMatrix edge;
    edge.toUnity();
    double k = B/hr;
    edge[1][0] = edge[3][2] = 0.125*k*k*A;
    if(st==1) { mi = edge*mi;    } 
    if(st==2) { mi = mi*edge;    }
    if(st==3) { mi = mi*edge*mi; }
  }

  return mi;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::setParameters( int np, double dat[], ... )
{
  double* rdat[] = { &L_, &B, &A};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  
  //L_ = dat[0];
  //B = dat[1];	 
  //A = dat[2];
  //G = S = T_ = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::setParameters( int np, std::vector<double> const& dat, ... )
{
  double* rdat[] = { &L_, &B, &A};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  
  //L_ = dat[0];
  //B = dat[1];	 
  //A = dat[2];
  //G = S = T_ = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Solenoid::trackOnce( double ms,   double &Enr0,  int n_elem,   int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v ) const
{

  double m00, m01, m10, n00, n01, n10, n02, n03, n12;
  
  int status = 0; 
  if (status = backwardTest(prm, n_elem, n_turn, v )) return status;
  double p = prm.p0*(1.0+v[5]);  // p = p0 (1 + dp/p)  

  double Hr = p/C_DERV1; // brho  =  p/ec   p is in MeV/c    
  double k =  B / Hr;

  if ( fabs(k) < std::numeric_limits<double>::epsilon() ) { // a drift
    return Element::trackOnce( ms, Enr0, n_elem, n_turn, prm, m1, v );
  }

  if(A > 0.0) SolEdge(v, 0.5*k, A, 1);

  double phi = L_ * k;

  if ( toupper(name()[1]) == 'C') {  // edge focussing only; no rotation
    double s     = sin( phi/2.);
    double c     = cos( phi/2.);
    m00   = c;
    m01   = 2.*s/k;
    m10   = -k*s/2.;
    double x  =  m00*v[0]+m01*v[1];
    v[1]  =  m10*v[0]+m00*v[1];
    v[0]  =  x;
    x     =  m00*v[2]+m01*v[3];
    v[3]  =  m10*v[2]+m00*v[3];
    v[2] =   x;
  }
  else { // 4-dimensional transfer matrix

    double s    =  sin(phi);
    double c    =  cos(phi);

    n00  =  (1+c)/2.;
    n01  =  s/k;
    n10  = -k*s/4.;
    n02  =  s/2.;
    n03  =  (1-c)/k;
    n12  = -(1-c)*k/4.;

    double x    = v[0];
    double xp   = v[1];
    double y    = v[2];
    double yp   = v[3];

    v[0] =  n00*x + n01*xp + n02*y + n03*yp;
    v[1] =  n10*x + n00*xp + n12*y + n02*yp;
    v[2] = -n02*x - n03*xp + n00*y + n01*yp;
    v[3] = -n12*x - n02*xp + n10*y + n00*yp;
  }

  if(A>0.0 ) SolEdge(v, 0.5*k, A, 2);

  if (status = transAmpTest(prm, n_elem, n_turn, v )) return status; 

  return 0;
}
	

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Solenoid* Solenoid::split(int nslices) const
{
  Solenoid* e = this->clone();
  e->L_ /= nslices;
  return e;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::SolLinearEdge(RMatrix& mi, double k, double a, int edge)
{

 //edge=1 - input edge, edge=2 - output edge

  double ka  = k*a;
  double ka2 = ka*ka;
  double ka4 = ka2*ka2;
  double ka6 = ka4*ka2;

  mi.toUnity();
 
  if(edge==1){
    mi[0][0] = mi[2][2] = 1. - ka2/2.5  + ka4/4;
    mi[0][1] = mi[2][3] = -ka2*a/2.434 + ka4*a/2.5 - ka6*a/3.1;
    mi[1][0] = mi[3][2] = ka*k/2. - 0.381*ka*ka2*k + ka4*ka*k/3.3;
    mi[1][1] = mi[3][3] = (1 + mi[0][1]*mi[1][0])/mi[0][0];
   }
  if(edge==2){
    mi[1][1] = mi[3][3] = 1. - ka2/2.5  + ka4/4;
    mi[0][1] = mi[2][3] = -ka2*a/2.434 + ka4*a/2.5 - ka6*a/3.1;
    mi[1][0] = mi[3][2] = ka*k/2. - 0.381*ka*ka2*k + ka4*ka*k/3.3;
    mi[0][0] = mi[2][2] = (1 + mi[0][1]*mi[1][0])/mi[1][1];
   }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void Solenoid::SolEdge(Coordinates& v, double k, double a, int edge)
{
  // edge=1 - input edge, edge=2 - output edge

  RMatrix mi;
  Coordinates v1;
  
  SolLinearEdge(mi, k, a, edge);

  double r2 = v[0]*v[0] + v[2]*v[2];

  v1.c[0] = mi[0][0]*v[0] + mi[0][1]*v[1];
  v1.c[1] = mi[1][0]*v[0] + mi[1][1]*v[1];
  v1.c[2] = mi[2][2]*v[2] + mi[2][3]*v[3];
  v1.c[3] = mi[3][2]*v[2] + mi[3][3]*v[3];

  v[0] = v1.c[0];
  v[1] = v1.c[1] - k*k*v1.c[0]*r2/(6.*a);
  v[2] = v1.c[2];
  v[3] = v1.c[3] - k*k*v1.c[2]*r2/(6.*a);

  /*
  double teta2, ka, a3, m11, m12, m21, m22;
  teta2 = v[1]*v[1] + v[3]*v[3];
  ka =k*a;
  a3 = 40.*a*a*a;
  v[1] + =0.5*k*ka*(1.+3.*ka*ka)*v[0];
  v[1] - =k*k*(1.+12.5*ka*ka)*r2*v[0]/(6.*a);
  v[1] - =(r2*k*k/24.-teta2/4.)*k*v[2];
  v[1] - =r2*r2*k*k*v[0]/a3;
  v[3] + =0.5*k*ka*(1.+3.*ka*ka)*v[2];
  v[3] - =k*k*(1.+12.5*ka*ka)*r2*v[2]/(6.*a);
  v[3] + =(r2*k*k/24.-teta2/4.)*k*v[0];
  v[3] - = r2*r2*k*k*v[2]/a3; 
*/
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

