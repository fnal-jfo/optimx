//  =================================================================
//
//  Orbit.cpp
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

//-------------------------------------------------------------------
// OptiM   (c) FRA/Fermilab 
// Please refer to OptiM.Copyright.txt for copyright and license 
// information. All other rights reserved.
//
// ViewOrbit.cpp
// ------------------------------------------------------------------

#include <iostream>
#include <iterator>
#include <cmath>
#include <fmt/format.h>

#include <OptimMainWindow.h>
#include <OptimTextEditor.h>
#include <OptimApp.h>
#include <Element.h>
#include <Constants.h>
#include <OrbitDialog.h> 

using Constants::PI;
using Constants::C_DERV1;
using Utility::filterName;

#ifdef __linux__
#define O_BINARY 0
#endif


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DeltOrb(double hr, double ms, Element const& el, double& x, double& y, double& z,
	                                              double& tetax, double& tetay)
{
  //.........................................................................................................
  //
  // Propagate the position and orientation of the local coordinate system  
  //
  // -  (x,y,z)      : the position vector in the global coordinates X,Y,Z  is (x,y,z)
  // -  tetax, tetay : the orientation of the local s-axis  w/r to the orientation of the global Z-axis
  //                   tetax is measured from Z to X in the X-Z plane and tetay is measured from the X-Z plane
  //                   to the Y axis.
  // -  the orientation about the local s-axis (i.e. the element roll angle) is specified
  //    separately expressed in the local coordinate frame (xys).
  //	A positive roll (as it has to be in a right-handed system) rotates from x toward y (clockwise). 
  //
  // NOTE: thetax = thetay = 0 ==>  (nx, ny, nz) = ( 0, 0, 1)
  // nx,ny,nz is the orientation of the unit vector tangent to s expressed in global coordinates. 
  //
  //.........................................................................................................
  
  double alf, fi;
  double P,En,bt,r,ri;

  double tx = tetax/180.*PI;  	
  double ty = tetay/180.*PI;

  double nx = cos(ty) * sin(tx);
  double ny = sin(ty);
  double nz = cos(ty) * cos(tx);

  
  switch ( toupper(el.name()[0]) ){
       
  case 'R': // CFEBend (electric bend) 
       P  = hr*C_DERV1;
       En = sqrt(P*P+ms*ms);
       bt = P/En;    // v/c
       ri = 0.001*el.S/(P*bt) + el.B / hr;
       if (fabs(ri) < std::numeric_limits<double>::epsilon()) { goto metka0; }
       r   = 1./ri;
       alf = 0.0;
       fi  = el.length()*ri; // *r; ???? 
       goto metka1;
     case 'B':  // CFBend  
     case 'D':
       if( fabs(el.B) < std::numeric_limits<double>::epsilon() ) { goto metka0; }
       r   = hr / el.B;
       alf = el.tilt() / 180. * PI;
       fi  = el.length() * el.B / hr;
metka1:
       {
        double vx  = -cos(alf)*(1.-cos(fi))*r;
        double vy  = -sin(alf)*(1.-cos(fi))*r;
        double vz  = sin(fi)*r;
        double mx  = -cos(alf)*sin(fi);
        double my  = -sin(alf)*sin(fi);
        double mz  = cos(fi);
        double sn  = sqrt(nx*nx + nz*nz);
        x +=   nz/sn*vx - nx*ny/sn*vy + nx*vz;
        y +=   sn*vy    + ny*vz;
        z +=  -nx/sn*vx - nz*ny/sn*vy + nz*vz;
        double nx0 = nz/sn*mx - nx*ny/sn*my + nx*mz;
        double ny0 = sn*my    + ny*mz;
        double nz0 = -nx/sn*mx- nz*ny/sn*my + nz*mz;
        tetay = asin(ny0)/PI*180.;
        tetax = atan2(nx0, nz0) /PI*180.;
       }
       break;
  
  case 'M': // multipole 

      if (el.N == 0) {
	double alf = el.tilt() / 180. * PI;
	 double fi  = el.S / hr;
	 double mx  = -cos(alf)*sin(fi);
	 double my  = -sin(alf)*sin(fi);
	 double mz  = cos(fi);
	 double sn  = sqrt(nx*nx + nz*nz);
	 double nx0 = nz/sn*mx - nx*ny/sn*my + nx*mz;
	 double ny0 = sn*my 	+ ny*mz;
	 double nz0 = -nx/sn*mx- nz*ny/sn*my + nz*mz;
	 tetay = asin(ny0)/PI*180.;
	 tetax = atan2(nx0, nz0) /PI*180.;
       }
       break;
  default:

metka0: x += el.length() * nx;
	y += el.length() * ny;
	z += el.length() * nz;
 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdViewOrbit()
{

  using fmt::format_to;  
  OrbitNStruct NSt;

  NSt.Filter[0] = 0;
  // get initial X, Y, X', Y' and step

  static OrbitDialog* dialog = 0;

  if (!dialog) {
    dialog = new OrbitDialog(0);
  }

  NSt.ds            = stepo_;
  NSt.StartFromFile = CtOr;         
  NSt.MatchCase     = OrbMatchCase;
  NSt.FirstLetter   = OrbFirstLetter; 
  strcpy(NSt.Filter, OrbFilter);
  NSt.x             = xo_; 
  NSt.y             = yo_;
  NSt.z             = zo_;			
  NSt.tetax         = tetaXo_;
  NSt.tetay         = tetaYo_;
  NSt.s             = so_;			
  dialog->data_     = NSt; 
  dialog->set();

  if (dialog->exec() == QDialog::Rejected) return; 

  NSt =  dialog->data_;
  
  viewOrbit (0, &NSt, "");

  //----------------------------------------------------------------
  // the original code sets the initial values to the final values
  // after computation. Uncomment the 2 lines below if this is what
  // you want. 
  //-----------------------------------------------------------------
  //dialog->data_ = NSt; 
  //dialog->set();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  OptimMainWindow::viewOrbit (char* filenm, OrbitNStruct* NSt, char const* comment)
{

  QGuiApplication::setOverrideCursor(Qt::WaitCursor);

  CtOr       = NSt->StartFromFile;   	

  // Below:   other functions may want to use the 
  // initial angles, so we update the override variable immediately 
  
  tetaXo_    = CtOr ? tetaXo0_ : NSt->tetax;              
  tetaYo_    = CtOr ? tetaYo_  : NSt->tetay;              

  OrbMatchCase     =  NSt->MatchCase;
  OrbFirstLetter   =  NSt->FirstLetter; 
  strcpy(OrbFilter, NSt->Filter);
    
  
  auto del = [](FILE* p) { (p ? fclose(p) : 0); };
  std::unique_ptr<FILE, decltype(del)>  ufp ( ( filenm ? fopen(filenm, "w"): 0), del);
  FILE* fp = ufp.get();

  OptimTextEditor* editor=0;
  auto DigCh = getAttachedSubWin(WindowId::DigCh);
  if (!DigCh) {
    DigCh = createAttachedSubWin( (editor = new OptimTextEditor()), WindowId::DigCh);
   }
  else  { 
    editor = qobject_cast<OptimTextEditor*>(DigCh->widget() );
     if (CtSt_.ClearText) editor->clear(); 
  } 
  DigCh->raise();
  
  char buf[257];

  if(interrupted_ ) { interrupted_ = false; return 0;}
  if(analyze(!analyzed_)) return 0;


  // This must be done here i.e. *after* calling analyze, because analyze
  // reads the initial values xo0, ... etc.

  xo_     = CtOr ?  xo0_     : NSt->x;
  yo_     = CtOr ?  yo0_     : NSt->y;
  zo_     = CtOr ?  zo0_     : NSt->z;
  tetaXo_ = CtOr ?  tetaXo0_ : NSt->tetax;
  tetaYo_ = CtOr ?  tetaYo0_ : NSt->tetay; 
  stepo_  = CtOr ?  stepo0_  : NSt->ds;
  so_     = CtOr ?  so0_     : NSt->s;
  
  if( CtSt_.CompAtExcitedOrb){

    Coordinates v;

    if(getTrajParamFromFile(false, false, v)) return 0;

    tetaYo_ += (180./PI)*v[3];
    tetaXo_ += (180./PI)*v[1]/cos(tetaYo_);

    // initial conditions in global coordinates
    
    xo_ +=  v[0]*cos(tetaXo_*PI/180.)-v[2]*sin(tetaYo_*PI/180.)*sin(tetaXo_*PI/180.);
    yo_ +=  v[2]*cos(tetaYo_*PI/180.);
    zo_ += -v[0]*sin(tetaXo_*PI/180.)-v[2]*sin(tetaYo_*PI/180.)*cos(tetaXo_*PI/180.);
  }
  
  // Print results
  if (!editor) return 0;

  auto mbufRaw = fmt::memory_buffer();
#if FMT_VERSION < 80000
  auto& mbuf = mbufRaw;
#else
  auto mbuf = std::back_inserter(mbufRaw);
#endif

format_to(mbuf, "#{:>5s} {:>24s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s}\n",
	           "N", "name","S[cm]","X[cm]", "Y[cm]", "Z[cm]", "ThetaX[deg]", "ThetaY[deg]", "Energy[Mev]");

  RMatrix tm;
  char const formatn[] = "{:6d} {:>24s} {:12.3f} {:12.3f} {:12.3f} {:12.3f} {:12.3f} {:12.3f} {:12.3f}\n";

  double Lp    = CtOr ? so0_     : so_;
  double Enr   = Ein;
  double tetaY = CtOr ? tetaYo0_ : tetaYo_; // this was set to  tetaYo0  ERROR ???? 

  format_to(mbuf, formatn, 0, "START",  Lp, xo_, yo_, zo_, tetaXo_, tetaYo_, Enr);

  for( int i=0; i<nelm_; ++i) {

    auto ep = beamline_[i];
    char        nm     = toupper(ep->name()[0]);
    char const* pname  = (OrbFirstLetter) ? &ep->fullName()[1] : ep->fullName();
     int ns = (stepo_ <= 0.0) ? 1 : fabs(ep->length()/stepo_)+1;
     if( (nm == 'A') || (nm == 'X') || (nm=='W') ) {ns=1;}

     std::shared_ptr<Element> e(ep->split(ns)); 

     for(int j=0; j<ns; ++j){
       double Hrt = sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;

       DeltOrb(Hrt, ms, *e, xo_, yo_, zo_, tetaXo_, tetaYo_);

       Lp +=  fabs(e->length());
       tm = e->rmatrix(Enr, ms, tetaY, 0.0, Element::checkEdge(j,ns)); // this is needed to update the energy. 


       if(filterName( e->fullName(), OrbFilter, OrbMatchCase)){
	 format_to(mbuf, formatn, i+1, pname, Lp, xo_, yo_, zo_, tetaXo_, tetaYo_, Enr);
        }
     }
   }

  format_to(mbuf, formatn, beamline_.size()+1, "END", Lp, xo_, yo_, zo_, tetaXo_, tetaYo_, Enr);

  so_ = Lp;

  NSt->x             = xo_; 
  NSt->y             = yo_;
  NSt->z             = zo_;			
  NSt->tetax         = tetaXo_;
  NSt->tetay         = tetaYo_;
  NSt->s             = so_;			

  format_to(mbuf, "{:c}", '\0');
  
  if (fp) {
    fprintf(fp, "%s",  mbufRaw.data());
  }
  else {
    editor->appendPlainText(mbufRaw.data());	  
    editor->moveCursor(QTextCursor::Start);
    editor->document()->setModified(false);
    editor->show();
  }

  QGuiApplication::restoreOverrideCursor();

  return 1;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
