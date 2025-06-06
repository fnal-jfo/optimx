//  =================================================================
//
//  cmdTrackerPlotDistribution.cpp
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
#include <omp.h>
#include <sqlite3.h>
#include <sqlite/private/private_accessor.hpp>
#include <Constants.h>
#include <Globals.h>
#include <BeamMoments.h>
#include <Element.h>
#include <OptimCalc.h>
#include <OptimExceptions.h>
#include <OptimMainWindow.h>
#include <OptimMdiArea.h>
#include <OptimMdiSubWindow.h>
#include <OptimMessages.h>
#include <OptimPlot.h>
#include <OptimTextEditor.h>
#include <OptimTracker.h>
#include <OptimUserRtti.h>
#include <RMatrix.h>
#include <ScatterData.h>
#include <ScatterPlotItem.h>
#include <Structs.h>
#include <Tracker3DSeriesData.h>
#include <TrackerPlot.h>
#include <TrackerSaveDistributionDialog.h>
#include <TrackingParametersDialog.h>
#include <TrackParam.h>
#include <Twiss.h>
#include <Utility.h>

#include <QApplication>
#include <QFileDialog>
#include <QMdiArea>
#include <QTextEdit>

#include <fmt/format.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

//using Utility::gauss;
//using Utility::decodeExtLine;
//using Utility::strcmpr;
//using Utility::filterName;
//using Utility::Filter7;

using Constants::PI;
using Constants::C_DERV1;

#define LSTR 1024

