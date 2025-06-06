//  =================================================================
//
//  cmdBetas.cpp
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

using Constants::PI; 


// ..........................................................................................

void OptimMainWindow::cmdBetas ()
{

  Twiss v;
  
  if(interrupted_ ) {interrupted_ = false; return;}

  if(analyze(!analyzed_)) return;
 
  RMatrix tm;
  int irt;
  double alfa;
  char buf[256];
  char const*cher[3]={"X","Y", "X&Y"};
   
  if( CtSt_.IsRingCh ){
    findRMatrix(tm);
    irt=find_tunes(tm, Length_, v, &alfa); // here alfa is (dL/L)/(dp/p) 
    if(irt){ strcpy(buf,"Cannot close for ");
      strcat(buf, cher[irt-1]);
      OptimMessageBox::warning(this, "Close Error", buf, QMessageBox::Ok);
      return;
    }
  }
  else { 
    setInitialBetas(v); 
  }
  
  betas(v);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Twiss OptimMainWindow::betas(Twiss const& vstart)
{
  
  Twiss v = vstart;
  v.nuX = v.nuY   = 0.0;     // set the initial phase to zero. Is this really needed ?? 
  
  std::complex<double> ev[4][4];
  char buf[128];

  int N = CtSt_.ArrayLen;
  std::vector<double> x(N+1);
  
  std::vector<std::vector<double> > y(4);
  for (int i=0; i<4; ++i) {y[i] = std::vector<double>(N+ 1);}

  std::vector<LegoData> legodata;

  x[0]    = 0.0;
  y[0][0] = v.BtX*0.01;
  y[1][0] = v.BtY*0.01; // cm to m
  y[2][0] = v.DsX*0.01;
  y[3][0] = v.DsY*0.01; // cm to m

  double h = Length_/N; // step size 
  
  double tetaY = tetaYo0_;
  double Enr   = Ein;

  v.eigenvectors(ev);

  double  L      = 0.0; // length for lattice display (all lengths assumed positive)  
  int    ovf     = 0;
  int      k     = 0;

  k=1; 
  double coupling = 0.0;
  int    nc       = 0;

  double  Lp  = 0.0;
  

  for(int i=0; i<nelm_; ++i) {
 
    if (ovf) break;
   
    auto ep = beamline_[i];
    char nm     = ep->etype();
    int ns      = fabs(ep->length()/h) +1;     // no of element slices
    
    if( nm=='A' || nm=='W' || nm=='X') { ns=1; }

    auto e = std::shared_ptr<Element>(ep->split(ns));

    // calculate Element's boxes located at the bottom of screen
   
    legodata.push_back( { L*0.01, ep->length()*0.01, ((ep->G>=0 ) ? 1 :-1) , ep->fullName() });

    L     +=  ep->length();

    // calculate lattice functions
    
    nc +=ns;        // counter to estimate coupling
    RMatrix tm;     // transfer matrix 

    double dalfa  = 0.0; // angle alpha for *partial* element (j+1 out of ns slices) 
    for(int j=0; j<ns; ++j) {
      
     switch( nm ) {
 	case 'B':  
        case 'D':
	  tm     = e->rmatrix(dalfa, Enr, ms, tetaY, dalfa, e->checkEdge(j,ns)); // alfap = output value (passed by reference)  
	  dalfa -= e->tilt();
	  break;
	default:
	    tm = e->rmatrix( Enr, ms, tetaY, 0.0, e->checkEdge(j,ns) );
	}

     e->propagateLatticeFunctions(tm, v, ev);
     
     coupling += sqrt(fabs(tm[2][0]*tm[3][1] - tm[2][1]*tm[3][0]));
   
     Lp +=  e->length();

     if (Lp >= Length_*k/(N-1) ) {

       x[k] = 0.01*Lp;

       y[0][k]   = v.BtX*0.01;
       y[1][k]   = v.BtY*0.01;
       y[2][k]   = v.DsX*0.01;
       y[3][k++] = v.DsY*0.01;
	 
     }
     if( ( v.BtX >1.0e8) || ( v.BtY > 1.0e8) ) {
       OptimMessageBox::warning(this, "View|Betas", "Beta-functions are above threshold of 1000 km", QMessageBox::Ok);
       ovf=1;
       break;
     }
     if (k == N) break; 
    } // split element loop 

     if (k == N) break; 
  } // main element loop 

  if((coupling /= nc) > CtSt_.CouplThreshold ) {
     sprintf(buf,"XY-coupling (%.2e) above threshhold (%.2e). Change threshold or use View_4D menu instead", coupling,  CtSt_.CouplThreshold);
     OptimMessageBox::warning(this, "View|Betas", buf, QMessageBox::Ok);
     return v; 
  }
  legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 
  
  // specify the plot format and data ... 
   
  PlotSpec plotspecs;
  plotspecs.title        = "Uncoupled Lattice Functions";
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title       bottom_title   color       vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  
  curvespecs.push_back({ "BetaX",    &x[0], &y[0][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "X/Y Beta Function [m]", std::string()} );  
  curvespecs.push_back({ "BetaY",    &x[0], &y[1][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "X/Y Beta Function [m]", std::string()} );  
  curvespecs.push_back({ "DispX",    &x[0], &y[2][0], k, QwtSymbol::NoSymbol, QwtPlot::yRight, "X/Y Dispersion [m]",    std::string()} );  
  curvespecs.push_back({ "DispY",    &x[0], &y[3][0], k, QwtSymbol::NoSymbol, QwtPlot::yRight, "X/Y Dispersion [m]",    std::string()} );  

  addPlot(WindowId::BetaCh, plotspecs, legodata);

  return v;
}

