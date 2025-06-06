//  =================================================================
//
//  Integrals4D.cpp
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
#include <Constants.h>
#include <GeneralPreferencesDialog.h>
#include <Element.h>
#include <Globals.h>
#include <RMatrix.h>
#include <OptimCalc.h>
#include <Twiss.h>
#include <OptimMainWindow.h>
#include <OptimApp.h>
#include <OptimTextEditor.h>
#include <OptimMessages.h>

using Constants::PI;
using Constants::KVSR;
using Constants::KE;
using Constants::C_DERV1;

static constexpr std::complex<double> J{0.0,1.0};


//......................................................................................

static std::complex<double> B1(Twiss4D const& v)
{
    auto& b1x = v.btx1;
    auto& b1y = v.bty1;
    auto& b2x = v.btx2;
    auto& b2y = v.bty2;
    auto& a1x = v.alfx1;
    auto& a1y = v.alfy1;
    auto& a2x = v.alfx2;
    auto& a2y = v.alfy2;
    auto& u   = v.u;
    auto& nu1 = v.teta1;
    auto& nu2 = v.teta2;
    auto& dx  = v.dx;
    auto& dxp = v.dxp;
    auto& dy  = v.dy;
    auto& dyp = v.dyp;

    std::complex<double> value = (1.0/sqrt(2.0)) * (    ((dx*a1x+dxp*b1x - J*(1-u)*dx)/sqrt(b1x))
						      + ((dy*a1y+dyp*b1y - J*u*dy)/sqrt(b1y)) * exp(-J*nu1) );
 
    return value;
};  
    
static std::complex<double> B2(Twiss4D const& v )
{
     auto& b1x = v.btx1;
     auto& b1y = v.bty1;
     auto& b2x = v.btx2;
     auto& b2y = v.bty2;
     auto& a1x = v.alfx1;
     auto& a1y = v.alfy1;
     auto& a2x = v.alfx2;
     auto& a2y = v.alfy2;
     auto& u   = v.u;
     auto& nu1 = v.teta1;
     auto& nu2 = v.teta2;
     auto& dx  = v.dx;
     auto& dxp = v.dxp;
     auto& dy  = v.dy;
     auto& dyp = v.dyp;

      std::complex<double> value = (1.0/sqrt(2.0)) * (   ((dx*a2x+dxp*b2x-J*u*dx)/sqrt(b2x)) * exp(-J*nu2)
							 + ((dy*a2y+dyp*b2y-J*(1-u)*dy)/sqrt(b2y)) );

     return value;
   };


static double g1ds(double B0, double alpha, double Hr, double G, Twiss4D const& v)
{
     using std::imag;
     auto& b1x = v.btx1;
     auto& b1y = v.bty1;
     auto& b2x = v.btx2;
     auto& b2y = v.bty2;
     auto& a1x = v.alfx1;
     auto& a1y = v.alfy1;
     auto& a2x = v.alfx2;
     auto& a2y = v.alfy2;
     auto& u   = v.u;
     auto& nu1 = v.teta1;
     auto& nu2 = v.teta2;
     auto& dx  = v.dx;
     auto& dxp = v.dxp;
     auto& dy  = v.dy;
     auto& dyp = v.dyp;

     std::complex<double> B1v = B1(v); 
     double value = B0*(B0*B0/Hr + 2*G)* (cos(alpha)*sqrt(2*b1x)*imag(B1v) 
					+ sin(alpha)*sqrt(2*b1y)*imag(exp(J*nu1)*B1v) ); 

     return value;
   };

