//  =================================================================
//
//  cmdDispersion4D.cpp
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

#include <iostream>
#include <complex>
#include <cmath>
#include <cstdio>
#include <exception>
#include <memory>
#include <cstring>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QObject>

#include <Constants.h>
#include <Element.h>
#include <Control4DDialog.h>
#include <ElementSelectionDialog.h>
#include <OptimCalc.h>
#include <OptimEditor.h>
#include <OptimTextEditor.h>
#include <OptimMainWindow.h>
#include <OptimMdiSubWindow.h>
#include <OptimMdiArea.h>
#include <OptimPlot.h>
#include <OptimMessages.h>
#include <OptimApp.h>
#include <Structs.h>
#include <Twiss.h>
#include <Utility.h>
#include <RMatrix.h>

#include <QwtLegend>

using Constants::PI;
using Utility::matr_invert;
using Utility::balanc;
using Utility::elmhes;
using Utility::hqr;
using Utility::strcmpr;
using Utility::decodeLine;
using Utility::decodeExtLine;
using Utility::filterName;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#define LSTR 1024

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdView4dDispersion()
{
    Twiss4D v;
    if( interrupted_ ){  interrupted_=false; return; };
    if( analyze(!analyzed_) ) return;

    RMatrix tm;
    std::complex<double> lambda[4], ev[4][4];
    char err[256];
    if( CtSt_.IsRingCh ){
      findRMatrix(tm);
      if(RMatrix_t<4,double>(tm).findEigenValues(lambda, ev)!=0){
	OptimMessageBox::warning(this,"4D View", "Cannot close lattice.", QMessageBox::Ok);
        return;
      }

      v.betatronFromEV(ev);

      getDisp4D(tm, v, err);
      if(err[0]){ OptimMessageBox::warning(this,"4D View", err, QMessageBox::Ok); return;}
    }
    else if(Get4DBetasFromFile(false, false, v))return;
    Dispersion4D(v);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::Dispersion4D (Twiss4D& v)
{
  int N = CtSt_.ArrayLen;

  std::vector<double> x(N+1);
  
  std::vector<std::vector<double> > y(4);
  for (int i=0; i<4; ++i) {y[i] = std::vector<double>(N+ 1);}

  std::vector<LegoData> legodata;

  RMatrix tm;
     
  x[0]         = 0;
  y[0][0]      = v.dx*0.01;
  y[1][0]      = v.dy*0.01;
  double h     = Length_/N;
  double tetaY = tetaYo0_;
  double Enr   = Ein;
  double L     = 0.0;
  double Lp    = 0.0;

  int    k = 1; 

  for(int i=0; i<nelm_; ++i){
    auto ep  = beamline_[i];
    char nm  = ep->etype();
    int  ns  = fabs(ep->length()/h)+1;
    if( nm=='A' || nm=='W' ||nm=='X'){ns=1;}

    auto e = std::shared_ptr<Element>(ep->split(ns) );

    legodata.push_back( { L*0.01, ep->length()*0.01, (ep->G>=0.0 ? 1 : -1), ep->fullName() });

    // calculate Element boxes located at the bottom of screan
    L     +=  ep->length();

    // calculate beta-functions
    double dalfa =0.0;

    for(int j=0; j<ns; ++j) {
      switch(nm ) {
	case 'B':  
        case 'D':
          tm = e->rmatrix(dalfa, Enr, ms, tetaY, dalfa, Element::checkEdge(j,ns));
	  dalfa -= e->tilt();
	  break;
        default:
          tm = e->rmatrix(Enr, ms, tetaY, 0.0, Element::checkEdge(j,ns));
      }

      v =  tm * (Vector_t<6,double>) v; // propagate dispersion

      Lp +=  e->length();
      if( Lp > Length_*k/(N-1) ){
        x[k]      = Lp*0.01;
	y[0][k]   = 0.01*v.dx;
        y[1][k++] = 0.01*v.dy;
      }
    if (k == N-1 ) break;
    } // split element loop
    if (k == N-1 ) break;
  } // main element loop 

  legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 
  int const n = k;

   //  char const* sb[6]= {"DispX", "DispY", "",  "",  "DISPERSION[M]", ""};

  QwtPlotCurve* cdispx        = new QwtPlotCurve("dispx");
  QwtPlotCurve* cdispy        = new QwtPlotCurve("dispy");

  cdispx->setPen (QColor("red"),            2.0, Qt::SolidLine);
  cdispy->setPen (QColor("green"),          2.0, Qt::SolidLine);

  cdispx->setSamples( &x[0], &y[0][0], k);  
  cdispy->setSamples( &x[0], &y[1][0], k); 

  cdispx->setYAxis(QwtPlot::yLeft);
  cdispy->setYAxis(QwtPlot::yLeft);
  

  PlotSpec plotspecs;
  plotspecs.title        = "Dispersion (4D)";
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  

  curvespecs.push_back({ "DispX",     &x[0], &y[0][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Dispersion [m]",   "" });  
  curvespecs.push_back({ "DispY",     &x[0], &y[1][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Dispersion [m]",   "" });  
   
  addPlot( WindowId::Disp4Ch, plotspecs, legodata);
}

