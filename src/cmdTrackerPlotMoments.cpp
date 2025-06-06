//  =================================================================
//
//  cmdTrackerPlotMoments.cpp
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

#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <omp.h>

#include <memory>
#include <string>
#include <Constants.h>
#include <BeamMoments.h>
#include <Globals.h>
#include <Element.h>
#include <RMatrix.h>
#include <OptimMainWindow.h>
#include <MomentsSelectionDialog.h>
#include <OptimMessages.h>
#include <OptimPlot.h>
#include <OptimTrackerNew.h>
#include <TrackerParameters.h>
#include <OptimUserRtti.h>
#include <SQLSeriesData.h>
#include <TrackParam.h>
#include <Twiss.h>
#include <Utility.h>
//#include <QMdiArea>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>
#include <QwtPlotCurve>

#undef emit
#include <sqlite/execute.hpp>
#include <sqlite/private/private_accessor.hpp>
#include <sqlite/query.hpp>
#include <sqlite3.h>

using Constants::PI;
using Constants::C_DERV1;


void OptimTrackerNew::cmdTrackerPlotMoments()
{

  sqlite::connection& con = *Globals::preferences().con;
  
  static MomentsSelectionDialog* dialog = 0;

  if (!dialog) {
    dialog = new MomentsSelectionDialog();
  }  

  if ( dialog->exec() == QDialog::Rejected ) return;


  auto selection = dialog->data_; // selected moments;
  
  int nturns = -1;
  { sqlite::query q( con,   "SELECT max(turn) FROM Moments;");
    auto res = q.emit_result();
    nturns = res->get_int(0);
  }

  std::string labels[6][6];
  for (int i=0; i<6; ++i) {
     for (int j=0; j<=i; ++j) {
       labels[i][j] = labels[j][i] = fmt::format("M_{:1d}{:1d}",i,j);
     }
  }

  std::vector<LegoData> legodata;
  double L = 0.0;

  if ( dataspec_ == TrackerParameters::all ) { 
    legodata.push_back( { 0.0, 0.0, 1 , std::string("START") } );

    for(int i=0; i <mainw_->beamline_.size(); ++i) {
      auto ep = mainw_->beamline_[i];
      legodata.push_back( { L*0.01, ep->length()*0.01, ((ep->G>=0 ) ? 1 :-1) , ep->fullName() });
      L     +=  ep->length();
    }
  
    legodata.push_back( { L*0.01,  0,  1,   std::string("END") } ); 
  }

  std::list<std::string> qlist;

  PlotSpec plotspecs;
  plotspecs.title     = "Tracker: Second Order Moments";
  plotspecs.bottom_title = (dataspec_ == TrackerParameters::all) ? "S [m]" : "Turn No";

  auto& curvespecs = plotspecs.curvespecs;

  for (int i=0; i<6; ++i) { 
    for (int j=0; j<i+1; ++j) {

      if ( !selection[i][j] ) continue; // not selected 
      std::string q;
      std::string sel;
      
      if (dataspec_ == TrackerParameters::all) {  
         // moments are stored in cm.
	 q = fmt::format("SELECT pathlen, avg(extract_cov({:1d}, {:1d}, covariance)) FROM Moments GROUP BY eidx ORDER BY eidx;",i,j);
	 curvespecs.push_back( {labels[i][j].c_str(),  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,  "magnitude [cm**2, cm or none]",  ""} );
         qlist.insert(qlist.end(), q);
      }
      else {
	std::string sel;
	for (auto it=elm_selection_.s.begin(); it != elm_selection_.s.end(); ++it) {
	  q = fmt::format("SELECT turn,  extract_cov({:1d}, {:1d}, covariance) FROM Moments WHERE (eidx=={:d}) ORDER BY turn;", i,j, (*it).first);
          curvespecs.push_back( {(labels[i][j] + std::string("-") + (*it).second + std::string("-[") + std::to_string((*it).first) +"]").c_str(),  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,  "magnitude [cm**2, cm or none]", ""} );
          qlist.insert(qlist.end(), q);
       }
     }
   }
  }
  DBSpecs dbspecs;
  
  dbspecs.dbfile            = "moments.db"; 
  dbspecs.init_custom_fncts = 0;
  //dbspecs.init_custom_fncts = &BeamMoments::initDBCustomFunctions;

  for ( auto const& q: qlist) {
    dbspecs.queries.push_back(q);  
  }
				
  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n   QwtSymbol::Style  axis          vertical_title  color 
  //------------------------------------------------------------------------------------------------------------------------------------  
  //curvespecs.push_back({ "sigma**2_xx",    0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,     "rms size [mm]",  0});
  //curvespecs.push_back({ "sigma**2_yy",    0,  0,   0, QwtSymbol::Rect,  QwtPlot::yRight,    "rms size [mm]",  0});

  auto  plot = mainw_->addPlot(WindowId::TrackerMomentsPlt, plotspecs, legodata, dbspecs);
  plot->show();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTrackerNew::cmdTrackerPlotCorrelations()
{

  sqlite::connection& con = *Globals::preferences().con;
  
  static MomentsSelectionDialog* dialog = 0;

  if (!dialog) {
    dialog = new MomentsSelectionDialog();
  }  

  if ( dialog->exec() == QDialog::Rejected ) return;


  auto selection = dialog->data_; // selected moments;
  
  int nturns = -1;
  { sqlite::query q( con,   "SELECT max(turn) FROM Moments;");
    auto res = q.emit_result();
    nturns = res->get_int(0);
  }

  std::string labels[6][6];
  for (int i=0; i<6; ++i) {
     for (int j=0; j<=i; ++j) {
       labels[i][j] = labels[j][i] = fmt::format("C_{:1d}{:1d}",i,j);
     }
  }

  std::vector<LegoData> legodata;
  double L = 0.0;

  if ( dataspec_ == TrackerParameters::all ) { 
    legodata.push_back( { 0.0, 0.0, 1 , std::string("START") } );

    for(int i=0; i <mainw_->beamline_.size(); ++i) {
      auto ep = mainw_->beamline_[i];
      legodata.push_back( { L*0.01, ep->length()*0.01, ((ep->G>=0 ) ? 1 :-1) , ep->fullName() });
      L     +=  ep->length();
    }
  
    legodata.push_back( { L*0.01,  0,  1,   std::string("END") } ); 
  }

  std::list<std::string> qlist;

  PlotSpec plotspecs;
  plotspecs.title     = "Tracker: Correlations";
  plotspecs.bottom_title = (dataspec_ == TrackerParameters::all) ? "S [m]" : "Turn No";

  auto& curvespecs = plotspecs.curvespecs;

  for (int i=0; i<6; ++i) { 
    for (int j=0; j<i+1; ++j) {

      if ( !selection[i][j] ) continue; // not selected 
      std::string q;
      std::string sel;
      
      if (dataspec_ == TrackerParameters::all) {  
         // moments are stored in cm.
	 q = fmt::format("SELECT pathlen, avg(extract_cor({:1d}, {:1d}, covariance)) FROM Moments GROUP BY eidx ORDER BY eidx;",i,j);
         curvespecs.push_back( {labels[i][j].c_str(),  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,  "correlation",  ""} );
         qlist.insert(qlist.end(), q);
      }
      else {
	std::string sel;
	for (auto it=elm_selection_.s.begin(); it != elm_selection_.s.end(); ++it) {
	  q = fmt::format("SELECT turn,  extract_cor({:1d}, {:1d}, covariance) FROM Moments WHERE (eidx=={:d}) ORDER BY turn;", i,j, (*it).first);
          curvespecs.push_back( {(labels[i][j] + std::string("-") + (*it).second + std::string("-[") + std::to_string((*it).first) +"]").c_str(),  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,  "correlation",  ""} );
          qlist.insert(qlist.end(), q);
       }
     }
   }
  }
  DBSpecs dbspecs;
  
  dbspecs.dbfile            = "moments.db"; 
  dbspecs.init_custom_fncts = 0;
  //dbspecs.init_custom_fncts = &BeamMoments::initDBCustomFunctions;

  for ( auto const& q: qlist) {
    dbspecs.queries.push_back(q);  
  }
				

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n   QwtSymbol::Style  axis          vertical_title  color 
  //------------------------------------------------------------------------------------------------------------------------------------  
  //curvespecs.push_back({ "sigma**2_xx",    0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,     "rms size [mm]",  0});
  //curvespecs.push_back({ "sigma**2_yy",    0,  0,   0, QwtSymbol::Rect,  QwtPlot::yRight,    "rms size [mm]",  0});

  auto  plot = mainw_->addPlot(WindowId::TrackerCorrelationsPlt, plotspecs, legodata, dbspecs);
  plot->show();
}
