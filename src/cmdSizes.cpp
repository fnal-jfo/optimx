//  =================================================================
//
//  cmdSizes.cpp
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

void OptimMainWindow::cmdSizes()
{

 Twiss v;
	 
 if(interrupted_ ) { interrupted_ = false; return;}
  
 if(analyzed_) { 
  if( analyze(false) ) return;
 } 
 else { 
  if(analyze(true)) return;
 }

 RMatrix tm;
 int irt;
 double alfa;
 char buf[256];

 char const* cher[3]={"X","Y","X&Y"};

 if( CtSt_.IsRingCh ) {
   findRMatrix(tm);
   irt = find_tunes(tm, Length_, v, &alfa);
   if(irt){ 
     strcpy(buf,"Cannot close for ");
     strcat(buf, cher[irt-1]);
     OptimMessageBox::warning(this, "Close Error", buf, QMessageBox::Ok);
     return;
   }
 }
 else {
  setInitialBetas(v);
 }
 Sizes(v);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::Sizes(Twiss& v)
{
  int N = CtSt_.ArrayLen;
 
  std::vector<double> x(N+1);
  std::vector<LegoData> legodata;

  std::vector<std::vector<double> > y(4);
  for (int i=0; i<4; i++) {
     y[i] = std::vector<double>(N + 1);
  }
    
  std::vector<double>                 scraperS; 
  std::vector<std::vector<double> >   scraperPos(2);
   
   RMatrix tm, me;
   int j, brk=0;
	
   x[0]=0;
   y[0][0] = sqrt(ex_ * v.BtX);		
   y[1][0] = sqrt(ey_ * v.BtY);
   y[2][0] = v.DsX * dpp_;          
   y[3][0] = v.DsY * dpp_;
  
   if(CtSt_.PlotTotalSize){
     y[0][0] = sqrt( y[0][0]*y[0][0] + y[2][0]*y[2][0]);
     y[1][0] = sqrt( y[1][0]*y[1][0] + y[3][0]*y[3][0]);
   }
    
   double h       = Length_/N;
   double Enr     = Ein;
   double tetaY   = tetaYo0_;
   int nscrapers = 0;

   int k     = 1;
   double L  = 0.0;
   double Lp = 0.0;

   tm.toUnity();

   for( int i=0; i<nelm_; ++i) {
     if(brk) break;
     auto ep = beamline_[i];
     char nm = ep->etype();
     int  ns = fabs(ep->length()/h)+1;
    
     if( nm=='H') ++nscrapers;
     if( nm=='A' || nm=='W' ||nm=='X' ) { ns=1; }
    

     auto e = std::shared_ptr<Element>( ep->split(ns) );     

     // calculates Element's boxes located at the bottom of screan
    
     legodata.push_back( { L*0.01, ep->length()*0.01, ((e->G >=0.0) ? 1 :-1), ep->fullName() });

     L    +=  ep->length();
    
     // calculates beta-functions and sizes
     double dalfa = 0.0;
     for( j=0; j<ns; ++j) {
       switch( toupper(nm) ) {
        case 'B':  
        case 'D':
	  me = e->rmatrix(dalfa, Enr, ms, tetaY, dalfa, e->checkEdge(j,ns));
	  dalfa -= e->tilt();
	  break;
        default:
	  me = e->rmatrix(Enr, ms, tetaY, 0.0, e->checkEdge(j,ns));
     } // switch 
      
     tm  = me*tm;
     Lp +=  e->length();

     //------------------------------------------------------------------------------------
     // NOTE: the calculation below account for the fact that a small amount of coupling may
     //       be present.
     // It uses the fact that the entries of transfer matrix from 1 -> 2 may be expressed
     // in terms of the lattice functions as:
     //
     // M(1->2) [0][0] = (beta_2/beta_1)^(1/2) cos psi + alpha_1 psi  
     // M(1->2) [0][1] = (beta_1*beta_2)^(1/2) sin psi
     // M(1->2) [1][0] = ... 
     // M(1->2)[1][1] = ...
     //--------------------------------------------------------------------------------------
     
     if ( Lp > Length_*k/(N-1) ){

       double capa  = Ein*(2.0*ms+Ein)/(Enr*(2.0*ms+Enr)); // kappa   = (cp_in)^2/(cp)^2   = 1 for Enr = E_in 
       double capaP = capa*(Enr+ms)/(Ein+ms);              // kappa_P = kappa^2            = 1 for Enr = E_in  

       capa  = sqrt(capa);    // kappa = (c p_i) / (c p) = p_i/p   
       x[k]  = Lp*0.01;

       double s1 = tm[0][1] / v.BtX;
       double sx = tm[0][0] - v.AlX * s1;
       double s2 = tm[0][3] / v.BtY;
       double sy = tm[0][2] - v.AlY * s2;

       y[0][k] = sqrt(capa*(ex_* v.BtX *(sx*sx+s1*s1) + ey_* v.BtY *(sy*sy+s2*s2)));

       s1 = tm[2][1] / v.BtX;
       sx = tm[2][0] - v.AlX * s1;
       s2 = tm[2][3] / v.BtY;
       sy = tm[2][2] - v.AlY * s2;

       y[1][k] = sqrt(capa*(ex_* v.BtX *(sx*sx+s1*s1) + ey_* v.BtY *(sy*sy+s2*s2)));

       y[2][k] = capaP*fabs((tm[0][0]*v.DsX + tm[0][1]*v.DsXp +
			     tm[0][2]*v.DsY + tm[0][3]*v.DsYp + tm[0][5])*dpp_);
       y[3][k] = capaP*fabs((tm[2][0]*v.DsX + tm[2][1]*v.DsXp +
			     tm[2][2]*v.DsY + tm[2][3]*v.DsYp + tm[2][5])*dpp_);

       if(CtSt_.PlotTotalSize) {
          y[0][k] = sqrt( y[0][k]*y[0][k] + y[2][k]*y[2][k]);
          y[1][k] = sqrt( y[1][k]*y[1][k] + y[3][k]*y[3][k]);
       }
       if(fabs(y[0][k])>100. || fabs(y[1][k])>100.) {
	  OptimMessageBox::warning(this, "View|Size", "Beam size is above threshold of 1 m ", QMessageBox::Ok);
          brk=1;
          break;
       }
       ++k;
    } 
    if ( k == N) break;  
   } // split element loop 
   if ( k == N) break;  
  } //main element loop  

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
      char nm = ep->etype();

      if( nm == 'H' ) {
	scraperS[nscrapers]                     = L*0.01;
        scraperPos[0][nscrapers]                = ep->S;
        scraperPos[1][nscrapers]   = (ep->N==2) ? ep->S : ep->A;
        ++nscrapers;
      }
      L +=  ep->length();
   } //for
  } // if 


  PlotSpec plotspecs;
  if  (CtSt_.PlotTotalSize) {
    plotspecs.title     = "Total (Betatron + Dispersion) Beam Envelopes";
  }
  else { 
    plotspecs.title     = "Betatron Beam Envelopes";
  }
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //  legendname xv       yv    n  axis   title     bottom_title  vertical_title  color    
  //------------------------------------------------------------------------------------------------------------------------------------  

  if ( (nscrapers > 0)  &&  (CtSt_.PlotApertures) ) {
  
    curvespecs.push_back({ "H-apert",  &scraperS[0], &scraperPos[0][0],   nscrapers, QwtSymbol::NoSymbol, QwtPlot::yLeft,    "", "cyan"    });  
    curvespecs.push_back({ "V-apert",  &scraperS[0], &scraperPos[1][0],   nscrapers, QwtSymbol::NoSymbol, QwtPlot::yRight,   "", "magenta" });  


    //QwtPlotCurve* caperturesh = new QwtPlotCurve("H Apertures");
    //QwtPlotCurve* caperturesv = new QwtPlotCurve("V Apertures");
    //caperturesh->setPen (QColor("cyan"),  2.0, Qt::DashLine);
    //caperturesv->setPen (QColor("cyan"),  2.0, Qt::DashLine);
  }

  if  (CtSt_.PlotTotalSize) {
    curvespecs.push_back({ "Ax_tot",  &x[0], &y[0][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Size_X [cm]", std::string() });  
    curvespecs.push_back({ "Ay_tot",  &x[0], &y[1][0], n, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Size_Y [cm]", std::string() });
    curvespecs.push_back({ "Ax_disp", &x[0], &y[2][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Size_X [cm]", std::string() });  
    curvespecs.push_back({ "Ay_disp", &x[0], &y[3][0], n, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Size_Y [cm]", std::string() });

  } 
  else {
    curvespecs.push_back({ "Ax",        &x[0], &y[0][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Size_X [cm]", std::string() });  
    curvespecs.push_back({ "Ay",        &x[0], &y[1][0], n, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Size_Y [cm]", std::string() });
    curvespecs.push_back({ "Ax_disp",   &x[0], &y[2][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Size_X [cm]", std::string() });  
    curvespecs.push_back({ "Ay_disp",   &x[0], &y[3][0], n, QwtSymbol::NoSymbol, QwtPlot::yRight,  "SIze_Y [cm]", std::string() });  
  }

  if  (CtSt_.PlotTotalSize) {
      addPlot(WindowId::SizeTotCh, plotspecs, legodata);
  }
  else {
      addPlot(WindowId::SizeCh, plotspecs, legodata);
  }

}

