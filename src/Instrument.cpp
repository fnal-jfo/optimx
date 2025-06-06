//  =================================================================
//
//  Instrument.cpp
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
#include <TrackParam.h>
#include <Coordinates.h>

using Constants::C_CGS;
using Constants::C_DERV1;

Instrument::Instrument(const char* nm, char const* fnm)  // 'I'
  : Element(nm,fnm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Instrument::Instrument(Instrument const& o) // 'I'
  : Element(o)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//RMatrix Instrument::rmatrix()
//{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Instrument::setParameters( int np, double dat[], ... )
{
  double* rdat[] = { &L_, &T_};
  for(int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  
  L_ = dat[0];
  T_ = dat[1];
  // B = G = S = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Instrument::setParameters( int np, std::vector<double> const& dat, ... )
{
  double* rdat[] = { &L_, &T_};
  for(int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  
  L_ = dat[0];
  T_ = dat[1];
  // B = G = S = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Instrument::toString(char* buf) const
{
  sprintf(buf, "  %g %g %g %g", 0.0, 0.0, 0.0, T_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Instrument::trackOnce( double ms,   double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		      RMatrix const& m1, Coordinates& v) const
{
  int status = 0; 
  if (status = backwardTest(prm, n_elem, n_turn, v)) return status;
  double p = prm.p0*(1.0+v[5]);  // p = p0 (1 + dp/p)
 
  v[0] += L_*tan(v[1]);
  v[2] += L_*tan(v[3]);

  double vp  = C_CGS*p/sqrt( p*p*(1.0 + v[1]*v[1] + v[3]*v[3]) + ms*ms );  // velocity
  v[4] += (vp - prm.vp0) * L_ / prm.vp0;

 done:	

  if (status = transAmpTest(prm, n_elem, n_turn, v)) return status; 

  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix Instrument::rmatrixsc(double& alfap, double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st ) const
{

  //    case 'O': // drift  
  //    case 'I': // instrument
  //    case 'K': // transverse corrector 
  //    case 'Z': // long corrector
  //    case 'S': // sextupole

  alfap = 0.0; // is this right ??? 

  double P  = energy+ms;
         P  = sqrt(P*P-ms*ms);
  double hr = P/C_DERV1;
  double bt = P/(energy+ms);

  RMatrix mi;
  mi.toUnity();

  // Zero current case

  if(fabs(current)<1.e-12){
    return Element::rmatrix(alfap, energy, ms, tetaY, dalfa, st);
  }

  double gx = 0.0;
  double gy = 0.0;
  double gs = 0.0;

  GetBeamFocusing(current, bt, bs, gx, gy, gs);

  double rotat = 0.0;
  double Gx    = 0.0;
  double Gy    = 0.0;

  GetNormalizedGradientsNew(gx, gy, gs, Gx, Gy, rotat); // rotat in rd
	
  get2DMatrix(hr, Gx, L_, mi, Horizontal);
  get2DMatrix(hr, Gy, L_, mi, Vertical);

  mi = mi.similarOrtho(RMatrix::m_tilt(-rotat));
  
  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix  Instrument::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
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
