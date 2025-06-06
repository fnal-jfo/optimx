//  =================================================================
//
//  cmdPhases.cpp
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
#include <ControlDialog.h>
#include <GeneralPreferencesDialog.h>
#include <Element.h>
#include <Globals.h>
#include <RMatrix.h>
#include <Twiss.h>
#include <OptimApp.h>
#include <OptimEditor.h>
#include <OptimMainWindow.h>
#include <OptimMessages.h>
#include <OptimPlot.h>
#include <Utility.h>
#include <QMdiArea>
#include <QMdiSubWindow>

#include <iostream>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <functional>

static const unsigned int LSTR = 1024;


using Constants::C_DERV1; 
using Constants::C_DERV3;
using Constants::C_CGS;

using Constants::PI; 

using Utility::gauss; 
using Utility::decodeLine;
using Utility::getFileNameOpt;
using Utility::decodeNumber;
using Utility::GetVariableNameSp;
using Utility::copyname;
using Utility::filterName;
using Utility::getElmName;
using Utility::getElmNameX;
using Utility::strcmpr;
using Utility::correctNames;
using Utility::getVariableName;
using Utility::strippedName;
using Utility::isValidType;

// ..........................................................................................

void OptimMainWindow::cmdPhases() {

 Twiss v;
 
 if( interrupted_ ) { interrupted_= false; return;}
 
 if(analyzed_) { 
   if( analyze(false)) return;
 } 
 else {
   if( analyze(true)) return;
 }

 RMatrix tm;
 int irt; 
 double alfa;
 char buf[256];
 
 char const* cher[3]={"X","Y","X&Y"};
	 
 if( CtSt_.IsRingCh ){
  findRMatrix(tm);
  irt = find_tunes(tm, Length_, v, &alfa);
  if(irt){ 
    strcpy(buf,"Cannot close for ");
    strcat(buf, cher[irt-1]);
    OptimMessageBox::warning(this, "Close Error", buf, QMessageBox::Ok);
    return;
  }
  v.nuY = v.nuX = 0.0;
 }
 else {
  setInitialBetas(v);
 }
 Phases(v);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::Phases (Twiss& v)
{
  int N = CtSt_.ArrayLen;

  RMatrix tm;
  Element e;
  double dalfa;
  std::complex<double> ev[4][4];
 
  double h     = Length_/N;
  double tetaY = tetaYo0_;
  double Enr   = Ein;
   
  v.eigenvectors(ev);

  double L  = 0.0;
  double Lp = 0.0;

  std::vector<double> x(N+1);
  
  std::vector<std::vector<double> > y(4);
  for (int i=0; i<4; ++i) { y[i] = std::vector<double>(N+1);}

  std::vector<LegoData> legodata;

  int k = 0; 
  int i;

  for(i=0; i<nelm_; ++i) {
    auto ep = beamline_[i];
    char nm = ep->etype();
    int  ns = fabs(ep->length()/h) + 1;

    if( nm=='A' || nm=='W' ||nm=='X' ) { ns=1; }

    auto e = std::shared_ptr<Element>( ep->split(ns) );
    
    // calculate Element's boxes located at the bottom of screan
    
    legodata.push_back( { L*0.01, ep->length()*0.01, ((e->G >=0.0) ? 1 :-1), ep->fullName() });

    L    +=  ep->length();

    // calculates beta-functions

    dalfa=0;
    for(int j=0; j<ns; ++j) {

      switch( ep->etype() ) {
	case 'B': 
        case 'D':
	  tm = e->rmatrix( dalfa, Enr, ms, tetaY, dalfa, e->checkEdge(j,ns) );
          dalfa -=  e->tilt(); 
	  break;
	default:
 	  tm = e->rmatrix( Enr, ms, tetaY, 0.0, e->checkEdge(j,ns) );
      }

      e->propagateLatticeFunctions(tm, v, ev);

      Lp +=  e->length();
      if( Lp > Length_*k/(N-1) ) {
        x[k]      = Lp*0.01;
        y[0][k]   = v.nuX - 0.5 * int(2.*v.nuX);
        y[1][k++] = v.nuY - 0.5 * int(2.*v.nuY);
      }
      if (k == N) break; 
    } // split element loop
    if (k == N) break; 
  } // main element loop     

  legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 
  x[0]    = 0;

  PlotSpec plotspecs;
  plotspecs.title     = "Phases (Uncoupled Lattice Functions)";
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;
  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis  symbol            title     bottom_title     color 
  //------------------------------------------------------------------------------------------------------------------------------------  
  curvespecs.push_back({ "PhaseX",   &x[0], &y[0][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "Phase (X/Y) [2 pi]",    ""} );  
  curvespecs.push_back({ "PhaseY",   &x[0], &y[1][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "Phase (X/Y) [2 pi]",    ""} );  


  addPlot(WindowId::PhaseCh, plotspecs, legodata);

}

