//  =================================================================
//
//  cmdTrackerPlotEigenEmittances.cpp
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
#include <sqlite/query.hpp>
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
#include <OptimTrackerNew.h>
#include <TrackerParameters.h>
#include <OptimUserRtti.h>
#include <Structs.h>
#include <TrackerPlot.h>
#include <TrackParam.h>
#include <Twiss.h>
#include <Utility.h>

#include <QApplication>
#include <QMdiArea>

#include <fmt/format.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

#include <spdlog/spdlog.h>

using Constants::PI;
using Constants::C_DERV1;


void OptimTrackerNew::cmdTrackerPlotEigenEmittances()
{

  sqlite::connection& con = *Globals::preferences().con;
  
  int nturns = 0;
  {
    sqlite::query q( con, "SELECT max(turn) FROM Moments;");
    auto res = q.emit_result();
    nturns = res->get_int(0);
  }

  std::vector<LegoData> legodata;

  if (dataspec_ == TrackerParameters::all) { 
    double L = 0.0;
    legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 

    for(int i=0; i <mainw_->beamline_.size(); ++i) {
      auto ep = mainw_->beamline_[i];
      legodata.push_back( { L*0.01, ep->length()*0.01, ((ep->G>=0 ) ? 1 :-1) , ep->fullName() });
      L     +=  ep->length();
    }
    legodata.push_back( { L*0.01,  0,  1,   std::string("END") } ); 
  }

  std::list<std::string> qlist;

  PlotSpec plotspecs;
  plotspecs.title     = "Tracker: EigenEmittances Evolution ";
  plotspecs.bottom_title = "S [m] or Turn No";
  auto& curvespecs = plotspecs.curvespecs;

  DBSpecs dbspecs;
  std::string q; 
  
  if (dataspec_ == TrackerParameters::all) {
    qlist.push_back("SELECT pathlen, avg(eps1) FROM Moments GROUP by eidx ORDER by eidx;");
      curvespecs.push_back( {"eps1",  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,  "eps1 [cm-rad]",  ""} );
    qlist.push_back("SELECT pathlen, avg(eps2) FROM Moments GROUP by eidx ORDER by eidx;");
      curvespecs.push_back( {"eps2",  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,  "eps2 [cm-rad]",  ""} );
      //qlist.push_back("SELECT pathlen, max(extract_dvec(0, umax)) FROM Moments GROUP by eidx ORDER by eidx;");
      // curvespecs.push_back( {"Xmax",  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yRight,  "eps [cm-rad]",  ""} );
      //qlist.push_back("SELECT pathlen, max(extract_dvec(2, umax)) FROM Moments GROUP by eidx ORDER by eidx;");
      //curvespecs.push_back( {"Ymax",  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yRight,  "eps [cm-rad]",  ""} );
  }
  else {
    for (auto it=elm_selection_.begin(); it != elm_selection_.end(); ++it) {

      std::string sidx =  "-["+ std::to_string((*it).first) +"]";
      q = fmt::format("SELECT turn, eps1 FROM Moments WHERE (eidx=={:d}) ORDER BY turn;", (*it).first);
      qlist.push_back(q);
      curvespecs.push_back( {(std::string("eps1") + "-" + (*it).second + sidx).c_str(),  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,  "eps1 [cm-rad]",  ""} );

      q = fmt::format("SELECT turn, eps2 FROM Moments WHERE (eidx=={:d}) ORDER BY turn;", (*it).first);
      qlist.push_back(q);
      curvespecs.push_back( {(std::string("eps2") + "-" + (*it).second +sidx).c_str(),  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,  "eps2 [cm-rad]",  ""} );
 
      //      q = fmt::format("SELECT turn, extract_dvec(0, umax) FROM Moments WHERE (eidx=={:d})  ORDER BY turn;", (*it).first);
      //qlist.push_back(q);
      //curvespecs.push_back( {(std::string("Xmax") + "-" + (*it).second +sidx).c_str(),  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yRight,  "Xmax or Ymax [cm]",  ""} );

      //q = fmt::format("SELECT turn, extract_dvec(1, umax) FROM Moments WHERE (eidx=={:d})  ORDER BY turn;", (*it).first);
      //qlist.push_back(q);
      //curvespecs.push_back( {(std::string("Ymax") + "-" + (*it).second +sidx).c_str(),  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yRight,  "Xmax or Ymax [cm]",  ""} );
    }
  }
    
  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n   QwtSymbol::Style  axis          vertical_title  color 
  //------------------------------------------------------------------------------------------------------------------------------------  

  //curvespecs.push_back({ "eps1",        0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,          "eps [cm-rad]",  0});
  //curvespecs.push_back({ "eps2",        0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,          "eps [cm-rad]",  0});
  //curvespecs.push_back({ "xmax",        0,  0,   0, QwtSymbol::Rect,  QwtPlot::yRight,         "Xmax [cm]",  0});
  //curvespecs.push_back({ "ymax",        0,  0,   0, QwtSymbol::Rect,  QwtPlot::yRight,         "Ymax [cm]",  0});

  
   dbspecs.dbfile            = "moments.db"; 
   dbspecs.init_custom_fncts = 0;
   //dbspecs.init_custom_fncts = &BeamMoments::initDBCustomFunctions;

   for ( auto const& q: qlist) {
    dbspecs.queries.push_back(q);  
   }

   // debugging
   auto dbgout = [&curvespecs] () {
                           std::string str;  
                           for ( auto const& cv : curvespecs) {
			      str += fmt::format("{:s}\n",  cv.legendname );
			   }
			   return str;
		       };
       
   auto optimx_logger =  spdlog::get("optimx_logger");
   SPDLOG_LOGGER_INFO( optimx_logger, "List of curvespec labels: \n" + dbgout()); 

   auto  plot = mainw_->addPlot(WindowId::Tracker4DEpsPlt, plotspecs, legodata, dbspecs);
   plot->show();

}  
