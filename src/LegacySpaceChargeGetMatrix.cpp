//  =================================================================
//
//  LegacySpaceChargeGetMatrix.cpp
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
#include <iostream>

#include <Element.h>
#include <Constants.h>
#include <OptimMainWindow.h>
#include <Utility.h>
#include <RMatrix.h>

using Constants::PI;
using Constants::C_DERV1;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double OptimMainWindow::GetCWSpaceChargeMatr(double& Enr, double ms, double current,
	BeamSize& bs, Element* el, double& tetaY, double dalfa,	RMatrix& mi, int st)
{

  double dg;
  double rotat;

  double Gx; // "normalized" hor gradient (includes element field + sc)  
  double Gy; 

  double Gx0;
  double Gy0;
  double Gs0;

  double ri, cs, sn, k, ks, kp, s, c;

  std::complex<double> Kx, Ky;
  
  double P  = Enr+ms;
         P  = sqrt(P*P-ms*ms);
  double hr = P/C_DERV1;
  double bt = P/(Enr+ms);
  double alfap = 0.;

  mi.toUnity();

  // Zero current case

  if(fabs(current)<1.e-12){
    mi = el->rmatrix(alfap, Enr, ms, tetaY, dalfa, st);
    return alfap;
  }

  double gx = 0.0; // hor beam focusing due to sc
  double gy = 0.0; // ver beam focusing due to sc
  double gs = 0.0; // lon beam focusing due to sc

  GetBeamFocusing(current, bt, bs, gx, gy, gs);

  // Non-zero current case
  switch ( toupper(el->name[0]) ){
    default:     // X, W, H, M, G, A, W
      mi = el->rmatrix(alfap, Enr, ms, tetaY, dalfa, st);
      break;

      case 'O': 
      case 'I': 
      case 'K': 
      case 'Z': 
      case 'S': // empty line, corrector, sextupole
	GetNormalizedGradients(gx, gy, gs, Gx, Gy, rotat); // rotat in degs !
	get2DMatrix(hr, Gx, el->L, mi, Horizontal);
	get2DMatrix(hr, Gy, el->L, mi, Vertical);
        mi = mi.similarOrtho(RMatrix::m_tilt(-rotat*PI/180.0));
        break;

      case 'Q':
	// quad

	dg = el->G*cos(el->T*PI/90.0); // dg = gradient correction due to quad rotation angle T 

	GetNormalizedGradients(gx+dg, gy-dg, gs-el->G*sin(el->T*PI/90.), Gx, Gy, rotat);
	get2DMatrix(hr, Gx, el->L, mi, Horizontal);
	get2DMatrix(hr, Gy, el->L, mi, Vertical);
	mi = mi.similarOrtho(RMatrix::m_tilt(-rotat*(PI/180)));
	break;

      case 'L':  // electrostatic quad
    	dg=el->G*cos(el->T*PI/90.)/(C_DERV1*bt);
	GetNormalizedGradients(gx+dg, gy-dg, gs-el->G*sin(el->T*PI/90.)/(C_DERV1*bt), Gx, Gy, rotat);
	get2DMatrix(hr, Gx, el->L, mi, Horizontal);
	get2DMatrix(hr, Gy, el->L, mi, Vertical);
	mi = mi.similarOrtho(RMatrix::m_tilt(-rotat*(PI/180)));
	break;

      case 'F': // lithium lens
    	dg=0.2*PI*el->G; //  el->G   - current in kA
	GetNormalizedGradients(gx+dg, gy+dg, gs, Gx, Gy, rotat);
	get2DMatrix(hr, Gx, el->L, mi, Horizontal);
	get2DMatrix(hr, Gy, el->L, mi, Vertical);
        mi = mi.similarOrtho(RMatrix::m_tilt(-rotat*(PI/180)));
       break;

      case 'C':                              // solenoid
    	if( fabs(el->B) < std::numeric_limits<double>::epsilon() ) { // B = 0.0 is a drift
      	  GetNormalizedGradients(gx, gy, gs, Gx, Gy, rotat);
	  get2DMatrix(hr, Gx, el->L, mi, Horizontal);
	  get2DMatrix(hr, Gy, el->L, mi, Vertical);
          mi = mi.similarOrtho(RMatrix::m_tilt(-rotat*(PI/180)));
      	  break; 
	}
	if ( toupper(el->name[1]) == 'C'){     // focusing part only
    	  dg=0.25 * el->B * el->B / hr; //
	  GetNormalizedGradients(gx+dg, gy+dg, gs, Gx, Gy, rotat);
	  get2DMatrix(hr, Gx, el->L, mi, Horizontal);
	  get2DMatrix(hr, Gy, el->L, mi, Vertical);
          mi = mi.similarOrtho(RMatrix::m_tilt(-rotat*(PI/180)));
        }
	else {                              // 4-dimensional transfer matrix
	  GetNormalizedGradients(gx, gy, gs, Gx, Gy, rotat);
      	  Kx = sqrt( -std::complex<double>(Gx, 0.) / hr);
      	  Ky = sqrt( -std::complex<double>(Gy, 0.) / hr);
	  GetSolenoidMatrix(el->B/hr, Kx, Ky, el->L, &mi);
          mi = mi.similarOrtho(RMatrix::m_tilt(-rotat*(PI/180)));
        }
        if(st){
      	  RMatrix edge;
	  edge.toUnity();
      	  k=el->B/hr;
      	  edge[1][0] = edge[3][2] = 0.125*k*k*el->A;
          if(st==1) { mi = edge*mi;    } 
          if(st==2) { mi = mi*edge;    }
          if(st==3) { mi = mi*edge*mi; }
        }
	break;

      case 'D': 	 
      case 'B':                // combined functions dipole
    	ri = el->B / hr;
        s=-(el->T+dalfa)*PI/180.;
    	dg	= ( el->G + 0.5*el->B*ri )*cos(2.*s);
    	Gx0 = 0.5*el->B*el->B/hr + dg;
    	Gy0 = 0.5*el->B*el->B/hr -dg;
        Gs0 = ( el->G + 0.5*el->B*ri )*sin(2.*s);
	GetNormalizedGradients(gx+Gx0, gy+Gy0, gs+Gs0, Gx, Gy, rotat);
	get2DMatrix(hr, Gx, el->L, mi, Horizontal);
	get2DMatrix(hr, Gy, el->L, mi, Vertical);
        s -= rotat*(PI/180.0);
        cs = cos(s);
        sn = sin(s);
	// horizontal motion
        k=Gx/hr;
	if( fabs(k) < std::numeric_limits<double>::epsilon() ){
	  mi[4][1] = mi[0][5] = 0.5 * el->L * el->L * ri * cs;
	  mi[4][0] = mi[1][5] = el->L * ri * cs;
	  mi[4][5] = el->L * el->L* el->L * ri * ri * cs *cs  /6.;
        }
        else {
	  if(k > 0) { 
            ks=sqrt(k);	s = el->L * ks;	c=cos(s);   	s=sin(s);
          }
      	  else {
            ks=sqrt(-k); s = el->L * ks;   c=cosh(s);  s=sinh(s);
          }
	  mi[4][1] = mi[0][5] = (1.-c)*ri/k * cs;
	  mi[4][0] = mi[1][5] = s*ri/ks * cs;
	  mi[4][5] = (el->L * ks - s)*ri*ri/(k*ks) * cs * cs ;
        }
	// vertical motion
        k=Gy/hr;
	if( fabs(k) < std::numeric_limits<double>::epsilon() ){
	  mi[4][3] = mi[2][5] = 0.5 * el->L * el->L * ri * sn;
	  mi[4][2] = mi[3][5] = el->L * ri * sn;
	  mi[4][5] += el->L * el->L* el->L * ri * ri * sn * sn  /6.;}
        else {
	  if(k > 0){ 
            ks = sqrt(k);
	    s  = el->L * ks;
	    c  = cos(s);
	    s  = sin(s);
          }
      	  else { 
            ks = sqrt(-k);
	    s = el->L * ks;
	    c = cosh(s);
	    s = sinh(s);
          }
	  mi[4][3] = mi[2][5] = (1.-c)*ri/k * sn;
	  mi[4][2] = mi[3][5] = s*ri/ks * sn;
	  mi[4][5] += (el->L * ks - s)*ri*ri/(k*ks) * sn * sn ;
        }
        alfap = tiltsp(el->T + dalfa,  el->B * el->L / hr, -rotat, &tetaY,  &mi);
	break;

      case 'R':
	// combined functions dipole + electrostatic bend
	//el->S - electric field(kV/cm); el->T gradient(kV/cm**2)

	Gy0  = - (el->G + el->T/(C_DERV1*bt));
        ri   =  0.001*el->S/(P*bt) + el->B / hr;
	s    =  0.001*el->S * ms / (P*P);
	Gx0  = (ri*ri + s*s)*hr-Gy0;
	GetNormalizedGradients(gx+Gx0, gy+Gy0, gs, Gx, Gy, rotat);
	get2DMatrix(hr, Gx, el->L, mi[0][0], mi[0][1], mi[1][0], mi[1][1]);
	get2DMatrix(hr, Gy, el->L, mi[2][2], mi[2][3], mi[3][2], mi[3][3]);
	kp   = ri + 0.001*el->S * ms * ms / (P*P*(Enr+ms));
	cs     =  cos(rotat*PI/180);
        sn     = -sin(rotat*PI/180);
	// horizontal motion
        k = Gx/hr;
	if( fabs(k) < std::numeric_limits<double>::epsilon() ){
	  mi[4][1] = mi[0][5] = 0.5 * el->L * el->L * kp * cs;
	  mi[4][0] = mi[1][5] = el->L * kp * cs;
	  mi[4][5] = kp*ri * el->L * el->L* el->L * cs * cs/ 6.;
        }
        else {
	  if(k > 0.) { 
            ks=sqrt(k);
	    s = el->L * ks;
	    c = cos(s);
	    s = sin(s);
          }
      	  else{ 
            ks = sqrt(-k);
	    s  = el->L * ks;
	    c  = cosh(s);
	    s  = sinh(s);}
	    mi[4][1] = mi[0][5] = (1.-c)*kp/k * cs;
	    mi[4][0] = mi[1][5] = s*kp/ks  * cs;
	    mi[4][5] = (el->L * ks - s)*kp*ri/(k*ks)  * cs  * cs;
        }
        // vertical motion
        k = Gy/hr;
	if( fabs(k) < std::numeric_limits<double>::epsilon()) {
	  mi[4][3]  = mi[2][5] = 0.5 * el->L * el->L * kp * sn;
	  mi[4][2]  = mi[3][5] = el->L * kp * sn;
	  mi[4][5] += kp*ri * el->L * el->L* el->L * sn * sn/ 6.;
        }
        else {
	  if(k > 0.0){ 
            ks = sqrt(k);
	    s  = el->L * ks;	
            c  = cos(s);
	    s  = sin(s);
          }
      	  else{ 
            ks = sqrt(-k);
	    s  = el->L * ks;		
            c  = cosh(s);
	    s  = sinh(s);
          }
	  mi[4][3] = mi[2][5] = (1.-c)*kp/k * sn;
	  mi[4][2] = mi[3][5] = s*kp/ks  * sn;
	  mi[4][5] += (el->L * ks - s)*kp*ri/(k*ks)  * sn  * sn;

	}
        //tilt(-rotat, mi);
        mi = mi.similarOrtho(RMatrix::m_tilt(-rotat*(PI/180)));
	break;

      case 'E':                       // electrostatic accelerator section

	 double E1 = Enr;
         double E2 = Enr+el->B;
         double P1 = sqrt(2.*ms*E1+E1*E1);
         double P2 = sqrt(2.*ms*E2+E2*E2);
         double gamma1 = 1.0 + E1 / ms;
         double gamma2 = 1.0 + E2 / ms;
         double beta1  = sqrt(E1*(E1+2.*ms))/(E1+ms);
         double beta2  = sqrt(E2*(E2+2.*ms))/(E2+ms);
         double kapa   = sqrt(P2/P1);
         double ksi    = el->L/(gamma2-gamma1)*log( (gamma2+sqrt(gamma2*gamma2-1.))/ (gamma1+sqrt(gamma1*gamma1-1.)));
	 double fi1    = (E2-E1)/(2.*el->L)*(ms+E1)/(2.*ms*E1+E1*E1);
	 double fi2    = (E2-E1)/(2.*el->L)*(ms+E2)/(2.*ms*E2+E2*E2);
	 GetNormalizedGradients(gx, gy, gs, Gx, Gy, rotat);
         s = C_DERV1/ms*gamma1*beta1*(1.+gamma1*beta1/(gamma2*beta2))/2.;
         // x matrix
         if(Gx>0.0){
      	   k  = sqrt( s*Gx);
      	   cs = cos(k*ksi);
      	   sn = sin(k*ksi);
      	   k  = k/sqrt(gamma1*gamma2*beta1*beta2);
      	   mi[0][0] = cs*kapa-fi1*sn/k;
      	   mi[0][1] = sn/k;
      	   mi[1][0] = (kapa*fi2-fi1/kapa)*cs-(k+fi1*fi2/k)*sn;
      	   mi[1][1] = cs/kapa+fi2*sn/k;
         } 
         else {
      	   k  = sqrt( -s*Gx);
           cs = cosh(k*ksi);
      	   sn = sinh(k*ksi);
      	   k  = k/sqrt(gamma1*gamma2*beta1*beta2);
      	   mi[0][0] = cs*kapa-fi1*sn/k;
      	   mi[0][1] = sn/k;
      	   mi[1][0] = (kapa*fi2-fi1/kapa)*cs+(k-fi1*fi2/k)*sn;
      	   mi[1][1] = cs/kapa+fi2*sn/k;
         }
         // y matrix
         if(Gy>0.) {
      	   k  = sqrt( s*Gy);
      	   cs = cos(k*ksi);
      	   sn = sin(k*ksi);
      	   k=k/sqrt(gamma1*gamma2*beta1*beta2);
      	   mi[2][2] = cs*kapa-fi1*sn/k;
      	   mi[2][3] = sn/k;
      	   mi[3][2] = (kapa*fi2-fi1/kapa)*cs-(k+fi1*fi2/k)*sn;
      	   mi[3][3] = cs/kapa+fi2*sn/k;
         } 
         else {
      	   k  = sqrt( -s*Gy);
      	   cs = cosh(k*ksi);
      	   sn = sinh(k*ksi);
      	   k  = k/sqrt(gamma1*gamma2*beta1*beta2);
      	   mi[2][2] = cs*kapa-fi1*sn/k;
      	   mi[2][3] = sn/k;
      	   mi[3][2] = (kapa*fi2-fi1/kapa)*cs+(k-fi1*fi2/k)*sn;
      	   mi[3][3] = cs/kapa+fi2*sn/k;
         }
         mi = mi.similarOrtho(RMatrix::m_tilt(-rotat*(PI/180)));
         Enr = E2;
	 break;
   }

   return alfap;
}   
