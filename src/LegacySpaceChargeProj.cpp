//  =================================================================
//
//  LegacySpaceChargeProj.cpp
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
#include <Cavity.h>
#include <Constants.h>
#include <FunctionDialog.h>
#include <OptimCalc.h>
#include <OptimEditor.h>
#include <OptimMainWindow.h>
#include <OptimPlot.h>
#include <OptimTextEditor.h>
#include <OptimMessages.h>
#include <RootFinder.h>
#include <SpaceChargeControlDialog.h>
#include <Structs.h>
#include <Twiss.h>
#include <Utility.h>
#include <RMatrix.h>

#include <QCoreApplication>
#include <QProgressDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPointer>
#include <QTextEdit>

#include <QwtLegend>

#include <memory>
#include <iostream>

using Constants::PI;
using Constants::C_DERV1;

using Utility::strcmpr;
using Utility::filterName;
using Utility::decodeLine;

#define LSTR 1024

void OptimMainWindow::SpaceChargeProj(Twiss4D& v, BunchParam& bunch)
{
  char err[256];

  int N = CtSt_.ArrayLen;
  std::vector<double> x(N+1);
  
  std::vector<std::vector<double> > y(4);
  for (int i=0; i<4; ++i) {y[i] = std::vector<double>(N+ 1);}

  std::vector<LegoData> legodata;

  RMatrix tm;
  std::complex<double> ev[4][4];
  BeamSize bs;

  std::vector<double>                 scraperS; 
  std::vector<std::vector<double> >   scraperPos(2);
  int nscrapers = 0;

  v.eigenvectors(ev);
  
  x[0] = 0.0;
  double x1   = sqrt(v.e1 * v.btx1);
  double x2   = sqrt(v.e2 * v.btx2);
  double y1   = sqrt(v.e1 * v.bty1);
  double y2   = sqrt(v.e2 * v.bty2);

  bs.a     = y[0][0] =  sqrt(x1*x1+x2*x2 + dpp*dpp*v.dx*v.dx );
  bs.b     = y[1][0] =  sqrt(y1*y1+y2*y2 + dpp*dpp*v.dy*v.dy );
  bs.alpha = y[2][0] = -(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2) + dpp*dpp*v.dx*v.dy ) / (y[0][0]*y[1][0]);

  double h     = Length_/N;
  double tetaY = tetaYo0_;
  double Enr   = Ein;
  int    k   = 1;
  double L   = 0.0;
  double Lp  = 0.0;

  for(int i=0 ; i<nelm_; ++i){
    auto ep = beamline_[i];
    char     nm = toupper(ep->name[0]);
    int ns = (space_charge_step_<h) ? fabs(ep->L/space_charge_step_)+1 : fabs(ep->L/h)+1;
    if( nm=='H') ++nscrapers;
    if( nm=='A' || nm=='W' ||nm=='X'){ns=1;}

    auto e = std::shared_ptr<Element>( ep->split(ns) );
    legodata.push_back( { L*0.01, ep->L*0.01, (ep->G>=0.0 ? 1:-1), ep->fullname});

    // calculate Element's boxes located at the bottom of screan
    L    +=  ep->L;

    // calculate beta-functions

    double dalfa=0.0;
    for(int j=0; j<ns; ++j){
      switch( toupper(nm) ){
        case 'B':  
        case 'D':
          dalfa = GetCWSpaceChargeMatr(Enr, ms, bunch.I, bs, e.get(), tetaY, dalfa, tm, Element::checkEdge(j,ns))-e->T;
          break;
        default:
          GetCWSpaceChargeMatr(Enr, ms, bunch.I, bs, e.get(), tetaY, 0., tm, Element::checkEdge(j,ns));
      }
      for(int i=0; i<4; ++i){
        ev[1][i]=std::complex<double>(0., 0.);
        ev[3][i]=std::complex<double>(0., 0.);
        for(int j=0; j<4; ++j){
          ev[1][i] += tm[i][j]*ev[0][j];
          ev[3][i] += tm[i][j]*ev[2][j];
	}
      }
      for(int j=0; j< 4; ++j){
           ev[0][j] = ev[1][j];
           ev[2][j] = ev[3][j];
      }

      v.betatronFromEV(ev);
      v = tm * (Coordinates) v;

      Lp     += e->L;
      double capa    = Ein*(2.*ms+Ein)/(Enr*(2.*ms+Enr));
      double capaP   = dpp*capa*(Enr+ms)/(Ein+ms);
             capaP   = capaP*capaP;
             capa    = sqrt(capa);
        
      x1  = sqrt(capa * v.e1 * v.btx1);
      x2  = sqrt(capa * v.e2 * v.btx2);
      y1  = sqrt(capa * v.e1 * v.bty1);
      y2  = sqrt(capa * v.e2 * v.bty2);

      bs.a     = sqrt(x1*x1+x2*x2 + capaP*v.dx*v.dx );
      bs.b     = sqrt(y1*y1+y2*y2 + capaP*v.dy*v.dy );
      bs.alpha = (x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2) + capaP*v.dx*v.dy )/(bs.a*bs.b);
        
      if(Lp*1.0001 > Length_*k/(N-1) ){
        x[k]    = Lp*.01;
        y[0][k] = bs.a;
        y[1][k] = bs.b;
        y[2][k] = bs.alpha;
        ++k;
      }
    }
  }
  legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 

  int n = k;

  if( nscrapers > 0 ){
    scraperS.resize(nscrapers);
    scraperPos[0] = std::vector<double>(nscrapers);
    scraperPos[1] = std::vector<double>(nscrapers);

    for (int i=0; i<nscrapers; ++i) {
      scraperS[i] = scraperPos[0][i] =  scraperPos[1][i] = 0.0;  
    }				    

    L         = 0.0;
    nscrapers = 0;

    for( int i=0; i<nelm_; ++i) {
      auto ep = beamline_[i];
      char nm = toupper(ep->name[0]);

      if( nm == 'H' ) {
	scraperS[nscrapers]                     = L*0.01;
        scraperPos[0][nscrapers]                = ep->S;
        scraperPos[1][nscrapers]   = (ep->N==2) ? ep->S : ep->A;
        ++nscrapers;
      }
      L +=  ep->L;
   } //for
  } // if 


  
  PlotSpec plotspecs;
  plotspecs.title        = "Projections (4D w/Space Charge)";
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  

  if ( (nscrapers > 0)  &&  (CtSt_.PlotApertures) ) {
  
    curvespecs.push_back({ "H-apert",  &scraperS[0], &scraperPos[0][0],   nscrapers, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "", "cyan" });  
    curvespecs.push_back({ "V-apert",  &scraperS[0], &scraperPos[1][0],   nscrapers, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "", "magenta" });  
  }

  curvespecs.push_back({ "A_x",        &x[0], &y[0][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Betatron + Disp.  Proj. [cm]",   0 });  
  curvespecs.push_back({ "A_y",        &x[0], &y[1][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Betatron + Disp.  Proj. [cm]",   0 });  
  curvespecs.push_back({ "alpha_xy",   &x[0], &y[2][0], n, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Alpha_xy [-1,1]",       0 });  

  addPlot( WindowId::ProjSpCh, plotspecs, legodata);
}
