//  =================================================================
//
//  cmdBetas4DNew.cpp
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


#include <complex>
#include <cmath>
#include <cstdio>
#include <exception>
#include <memory>
#include <cstring>

#include <Constants.h>
#include <Element.h>
#include <OptimTextEditor.h>
#include <OptimMainWindow.h>
#include <OptimPlot.h>
#include <OptimMessages.h>
#include <Structs.h>
#include <Twiss.h>
#include <RMatrix.h>

using Constants::PI;

using Utility::strcmpr;
using Utility::filterName;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#define LSTR 1024


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdView4DBetaNew ()
{
  
   if(interrupted_) { interrupted_=false; return;}
   if(analyze(!analyzed_))return;

   Twiss4D v;
   RMatrix tm;
   std::complex<double> lambda[4], ev[4][4];
   char err[256];

   if( CtSt_.IsRingCh ){
     findRMatrix(tm);
     if( RMatrix_t<4,double>(tm).findEigenValues(lambda, ev) !=0 ){
       OptimMessageBox::warning(this, "4D View", "Cannot close lattice.", QMessageBox::Ok);
       return;
     }

     v.betatronFromEV(ev);
     getDisp4D(tm, v, err);
     if(err[0]){ OptimMessageBox::warning(this, "4D View", err, QMessageBox::Ok); return;}
    }
   else { 
     if(Get4DBetasFromFile(false, false, v)) return;
   }
   
   Betas4DNew(v);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::Betas4DNew (Twiss4D& v)
{

  char err[256];

  int N = CtSt_.ArrayLen;
  int n = CtSt_.ArrayLen;

  std::vector<double> x(N+1);
  
  std::vector<std::vector<double> > y(4);
  for (int i=0; i<4; ++i) {y[i] = std::vector<double>(N+ 1);}

  std::vector<LegoData> legodata;

  double dalfa;
  int ovf = 0;
  std::complex<double> ev[4][4];
  
  v.eigenvectors(ev);
  
  x[0]    = 0;
  y[0][0] = v.btx1*0.01; // convert from cm to m.
  y[1][0] = v.bty2*0.01;
  y[2][0] = v.bty1*0.01;
  y[3][0] = v.btx2*0.01;

  double phi    = tetaYo0_ * (PI/180.0);
  double teta   = tetaXo0_ * (PI/180.0);

  double sp = sin(phi);  // theta_x
  double cp = cos(phi);  // theta_x

  double st = sin(teta); //theta_y
  double ct = cos(teta); //theta_y
  
  RMatrix_t<3> rotx = {{ 1.0, 0.0, 0.0}, { 0.0, cp,  -sp }, { 0.0,  sp, cp}};
  RMatrix_t<3> roty = {{ ct,  0.0,  st}, { 0.0, 1.0, 0.0},  { -st, 0.0, ct}};
  
  RMatrix_t<3> frame = rotx*roty; // frame matrix describes orientation of local vector in global coordinates X Y Z
                                  // the unit vector tangent to the reference trajectory is (0, 0 ,1) in local coordinates  
                                  // (n_x, n_y, n_z)^T = frame * ( 0, 0, 1)^T      
  
  
  double h     = Length_/N;
  double tetaY = tetaYo0_;
  double Enr   = Ein;
  int k     = 1;
  double L  = 0.0;
  double Lp = 0.0;

  RMatrix tm;

  for( int i = 0; i<nelm_; ++i){
    if(ovf)break;
    auto ep = beamline_[i];
    int  ns = fabs(ep->length()/h)+1;

    // calculates Element's boxes located at the bottom of screan
    legodata.push_back(  { L*0.01, ep->length()*0.01, (ep->G>=0.0 ? 1 : -1), ep->fullName()} );
    L     +=  ep->length();

    std::unique_ptr<Beamline> bml(ep->splitnew(ns));
    int j=0;
    double dalfa = 0.0;
    for (auto it = bml->deep_begin(); it !=  bml->deep_end(); ++it,++j) {
       auto e = *it;
       auto nm = e->etype();

      // calculate beta-functions

#if 0
       // this seems broken FIXME !  
     RMatrix tm = (*it)->rmatrix(frame, Enr, ms, Element::checkEdge(j,ns));
#else	
      switch( nm ) {
 	case 'B':  
        case 'D':
	  tm     = e->rmatrix(dalfa, Enr, ms, tetaY, dalfa, e->checkEdge(j,ns)); // alfap = output value (passed by reference)  
	  dalfa -= e->tilt();
	  break;
	default:
	    tm = e->rmatrix( Enr, ms, tetaY, 0.0, e->checkEdge(j,ns) );
       }
#endif      
       (*it)->propagateLatticeFunctions( tm, v, ev);
   
       Lp +=  (*it)->length();

      if(Lp > Length_*k/(N-1)) {
	x[k]    = Lp*0.01;
        y[0][k] = v.btx1*0.01;
        y[1][k] = v.bty2*0.01;
        y[2][k] = v.bty1*0.01;
        y[3][k++]=v.btx2*0.01;
      }
      if((v.btx1>1.0e8)||(v.btx2>1.0e8)||(v.bty1>1.0e8)||(v.bty2>1.0e8)){
	OptimMessageBox::warning(this, "View|Betas", "Beta-functions are above threshold of 1000 km", QMessageBox::Ok);
        ovf = 1;
        break;
      }
   if (k == N) break;  
    } // split element loop
    if (k == N) break;  
  } // main element loop
  
  legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 

  PlotSpec plotspecs;
  plotspecs.title        = "4D Lattice Functions";
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         color  
  //------------------------------------------------------------------------------------------------------------------------------------  

  curvespecs.push_back({ "Beta1X",    &x[0], &y[0][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Beta_1X & Beta_2Y[m]", "" });  
  curvespecs.push_back({ "Beta2Y",    &x[0], &y[1][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Beta_1X & Beta_2Y[m]", "" });  
  curvespecs.push_back({ "Beta1Y",    &x[0], &y[2][0], k, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Beta_2X & Beta_1Y[m]", "" });  
  curvespecs.push_back({ "Beta2X",    &x[0], &y[3][0], k, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Beta_2X & Beta_1Y[m]", "" });  
  
  addPlot(WindowId::Beta4Ch, plotspecs, legodata); 
}

