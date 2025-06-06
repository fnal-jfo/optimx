//  =================================================================
//
//  cmdTrackerPlotEmittances.cpp
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
#include <Constants.h>
#include <BeamMoments.h>
#include <Globals.h>
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
#include <Structs.h>
#include <TrackingParametersDialog.h>
#include <TrackParam.h>
#include <Twiss.h>
#include <Utility.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QFileDialog>
#include <QMdiArea>

#include <fmt/format.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

using Utility::gauss;
using Utility::decodeExtLine;
using Utility::strcmpr;
using Utility::filterName;
using Utility::Filter7;

using Constants::PI;
using Constants::C_DERV1;



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::cmdPlotEmittance() // V7
{
  int n=0; 

 
  OptimPlot*     plot   = 0;
  auto TrackerEmittancePlt     = mainw_->getAttachedSubWin(WindowId::TrackerEmittancePlt);

  if (!TrackerEmittancePlt) { 
    TrackerEmittancePlt = mainw_->createAttachedSubWin( (plot = new OptimPlot(mainw_) ), WindowId::TrackerEmittancePlt);
  }
  else {
    plot = qobject_cast<OptimPlot*>( TrackerEmittancePlt->widget() );
    if (mainw_->CtSt_.ClearPlot) { 
      plot->clear();
    }
  }
  
  TrackerEmittancePlt->raise();
  
  double xmax;

  if(nturn_ == 1 ){
    xmax = mainw_->Length_*0.01; // is this necessary 
    if( view_at_elem_ > 0 && view_at_elem_ < mainw_->nelm_) { 
      n = view_at_elem_ + 1;
    } 
    else { 
      n = mainw_->nelm_+1;
    }
  } 
  else {
    xmax   = nturn_;
    n =nturn_+1;
  }

  double yminL = 0.0;
  double ymaxL = 0.0;

  double yminR = 0.0;
  double ymaxR = 0.0;
  
  if( history_[0][0].emitX() > history_[0][0].emitY()) ymaxL = 2.*history_[0][0].emitX();
  else ymaxL = 2.0*history_[0][0].emitY();

  if( history_[0][0].Xmax() > history_[0][0].Ymax() )  ymaxR = 2.*history_[0][0].Xmax();
  else ymaxL = 2.*history_[0][0].Ymax();
   
  std::vector<double>   xv (n);
  std::vector<double>   exv(n); 
  std::vector<double>   eyv(n); 
  std::vector<double> xmaxv(n); 
  std::vector<double> ymaxv(n); 

  for (int i=0; i<n; ++i) { 
    exv[i]   = history_[0][i].emitX();
    eyv[i]   = history_[0][i].emitY();
    xmaxv[i] = history_[0][i].Xmax();
    ymaxv[i] = history_[0][i].Ymax();
    xv[i]    = (nturn_==1) ? history_[0][i].s : i;
  }


  PlotSpec plotspecs;
  plotspecs.title     = "Tracker: Beam Emittance & Beam Size";
  plotspecs.bottom_title = "S [m] or Turn No";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n   QwtSymbol::Style  axis  title   bottom_title         vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  
  curvespecs.push_back({ "epsX",    &xv[0],  &exv[0],   n, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Emittance [cm-rad]",    ""     });
  curvespecs.push_back({ "epsY",    &xv[0],  &eyv[0],   n, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Emittance [cm-rad]",    ""     });
  curvespecs.push_back({ "Xmax",    &xv[0],  &xmaxv[0], n, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Xmax & Ymax [cm]",      ""     });  
  curvespecs.push_back({ "Ymax",    &xv[0],  &ymaxv[0], n, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Xmax & Ymax [cm]",      ""     });  

  plot->configure(plotspecs); 
  plot->replot(); 
  plot->show(); 
 }
 


