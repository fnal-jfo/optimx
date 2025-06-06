//  =================================================================
//
//  BetasNew.cpp
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
#include <OptimMainWindow.h>


Twiss OptimMainWindow::betasnew(Twiss const& vstart)
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

  double h = Length/N; // step size 
  
  double tetaY = tetaYo0;
  double Enr   = Ein;

  v.eigenvectors(ev);

  double  L      = 0.0; // length for lattice display (all lengths assumed positive)  
  bool    ovf    =  false;
  int      k     = 0;

  k=1; 
  double coupling = 0.0;
  int    nc       = 0;

  double  Lp  = 0.0;
  

  StepIterator  sit( beamline_, h);
  
  for(int i=0; i<nelm_; ++i) {
 
    if (ovf) break;
   
    auto ep = beamline_[i];
    char nm     = toupper(ep->name[0]);
    int ns      = fabs(ep->L/h)+1;     // no of samples
   
    
    
    if( nm=='A' || nm=='W' || nm=='X') { ns=1; }

    auto e = std::shared_ptr<Element>(ep->split(ns));

    // calculate Element's boxes located at the bottom of screen
   
    legodata.push_back( { L*0.01, ep->L*0.01, ((ep->G>=0 ) ? 1 :-1) , ep->fullname });

    L     +=  ep->L;

    // calculate lattice functions
    
    nc +=ns;        // counter to estimate coupling
    RMatrix tm;     // transfer matrix 

    double dalfa  = 0.0;
    for(int j=0; j<ns; ++j) {
     switch( nm ) {
 	case 'B':  
        case 'D':
	  tm     = e->rmatrix(dalfa, Enr, ms, tetaY, dalfa, checkEdge(j,ns)); 
	  dalfa -= e->T;
	  break;
	default:
	    tm = e->rmatrix( Enr, ms, tetaY, 0.0, checkEdge(j,ns) );
	}

     e->propagateLatticeFunctions(tm, v, ev);
     
     coupling += sqrt(fabs(tm[2][0]*tm[3][1] - tm[2][1]*tm[3][0]));
   
     Lp +=  e->L;

     if (Lp > Length*k/(N-1) ) {

       x[k] = 0.01*Lp;

       y[0][k]   = v.BtX*0.01;
       y[1][k]   = v.BtY*0.01;
       y[2][k]   = v.DsX*0.01;
       y[3][k++] = v.DsY*0.01;
	 
     }
     if( ( v.BtX >1.0e8) || ( v.BtY > 1.0e8) ) {
       OptimWarningMessage(this, "View|Betas", "Beta-functions are above threshold of 1000 km", QMessageBox::Ok);
       ovf=true;
       break;
     }
   }
  }
  if((coupling /= nc) > CtSt_.CouplThreshold ) {
     sprintf(buf,"XY-coupling (%.2e) above threshhold (%.2e). Change threshold or use View_4D menu instead", coupling,  CtSt_.CouplThreshold);
     OptimWarningMessage(this, "View|Betas", buf, QMessageBox::Ok);
     return v; 
  }
  legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 
  
  // specify the plot format and data ... 
   
  PlotSpec plotspecs;
  plotspecs.title        = "Uncoupled Lattice Functions";
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  
  curvespecs.push_back({ "BetaX",    &x[0], &y[0][0], k, QwtPlot::yLeft,  "X/Y Beta Function [m]", 0 });  
  curvespecs.push_back({ "BetaY",    &x[0], &y[1][0], k, QwtPlot::yLeft,  "X/Y Beta Function [m]", 0 });  
  curvespecs.push_back({ "DispX",    &x[0], &y[2][0], k, QwtPlot::yRight, "X/Y Dispersion [m]",    0 });  
  curvespecs.push_back({ "DispY",    &x[0], &y[3][0], k, QwtPlot::yRight, "X/Y Dispersion [m]",    0 });  

  addPlot(WindowId::BetaCh, plotspecs, legodata);

  return v;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
