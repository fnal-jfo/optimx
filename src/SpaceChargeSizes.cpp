//  =================================================================
//
//  SpaceChargeSizes.cpp
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
#include <limits>
#include <cstdlib> // for gentenv
#include <memory>
#include <iostream>

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
#include <QMdiSubWindow>

using Constants::PI;
using Constants::C_DERV1;

#define LSTR 1024


void OptimMainWindow::spChargeSizes(Twiss4D& v, BunchParam& bunch)
{
  // Space Charge sizes are always (and only) total (betatron + dispersion)

  char err[256];
  int N = CtSt_.ArrayLen;
  std::vector<double> x(N+1);
  
  std::vector<std::vector<double> > y(4);
  for (int i=0; i<4; ++i) {y[i].resize(N+1);}

  std::vector<LegoData> legodata;

  RMatrix tm;
  double dalfa, capa,capaP;
  std::complex<double> ev[4][4];
  
  v.eigenvectors(ev);

  x[0]  = 0.;
  double x1 = sqrt(v.e1 * v.btx1);
  double x2 = sqrt(v.e2 * v.btx2);
  double y1 = sqrt(v.e1 * v.bty1);
  double y2 = sqrt(v.e2 * v.bty2);
  
  // the size may not be computed properly ... FIXME
  BeamSize bs;
  bs.a     = y[0][0]  =  sqrt(x1*x1 + x2*x2 +  dpp_*dpp_*v.dx*v.dx );
  bs.b     = y[1][0]  =  sqrt(y1*y1 + y2*y2 +  dpp_*dpp_*v.dy*v.dy );
  bs.alpha = y[2][0]  = -(x1*y1*cos(v.teta1) + x2*y2*cos(v.teta2) + dpp_*dpp_*v.dx*v.dy)/(bs.a*bs.b);

  double xm =  bs.a;
  double ym  = bs.b;
  double alf = bs.alpha;
  double e2 =  xm*xm-ym*ym;

  y[0][0] = xm*ym*sqrt(2.0*(1.0-alf*alf)/(xm*xm+ym*ym-sqrt(e2*e2+4.*alf*alf*xm*xm*ym*ym)));
  y[1][0] = xm*ym*sqrt(2.0*(1.0-alf*alf)/(xm*xm+ym*ym+sqrt(e2*e2+4.*alf*alf*xm*xm*ym*ym)));
  y[2][0]  = (fabs(e2)<1.e-10) ? 90.0 : 90.0/PI*atan2(2.*alf*xm*ym, e2);
  
  double h     = Length_/N;
  double tetaY = tetaYo0_;
  double Enr   = Ein;

  double L  = 0.0;
  double Lp = 0.0;

  int k = 1;
  for(int i=0; i<nelm_; ++i) {
    auto ep      = beamline_[i];
    char nm = toupper(ep->name()[0]);
    int ns = (space_charge_step_<h) ? fabs(ep->length()/space_charge_step_)+1 : fabs(ep->length()/h)+1;

    if( nm=='A' || nm=='W' ||nm=='X'){ns=1;}
     
    auto e = std::shared_ptr<Element>( ep->split(ns) );
    legodata.push_back( { L*0.01, ep->length()*0.01, (ep->G>=0.0 ? 1:-1), ep->fullName()});
    // calculates Element's boxes located at the bottom of screan

    L    += ep->length();

    // calculates beta-functions

    double dalfa = 0.0;
    double alfap = 0.0;
    double current = bunch.I; // no scale factor
  
    for(int j=0; j<ns; ++j){

      int st     =  Element::checkEdge(j,ns); // element "slice type"
      RMatrix tm =  e->rmatrixsc(alfap, Enr, ms, current, bs, tetaY, dalfa, st );

    switch( toupper(nm) ){
        case 'B':  
        case 'D':
          dalfa = alfap - e->tilt();
          break;
        default:
	  break;  
      }

      e->propagateLatticeFunctions( tm, v, ev);

      Lp      += e->length();

      double capa     = Ein*(2.*ms+Ein)/(Enr*(2.*ms+Enr));
      double capaP    = dpp_*capa*(Enr+ms)/(Ein+ms);
      capaP    = capaP*capaP;
      capa     = sqrt(capa);

      x1       = sqrt( capa * v.e1 * v.btx1);
      x2       = sqrt( capa * v.e2 * v.btx2);
      y1       = sqrt( capa * v.e1 * v.bty1);
      y2       = sqrt( capa * v.e2 * v.bty2);

      bs.a     = sqrt(x1*x1+x2*x2 + capaP*v.dx*v.dx);
      bs.b     = sqrt(y1*y1+y2*y2 + capaP*v.dy*v.dy);

      bs.alpha = (x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2) + capaP*v.dx*v.dy )/(bs.a*bs.b);

      if (Lp > Length_*k/(N-1) ){

	x[k]= Lp*0.01;

	xm  = bs.a;
        ym  = bs.b;
        alf = bs.alpha;
        e2  = xm*xm-ym*ym;

	y[0][k] = xm*ym*sqrt(2*(1-alf*alf)/(xm*xm+ym*ym - sqrt(e2*e2 +4*alf*alf*xm*xm*ym*ym)));
        y[1][k] = xm*ym*sqrt(2*(1-alf*alf)/(xm*xm+ym*ym + sqrt(e2*e2 +4*alf*alf*xm*xm*ym*ym)));
	y[2][k] = (fabs(e2) < 1.0e-10) ? 90.0 : (90.0/PI) * atan2(2.*alf*xm*ym, e2);
        ++k;
      }
    if (k==N) break;
    } // split element loop
   if (k==N) break;
  } //main element loop
  
  legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 

  PlotSpec plotspecs;
  plotspecs.title        = "Beam Size (4D w/Space Charge)";
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  

  curvespecs.push_back({ "Ax_tot",        &x[0], &y[0][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Betatron + Disp. size X&Y [cm]",   "" });  
  curvespecs.push_back({ "Ay_tot",        &x[0], &y[1][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Betatron + Disp. size X&Y [cm]",   "" });  
  curvespecs.push_back({ "Angle",         &x[0], &y[2][0], k, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Angle[deg][-90,+90]",              "" });  

  addPlot( WindowId::SizeSpCh, plotspecs, legodata); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