static double g2ds(double B0, double alpha, double Hr, double G, Twiss4D const& v)
{
     using std::imag;
     auto& b1x = v.btx1;
     auto& b1y = v.bty1;
     auto& b2x = v.btx2;
     auto& b2y = v.bty2;
     auto& a1x = v.alfx1;
     auto& a1y = v.alfy1;
     auto& a2x = v.alfx2;
     auto& a2y = v.alfy2;
     auto& u   = v.u;
     auto& nu1 = v.teta1;
     auto& nu2 = v.teta2;
     auto& dx  = v.dx;
     auto& dxp = v.dxp;
     auto& dy  = v.dy;
     auto& dyp = v.dyp;

     std::complex<double> B2v = B2(v); 
     double value = B0*(B0*B0/Hr + 2*G)*(cos(alpha)*sqrt(2*b2x)*imag(exp(J*nu2)*B2v)
					+sin(alpha)*sqrt(2*b2y)*imag(B2v) ); 
     return value;
   };


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::integrStep(double dL, double gamma, double Hrt, double dEn2,
	Twiss4D const& v, Element& e, double& dg1,  double& dg2, double& dB2, double& nu1pr,
	double& nu2pr, double& em1n, double& em2n)
{

  
  // integral quantities through an element 
  // NOTHING HAPPENS IN THE CFEBEND ??? This is probably WRONG !!!!
  
  double ca, sa, c2a, c3a, s3a, gmsr;
  
  double gm2  = gamma*gamma;
         gmsr = sqrt(gm2-1.0);
	 double A1   = 2*std::norm(B1(v));
	 double A2   = 2*std::norm(B2(v));
         double knu  = 1./(4.*PI*Hrt);

   double s = 0.0; // WARNING: s is sometimes used a temp variable here  

   double alpha = 0.0; 

   switch( toupper(e.name()[0]) ) {
    case 'B':  
    case 'D':
     {
      alpha =  e.tilt()*PI/180.; // Tilt in rd
      
      ca     = cos(alpha);
      sa     = sin(alpha);
      c2a    = cos(2.*alpha);
  
      
      // contribution to equilibrium emittance 
      s      = 0.5*dEn2*gm2/(gmsr*gmsr*gmsr*ms*ms*1.e6)*dL/e.length(); 

      em1n  += A1*s;
      em2n  += A2*s;

	  // contribution to damping decrement factors
      dg1   += dL*g1ds(e.B, alpha, Hrt, e.G, v);
      dg2   += dL*g2ds(e.B, alpha, Hrt, e.G, v);
      dB2   += e.B*e.B*dL;

      
      // Chromaticity contribution
      double D = v.dx*ca + v.dy*sa;
      double h = e.B/Hrt;
      double k = e.G*c2a/Hrt;
      double A = dL/(4.*PI);
      double gammaX = (1. + v.alfx1 * v.alfx1) / v.btx1;
      double gammaY = (1. + v.alfy2 * v.alfy2) / v.bty2;
      s       = v.btx1*( k*(h*(v.dx*ca+D)-1.) - h*h*ca*ca);
      nu1pr  += 0.0; // A*(s - 2.*v.alfx1*v.dxp*h*ca + gammaX*h*D); // dnu1/(dp/p) ??? 
      s       = v.bty2*( k*(1.-h*(v.dy*sa+D)) - h*h*sa*sa);
      nu2pr  += 0.0;// A*(s - 2.*v.alfy2*v.dyp*h*sa + gammaY*h*D);     
     }
   break;
   case 'Q':
     s   = e.tilt()*PI/180.;
     c2a =cos(2*s);
      // Chromaticity contribution
     nu1pr -= 0.0;// knu*v.btx1*dL*e.G*c2a;
     nu2pr += 0.0;// knu*v.bty2*dL*e.G*c2a;
     break;
   case 'S':
     s   = e.tilt()*PI/180.;
     c3a = cos(3.*s);
     s3a = sin(3.*s);
     s   = knu*dL*e.S*(c3a*v.dx+s3a*v.dy);
      // Chromaticity contribution
     nu1pr += 0.0;// v.btx1*s;
     nu2pr -= 0.0;// v.bty2*s;
   default:
     break;
  }
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdIntegrals4D()
{

  char buf[257];
  char buf1[40];

  if(interrupted_ ) { interrupted_  =  false; return;}
   
  if ( analyze(!analyzed_) ) return;

  // The following 2 lines have been commented out as they appear to be superfluous.  

  //int status = getTrajParamFromFile(false, false, v_anlz); // dummy call to make sure parameter block exists.
  //if (status != 0 ) return;
  
  using std::imag;
 
  Twiss4D v;
  RMatrix tm;     
  double dEn2t, D, Dp, h, k, s, ca, sa, c2a, tg;
  char format[]  = {"%12d %12.1f %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g\n"};
  int j;

int   status = Get4DBetasFromFile(true, false, v); // 1st arg: Reprint
                                                   // 2nd arg: Update 4D block   
  if (status != 0 ) return;

  double Lp = 0.0;

  std::complex<double> ev[4][4];
  std::complex<double> lambda[4];

  char errmsg[80];
  errmsg[0] = 0;

  if( CtSt_.IsRingCh ) {
    findRMatrix(tm);
    status = 0;
    if ( (status = RMatrix_t<4,double>(tm).findEigenValues( lambda, ev) !=0 ) ) {
      OptimMessageBox::warning(this, "Integrals 4D", "Cannot close lattice.", QMessageBox::Ok);
    }
    else{
      v.betatronFromEV(ev);
      getDisp4D(tm, v, errmsg);
      if(errmsg[0]) { OptimMessageBox::warning(this, "Integrals 4D", errmsg, QMessageBox::Ok);   return; }
    }
    v.betatronFromEV(ev);   // get the periodic lattice functions 
  }  

  //......................................................................................................................

  auto restore = [](int* p){ QGuiApplication::restoreOverrideCursor(); delete p;};
  std::unique_ptr<int,decltype(restore)> cursor_guard(new int, restore); 

  QGuiApplication::setOverrideCursor(Qt::WaitCursor);

  OptimTextEditor* editor = 0;
  auto DigCh = getAttachedSubWin(WindowId::DigCh); 
  if (!DigCh) {
    DigCh = createAttachedSubWin( (editor = new OptimTextEditor()),WindowId::DigCh );
    connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
    connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
    connect(editor, SIGNAL(redoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
  }
  else { 
    editor = qobject_cast<OptimTextEditor*>( DigCh->widget() );
    if (CtSt_.ClearText) editor->clear();
  }
  DigCh->raise();
  

  if (CtSt_.ClearText) editor->clear();

  char const* header[] = {"#          N", "L", "bt1x",  "a1x",  "bt1y", "a1y",
                                               "bt2x",  "a2x",  "bt2y", "a2y", "dx", "dxp", "dy", "dyp", "nu1",  "nu2"};

  buf[0] = 0;
  for (uint i=0; i< sizeof(header)/sizeof(header[0]); ++i) {
    sprintf(buf1, "%12s ", header[i] );  
    strcat(buf, buf1);
  }
  strcat(buf,"\n");
  editor->insertPlainText(buf); 

  //................................................................................................................
  
  v.qx = v.qy =0.0;
 
  sprintf(buf, format, 0, Lp, v.btx1, v.alfx1, v.bty1, v.alfy1,
                              v.btx2, v.alfx2, v.bty2, v.alfy2, v.dx, v.dxp, v.dy, v.dyp, v.qx, v.qy); // FIXME
 
  editor->insertPlainText(buf); 
  
  double tetaY = tetaYo0_;
  double Enr   = Ein;
  double Hrt   = Hr; 
  double gamma = 1.+Enr/ms;   
  double gm2   = gamma*gamma;
  double gmsr  = sqrt(gm2-1);
  double VSR   = 0.0;
  double dEn2  = 0.0;
  double em1n  = 0.0;
  double em2n  = 0.0;
  double nu1pr = 0.0;
  double nu2pr = 0.0;
  double dg1   = 0.0;
  double dg2   = 0.0;
  double dB2   = 0.0;
  
  v.eigenvectors(ev);
 
  double alpha = 0.0; // FIXME !
  int i = 0;

  for( i=0; i<nelm_; ++i) {  // for_1

    auto ep = beamline_[i];
    char nm = toupper(ep->etype());
    int  ns = fabs(ep->length()/stepint)+1;

    // computed directly for over individual elements.
 
    switch(nm) { // Switch 1

    case 'M': // multipole
        if(ep->N==1){
	  s      = ep->tilt()*PI/180.;
	  c2a    = cos(2*s);
	  s      = 1./(4.*PI*Hrt)*ep->S*c2a;
	  nu1pr -= 0; // s*v.btx1;
	  nu2pr += 0; //s*v.bty2;
	  break;
        }
    	if(ep->N==2){
	  s   = ep->tilt()*PI/180.;
	  double c3a = cos(3*s);
	  double s3a = sin(3*s);
	  s   = 1./(4.*PI*Hrt)*ep->S*(c3a*v.dx+s3a*v.dy);
	  nu1pr += 0; // v.btx1*s;
	  nu2pr -= 0; //v.bty2*s;
      	}
	break;
    case 'G': // dipole edge
          alpha = ep->tilt()*PI/180.0;  
           tg   = tan(ep->G*PI/180.);
	    s   = (ep->B*ep->B)*tg;
        // damping parameters contribution
	dg1 -= s* (cos(alpha)*sqrt(2*v.btx1)*imag(B1(v))                + sin(alpha)*sqrt(2*v.bty1)*imag(exp(J*v.teta1)*B1(v))); 
	dg2 -= s* (cos(alpha)*sqrt(2*v.btx2)*imag(exp(J*v.teta2)*B2(v)) + sin(alpha)*sqrt(2*v.bty2)*imag(B2(v))); 

	// Chromaticity contribution
        if (i<(nelm_-1)){
	  if(  (beamline_[i+1]->etype() =='B') ||
               (beamline_[i+1]->etype() =='D') ) {
	    D  = v.dx* cos(alpha) +  v.dx*sin(alpha);
	    Dp = v.dxp*cos(alpha) + v.dyp*sin(alpha);

	    h  = beamline_[i+1]->B/Hrt;  // dipole    optical strength 
	    k  = beamline_[i+1]->G/Hrt;  // gradient  optical strength 


	    nu1pr += 1./(4.*PI)* 0.0;
	    nu2pr += 1./(4.*PI)* 0.0;
          }
	}
	break;
    case 'B': // combined function magnet
    case 'D':
        // synchrotron loss, variation     
	VSR   += KVSR * gamma * gmsr * (ep->B * ep->B) * ep->length() / (ms*ms);//keV
	dEn2t  = KE* (ep->B* ep->B)*fabs(ep->B) *ep->length() *gm2*gm2*gamma/(ms*ms*ms*gmsr);
	dEn2  += dEn2t;   // keV
        if(i>0) if( beamline_[i-1]->etype() == 'G') break;
        
	// Chromaticity contribution
        s  = ep->tilt()*PI/180.;
	ca  = cos(s);
	sa  = sin(s);
	c2a = cos(2.0*s);
        Dp = v.dxp*ca + v.dyp*sa;
        h  = ep->B/Hrt;
        
        nu1pr -=1.0/(4.*PI)*0.0; // Dp*sa*c2a*v.btx1 *h;
	nu2pr +=1.0/(4.*PI)*0.0; // Dp*ca*c2a*v.bty2 *h;
	break;
      case 'O': 
      case 'I':
	ns=1; // do not split drifts since they contribute nothing.
      default:
	break;
    }  // Switch 1 end

    //  numerical integration over elements
    //...............................................................
    // This is used for debugging 
    bool diags = false;
    if ( (ep->etype() == 'D') || (ep->etype() == 'B'))   {
	diags = true;
    }
    else {
	diags = true;
    }
    //...................................................................

    auto e = std::shared_ptr<Element>(ep->split(ns));
    
    //-----------------------------------------------------------------------
    // *** WARNING *** 
    // Normally cavity are non-splitable elements
    // so (ep->split(ns) when e is a cavity does not divide the length by ns
    // here we integrate explicitly through the cavity with integrStep
    if ((nm == 'A') ||  (nm == 'W') ) e->length(e->length()/ns);  // FORCE CAVITY SPLIT !!!    
    //-----------------------------------------------------------------------
    // *** WARNING ****
    // EAcc is a splitable element
    // for the electrostatic accelerating section, the total energy gain (parameter B)
    // is split by the virtual function split.  
    // if (nm == 'E')   e->B /= ns; THIS LINE, WHICH APPEARS IN THE ORIGINAL Optim32 is
    // NOT NEEDED HERE
    //------------------------------------------------------------------------ 

    integrStep(0.5*e->length(), gamma, Hrt, dEn2t, v, *ep, dg1, dg2, dB2, nu1pr, nu2pr, em1n, em2n);

    double dalfa = 0.0;
    double alfap = 0.0;
    
    for( int j=0 ; j<ns; ++j) {

      switch(nm) {
        case 'B':  // CF dipole 
        case 'D':
	  tm     = e->rmatrix( dalfa, Enr, ms, tetaY, dalfa, e->checkEdge(j,ns) );
          dalfa  -= e->tilt(); // what is alfap for ??? It looks like it is not used. 
	  break;
	default:
	  tm  = e->rmatrix( Enr, ms, tetaY, 0.0, e->checkEdge(j,ns) );
      }

      e->propagateLatticeFunctions(tm, v, ev);

      integrStep(e->length(), gamma, Hrt, dEn2t, v, *ep, dg1, dg2, dB2,
        		                           nu1pr, nu2pr, em1n, em2n);
      Lp    += e->length();
      Hrt    = sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;
      gamma  = 1.0+Enr/ms;
      gm2    = gamma*gamma;
      gmsr   = sqrt(gm2-1.);

    } //  for( int j=0 ; j<ns; ++j)

   integrStep(-0.5*e->length(), gamma, Hrt, dEn2t, v, *ep, dg1, dg2, dB2, nu1pr, nu2pr, em1n, em2n);
   
    // Chromaticity contribution
    switch(nm){ // Switch 2
    case 'G': // magnet edge 
         if(i>0){
	   if( ( beamline_[i-1]->etype() =='B') ||
	       ( beamline_[i-1]->etype() =='D') ) {
	      tg  = tan(ep->G*PI/180.);
	      s      = ep->tilt()*PI/180.;
	      ca     = cos(s);
	      sa     = sin(s);
	      c2a    = cos(2.*s);
	      D      = v.dx*ca + v.dy*sa;
	      Dp     = v.dxp*ca + v.dyp*sa;
	      h      = beamline_[i-1]->B/Hrt;
	      k      = beamline_[i-1]->G/Hrt;

	      nu1pr += 1./(4.*PI)* 0.0; // (s + Dp*(tg*tg+sa)*c2a*v.btx1*h);
	      nu2pr += 1./(4.*PI)* 0.0; //  (s - Dp*(tg*tg+ca)*c2a*v.bty2*h);
           }
	 }
	 break;
    case 'B':  // CF bending magnet
       case 'D':
         if(i+1<nelm_) if (beamline_[i+1]->etype() =='G') break;
         s   = ep->tilt()*PI/180.;
	 ca  = cos(s);
	 sa  = sin(s);
	 c2a = cos(2.*s);
         Dp      = v.dxp*ca + v.dyp*sa;
         h      = ep->B/Hrt;
         nu1pr += 1./(4.*PI)*0.0;// Dp*sa*c2a*v.btx1*h;
         nu2pr -= 1./(4.*PI)*0.0;// Dp*ca*c2a*v.bty2*h;
	 break;
       default:
	 break;
     } // Switch 2 end
  } // for_1 end

  // dB2 is the integral of B^2. If this integral is 0, there is no synchrotron radiation. 
  double g1 = 1.0;
  double g2 = 1.0;
  if ( fabs(dB2) > 100*std::numeric_limits<double>::min() ) {
    g1 += dg1/dB2;
    g2 += dg2/dB2;
  } 
  double gs = 4.0 - g1 - g2;

  // VSR = synchrotron radiation "voltage" (energy) loss
  double em10 = 0.0;
  double em20 = 0.0;
  double dEn0 = 0.0;  
  if (fabs(VSR) > 100*std::numeric_limits<double>::min()  ) {
    em10 = 1000.0 *em1n *Enr/(VSR*g1*gamma);
    em20 = 1000.0 *em2n *Enr/(VSR*g2*gamma);
    dEn0 = (gs > 100*std::numeric_limits<double>::min() ) ? sqrt(dEn2/(2000.*VSR*gs*Enr)) : -1.0 ; // Why -1 when gs < 0.0 ?? 
  };

  findRMatrix(tm);
  //----------------------------------------------------------------------------------------
  // DANGER: THIS VALUE OF ALFA WILL BE WRONG IF THERE IS ANY ACCELERATION (change in gm2)
  // NOTE: the transfer matrix computes Delta s which represents the distance w/r to the
  // reference particle. This distance is negative when a particle is behind the
  // reference particle.
  //
  // alpha = (delta L/L)/(delta p/ p) is the momentum compaction factor. 
  //  
  // delta s/L  = [delta L/L - delta v/v] =  [alpha - 1/gma**2] * (dp/p)       
  //
  // with dp/1 = 1 one has alpha =  delta s/L - 1/gma*2   
  //
  //---------------------------------------------------------------------------------------

  double alfa = - (v.dx * tm[4][0] + v.dxp * tm[4][1] +
	           v.dy * tm[4][2] + v.dyp * tm[4][3] +  tm[4][5] - Lp/gm2 ) / Lp;
  double eta  = - (v.dx * tm[4][0] + v.dxp * tm[4][1] +
	           v.dy * tm[4][2] + v.dyp * tm[4][3] +  tm[4][5] ) / Lp;


  chroma4D(nu1pr, nu2pr);
	   
  // printing

  sprintf(buf, format, i+1, Lp, v.btx1, v.alfx1, v.bty1, v.alfy1,
                                v.btx2, v.alfx2, v.bty2, v.alfy2, v.dx, v.dxp, v.dy, v.dyp, v.qx, v.qy); // FIXME
 
  editor->insertPlainText(buf);
  editor->insertPlainText("\n");
  editor->insertPlainText("\n");
  sprintf(buf,"Transfer Matrix for Structure");
  editor->insertPlainText(buf);
  editor->insertPlainText("\n");

  char const* header2[] = {"X[cm]", "Px", "Y[cm]", "Py", "ds[cm]", "dP/P"};

  buf[0] = 0;
  for (uint i=0; i< sizeof(header2)/sizeof(header2[0]); ++i) {
    sprintf(buf1, "%12s ", header2[i]);  
    strcat( buf,buf1);
  }
  strcat(buf,"\n");
  
  editor->insertPlainText(buf);
  editor->insertPlainText("\n");

  for (int i=0; i<6; ++i){
      buf[0]=0;
      for (int j=0; j<6; ++j){
	sprintf(buf1,"%12g ", tm[i][j]);
	strcat(buf, buf1);
      }
    editor->insertPlainText(buf);
    editor->insertPlainText("\n");
  } 
  editor->insertPlainText("\n");
    
  editor->insertPlainText(QString::asprintf( "Total Length : \t %10g cm\n", Lp ));
  editor->insertPlainText(QString::asprintf( "Tunes : \tQ1 = % 10g,  Q2 = % 10g\n", v.qx, v.qy));
  editor->insertPlainText(QString::asprintf( "Momentum compaction : \t% 10g\n", alfa));
  editor->insertPlainText(QString::asprintf( "Phase Slip Factor : \t% 10g\n", eta));
  editor->insertPlainText(QString::asprintf( "Chromaticities : \t% 10g (mode 1) \t% 10g (mode 2)\n", nu1pr, nu2pr));
  editor->insertPlainText(QString::asprintf( "Initial Energy : \t% 10g MeV\n", Ein));
  editor->insertPlainText(QString::asprintf( "Final Energy : \t% 10g MeV\n", Enr));
  editor->insertPlainText(QString::asprintf( "Synchrotron Rad. Losses : \tVSR = % 10g keV, \t VSR [rms] = % 10g keV\n", VSR, sqrt(dEn2)));
  editor->insertPlainText(QString::asprintf( "VSR/E0 = \t% 10g, \tVSR/E0 [rms] = % 10g    \n", VSR/(Enr*1000.), sqrt(dEn2)/(Enr*1000.)));
  editor->insertPlainText(QString::asprintf( "Emittance increase due to SR : \t(absolute) e1 = % 10g cm,  \te2 = % 10g cm\n",   em1n/gmsr, em2n/gmsr));
  editor->insertPlainText(QString::asprintf( "\t\t(normalized) e1n = % 10g cm,  \te2n = % 10g cm\n\n", em1n,em2n));
    
  if (NmbPer!=1) {
    editor->insertPlainText(QString::asprintf( "\nParameters for %d periods\n", NmbPer));
    editor->insertPlainText(QString::asprintf( "Total Length : \t%10g cm\n" ,  NmbPer*Lp));
    editor->insertPlainText(QString::asprintf( "Tunes :     \tQ1 = % 10g \tQ2 = % 10g \n", v.qx * NmbPer, v.qy * NmbPer));
    editor->insertPlainText(QString::asprintf( "Chromaticities :   \tnu1p = % 10g \tnu2p = % 10g \n", nu1pr* NmbPer, nu2pr* NmbPer));
    editor->insertPlainText(QString::asprintf( "SR Losses : \t(absolute) VSR = % 10g keV  \tRMS VSR = % 10g keV\n", VSR*NmbPer, sqrt(dEn2*NmbPer)));
    editor->insertPlainText(QString::asprintf( "\t(relative) VSR/E0 = % 10g \tRMS VSR/E0 = % 10g\n\n", NmbPer*VSR/(1000.*Enr), sqrt(NmbPer*dEn2)/(1000.*Enr)));
  }
    
  editor->insertPlainText(QString::asprintf( "Parameters  for storage ring\n"));
  editor->insertPlainText(QString::asprintf( "Partition Numbers : \tJ1 = % 10g \tJ2 = %10g \tJs = % 10g \n", g1, g2, gs));
  editor->insertPlainText(QString::asprintf( "Equilibrium RMS emittances: \te1 = % 10g cm  \te2 = %10g cm\n", em10, em20));
  editor->insertPlainText(QString::asprintf( "Equilibrium RMS relative momentum spread : \t% 10g\n", dEn0));
  
  editor->insertPlainText(QString::asprintf( "Amplitude damping decrement per turn:  Lambda1=%g Lambda2=%g LambdaS=%g \n",
                                             VSR*NmbPer*g1/(2000.*Enr), VSR*NmbPer*g2/(2000.*Enr), VSR*NmbPer*gs/(2000.*Enr)));
							
  editor->document()->setModified(false);
  editor->show();


  
}