void OptimTracker::cmdTrackerPlotDistribution()
{

  if (history_.size() == 0 ) {
      OptimMessageBox::warning(this, "Tracking", "There is no tracking history.", QMessageBox::Ok); 
      return;
  }

  int  Nelm_f; 
  bool plt[6];
  Coordinates *v;

  char const* nm[] = {"X[cm]", "Px[mrad]", "Y[cm]", "Py[mrad]", "S[cm]", "dP/P[mrad]"};

  double minv[6];
  double maxv[6];
  
  /*
  Coordinates* c = 0;

  // determine the min/max values in the input and output coordinates array  
  for (int i=0; i<6; ++i ) { 
    c = std::min_element( &v_[0],   &v_[N_-1],   [&i]( Coordinates const& lhs,  Coordinates const& rhs) { return lhs[i]  < rhs[i]; } ); 
    c = std::min_element( &vin_[0], &vin_[N_-1], [&i]( Coordinates const& lhs,  Coordinates const& rhs) { return lhs[i]  < rhs[i]; } ); 
    minv[i] = (*c)[i];
    c = std::max_element( &v_[0],   &v_[N_-1],   [&i]( Coordinates const& lhs,  Coordinates const& rhs) { return lhs[i]  < rhs[i]; } ); 
    c = std::max_element( &vin_[0], &vin_[N_-1], [&i]( Coordinates const& lhs,  Coordinates const& rhs) { return lhs[i]  < rhs[i]; } ); 
    maxv[i] =  (*c)[i];
  }
  for (int i=0; i<6; ++i ) { 
    c = std::min_element( &v_[0],   &v_[N_-1],   [&i]( Coordinates const& lhs,  Coordinates const& rhs) { return lhs[i]  < rhs[i]; } ); 
    c = std::min_element( &vin_[0], &vin_[N_-1], [&i]( Coordinates const& lhs,  Coordinates const& rhs) { return lhs[i]  < rhs[i]; } ); 
    minv[i] = (*c)[i];
    c = std::max_element( &v_[0],   &v_[N_-1],   [&i]( Coordinates const& lhs,  Coordinates const& rhs) { return lhs[i]  < rhs[i]; } ); 
    c = std::max_element( &vin_[0], &vin_[N_-1], [&i]( Coordinates const& lhs,  Coordinates const& rhs) { return lhs[i]  < rhs[i]; } ); 
    maxv[i] =  (*c)[i];
  }
  */
  
  static TrackPlotDistributionDialog* dialog = 0;
  if (!dialog) {
    dialog = new TrackPlotDistributionDialog(0);
    dialog->data_ = TrGs_;
    dialog->set();
  }

  if (dialog->exec() == QDialog::Rejected ) return;
  TrGs_ = dialog->data_; 

  int nb = std::min(TrGs_.Nbin, 10000);    // number of bins
  TrGs_.Nbin = nb;

  double xmin = TrGs_.Xin;
  double xmax = TrGs_.Xfin;

  if( fabs(xmax-xmin) < 1.0e-6) {xmax = xmin + 1.1e-6;}
  
  v = (view_elem_) ? &vfin_[0] : &vin_[0];
  
  std::vector<double> g(6*nb);
  
  for(int i=0; i<6*nb; ++i) g[i]=0;
  
  // build histogram
  //................

  for(int k=0; k<6; ++k) {
    double A = 1.0 + 999.0*(k&1);    
    plt[k] = false;
    if( k==0 && !TrGs_.X ) continue;
    if( k==1 && !TrGs_.Px) continue;
    if( k==2 && !TrGs_.Y ) continue;
    if( k==3 && !TrGs_.Py) continue;
    if( k==4 && !TrGs_.S ) continue;
    if( k==5 && !TrGs_.Ps) continue;
    plt[k] = true;

    int j=0;
    for( int i=0; i<N_; ++i){
      if( v[i].lost && view_elem_) continue;
      j = (v[i][k]*A - xmin)/(xmax-xmin) * nb;
      	if (j<0)   j = 0;
      	if (j>=nb) j = nb-1;
      	g[j+nb*k] += v[i].weight;
      }
  }

  int j=0;

  for( int i=0; i<6*nb; ++i) if(g[i]>j) j=g[i];  // find maximum

  xmin_ = xmin;
  xmax_ = xmax;

  double yminL = 0.0;
  double ymaxL = j+1;
   
   // plot histogram
   
   Nelm_f=0;

   if(view_elem_) {
     Nelm_f = (view_at_elem_>0 ) ? view_at_elem_ : mainw_->nelm_;
   }

   char buf[128];
   int n = 2*nb;
   
   std::vector<double> xv(nb*2);
   std::vector<std::vector<double>> bins(6);
   
   for(int i=0; i<nb; ++i){
      xv[2*i]   = xmin +(xmax-xmin)*i/nb;
      xv[2*i+1] = xmin +(xmax-xmin)*(i+1)/nb;
   }

   if(Nelm_f) {
     char const* fnm = (Nelm_f-1 ==0 ) ? "START" : mainw_->beamline_[Nelm_f-1]->fullName();
     sprintf(buf, "Distr.at elem.%d(%s)", Nelm_f, fnm);
   }
   else sprintf(buf, "Distr.at lattice start");


  OptimPlot*     plot   = 0;
  auto TrackerDistPlt = mainw_->getAttachedSubWin(WindowId::TrackerDistPlt ); 
  if (!TrackerDistPlt ) { 
    TrackerDistPlt = mainw_->createAttachedSubWin( (plot = new OptimPlot(mainw_) ), WindowId::TrackerDistPlt);
  }
  else {
    plot = qobject_cast<OptimPlot*>( TrackerDistPlt->widget() );
    if (mainw_->CtSt_.ClearPlot) { 
      plot->clear();
    }
  }
  TrackerDistPlt->raise();
   
  PlotSpec plotspecs;
  plotspecs.title     = buf;
  plotspecs.bottom_title = "S [m] or Turn No";

  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  

  int i=0;
  for(int k=0; k<6; ++k){
     if( i >=4 )    continue; // WHY ? No long info available? 
      if( !plt[k])   continue;
      bins[i].resize(nb*2);
      for(j=0; j<nb; ++j){
        bins[i][2*j]    = g[j+nb*k];
      	bins[i][2*j+1]  = g[j+nb*k];
      }
      curvespecs.push_back({ nm[k], &xv[0], &bins[i][0], nb*2, QwtSymbol::NoSymbol, QwtPlot::yLeft,    "Count",      ""     }); 
    ++i;
  }
  plot->configure(plotspecs);
  plot->replot();
  plot->show();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

