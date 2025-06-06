//  =================================================================
//
//  OptimCalc.cpp
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

#include <iostream>
#include <cstdio>
#include <complex>
#include <limits>

#include <Coordinates.h>
#include <Structs.h>
#include <OptimCalc.h>
#include <Cavity.h>
#include <RMatrix.h>
#include <Element.h>
#include <Utility.h>
#include <Twiss.h>
#include <Constants.h>

#include <QMessageBox>

using Constants::E_CGS;
using Constants::E_SI; 
using Constants::C_SI;
using Constants::ME_kg;
using Constants::B_Euler;
using Constants::PI;
using Constants::C_DERV1; 
using Constants::C_DERV3;
using Constants::C_CGS;
using Utility::gauss;
using Utility::matr_invert;

char const* intgrerr[] = {" ", "Spline approximation error",
  			       "Step size underflow", "Too many steps in routine odeint",
			       "Cavity phase fails to converge"};



inline double degToRad(double degs) { return degs*PI/180.0; }
inline double radToDeg(double rads) { return rads*180.0/PI; }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double tiltsp(double alfa, double fi, double rotat, double *tetaY, RMatrix* tm)
{
  // **** NOTE ****

  // looks the same as tiltb, with one extra argument

  // alfa  in degrees
  // tetaY in degrees
  // fi    in rad  [this is inconsistent and unexpected !] FIXME
  //-------------------

   RMatrix tl;
   alfa        =     degToRad(alfa);
   double teta =   degToRad(*tetaY);
   rotat       =    degToRad(rotat);

//    calculation of return tilt angle

   double nx       = -cos(alfa)*sin(fi);
   double ny       =  sin(teta)*cos(fi) - cos(teta)*sin(alfa)*sin(fi);
   double nz       =  cos(teta)*cos(fi) + sin(teta)*sin(alfa)*sin(fi);

   double ns       =  sqrt(nx*nx + nz*nz);

   double bx       = -sin(alfa);
   double by       =  cos(alfa)*cos(teta);
   double bz       = -cos(alfa)*sin(teta);

   double c        = -bx*nx*ny/ns + by*ns - bz*ny*nz/ns;
   double s        = -bx*nz/ns + bz*nx/ns;

   double alfap    =  atan2(s,c);

   (*tetaY) =  radToDeg(asin(ny));

//-------------------

   tl = RMatrix::m_tilt(rotat);
   (*tm) = (*tm) * tl;
   tl = RMatrix::m_tilt(alfa-alfap-rotat);   // <===== rotat is used here;
                                             // this is the only difference with tiltb
   (*tm) =  tl* (*tm);
   return alfap/PI*180.;  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// alfa[deg], fi[rad], tetaY[deg]

double tiltb_old(double alfa, double fi, double& tetaY, RMatrix* tm)
{

  // THIS IS THE OLD FUNCTION !
  //...................................................
  // Update  tetaY  and apply input and output rotations 
  // if tm = 0, update tetaY only !!!! 
  //...................................................
  
         alfa =   (alfa/180.0)*PI;
  double teta =  (tetaY/180.0)*PI;

   //  calculation of return tilt angle
   double nx    = -cos(alfa)*sin(fi);
   double ny    = sin(teta)*cos(fi) - cos(teta)*sin(alfa)*sin(fi);
   double nz    = cos(teta)*cos(fi) + sin(teta)*sin(alfa)*sin(fi);
   double ns    = sqrt(nx*nx + nz*nz);
   double bx    = -sin(alfa);
   double by    =  cos(alfa)*cos(teta);
   double bz    = -cos(alfa)*sin(teta);
   double c     = -bx*nx*ny/ns + by*ns - bz*ny*nz/ns;
   double s     = -bx*nz/ns + bz*nx/ns;
   double alfap = atan2(s,c);

   tetaY  = asin(ny)*(180./PI);;
	 
   if (tm) { 
    RMatrix tl = RMatrix::m_tilt(alfa);
    (*tm) = (*tm) * tl;
    tl = RMatrix::m_tilt(-alfap);
    (*tm) =  tl * (*tm);
   }

   alfap  *= (180./PI);

   //std::cout <<  "alfa  =  " << alfa*(180.0/PI) << std::endl;
   //std::cout <<  "alfap =  " << alfap           << std::endl;
   //std::cout <<  "tetaY =  " << tetaY           << std::endl;

   
   return alfap;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double tiltb_new(double alfa, double fi, double& tetaY, RMatrix* tm)
{

  
  //...................................................
  // Update  tetaY  and apply input and output rotations 
  // if tm = 0, update tetaY only !!!! 
  //...................................................
  
         alfa =   (alfa/180.0)*PI;
  double teta =  (tetaY/180.0)*PI;


  double sa = sin(alfa);
  double ca = cos(alfa);

  double sx = sin(teta);
  double cx = cos(teta);

  double sf = sin(fi);
  double cf = cos(fi);
  
  
  
  RMatrix_t<3> rotx( { { 1.0,    0.0,    0.0  },
                       { 0.0,    cx,    -sx  },
                       { 0.0,    sx ,    cx  }} );

  RMatrix_t<3> roty = { {  cf,    0.0,    -sf },
                        { 0.0,    1.0,    0.0 },
                        {  sf,    0.0,    cf  } };

  RMatrix_t<3> S   ( { {  ca,     sa,   0.0 },
                       { -sa,     ca,   0.0 },
                       { 0.0,    0.0,   1.0 }} );




  RMatrix_t<3> W = rotx*roty; // orientation of the local frame in absolute coordinates 

  
  double alfap  = atan2( W[0][2], W [2][2]);
  tetaY         = asin(  W[1][2])*(180./PI); 

   if (tm) { 
    RMatrix tl = RMatrix::m_tilt(alfa); // go to a frame where the  
    (*tm) = (*tm) * tl;                 // apply the transfer matrix
    tl = RMatrix::m_tilt(-alfap);       // particle emerges at angle alfap (alfa prime) w/r to original  
    (*tm) =  tl * (*tm);                // we take this out so that the trajectory tangent vector is  (e_s x e_z) = 0 is along 
   }

   alfap  *= (180./PI);

   //std::cout <<  "alfa  =  " << alfa*(180.0/PI) << std::endl;
   //std::cout <<  "alfap =  " << alfap  << std::endl;
   //std::cout <<  "tetaY =  " << tetaY  << std::endl;
   
   return alfap;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double tiltb(double alfa, double fi, double& tetaY, RMatrix* tm)
{
  return tiltb_old(alfa, fi, tetaY, tm);
  //return tiltb_new(alfa, fi, tetaY, tm);
}
  
int find_tunes (RMatrix& tm, double L, Twiss& v, double* alfa) 
{

  // compute periodic lattice functions and tunes corresponding to (decoupled) 4x4 matrix tm  

  // alfa = momentum compaction factor 

    double mu  = 0.0 ;
    int    ret = 0;

    double cx=0.5 *(tm[0][0] + tm[1][1]);

    if( fabs(cx) < 1.0) {
        v.BtX = fabs(tm[0][1])/sqrt(1.-cx*cx);
	mu = atan2( tm[0][1] / v.BtX, cx);
	v.nuX = mu / (2.*PI);
	v.AlX = (tm[0][0] - tm[1][1])/(2.*sin(mu));
	v.DsX = ( tm[0][5] * (1. - tm[1][1]) + tm[1][5] * tm[0][1] )
						 /(2. - tm[0][0] - tm[1][1]);
	v.DsXp =( tm[1][5] * (1. - tm[0][0]) + tm[0][5] * tm[1][0])/
						 (2. - tm[0][0] - tm[1][1]);
	if(v.nuX < 0.0) { v.nuX += 1.; }
    }
    else { 
       v.nuX = cx;  
       ret = ret|1;
    }

    double cy=0.5 *(tm[2][2] + tm[3][3]);

    if(fabs(cy) < 1.0) {
        v.BtY = fabs(tm[2][3])/sqrt(1.-cy*cy);
	mu = atan2( tm[2][3] / v.BtY, cy);
	v.nuY = mu / (2.*PI);
	v.AlY = (tm[2][2] - tm[3][3])/(2.*sin(mu));
	v.DsY =  ( tm[2][5] * (1. - tm[3][3]) + tm[3][5] * tm[2][3] )
						 / (2. - tm[2][2] - tm[3][3]);
	v.DsYp = ( tm[3][5] * (1. - tm[2][2]) + tm[2][5] * tm[3][2])
	                                         / (2. - tm[2][2] - tm[3][3]);
	if(v.nuY < 0.0) { v.nuY += 1.0;}
    }
    else { 
       v.nuY = cy;  
       ret = ret|2;
    }


    // ***FIX ME**** this does not belong here.  Note this is (dS/L)/(dp/p) , not mcf.
    
    
    if(ret==0) {
          *alfa = ( v.DsX * tm[4][0] + v.DsXp * tm[4][1] +
    	            v.DsY * tm[4][2] + v.DsYp * tm[4][3] + tm[4][5] ) / L;
    }

    
    return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double tnorm(double x)
{
  return ( x >  PI) ? (x -= 2.0*PI*int((PI+x)/(2.0*PI)))
                    : (x += 2.0*PI*int((PI-x)/(2.0*PI)));
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void getDisp4D(RMatrix& tm, Twiss4D& v, char* err)
{
  double    m[4][4];
  double minv[4][4];

  for(int i=0; i<4; ++i) {
     for(int j=0; j<4; ++j) {
        m[i][j] = -tm[i][j];
     }
   }
   for(int i=0; i<4; ++i) m[i][i] += 1.0;

   err[0]=0;

   matr_invert(4, m, minv, err);

   if(err[0]) return;

   v.dx  = minv[0][0]*tm[0][5] + minv[0][1]*tm[1][5] + minv[0][2]*tm[2][5] + minv[0][3]*tm[3][5];
   v.dxp = minv[1][0]*tm[0][5] + minv[1][1]*tm[1][5] + minv[1][2]*tm[2][5] + minv[1][3]*tm[3][5];
   v.dy  = minv[2][0]*tm[0][5] + minv[2][1]*tm[1][5] + minv[2][2]*tm[2][5] + minv[2][3]*tm[3][5];
   v.dyp = minv[3][0]*tm[0][5] + minv[3][1]*tm[1][5] + minv[3][2]*tm[2][5] + minv[3][3]*tm[3][5];

   return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void GetSolenoidMatrix(double kb, std::complex<double> Kx, std::complex<double> Ky, double len, RMatrix* mt)
{

  // called from SpaceCharge 
  
  std::complex<double> S1, S2, K0, K2, K02, K22, Kx2, Ky2, Cs0, Cs2, Sn0, Sn2, Dn, S;
  double s, cs, sn;
  int i,j;
  RMatrix ms;
  ms.toUnity();
  
  // Zero magnetic field case
  
  if(fabs(kb*len)<1.e-20){   //Zero magnetic field case
    if(abs(Kx)<1.e-20) { 
      ms[0][1]=len;
    }
    else{
      Cs0=cosh(Kx*len);
      Sn0=sinh(Kx*len);
      ms[0][0]=ms[1][1]=real(Cs0);
      ms[0][1]=real(Sn0/Kx);
      ms[1][0]=real(Sn0*Kx);
    }

    if(abs(Ky)<1.e-20) { 
      ms[2][3]=len;
    }  
    else {
      Cs2 = cosh(-Ky*len);
      Sn2 = sinh(-Ky*len);
      ms[2][2]=ms[1][1]=real(Cs2);
      ms[2][3]=real(Sn2/Kx);
      ms[3][2]=real(Sn2/Kx);
    }
    return;
  }
  
  // Non-zero magnetic field case
  if(abs(Kx*Ky) < 1.e-40 ) {
    cs=cos(kb*len);
    sn=sin(kb*len);
    ms[0][0] = ms[1][1]=ms[2][2]=ms[3][3]=(1+cs)/2.;
    ms[0][1] = ms[2][3]=sn/kb;
    ms[1][0] = ms[3][2]=-kb*sn/4.;
    ms[0][2] = ms[1][3]=sn/2.;
    ms[2][0] = ms[3][1]=-sn/2.;
    ms[0][3] =  (1-cs)/kb;
    ms[2][1] = -ms[0][3];
    ms[1][2] = -(1-cs)*kb/4.;
    ms[3][0] =-ms[1][2];
    return;
   }
   Kx2 = Kx*Kx;
   Ky2 = Ky*Ky;
   S1  = 0.5*(kb*kb-Kx2-Ky2);
   S2  = S1*S1-Kx*Kx*Ky*Ky;
   S2  = sqrt(S2);
   K02 = S1+S2;
   K0  = sqrt(K02);
   S   = Kx*Ky/(kb*kb);
   if(abs(S*S)>1.e-6) {
     K22 = S1-S2;
     K2  = sqrt(K22);
   }
   else {
     K22=Kx*Kx*Ky*Ky/(kb*kb-Kx*Kx-Ky*Ky);
     K2=sqrt(K22);
   }
   Dn  = 2.*S2;
   Cs0 = cos(K0*len);
   Cs2 = cos(K2*len);
   Sn0 = sin(K0*len);
   Sn2 = sin(K2*len);

   ms[0][0] =  real( (Cs2*(K02+Kx2)-Cs0*(K22+Kx2)) / Dn );
   ms[0][1] =  real( (Sn0*(K2*Ky+K0*Kx)-Sn2*(K0*Ky+K2*Kx)) / (Kx*Dn) );
   ms[0][2] =  real( (Sn2*K0-Sn0*K2)*kb*Ky / (Kx*Dn ) );
   ms[0][3] =  real( (Cs2-Cs0)*kb / Dn );
   ms[1][0] =  real( ms[0][1]*Kx*Kx );
   ms[1][1] =  real( (Cs0*(K02+Ky2)-Cs2*(K22+Ky2)) / Dn );
   ms[1][2] =  real( ms[0][3]*Ky*Ky );
   ms[1][3] =  real( (Sn0*K0-Sn2*K2)*kb / Dn );
   ms[2][0] =  real( (Sn0*K2-Sn2*K0)*kb*Kx / (Ky*Dn ) );
   ms[2][1] = -ms[0][3];
   ms[2][2] =  real( (Cs2*(K02+Ky2)-Cs0*(K22+Ky2)) / Dn );
   ms[2][3] =  real( (Sn0*(K0*Ky+K2*Kx)-Sn2*(K2*Ky+K0*Kx)) / (Ky*Dn) );
   ms[3][0]=  -real( ms[0][3]*Kx*Kx);
   ms[3][1]=  -ms[1][3];
   ms[3][2]=   real( (Sn0*(K0*Ky+K2*Kx)-Sn2*(K2*Ky+K0*Kx))*Ky / Dn );
   ms[3][3]=   real( (Cs0*(K02+Kx2)-Cs2*(K22+Kx2)) / Dn );

   for(i=0; i<6; i++) { 
     for(j=0; j<6; j++) { 
       (*mt)[i][j] = ms[i][j];
        s=0.5*kb;
     }
   } 

   (*mt)[0][0] = ms[0][0] - s*ms[0][3];
   (*mt)[0][2] = ms[0][2] + s*ms[0][1];
   (*mt)[1][0] = ms[1][0] - s*(ms[2][0] + ms[1][3]) + s*s*ms[2][3];
   (*mt)[1][1] = ms[1][1] - s*ms[2][1];
   (*mt)[1][2] = ms[1][2] + s*(ms[1][1] - ms[2][2]) - s*s*ms[2][1];
   (*mt)[1][3] = ms[1][3] - s*ms[2][3];
   (*mt)[2][0] = ms[2][0] - s*ms[2][3];
   (*mt)[2][2] = ms[2][2] + s*ms[2][1];
   (*mt)[3][0] = ms[3][0] + s*(ms[0][0] - ms[3][3]) - s*s*ms[0][3];
   (*mt)[3][1] = ms[3][1] + s*ms[0][1];
   (*mt)[3][2] = ms[3][2] + s*(ms[3][1] + ms[0][2]) + s*s*ms[0][1];
   (*mt)[3][3] = ms[3][3] + s*ms[0][3];
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void GetBeamFocusing(double current, double bt, BeamSize const& bs, double& gx, double& gy, double& gs)
{
   // compute space charge focusing in a bunch  
   //
   // bt = v/c
   //
   // (linear) (de-)focusing due to beam space charge   
   // bs.alpha = ellipse orientation w/r to trajectory
   // charge is assumed to be uniformly distributed within an ellipse of dims a,b    
   // NOTE: the formula for s is in CGS units

   // gs, gx, gy ==> equivalent magnetic strengths for space charge 
   // gx = gradient x direction   
   // gy = gradient y direction   
   // gs = skew component magnitude
   //
   // Note current is specified in [Ampere] in the space charge block.
   // The formula below is in cgs, current is in [Abampere]
   //
   // 1 abampere = 0.01 Ampere (hence the factor 0.0004)
   //--------------------------------------------------------------------------

   double const& a     = bs.a; 
   double const& b     = bs.b; 
   double const& alpha = bs.alpha; 
   
   double sq = sqrt(1. - alpha*alpha);
   double s  = 0.0004*current*(1./(bt*bt)-1.)/(sq*( a*a + b*b + 2.*a*b*sq));

   
   gx = -s*(b/a + sq);  // Note: negative sign = defocusing 
   gy = -s*(a/b + sq);

   gs = s*bs.alpha;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void get2DMatrix(double hr, double g, double len, RMatrix& mi, SubSpace n)
{
  // [n,n] => offset in matrix mi 

  // n = 0 for x, x' 
  // n = 2 for y, y'
  // n = 4 for s, dp/p 

  double k1 = sqrt( fabs(g/hr) ); // g/hr = B'/(Brho) : optical focusing strength

  if( k1  < 1.0e-40 ) { 
     mi[n] [n  ] = mi[n+1][n+1] = 1.0;
     mi[n] [n+1] = len;
     mi[n+1][n ] = 0.0;
     return; 
  }
   
   double fi = len*k1;

   mi[n  ][n  ]  =  (g > 0.0) ?  cos(fi)    :  cosh(fi);
   mi[n+1][n+1] =   mi[n  ][n  ]; 
   mi[n  ][n+1]  =  (g > 0.0) ?  sin(fi)/k1 :  sinh(fi)/k1;
   mi[n+1][n  ]  =  (g > 0.0) ? -k1*k1*mi[n][n+1] : k1*k1*mi[n][n+1];

   return;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void get2DMatrix(double hr, double g, double len, double& M11, double& M12, double& M21, double& M22)
{

  
  double s, fi;
   
   if( fabs(g/hr) <1.e-40 ) {

     M11 = 1.0;
     M22 = 1.0;
     M12 = len;
     M21 = 0.0;
     return;
   }

   if(g >0){
     s    = sqrt( g / hr );
     fi   = len * s;
     M11  = M22 =   cos(fi);
     M12  = sin(fi)/s;
     M21  = - s*s*M12;
   }
   else {
     s    = sqrt( - g / hr );
     fi   = len * s;
     M11 = M22 =  cosh(fi);
     M12 = sinh(fi)/s;
     M21 = s *s*M12;
   }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void GetNormalizedGradients(double gx, double gy, double gs, double& Gx, double& Gy, double& rotat)
{

  // rotat =  rotation angle in degrees
      
  double s = gx-gy;
  
  rotat = ( fabs(gs*gs + s*s) < std::numeric_limits<double>::epsilon() ) ? 0.0 : 0.5*atan2(2*gs, s)*(180.0/PI);

  s   = sqrt(s*s+4.*gs*gs);

  Gx = 0.5*(gx+gy + s);
  Gy = 0.5*(gx+gy - s);
}

//
void GetNormalizedGradientsNew(double gx, double gy, double gs, double& Gx, double& Gy, double& rotat)
{

  // rotat =  rotation angle in radian
      
  double s = gx-gy;
  
  rotat = ( fabs(gs*gs + s*s) < std::numeric_limits<double>::epsilon() ) ? 0.0 : 0.5*atan2(2*gs, s);

  s   = sqrt(s*s+4.*gs*gs);

  Gx = 0.5*(gx+gy + s);
  Gy = 0.5*(gx+gy - s);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

