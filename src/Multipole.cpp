//  =================================================================
//
//  Multipole.cpp
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

Multipole::Multipole(const char* nm, char const* fnm) // 'M'
  : Element(nm,fnm)
{}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Multipole::Multipole(Multipole const& o) // 'M'
  : Element(o)
{}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Multipole::toString(char* buf ) const
{
  sprintf(buf, "  Order:m=%d   Bm*L[kG/cm**(m-1)]=%lg   Tilt[deg]=%lg", N, S, T_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix Multipole::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{
  //...............................................................................
  // NOTE: a matrix is computed only for a dipole , quadrupole 
  //       or first order radial focusing pole. for abs(N) > 1
  //       the matrix is unity. 
  //...............................................................................

  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  
  RMatrix mi;
  mi.toUnity();

  if( N ==1 ){
      mi[1][0] = -S / hr;
      mi[3][2] =  S / hr;
      //if(T != 0) tilt(T, &mi);
      if(T_ != 0) mi = mi.similarOrtho(RMatrix::m_tilt(T_*PI/180.0));
    }
    if( N ==- 1) {
      mi[1][0] = -S / hr;
      mi[3][2] = -S / hr;
    }

    if( N == 0) {

      double fi    = T_/180.0*PI; // fi = tilt 
      double theta = S / hr;

      // old code. should be removed 
      // mi[1][5] = S / hr * cos(fi);  
      // mi[3][5] = S / hr * sin(fi);

      mi[1][5] =  theta; // S = B*s/Brho = s/rho = theta (bend angle)  
      mi[4][0] = -theta;

      //if(T != 0.0)  tilt(T, &mi);
     if(T_ != 0) mi = mi.similarOrtho(RMatrix::m_tilt(T_*PI/180.0));
     if( (T_ != 0.0) ||(tetaY != 0.0) ) tiltb(T_, S / hr, tetaY, 0);

    }

    return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Multipole::setParameters( int np, double dat[], ... )
{
  L_ = 0.0;
  B  = 0.0;

  double Ndum = 0.0;
  double* rdat[] = {&Ndum, &S, &T_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  if (Ndum !=0.0 ) N = floor(Ndum);
  
  //N  = dat[0];  // multipole harmonic (< 0 ==> radial focussing)
  //S  = dat[1];  // integrated derivative of order N  int_0^L_ [dB_y/dz] dz 
  //T_ = dat[2];  // tilt (roll) angle 
  // B  = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Multipole::setParameters( int np, std::vector<double> const& dat, ... )
{
  L_ = 0.0;
  B  = 0.0;

  double Ndum = 0.0;
  double* rdat[] = {&Ndum, &S, &T_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  if (Ndum !=0.0 ) N = floor(Ndum);
  
  //N  = dat[0];  // multipole harmonic (< 0 ==> radial focussing)
  //S  = dat[1];  // integrated derivative of order N  int_0^L_ [dB_y/dz] dz 
  //T_ = dat[2];  // tilt (roll) angle 
  // B  = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Multipole::trackOnce( double ms,   double& Enr0, int n_elem, int n_turn, TrackParam& prm,
       		          RMatrix const& m1, Coordinates& v) const
{
  int status = 0; 
  if (status = backwardTest(prm, n_elem, n_turn, v)) return status;

  if(N != 0) {
    multipole_trans( this, prm.Hr0/(1.+v[5]), &v, &v);
  }
  else {

    v[1] += S * cos(T_ / 180. * PI) / prm.Hr0 * v[5];
    v[3] += S * sin(T_ / 180. * PI) / prm.Hr0 * v[5];
  }

 done:	

  if (status = transAmpTest(prm, n_elem, n_turn, v)) return status; 

  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Beamline* Multipole::splitnew(int nslices) const  // return a sliced element as a beamline 
{
 // Multipole cannot be split so we return a cloned element with slices_ = 1. 
  Beamline* bml = new Beamline();
  Multipole* e      = this->clone();
  e->L_      /= 1;
  e->slices_  = 1;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Multipole::propagate( double hr, double ms, RMatrix_t<3>& W) const
{
  // NOTE: Only bending elements alter the orientation of the local (Frenet-Serret) axes.
  //       a thin dipole has 0 length so V is unchanged.  
     
  // N: pole order N=0: Dipole

  if ( N != 0) return; // return if multipole element is not a dipole 
  
  double psi   = (T_/ 180.) * PI;
  double alfa  = S / hr;
  double sa    = sin(alfa);
  double ca    = cos(alfa);
  double si    = sin(psi);
  double ci    = cos(psi);

  RMatrix_t<3> S{ {  ca,    0.0,     sa  },
                  { 0.0,    1.0,    0.0  },
                  { -sa,    0.0,     ca  } };
         
  // if the element is not rotated about the local z-axis, just apply
  // the bend transformation and return.
  
  if ( fabs(si) < 1.0e-9 ) {
    W = W * S.transpose();
    return;
  }

  RMatrix_t<3> T{ {ci,    -si,      0.0 },
                  {si,     ci,      0.0 },
                  {0.0,    0.0,     1.0 } };


  W = W * T.transpose()* S.transpose() * T;

  return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Multipole::propagate( double hr, double ms, RMatrix_t<3>& W, Vector_t<3>& V ) const
{
  // NOTE: Only bending elements alter the orientation of the local (Frenet-Serret) axes.
  //       a thin dipole has 0 length so V is unchanged.  
     
  // N: pole order N=0: Dipole

  if ( N != 0) return; // return if multipole element is not a dipole 
  
  double psi   = (T_/ 180.) * PI;
  double alfa  = S / hr;
  double sa    = sin(alfa);
  double ca    = cos(alfa);
  double si    = sin(psi);
  double ci    = cos(psi);

  RMatrix_t<3> S{ {  ca,    0.0,     sa  },
                  { 0.0,    1.0,    0.0  },
                  { -sa,    0.0,     ca  } };
         
  // if the element is not rotated about the local z-axis, just apply
  // the bend transformation and return.
  
  if ( fabs(si) < 1.0e-9 ) {
    W = W * S.transpose();
    return;
  }

  RMatrix_t<3> T{ {ci,    -si,      0.0 },
                  {si,     ci,      0.0 },
                  {0.0,    0.0,     1.0 } };


  W = W * T.transpose()* S.transpose() * T;

  // To get the MADX coordinate system, exit here
  // if (MAD_LOCAL_FRAME) return

  //................................................................................................
  // What follows is specific to OPTIMX. 
  // OPTIMX defines the local x-axis so that it always lie in the global X-Z plane 
  
  // To bring x back in the global X-Z plane,  we determine and then "undo" the angle psi betweeen
  // the local x axis and the X-Z plane at the element exit.

  // we assume that cos(phi) ( aka cos(theta_y) is always positive
  // this equivalent to restrict  theta_y to the interval  -pi/2 < theta_y <pi/2 
  //................................................................................................

  double sp = W[1][2];          // sin(phi)
  double cp = sqrt(1.0 -sp*sp); // cos(phi) assumed always positive
   
  
  si =  W[1][0]/cp;                           // sin(psi)
  ci =  copysign(sqrt(1.0 -si*si), W[1][1]);  // cos(psi) has the sign of W[1][1]
  
  
  T[0][0] =  ci;
  T[0][1] = -si;
  T[1][0] =  si;
  T[1][1] =  ci;
 
  W =   W*T.transpose(); // apply inverse rotation 

  return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix  Multipole::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{
    //...............................................................................
  // NOTE: a matrix is computed only for a dipole , quadrupole 
  //       or first order radial focusing pole. for abs(N) > 1
  //       the matrix is unity. 
  //...............................................................................

  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  
  RMatrix mi;
  mi.toUnity();

  double psi = (T_* PI/180.0);
  double si   = sin(psi);
  double ci  = cos(psi);

  RMatrix T  = { {   ci,  0.0,   si,  0.0,  0.0,  0.0 },
                 {  0.0,   ci,  0.0,   si,  0.0,  0.0 },
                 {  -si,  0.0,   ci,  0.0,  0.0,  0.0 },
                 {  0.0,  -si,  0.0,   ci,  0.0,  0.0 },
                 {  0.0,  0.0,  0.0,  0.0,  1.0,  0.0 },
                 {  0.0,  0.0,  0.0,  0.0,  0.0,  1.0 }
              };


  
  if( N ==1 ){ // quadrupole
      mi[1][0] = -S / hr;
      mi[3][2] =  S / hr;
      if(T_ != 0) mi = T*mi*T.transpose();
    }
    if( N ==- 1) {
      mi[1][0] = -S / hr;
      mi[3][2] = -S / hr;
    }

    if( N == 0) {

      double theta = S / hr;

      mi[1][5] =  theta; // S = B*s/Brho = s/rho = theta (bend angle)  
      mi[4][0] = -theta;


      if( (fabs(T_) > 1.0-9)) {
        mi = T*mi*T.transpose();
        return mi;
      }
       
  
      propagate( hr, ms, frame);

      double psi_out   = psi + atan2( frame[1][0], frame[1][1] );
   
      double co  =  cos(psi_out);
      double so  =  sin(psi_out);

  

      RMatrix Tout     = { { co,    0.0,  so,   0.0,  0.0,  0.0 },
                           { 0.0,   co,   0.0,  so,   0.0,  0.0 },
                           {-so,    0.0,  co,   0.0,  0.0,  0.0 },
                           { 0.0,   -so,  0.0,  co,   0.0,  0.0 },
                           { 0.0,   0.0,  0.0,  0.0,  1.0,  0.0 },
                           { 0.0,   0.0,  0.0,  0.0,  0.0,  1.0 } };
           
      mi = Tout*mi*T.transpose(); 

   }

   return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// el->G - order of multipole; el->S - multipole strength;  el->T - tilt
void Multipole::multipole_trans(Element const* el, double hr, Coordinates* vp, Coordinates const* v)
{

  // v  = initial coodinates
  // vp = final coordinates 

  double a;
  double r;
  double rm;
  double teta;
  double factor;
  double alfa;

  int i;

  RMatrix tl;
   
   int m = el->N;
   for(int i=0; i<6; ++i)  { 
      vp->c[i] = v->c[i];
   }

   if(m==0) {  return; }	

   if(m<0) {
     m=-m;
      r=sqrt(vp->c[0] * vp->c[0] + vp->c[2] * vp->c[2]);
      i=1;
      rm = factor = 1.0;
      while(true) {
       factor *= i;
       i++;
       if(i>m) { 
         break;
       }
       rm *= r;
      }
      a = el->S*rm/factor/hr;
      vp->c[1] -= a*vp->c[0];
      vp->c[3] -= a*vp->c[2];
   }
   else {

    alfa = el->tilt()*PI/180.;
    tl = RMatrix::m_tilt(alfa);
    (*vp).c = tl * (*vp).c;

    r  = sqrt(vp->c[0] * vp->c[0] + vp->c[2] * vp->c[2]);
    if( fabs( r ) < std::numeric_limits<double>::epsilon() ) {
      teta=0.0; 
    } 
    else { 
      teta=atan2(vp->c[2], vp->c[0]);
    }
    for(i=1, rm=factor=1.; i <= m; i++) {
        rm *= r;	
        factor *= i;
    }
    a = el->S*rm/factor/hr;
    vp->c[1] -= a*cos(m*teta);
    vp->c[3] += a*sin(m*teta);
    tl = RMatrix::m_tilt(-alfa);
    (*vp).c = tl * (*vp).c;
   
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

