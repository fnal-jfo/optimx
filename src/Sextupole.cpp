//  =================================================================
//
//  Sextupole.cpp
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
using Constants::PI;

Sextupole::Sextupole(const char* nm, char const* fnm)
  : Element(nm,fnm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Sextupole::Sextupole(Sextupole const& o)
  : Element(o)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// should return unit matrix 

//RMatrix Sextupole::rmatrix()
//{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sextupole::setParameters( int np, double dat[], ... )
{
  double* rdat[] = { &L_, &S, &T_};

  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  //L_ = dat[0];
  //S  = dat[1];
  //T_ = dat[2];
  //B  = G = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sextupole::setParameters( int np, std::vector<double> const& dat, ... )
{
  double* rdat[] = { &L_, &S, &T_};

  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  //L_ = dat[0];
  //S  = dat[1];
  //T_ = dat[2];
  //B  = G = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sextupole::toString(char* buf) const
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Sextupole::trackOnce( double ms,   double& Enr0, int n_elem, int n_turn, TrackParam& prm,
	    	          RMatrix const& m1, Coordinates& v ) const
{
  int status = 0; 
  if (status = backwardTest(prm, n_elem, n_turn, v )) return status;

  sext_trans(this,  prm.Hr0/(1.+v[5]), &v, &v );

 done:	

  if (status = transAmpTest(prm, n_elem, n_turn, v )) return status; 

  return 0;
  
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix Sextupole::rmatrixsc(double& alfap, double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st ) const
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

  RMatrix mi;
  mi.toUnity();

  alfap = 0.0;

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

  GetNormalizedGradients(gx, gy, gs, Gx, Gy, rotat); // rotat in rd !
	
  get2DMatrix(hr, Gx, L_, mi, Horizontal);
  get2DMatrix(hr, Gy, L_, mi, Vertical);

  mi = mi.similarOrtho(RMatrix::m_tilt(-rotat));
 
  return mi;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sextupole::sext_trans_new( Element const* el, double hr, Coordinates* vp, Coordinates const* v) 
{


  using std::pow;
  
  // *** g++ treats pow as constexpr howeverm pow is NOT constexpr in pre c++26 std. This fails under clang++
  /***
  constexpr double c1 = 1.0/(2.0*(2.0-pow(2.0,1.0/3.0))); //0.6756
  constexpr double c4 = c1;                               //0.6756
  
  constexpr double c2 = (1.0-pow(2.0,1.0/3.0))/(2.0*(2.0-pow(2.0,1.0/3.0))); // -0.25992/1.4801 = -0.175
  constexpr double c3 = c2;

  constexpr double d1 = 1.0/(2.0-pow(2.0,1.0/3.0));// 1/0.74007 = 1.351207
  constexpr double d3 = d1; // 1.351207 
  
  constexpr double d2 = -pow(2.0,1.0/3.0)/(2.0-pow(2.0,1.0/3.0));// -1.2599/0.74007 = -1.702
  constexpr double d4 = 0;
  ***/

  const double c1 = 1.0/(2.0*(2.0-pow(2.0,1.0/3.0))); //0.6756
  const double c4 = c1;                               //0.6756
  
  const double c2 = (1.0-pow(2.0,1.0/3.0))/(2.0*(2.0-pow(2.0,1.0/3.0))); // -0.25992/1.4801 = -0.175
  const double c3 = c2;

  const double d1 = 1.0/(2.0-pow(2.0,1.0/3.0));// 1/0.74007 = 1.351207
  const double d3 = d1; // 1.351207 
  
  const double d2 = -pow(2.0,1.0/3.0)/(2.0-pow(2.0,1.0/3.0));// -1.2599/0.74007 = -1.702
  const double d4 = 0;

  //std::cout << "c = " << c1 + c2 + c3 +c4 << std::endl;
  // std::cout << "d = " << d1 + d2 + d3 +d4 << std::endl;
  // Ruth 4th order integrator  

  // sextupole transverse map 
   
   double alfa=el->tilt()*PI/180.;

   RMatrix tl = RMatrix::m_tilt(alfa); //  tl = tilt matrix 

   auto vi = tl * (*v).c;

   vp->c[4] = vi[4]; // long coordinates are unchanged 
   vp->c[5] = vi[5];  

   double L = el->length();
   double s = el->S/hr*L;   //sextupole strength in optical units 

   double x  = vi[0];
   double tx = vi[1];

   double y  = vi[2];
   double ty = vi[3];

   x  += c1*tx*L;   y  += c1*ty*L;
   tx += d1*0.5*s*(y*y-x*x); ty += d1*s*x*y;

   x  += c2*tx*L;   y  += c2*ty*L;
   tx += d2*0.5*s*(y*y-x*x); ty += d2*s*x*y;
   
   x  += c3*tx*L;   y  += c3*ty*L;
   tx += d3*0.5*s*(y*y-x*x); ty += d3*s*x*y;
   
   x  += c4*tx*L;            y  += c4*ty*L;
   tx += d4*0.5*s*(y*y-x*x); ty += d4*s*x*y;

   vp->c[0] = x;
   vp->c[1] = tx;
   vp->c[2] = y;
   vp->c[3] = ty;
   
   (*vp).c = tl.transpose() * (*vp).c;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sextupole::sext_trans_old( Element const* el, double hr, Coordinates* vp, Coordinates const* v) 
{

  //std::cout << "sext_trans" << std::endl;
  // sextupole transverse map 
   
   double alfa=el->tilt()*PI/180.;

   RMatrix tl = RMatrix::m_tilt(alfa); //  tl = tilt matrix 

   auto vi = tl * (*v).c;

   vp->c[4] = vi[4];
   vp->c[5] = vi[5];

   double L = el->length()/6.; // L/6  
   double s = 2*el->S * L /hr; // 2/6 * S * L 

   double x  = vi[0];
   double tx = vi[1];

   double y  = vi[2];
   double ty = vi[3];

   // c1 = 1/6    d1 = 1/3
   // c2 = 1/3    d2 = 1/3
   // c3 = 1/3    d3 = 1/3 
   // c4 = 1/6    d4 = 0

   
   x += L*tx;	  y += L*ty;	tx += 0.5*s*(y*y-x*x);   ty += s*x*y;
   x += 2.*L*tx;  y += 2*L*ty;	tx += 0.5*s*(y*y-x*x);	 ty += s*x*y;
   x += 2.*L*tx;  y += 2*L*ty;	tx += 0.5*s*(y*y-x*x);	 ty += s*x*y;

   vp->c[0] = x+L*tx;  vp->c[1] = tx;
   vp->c[2] = y+L*ty;  vp->c[3] = ty;

   (*vp).c = tl.transpose() * (*vp).c;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Sextupole::sext_trans( Element const* el, double hr, Coordinates* vp, Coordinates const* v) 
{
  return sext_trans_new(el, hr, vp, v);
}
