//  =================================================================
//
//  LegacySpaceChargeBetas.cpp
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

#include <QCoreApplication>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPointer>

#include <memory>
#include <iostream>

using Constants::PI;
using Constants::C_DERV1;

using Utility::strcmpr;
using Utility::filterName;
using Utility::decodeLine;

#define LSTR 1024


void OptimMainWindow::Spcharge_betas(Twiss4D& v, BunchParam& bunch, double iscale, bool display_on)   
{

  char err[256];

  std::complex<double> ev[4][4]; 
  
  int N = CtSt_.ArrayLen;

  std::vector<double> x(N+1);
  
  std::vector<std::vector<double> > y(4);
  for (int i=0; i<4; ++i) {y[i].resize(N+1);}

  std::vector<LegoData> legodata;

  RMatrix tm;
  double dalfa;
  BeamSize bs;

  v.eigenvectors(ev);

  double x1 = sqrt(v.e1 * v.btx1);  
  double x2 = sqrt(v.e2 * v.btx2);
  double y1 = sqrt(v.e1 * v.bty1);  
  double y2 = sqrt(v.e2 * v.bty2);
  
  bs.a     = sqrt(x1*x1+x2*x2);
  bs.b     = sqrt(y1*y1+y2*y2);
  bs.alpha = -(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2))/(bs.a*bs.a);

  x[0]=0;
  y[0][0] = v.btx1*0.01;	 y[1][0] = v.bty2*0.01;
  y[2][0] = v.bty1*0.01;	 y[3][0] = v.btx2*0.01;

  double h     = Length_/N;
  double tetaY = tetaYo0_;

  double Enr = Ein;
  
  double L  = 0.0;
  double Lp = 0.0;
  int k     = 1;
  int ns    = 1;
 
  for(int i=0; i<nelm_; ++i){

    auto ep  = beamline_[i];
    char nm  = toupper(ep->name[0]);
    ns =  (space_charge_step_<h) ? fabs(ep->L/space_charge_step_)+1 : fabs(ep->L/h)+1;

    if( nm=='A' || nm=='W' ||nm=='X'){ns=1;}

 
    auto e = std::shared_ptr<Element>( ep->split(ns) );
  
    legodata.push_back( { L*0.01, ep->L*0.01, (ep->G>=0.0 ? 1:-1), ep->fullname});
    
    // calculates Element's boxes located at the bottom of screan
    L     += ep->L;

    // calculates beta-functions

    dalfa=0.;

    for(int j=0; j<ns; j++){
      switch( toupper(nm) ){
        case 'B':  
        case 'D':
          dalfa = GetCWSpaceChargeMatr(Enr, ms, iscale*bunch.I, bs, e.get(), tetaY, dalfa, tm, Element::checkEdge(j,ns))-e->T;
          break;
        default:
          GetCWSpaceChargeMatr(Enr, ms, iscale*bunch.I, bs, e.get(), tetaY, 0.0, tm, Element::checkEdge(j,ns));
      }
      

      for(int i=0; i<4; ++i){
         ev[1][i]=std::complex<double>(0.0, 0.0);
         ev[3][i]=std::complex<double>(0.0, 0.0);
         for(int j=0; j<4; ++j){
           ev[1][i] += tm[i][j]*ev[0][j];
           ev[3][i] += tm[i][j]*ev[2][j];
         }
      }

      for(int i=0; i<4; ++i) { // Why is this required ??? 
           ev[0][i]=ev[1][i];
           ev[2][i]=ev[3][i];
      }
  
    
      v.betatronFromEV(ev);

      v = tm * (Coordinates) v;

      Lp +=  e->L;
      double capa     = Ein*(2.*ms+Ein)/(Enr*(2.*ms+Enr));
      double capaP    = dpp*capa*(Enr+ms)/(Ein+ms);
      capaP    = capaP*capaP;
      capa     = sqrt(capa);
      x1       = sqrt(capa * v.e1 * v.btx1);
      x2       = sqrt(capa * v.e2 * v.btx2);
      y1       = sqrt(capa * v.e1 * v.bty1);
      y2       = sqrt(capa * v.e2 * v.bty2);
      bs.a     = sqrt(x1*x1+x2*x2+capaP*v.dx*v.dx);
      bs.b     = sqrt(y1*y1+y2*y2+capaP*v.dy*v.dy);
      bs.alpha = -(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2)+capaP*v.dx*v.dy)/(bs.a*bs.b);

      if (Lp*1.0001 > Length_*k/(N-1) ){
        x[k]    = Lp*.01;
        y[0][k] = v.btx1*0.01;
        y[1][k] = v.bty2*0.01;
        y[2][k] = v.bty1*0.01;
        y[3][k] = v.btx2*0.01;
        ++k;
       }
     }
  }
  legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 
  int n = k;

  if (!display_on) return;
  
  PlotSpec plotspecs;
  plotspecs.title        = "Beta Functions (4D w/Space Charge)";
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  

  curvespecs.push_back({ "Beta_1x",        &x[0], &y[0][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Beta_X&Y [m]",   0 });  
  curvespecs.push_back({ "Beta_2y",        &x[0], &y[1][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Beta_X&Y [m]",   0 });  
  curvespecs.push_back({ "Beta_1y",        &x[0], &y[2][0], n, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Beta_X&Y [m]",   0 });  
  curvespecs.push_back({ "Beta_2x",        &x[0], &y[3][0], n, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Beta_X&Y [m]",   0 });  
   
  addPlot( WindowId::BetaSpCh, plotspecs, legodata);
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
