//  =================================================================
//
//  OrbitNew.cpp
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
// ViewOrbitNew.cpp
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


void OptimMainWindow::cmdViewOrbitNew()
{

  OrbitNStruct NSt;

  NSt.Filter[0] = 0;
  // get initial X, Y, X', Y' and step

  static OrbitDialog* dialog = 0;

  if (!dialog) {
    dialog = new OrbitDialog(0);
  }

  NSt.UseMADFrame   = false;
  NSt.ds            = stepo_;
  NSt.StartFromFile = CtOr;         
  NSt.MatchCase     = OrbMatchCase;
  NSt.FirstLetter   = OrbFirstLetter; 
  strcpy(NSt.Filter, OrbFilter);
  NSt.x             = xo0_; 
  NSt.y             = yo0_;
  NSt.z             = zo0_;			
  NSt.tetax         = tetaXo0_;
  NSt.tetay         = tetaYo0_;
  NSt.s             = so0_;			
  dialog->data_     = NSt; 
  dialog->set();

  if (dialog->exec() == QDialog::Rejected) return; 

  NSt =  dialog->data_;
  
  //----------------------------------------------------------------
  // the original code sets the initial values to the final values
  // after computation. Uncomment the 2 lines below if this is what
  // you want. 
  //-----------------------------------------------------------------

  viewOrbitNew (0, &NSt, "");

  //dialog->data_ = NSt; 
  //dialog->set();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  OptimMainWindow::viewOrbitNew (char* filenm, OrbitNStruct* NSt, char const* comment)
{

  auto cursor_guard = OptimApp::cursorWait();

  CtOr       = NSt->StartFromFile;   	

  // Below:   other functions may want to use the 
  // initial angles, so we update the override variable immediately 
  
  tetaXo_    = CtOr ? tetaXo0_ : NSt->tetax;              
  tetaYo_    = CtOr ? tetaYo0_ : NSt->tetay;              

  OrbMatchCase     =  NSt->MatchCase;
  OrbFirstLetter   =  NSt->FirstLetter; 
  strcpy(OrbFilter, NSt->Filter);

  
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

  Vector_t<3>  R = { xo_, yo_, zo_};

  double sy = sin(tetaXo_*PI/180.);
  double cy = cos(tetaXo_*PI/180.);

  double sx = sin(tetaYo_*PI/180.);
  double cx = cos(tetaYo_*PI/180.);
  
  RMatrix_t<3> rotx( { { 1.0,  0.0,     0.0},
                       { 0.0,   cx,     sx },
                       { 0.0,  -sx,     cx }} );
  
  
  RMatrix_t<3> roty( { {  cy,    0.0,    sy },
                       { 0.0,    1.0,   0.0 },
                       { -sy,    0.0,    cy }} );

  
  RMatrix_t<3> W = roty*rotx;

  auto del = [](FILE* p) { (p ? fclose(p) : 0); };
  std::unique_ptr<FILE, decltype(del)>  ufp ( ( filenm ? fopen(filenm, "w"): 0), del);

  OptimTextEditor* editor=0;
  QMdiSubWindow* DigCh = 0;

  if (!ufp) { 
    DigCh = getAttachedSubWin(WindowId::DigCh);
    if (!DigCh) {
      DigCh = createAttachedSubWin( (editor = new OptimTextEditor()), WindowId::DigCh);
    }
    else  { 
     editor = qobject_cast<OptimTextEditor*>(DigCh->widget() );
     if (CtSt_.ClearText) editor->clear(); 
    } 
    DigCh->raise();
  }
  
  // Output results
  if ((!editor) && (!ufp)) return 0;

  auto mbufRaw = fmt::memory_buffer();

#if FMT_VERSION < 80000
  auto& mbuf = mbufRaw;
#else
  auto mbuf = std::back_inserter(mbufRaw);
#endif
  
  if (NSt->UseMADFrame) {
    format_to(mbuf, "#{:>5s} {:>24s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s}\n",
            "N", "name","S[cm]","X[cm]", "Y[cm]", "Z[cm]", "TetaX[deg]", "TetaY[deg]", "Tilt[deg]", "Energy[Mev]");
  }
  else {
       format_to(mbuf, "#{:>5s} {:>24s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s}\n",
            "N", "name","S[cm]","X[cm]", "Y[cm]", "Z[cm]", "TetaX[deg]", "TetaY[deg]", "Energy[Mev]");
  };
  
  RMatrix tm;

  std::string formatn;
  if (NSt->UseMADFrame) {
    formatn =  std::string("{:6d} {:>24s} {:12.3f} {:12.3f} {:12.3f} {:12.3f} {:12.3f} {:12.3f} {:12.3f} {:12.3f}\n");
  }
  else { 
    formatn = std::string("{:6d} {:>24s} {:12.3f} {:12.3f} {:12.3f} {:12.3f} {:12.3f} {:12.3f} {:12.3f}\n");
  }

  double Lp    = CtOr ? so0_     : so_;
  double Enr   = Ein;
  double tetaY = CtOr ? tetaYo0_ : tetaYo_;
  double tetaX = CtOr ? tetaXo0_ : tetaXo_;

  if (NSt->UseMADFrame) {
    format_to(mbuf, formatn.c_str(), 0, "START",  Lp, xo_, yo_, zo_, tetaXo_, tetaYo_, 0.0,  Enr);
  }
  else {
    format_to(mbuf, formatn.c_str(), 0, "START",  Lp, xo_, yo_, zo_, tetaXo_, tetaYo_,  Enr);
  }

  for( int i=0; i<nelm_; ++i) {

    auto ep = beamline_[i];
    char        nm     = ep->etype();
    char const* pname  = (OrbFirstLetter) ? &ep->fullName()[1] : ep->fullName();
    int ns = (stepo_ <= 0.0) ? 1 : fabs(ep->length()/stepo_)+1;

    int j = 0;
    auto e = ep->splitnew(ns);
    for(auto it=e->deep_begin(); it != e->deep_end(); ++it,++j) {
      auto e = *it;
      double Hrt = sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;
      e->propagate(Hrt, ms, W, R );
      Lp +=  fabs(e->length());

      tm = e->rmatrix(Enr, ms, tetaY, 0.0, Element::checkEdge(j,ns)); // this is needed to update the energy. 

      if(filterName( e->fullName(), OrbFilter, OrbMatchCase)){
        double ty  = asin(W[1][2]);  
        double tx  = atan2(W[0][2],W[2][2]);
        double psi = atan2(W[1][0],W[1][1]);
        format_to(mbuf, formatn.c_str(), i+1, pname, Lp, R[0], R[1], R[2], tx*180.0/PI, ty*180.0/PI,  psi*180.0/PI, Enr);
      }
    }
  }

  double ty  = asin(W[1][2]);  
  double tx  = atan2(W[0][2],W[2][2]);
  double psi = atan2(W[1][0],W[1][1]);;

  format_to(mbuf, formatn.c_str(), nelm_, "END", Lp, R[0], R[1], R[2], tx*180.0/PI, ty*180.0/PI, psi*180.0/PI, Enr);

  so_ = Lp;

  format_to(mbuf, "{:c}", '\0');

  if (ufp) {
    fprintf(ufp.get(), "%s", mbufRaw.data());
  }
  else {
    editor->appendPlainText(mbufRaw.data());	  
    editor->moveCursor(QTextCursor::Start);
    editor->document()->setModified(false);
    editor->show();
  }

  return 1;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
