//  =================================================================
//
//  SpaceChargePhases.cpp
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
#include <cstdlib> // for gentenv
#include <memory>

#include <Element.h>
#include <Constants.h>
#include <OptimCalc.h>
#include <OptimMainWindow.h>
#include <OptimPlot.h>
#include <OptimMessages.h>
#include <Structs.h>
#include <Twiss.h>
#include <Utility.h>
#include <RMatrix.h>


using Constants::PI;
using Constants::C_DERV1;

using Utility::strcmpr;
using Utility::filterName;
using Utility::decodeLine;

#define LSTR 1024

void OptimMainWindow::spChargePhases(Twiss4D& v, BunchParam& bunch)
{
  char err[256];

  int N = CtSt_.ArrayLen;
  std::vector<double> x(N+1);
  
  std::vector<std::vector<double> > y(4);
  for (int i=0; i<4; ++i) {y[i].resize(N+ 1);}

  std::vector<LegoData> legodata;

  RMatrix tm;
  std::complex<double> ev[4][4];
  BeamSize bs;
  
  v.eigenvectors(ev);

  double x1 = sqrt(v.e1 * v.btx1);
  double x2 = sqrt(v.e2 * v.btx2);
  double y1 = sqrt(v.e1 * v.bty1);
  double y2 = sqrt(v.e2 * v.bty2);

  bs.a      = sqrt(x1*x1+x2*x2);
  bs.b      = sqrt(y1*y1+y2*y2);
  bs.alpha  = -(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2))/(bs.a*bs.a);
  x[0]     =  0;
  y[0][0]  =  0.0;
  y[1][0]  =  0.0;
  y[2][0]  =  v.teta1/(2.*PI);
  y[3][0]  =  v.teta2/(2.*PI);

  double h     = Length_/N;
  double tetaY = tetaYo0_;
  double Enr   = Ein;
  int    k  =  1;
  double L  =  0.0;
  double Lp =  0.0;

  for(int i=0; i<nelm_; ++i){
    auto ep = beamline_[i];
    char nm  = ep->etype();
    int ns   = (space_charge_step_<h) ? fabs(ep->length()/space_charge_step_)+1 : fabs(ep->length()/h)+1;
    if( nm=='A' || nm=='W' ||nm=='X'){ns=1;}

    auto e = std::shared_ptr<Element>( ep->split(ns) );

    // calculates Element's boxes located at the bottom of screan

    legodata.push_back( { L*0.01, ep->length()*0.01, (ep->G>=0.0 ? 1:-1), ep->fullName()});
    L    +=  ep->length();

    // calculates beta-functions

    double dalfa = 0.0;
    double alfap = 0.0;
    double current = bunch.I; // no scale factor.  

    for(int j=0; j<ns; ++j){

      int st         =  Element::checkEdge(j,ns); // element "slice type"
      RMatrix tm =  e->rmatrixsc(alfap, Enr, ms, current, bs, tetaY, dalfa, st );

      switch( nm ) {
        case 'B':  
        case 'D':
          dalfa = alfap-e->tilt();
          break;
        default:
	  break;
      }
      
      e->propagateLatticeFunctions( tm, v, ev);
      Lp +=  e->length();

      double capa     = Ein*(2.*ms+Ein)/(Enr*(2.*ms+Enr));
      double capaP    = dpp_*capa*(Enr+ms)/(Ein+ms);
      capaP     = capaP*capaP;
      capa      = sqrt(capa);
      x1        = sqrt(capa * v.e1 * v.btx1);
      double x2 = sqrt(capa * v.e2 * v.btx2);
      y1        = sqrt(capa * v.e1 * v.bty1);
      double y2 = sqrt(capa * v.e2 * v.bty2);
      bs.a      = sqrt(x1*x1+x2*x2+capaP*v.dx*v.dx);
      bs.b      = sqrt(y1*y1+y2*y2+capaP*v.dy*v.dy);
      bs.alpha  = -(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2)+capaP*v.dx*v.dy)/(bs.a*bs.b);

      if(Lp > Length_*k/(N-1) ){
       x[k]      =  Lp*0.01;

       while(v.qx > 0.5 ) v.qx -= 0.5;
       while(v.qy > 0.5 ) v.qy -= 0.5;
	  
       while(v.qx < -0.5 ) v.qx += 0.5;
       while(v.qy < -0.5 ) v.qy += 0.5;

       y[0][k] =  v.qx; //  -arg(ev[0][0])/(2.*PI);
       y[1][k] =  v.qy; //  -arg(ev[2][2])/(2.*PI);

       y[2][k]   = v.teta1/(2.*PI);
       y[3][k++] = v.teta2/(2.*PI);

      }  
      if (k == N) break;
    } // split element loop
    if (k == N) break;
  }// main element loop 
  legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 

  PlotSpec plotspecs;
  plotspecs.title        = "Phases (4D w/Space Charge)";
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         color  
  //------------------------------------------------------------------------------------------------------------------------------------  

  curvespecs.push_back({ "Q_1",        &x[0], &y[0][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Betatron size X&Y [cm]",   "" });  
  curvespecs.push_back({ "Q_2",        &x[0], &y[1][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Betatron size X&Y [cm]",   "" });  
  curvespecs.push_back({ "Nu_1",    &x[0], &y[2][0], k, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Phase/(2*pi)",             "" });  
  curvespecs.push_back({ "Nu_2",    &x[0], &y[3][0], k, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Phase/(2*pi)",             "" });  

  addPlot( WindowId::PhaseSpCh, plotspecs, legodata );
}

