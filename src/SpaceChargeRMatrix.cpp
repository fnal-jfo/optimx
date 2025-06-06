//  =================================================================
//
//  SpaceChargeRMatrix.cpp
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


double OptimMainWindow::spChargeRMatrix(RMatrix& tmf, Twiss4D& v, BunchParam& bunch, double iscale)
{

  //-----------------------------------------------------------------------------------------
  // Compute transfer matrix for the whole beamline,
  // in the presence of space charge, assuming periodicity
  //
  // returns: tmf and enr, the total (kinetic) energy 
  // v:      initial 4D Twiss functions (must include correct values for u, teta1 and teta2) 
  // iscale: scaling factor for the current
  //-----------------------------------------------------------------------------------------
  
  char err[256];
  int N = CtSt_.ArrayLen;
  std::complex<double> ev[4][4];
  
  std::vector<double> x(N+1);
  
  RMatrix tm;
  BeamSize bs;

  tmf.toUnity();

  v.eigenvectors(ev);


  double x1 = sqrt(v.e1 * v.btx1);  
  double x2 = sqrt(v.e2 * v.btx2);
  double y1 = sqrt(v.e1 * v.bty1);  
  double y2 = sqrt(v.e2 * v.bty2);
  
  bs.a     = sqrt(x1*x1+x2*x2);
  bs.b     = sqrt(y1*y1+y2*y2);
  bs.alpha = -(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2))/(bs.a*bs.a);
  
  double h     = Length_/N;
  double tetaY = tetaYo0_;

  double Enr = Ein;
  
  double L  = 0.0;
  double Lp = 0.0;
  int k     = 1;
  int ns    = 1;
 
  for(int i=0; i<nelm_; ++i) {

    auto ep  = beamline_[i];

    char nm  = ep->etype();
    ns =  (space_charge_step_<h) ? fabs(ep->length()/space_charge_step_)+1 : fabs(ep->length()/h)+1;

    if( nm=='A' || nm=='W' ||nm=='X'){ns=1;}
 
    auto e = std::shared_ptr<Element>( ep->split(ns) );
    L     += ep->length();

    double dalfa  = 0.0;
    double alfap = 0.0; 
    double current = iscale*bunch.I;  // iscale set by caller. Default = 1.0

    for(int j=0; j<ns; ++j) {

      int st         =  Element::checkEdge(j,ns); // element "slice type" 0 = whole element ??? 
      RMatrix tm =  e->rmatrixsc(alfap, Enr, ms, current, bs, tetaY, dalfa, st );
      switch( nm ){
        case 'B':  
        case 'D':
	  dalfa = alfap-e->tilt(); // WHY IS THIS NEEDED ??? DOES THIS BELONG HERE ???
          break;
        default:
          break;
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
           ev[0][i] = ev[1][i];
           ev[2][i] = ev[3][i];
      }
  
      v.betatronFromEV(ev);

      v   = tm * (Vector_t<6,double>) v;
      tmf = tm * tmf;
      
      Lp +=  e->length();
      double capa     = Ein*(2.*ms+Ein)/(Enr*(2.*ms+Enr));
      double capaP    = dpp_*capa*(Enr+ms)/(Ein+ms);
      capaP    = capaP*capaP;
      capa     = sqrt(capa);
      x1       = sqrt(capa * v.e1 * v.btx1);
      x2       = sqrt(capa * v.e2 * v.btx2);
      y1       = sqrt(capa * v.e1 * v.bty1);
      y2       = sqrt(capa * v.e2 * v.bty2);

      bs.a     = sqrt(x1*x1+x2*x2+capaP*v.dx*v.dx);
      bs.b     = sqrt(y1*y1+y2*y2+capaP*v.dy*v.dy);
      bs.alpha = -(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2)+capaP*v.dx*v.dy)/(bs.a*bs.b);

    }
   }

  return Enr;
}
