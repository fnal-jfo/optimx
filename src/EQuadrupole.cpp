//  =================================================================
//
//  EQuadrupole.cpp
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

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;


EQuadrupole::EQuadrupole(const char* nm, char const* fnm) // 'L_'
  : Element(nm,fnm)
{} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

EQuadrupole::EQuadrupole(EQuadrupole const& o) // 'L_'
  : Element(o)
{} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EQuadrupole::toString(char* buf) const
{
  sprintf(buf,"Ge[kV/cm**2]=%g  ", G);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix EQuadrupole::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{

  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  
  RMatrix mi;
  mi.toUnity();

  mi[4][5] = L_/(gamma1*gamma1);

  double kx2      = G/(C_DERV3*bt)/hr;
  double fi;
  double c;
  double s;
  
  if( fabs(kx2) < std::numeric_limits<double>::epsilon() ){
      mi[0][1] = mi[2][3] = L_;
      return std::move(mi);
  }

  if(kx2 > 0.){
    s  = sqrt( kx2 );
    fi = L_ * s;
    mi[0][0] = mi[1][1] = cos(fi);
    mi[0][1] = sin(fi)/s;
    mi[1][0] = - s*s* mi[0][1];
    mi[2][2] = mi[3][3] = cosh(fi);
    mi[2][3] = sinh(fi)/s;
    mi[3][2] = s*s * mi[2][3];
  }
  else {
    s  = sqrt( - kx2 );
    fi = L_ * s;
    mi[0][0] = mi[1][1] = cosh(fi);
    mi[0][1] = sinh(fi)/s;
    mi[1][0] = s*s * mi[0][1];
    mi[2][2] = mi[3][3] = cos(fi);
    mi[2][3] = sin(fi)/s;
    mi[3][2] = -s*s * mi[2][3];
  }

  //if(T != 0) tilt(T, &mi);
  if(T_ != 0)  mi = mi.similarOrtho(RMatrix::m_tilt(T_*(PI/180.0)));

  return std::move(mi);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix EQuadrupole::rmatrixsc( double& alfap, double& Enr, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st ) const
{
  // equad matrix with space/charge 

  RMatrix mi;
  mi.toUnity();

  double P  = Enr+ms;
         P  = sqrt(P*P-ms*ms);
  double hr = P/C_DERV1;
  double bt = P/(Enr+ms);

  alfap = 0.0; // is this correct ???  should not be changed ??? 

  double gx;
  double gy;
  double gs;

  GetBeamFocusing(current, bt, bs, gx, gy, gs);

  // verify the constant C_DERV3 below. I was set to C_DERV1 in optim32
  // I believe this was wrong

  double dg = G*cos( T_*PI/90.0)/(C_DERV3*bt);

  double Gx    = 0.0;
  double Gy    = 0.0;
  double rotat = 0.0;
  
  // verify the constant C_DERV3 below. I was set to C_DERV1 in optim32
  // I believe this was wrong

  GetNormalizedGradientsNew(gx+dg, gy-dg, gs-G*sin(T_*PI/90.)/(C_DERV3*bt), Gx, Gy, rotat);

  get2DMatrix(hr, Gx, L_, mi, Horizontal);
  get2DMatrix(hr, Gy, L_, mi, Vertical);
  
  mi = mi.similarOrtho(RMatrix::m_tilt(-rotat));

  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EQuadrupole::setParameters( int np, double dat[], ... )
{
  double* rdat[] = { &L_, &G, &T_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  
  //L_ = dat[0]; // length in cm   
  //G  = dat[1]; // gradient in kV/cm ??? 
  //T_ = dat[3]; // tilt [deg]
  //B = S;
 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void EQuadrupole::setParameters( int np, std::vector<double> const& dat, ... )
{
  double* rdat[] = { &L_, &G, &T_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  
  //L_ = dat[0]; // length in cm   
  //G  = dat[1]; // gradient in kV/cm ??? 
  //T_ = dat[3]; // tilt [deg]
  //B = S;
 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix EQuadrupole::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{

  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  
  RMatrix mi;
  mi.toUnity();

  mi[4][5] = L_/(gamma1*gamma1);

  double kx2      = G/(C_DERV3*bt)/hr;
  double fi;
  double c;
  double s;
  
  if( fabs(kx2) < std::numeric_limits<double>::epsilon() ){
      mi[0][1] = mi[2][3] = L_;
      return mi;
  }

  if(kx2 > 0.){
    s  = sqrt( kx2 );
    fi = L_ * s;
    mi[0][0] = mi[1][1] = cos(fi);
    mi[0][1] = sin(fi)/s;
    mi[1][0] = - s*s* mi[0][1];
    mi[2][2] = mi[3][3] = cosh(fi);
    mi[2][3] = sinh(fi)/s;
    mi[3][2] = s*s * mi[2][3];
  }
  else {
    s  = sqrt( - kx2 );
    fi = L_ * s;
    mi[0][0] = mi[1][1] = cosh(fi);
    mi[0][1] = sinh(fi)/s;
    mi[1][0] = s*s * mi[0][1];
    mi[2][2] = mi[3][3] = cos(fi);
    mi[2][3] = sin(fi)/s;
    mi[3][2] = -s*s * mi[2][3];
  }

  if (T_ != 0)  {
 
    double psi = T_*PI/180.0;

    double si = sin(psi);
    double ci = cos(psi);
 
    RMatrix S = {  {  ci,   0.0,   si,  0.0,  0.0,   0.0 },
                   { 0.0,    ci,  0.0,   si,  0.0,   0.0,},
                   { -si,   0.0,   ci,  0.0,  0.0,   0.0 },
                   { 0.0,   -si,  0.0,   ci,  0.0,   0.0 },
                   { 0.0,   0.0,  0.0,  0.0,  1.0,   0.0 },
                   { 0.0,   0.0,  0.0,  0.0,  0.0,   1.0 }
                };

                   
    mi = S.transpose()*mi*S;
  }

  return mi;
}
