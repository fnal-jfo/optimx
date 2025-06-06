//  =================================================================
//
//  Integrals.cpp
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

#include <platform.h>
#include <Constants.h>
#include <GeneralPreferencesDialog.h>
#include <Element.h>
#include <Globals.h>
#include <RMatrix.h>
#include <Twiss.h>
#include <fmt/format.h>
#include <OptimMainWindow.h>
#include <OptimTextEditor.h>
#include <OptimMessages.h>
#include <QGuiApplication>

using Constants::PI;
using Constants::KVSR;
using Constants::KE;
using Constants::C_DERV1;

//......................................................................................

void OptimMainWindow::integrStep(double dL, double gamma, double Hrt, double dEn2,
				 Twiss const& v, Element& e, double& dgx,  double& dgy, double& dB2, double& nuxpr,
	double& nuypr, double& emxn, double& emyn)
{

  // integral quantities through an element 
  // NOTHING HAPPENS IN THE CFEBEND ??? This is probably WRONG !!!!
  
  double ca, sa, c2a, c3a, s3a, gmsr;
  
  double gm2  = gamma*gamma;
         gmsr = sqrt(gm2-1.0);
  double s    = v.DsXp*v.BtX + v.DsX*v.AlX;
  double Ax   = (v.DsX*v.DsX + s*s)/v.BtX;
         s    = v.DsYp*v.BtY + v.DsY*v.AlY;
  double Ay   = (v.DsY*v.DsY + s*s)/v.BtY;
  double knu  = 1./(4.*PI*Hrt);
  
  switch( toupper(e.name()[0]) ) {
   case 'B':  
   case 'D':
     {
      s      = e.tilt()*PI/180.;
      ca     = cos(s);
      sa     = sin(s);
      c2a    = cos(2.*s);
      s      = 0.5*dEn2*gm2/(gmsr*gmsr*gmsr*ms*ms*1.e6)*dL/e.length();
      // contribution to equilibrium emittance 
      emxn  += Ax*s;
      emyn  += Ay*s;
      s      = e.B*(2.*e.G + e.B*e.B/Hrt)*dL;
      // contribution to damping decrement factors
      dgx   += v.DsX*ca*s;
      dgy   += v.DsY*sa*s;
      dB2   += e.B*e.B*dL;
      // Chromaticity contribution
      double D = v.DsX*ca + v.DsY*sa;
      double h = e.B/Hrt;
      double k = e.G*c2a/Hrt;
      double A = dL/(4.*PI);
      double gammaX = (1. + v.AlX * v.AlX) / v.BtX;
      double gammaY = (1. + v.AlY * v.AlY) / v.BtY;
      s       = v.BtX*( k*(h*(v.DsX*ca+D)-1.) - h*h*ca*ca);
      nuxpr  += A*(s - 2.*v.AlX*v.DsXp*h*ca + gammaX*h*D);
      s       = v.BtY*( k*(1.-h*(v.DsY*sa+D)) - h*h*sa*sa);
      nuypr  += A*(s - 2.*v.AlY*v.DsYp*h*sa + gammaY*h*D);     
     }
     break;
   case 'Q':
     s   = e.tilt()*PI/180.;
     c2a =cos(2*s);
      // Chromaticity contribution
     nuxpr -= knu*v.BtX*dL*e.G*c2a;
     nuypr += knu*v.BtY*dL*e.G*c2a;
     break;
   case 'S':
     s   = e.tilt()*PI/180.;
     c3a = cos(3.*s);
     s3a = sin(3.*s);
     s   = knu*dL*e.S*(c3a*v.DsX+s3a*v.DsY);
      // Chromaticity contribution
     nuxpr += v.BtX*s;
     nuypr -= v.BtY*s;
   default:
     break;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdIntegrals()
{
  using fmt::format_to;
  
  auto outbufraw = fmt::memory_buffer();
  auto outbuf     = std::back_inserter(outbufraw);

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
  

  if(interrupted_ ) { interrupted_  =  false; return;}
   
  if(analyzed_) {
    if ( analyze(false)) return;
  } 
  else { 
    if(analyze(true)) return;
  }

  if (CtSt_.ClearText) editor->clear();

  char const* header[] = {"#          N", "L", "BetaX",  "AlfaX",  "BetaY", "AlfaY", "DspX", "DspXp", "DspY", "DspYp", "NuX",  "NuY"};

  for (uint i=0; i< sizeof(header)/sizeof(header[0]); ++i) {
    format_to(outbuf, "{:>12s} ", header[i]);
  }
  format_to(outbuf, "\n");

  Twiss v;
  RMatrix tm;     
  double dEn2t, D, Dp, h, k, s, ca, sa, c2a, tg;
  char format[]  = {"{:12d} {:12.1f} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g}\n"};
  int j;
  
  std::complex<double> ev[4][4];

  v.BtX  = BetaXin;  
  v.BtY  = BetaYin;  
  v.AlX  = AlfaXin;        
  v.AlY  = AlfaYin;
  v.DsX  = DispXin;  
  v.DsY  = DispYin;  
  v.DsXp = DispPrimeXin;  
  v.DsYp = DispPrimeYin;
  
  v.nuY = 0.0;
  v.nuX = 0.0;

  double Lp = 0.0;

  if( CtSt_.IsRingCh ) {
    findRMatrix(tm);
    double dalfa = 0.0; 
    if ( find_tunes(tm, 100., v, &dalfa) != 0 ) { 
      OptimMessageBox::warning(this, "Close Error", "Cannot close for X or Y", QMessageBox::Ok);
      	return;
    }
    v.nuY=v.nuX=0.;
  }

  format_to(outbuf, format, 0, Lp, v.BtX, v.AlX, v.BtY, v.AlY, v.DsX, v.DsXp, v.DsY, v.DsYp, v.nuX, v.nuY);
  
  double tetaY = tetaYo0_;
  double Enr   = Ein;
  double Hrt   = Hr; 
  double gamma = 1.+Enr/ms;   
  double gm2   = gamma*gamma;
  double gmsr  = sqrt(gm2-1);
  double VSR   = 0.0;
  double dEn2  = 0.0;
  double emxn  = 0.0;
  double emyn  = 0.0;
  double nuxpr = 0.0;
  double nuypr = 0.0;
  double dgx   = 0.0;
  double dgy   = 0.0;
  double dB2   = 0.0;
  
  v.eigenvectors(ev);
 
  int i = 0;
  for( i=0; i<nelm_; ++i) {  // for_1
    auto ep = beamline_[i];
    char nm = ep->etype();
    int  ns = fabs(ep->length()/stepint)+1;

    // sum over whole Elements
    switch(nm) { // Switch 1
      case 'M':
        if(ep->N==1){
	  s      = ep->tilt()*PI/180.;
	  c2a    = cos(2*s);
	  s      = 1./(4.*PI*Hrt)*ep->S*c2a;
	  nuxpr -= s*v.BtX;
	  nuypr += s*v.BtY;
	  break;
        }
    	if(ep->N==2){
	  s   = ep->tilt()*PI/180.;
	  double c3a = cos(3*s);
	  double s3a = sin(3*s);
	  s   = 1./(4.*PI*Hrt)*ep->S*(c3a*v.DsX+s3a*v.DsY);
	  nuxpr += v.BtX*s;
	  nuypr -= v.BtY*s;
      	}
	break;
    case 'G': // magnet edge
        // damping parameters contribution
        tg   = tan(ep->G*PI/180.);
        s    = ep->tilt()*PI/180.;
	ca   = cos(s);
	sa   = sin(s);
	c2a  = cos(2.*s);
	s    = (ep->B*ep->B)*tg;
        dgx -= v.DsX*ca*s;
	dgy -= v.DsY*sa*s;
	// Chromaticity contribution
        if (i<(nelm_-1)){
	  if( (beamline_[i+1]->etype() =='B') || (beamline_[i+1]->etype() =='D') ) {
	    D  = v.DsX*ca + v.DsY*sa;
	    Dp = v.DsXp*ca + v.DsYp*sa;
	    h  = beamline_[i+1]->B/Hrt;
	    k  = beamline_[i+1]->G/Hrt;
	    s  =      (fabs(h)-2.*k*D)*tg*v.BtX*c2a - h*D*tg*tg*(v.BtX*fabs(h)*tg-2.*v.AlX*c2a);
	    nuxpr += 1./(4.*PI)*(s - Dp*(tg*tg+sa)*c2a*v.BtX*h);
	    s      = -(fabs(h)-2.*k*D)*tg*v.BtY*c2a - h*D*tg*tg*(v.BtY*fabs(h)*tg+2.*v.AlY*c2a);
	    nuypr += 1./(4.*PI)*(s + Dp*(tg*tg+ca)*c2a*v.BtY*h);
          }
	}
	break;
      case 'B': 
      case 'D':
	VSR   += KVSR * gamma * gmsr * (ep->B * ep->B) * ep->length() / (ms*ms);//keV
	dEn2t  = KE* (ep->B* ep->B)*fabs(ep->B) *ep->length() *gm2*gm2*gamma/(ms*ms*ms*gmsr);
	dEn2  += dEn2t;   // keV
	// Chromaticity contribution
        if(i>0) { if( beamline_[i-1]->etype() =='G') break;}
        
        s  = ep->tilt()*PI/180.;
	ca  = cos(s);
	sa  = sin(s);
	c2a = cos(2.0*s);
        Dp = v.DsXp*ca + v.DsYp*sa;
        h  = ep->B/Hrt;
        
        nuxpr -=1./(4.*PI)*Dp*sa*c2a*v.BtX*h;
        nuypr +=1./(4.*PI)*Dp*ca*c2a*v.BtY*h;
	break;
      case 'O': 
      case 'I':
	ns=1;
      default:
	break;
   }  // Switch 1 end

    auto e = std::shared_ptr<Element>(ep->split(ns));

    //-----------------------------------------------------------------------
    // *** WARNING *** 
    // Normally cavity are non-splitable elements
    // so (ep->split(ns) when e is a cavity does not divide the length by ns
    // here we integrate explicitly through the cavity with integrStep
    if ((nm == 'A') ||  (nm == 'W') ) e->length( e->length()/ns );  // FORCE CAVITY SPLIT !!!    
    //-----------------------------------------------------------------------
    // *** WARNING ****
    // EAcc is a splitable element
    // for the electrostatic accelerating section, the total energy gain (parameter B)
    // is split by the virtual function split.  
    // if (nm == 'E')   e->B /= ns; THIS LINE, WHICH APPEARS IN THE ORIGINAL Optim32 is
    // NOT NEEDED HERE
    //------------------------------------------------------------------------ 

    integrStep(0.5*e->length(), gamma, Hrt, dEn2t, v, *ep, dgx, dgy, dB2, nuxpr, nuypr, emxn, emyn);

    double dalfa = 0.0;
    double alfap = 0.0;
    for( int j=0 ; j<ns; ++j) {
      switch(nm) {
	case 'B':  
        case 'D':
	  tm     = e->rmatrix( dalfa, Enr, ms, tetaY, dalfa, e->checkEdge(j,ns) );
          dalfa  -= e->tilt(); // what is alfap for ??? It looks like it is not used. 
	  break;
	default:
	  tm = e->rmatrix( Enr, ms, tetaY, 0.0, e->checkEdge(j,ns) );
      }
      e->propagateLatticeFunctions(tm, v, ev);
      
      integrStep(e->length(), gamma, Hrt, dEn2t, v, *ep, dgx, dgy, dB2,
		                           nuxpr, nuypr, emxn, emyn);
      Lp    += e->length();
      Hrt    = sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;
      gamma  = 1.0+Enr/ms;
      gm2    = gamma*gamma;
      gmsr   = sqrt(gm2-1.);

    }

    integrStep(-0.5*e->length(), gamma, Hrt, dEn2t, v, *ep, dgx, dgy, dB2, nuxpr, nuypr, emxn, emyn);

    // Chromaticity contribution
    switch(nm){ // Switch 2
       case 'G':
         if(i>0){
	   if( ( beamline_[i-1]->etype() =='B') || (beamline_[i-1]->etype() =='D') ) {
	      tg  = tan(ep->G*PI/180.);
	      s      = ep->tilt()*PI/180.;
	      ca     = cos(s);
	      sa     = sin(s);
	      c2a    = cos(2.*s);
	      D      = v.DsX*ca + v.DsY*sa;
	      Dp     = v.DsXp*ca + v.DsYp*sa;
	      h      = beamline_[i-1]->B/Hrt;
	      k      = beamline_[i-1]->G/Hrt;
	      s      = (fabs(h)-2.*k*D)*tg*v.BtX*c2a - h*D*tg*tg*(v.BtX*fabs(h)*tg+2.*v.AlX*c2a);
	      nuxpr += 1./(4.*PI)*(s + Dp*(tg*tg+sa)*c2a*v.BtX*h);
	      s      = -(fabs(h)-2.*k*D)*tg*v.BtY*c2a - h*D*tg*tg*(v.BtY*fabs(h)*tg-2.*v.AlY*c2a);
	      nuypr += 1./(4.*PI)*(s - Dp*(tg*tg+ca)*c2a*v.BtY*h);
           }
	 }
	 break;
       case 'B': 
       case 'D':
         if(i+1<nelm_) if( beamline_[i+1]->etype()=='G') break;
         s   = ep->tilt()*PI/180.;
	 ca  = cos(s);
	 sa  = sin(s);
	 c2a = cos(2.*s);
         Dp      = v.DsXp*ca + v.DsYp*sa;
         h      = ep->B/Hrt;
         nuxpr += 1./(4.*PI)*Dp*sa*c2a*v.BtX*h;
         nuypr -= 1./(4.*PI)*Dp*ca*c2a*v.BtY*h;
	 break;
       default:
	 break;
     } // Switch 2 end
  } // for_1 end
  
  double gx = (dB2==0) ? 1.0 : 1.0-dgx/dB2;
  double gy = (dB2==0) ? 1.0 : 1.0-dgy/dB2;
  double gs = 4.0 - gx - gy;

  double emx0 = (VSR > 1.0e-12) ? 1000.0 *emxn *Enr/(VSR*gx*gamma) : 0.0;
  double emy0 = (VSR > 1.0e-12) ? 1000.0 *emyn *Enr/(VSR*gy*gamma) : 0.0;
  double dEn0 = (VSR > 1.0e-12) ? ( (gs>0.0) ? sqrt(dEn2/(2000.*VSR*gs*Enr)) : -1.0) : 0.0;

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

  double alfa = - (v.DsX * tm[4][0] + v.DsXp * tm[4][1] +
	           v.DsY * tm[4][2] + v.DsYp * tm[4][3] +  tm[4][5] - Lp/gm2 ) / Lp;
  double eta  = - (v.DsX * tm[4][0] + v.DsXp * tm[4][1] +
	           v.DsY * tm[4][2] + v.DsYp * tm[4][3] +  tm[4][5] ) / Lp;


  format_to(outbuf, format, i+1, Lp, v.BtX, v.AlX, v.BtY, v.AlY, v.DsX,
  	      v.DsXp, v.DsY, v.DsYp, v.nuX, v.nuY);

  format_to(outbuf,"\n\n");
  format_to(outbuf,"Transfer Matrix for Structure\n");

  char const* header2[] = {"X[cm]", "Px", "Y[cm]", "Py", "ds[cm]", "dP/P"};

  for (uint i=0; i< sizeof(header2)/sizeof(header2[0]); ++i) {
    format_to(outbuf, "{:>12s} ", header2[i]);  
  }
  format_to(outbuf,"\n");
  
  format_to(outbuf,"\n");
  for (int i=0; i<6; ++i){
    for (int j=0; j<6; ++j){
      format_to(outbuf,"{:12g} ", tm[i][j]);
    }
    format_to(outbuf,"\n");
  } 
  format_to(outbuf,"\n");
  
  format_to(outbuf,"{:20s} {:10g} cm \n", "Total Length:", Lp );
  format_to(outbuf,"{:20s} Qx = {:10g}, Qy = {:10g}\n", "Tunes:", v.nuX, v.nuY);
  format_to(outbuf,"{:20s} {:10g}\n", "Momentum compaction:", alfa);
  format_to(outbuf,"{:20s} {:10g}\n", "Phase Slip Factor:", eta);
  format_to(outbuf,"{:20s} {:10g} (hor) {:10g} (ver)\n", "Chromaticities:",nuxpr, nuypr);
  format_to(outbuf,"{:20s} {:10g} MeV\n", "Initial Energy:", Ein);
  format_to(outbuf,"{:20s} {:10g} MeV\n", "Final Energy:", Enr);
  format_to(outbuf,"Synchrotron Rad. Losses: VSR = {:10g} keV, VSR [rms] = {:10g} keV\n", VSR, sqrt(dEn2));
  format_to(outbuf,"                      VSR/E0 = {:10g},  VSR/E0 [rms] = {:10g} \n", VSR/(Enr*1000.), sqrt(dEn2)/(Enr*1000.));
  format_to(outbuf,"Emittance increase due to SR: (absolute)  ex = {:10g} cm, ey  = {:10g} cm\n",   emxn/gmsr, emyn/gmsr);
  format_to(outbuf,"                            (normalized) exn = {:10g} cm, eyn = {:10g} cm\n\n", emxn,emyn);

  
  if (NmbPer!=1) {
    format_to(outbuf,"\nParameters for {:d} periods\n", NmbPer);
    format_to(outbuf,"{:20s} {:10g} cm\n" , "Total Length:", NmbPer*Lp);
    format_to(outbuf,"{:20s} Qx = {:10g} Qy = {:10g} \n", "Tunes:", v.nuX * NmbPer, v.nuY * NmbPer);
    format_to(outbuf,"{:20s} nuxp = {:10g} \tnuyp = {:10g} \n", "Chromaticities:", nuxpr* NmbPer, nuypr* NmbPer);
    format_to(outbuf,"{:20s} (absolute) VSR = {:10g} keV  RMS VSR = {:10g} keV\n", "SR Losses:", VSR*NmbPer, sqrt(dEn2*NmbPer));
    format_to(outbuf,"{:20s} (relative) VSR/E0 = {:10g} RMS VSR/E0 = {:10g}\n\n", "", NmbPer*VSR/(1000.*Enr), sqrt(NmbPer*dEn2)/(1000.*Enr));
  }
    
  format_to(outbuf, "Parameters  for storage ring\n");
  format_to(outbuf, "Partition Numbers : Jx = {:10g} Jy = {:10g} Js = {:10g} \n", gx, gy, gs);
  format_to(outbuf, "Equilibrium RMS emittances: ex = {:10g} cm  ey = {:10g} cm\n", emx0, emy0);
  format_to(outbuf, "Equilibrium RMS relative momentum spread : \t{:10g}\n", dEn0);
  
  format_to(outbuf,"Amplitude damping decrement/turn: LambdaX = {:g} LambdaY = {:g} LambdaS = {:g} \n",
                                            VSR*NmbPer*gx/(2000.*Enr), VSR*NmbPer*gy/(2000.*Enr), VSR*NmbPer*gs/(2000.*Enr));


  format_to(outbuf,"{:c}",0);
  editor->insertPlainText(outbufraw.data());
  
  editor->document()->setModified(false);
  editor->show();

}

