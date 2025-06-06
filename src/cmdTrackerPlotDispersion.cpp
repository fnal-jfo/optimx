//  =================================================================
//
//  cmdTrackerPlotDispersion.cpp
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
#include <sstream>
#include <omp.h>
#include <memory>

#include <Constants.h>
#include <BeamMoments.h>
#include <Globals.h>
#include <Element.h>
#include <RMatrix.h>
#include <OptimMainWindow.h>
#include <OptimMdiArea.h>
#include <OptimMdiSubWindow.h>
#include <OptimMessages.h>
#include <OptimPlot.h>
#include <OptimTrackerNew.h>
#include <TrackerParameters.h>
#include <OptimUserRtti.h>
#include <Structs.h>
#include <SQLSeriesData.h>
#include <TrackerPlot.h>
#include <TrackParam.h>
#include <Twiss.h>
#include <Utility.h>
#include <QMdiArea>

#include <fmt/format.h>
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

#define LSTR 1024
 
// .................................................................
// loss_  array :  0 (no loss); 1(ampl. > 10 m); 2(lost on aperture) 
// .................................................................


void OptimTrackerNew::cmdTrackerPlotDispersion()
{

  sqlite::connection& con = *Globals::preferences().con;

  int nturns = -1;
  {
    sqlite::query q( con,   "SELECT max(turn) FROM Moments;");
    auto res = q.emit_result();
    nturns = res->get_int(0);
  }

  std::vector<LegoData> legodata;
  double L = 0.0;

  if (dataspec_ == TrackerParameters::all) { 
    legodata.push_back( { 0.0, 0.0, 1 , std::string("START") } );

    for(int i=0; i <mainw_->beamline_.size(); ++i) {
      auto ep = mainw_->beamline_[i];
      legodata.push_back( { L*0.01, ep->length()*0.01, ((ep->G>=0 ) ? 1 :-1) , ep->fullName() });
      L     +=  ep->length();
    }

    legodata.push_back( { L*0.01,  0,  1,   std::string("END") } ); 
  }
  
  DBSpecs dbspecs;
  dbspecs.dbfile =  "moments.db"; 
  std::string q;
  
  if  (dataspec_ == TrackerParameters::all) {
    q = fmt::format("SELECT pathlen, 0.01*avg(extract_cov({:1d}, {:1d}, covariance)/extract_cov({:1d}, {:1d}, covariance))  FROM Moments GROUP BY eidx ORDER BY eidx;",0,5,5,5);
    dbspecs.queries.push_back(q);
    q = fmt::format("SELECT pathlen, 0.01*avg(extract_cov({:1d}, {:1d}, covariance)/extract_cov({:1d}, {:1d}, covariance))  FROM Moments GROUP BY eidx ORDER BY eidx;",2,5,5,5);
    dbspecs.queries.push_back(q);
    q = fmt::format("SELECT pathlen, avg(extract_cov({:1d}, {:1d}, covariance)/extract_cov({:1d}, {:1d}, covariance)) FROM Moments GROUP BY eidx ORDER BY eidx;" ,1,5,5,5);
    dbspecs.queries.push_back(q);
    q = fmt::format("SELECT pathlen, avg(extract_cov({:1d}, {:1d}, covariance)/extract_cov({:1d}, {:1d}, covariance)) FROM Moments GROUP BY eidx ORDER BY eidx;" ,3,5,5,5);
    dbspecs.queries.push_back(q);

  }				
  else {
        dbspecs.queries = 
                    { "SELECT turn, 0.01*extract_lf(0, 0, mode1,mode2) FROM Moments WHERE (eidx==0) ORDER BY turn;",
		      "SELECT turn, 0.01*extract_lf(1, 1, mode1,mode2) FROM Moments WHERE (eidx==0) ORDER BY turn;",
		      "SELECT turn, 0.01*extract_lf(0, 1, mode1,mode2) FROM Moments WHERE (eidx==0) ORDER BY turn;",
		      "SELECT turn, 0.01*extract_lf(1, 0, mode1,mode2) FROM Moments WHERE (eidx==0) ORDER BY turn;"
		    };
  }
       
  PlotSpec plotspecs;
  plotspecs.title     = "Tracker: Dispersion ";
  plotspecs.bottom_title = "S [m] or Turn No";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n   QwtSymbol::Style  axis          vertical_title  color 
  //------------------------------------------------------------------------------------------------------------------------------------  
  curvespecs.push_back({ "etax",     0,  0,   0, QwtSymbol::Rect, QwtPlot::yLeft,   "etax,  etay  [m]",  ""});
  curvespecs.push_back({ "etay" ,    0,  0,   0, QwtSymbol::Rect, QwtPlot::yLeft,   "etax,  etay  [m]",  ""});
  curvespecs.push_back({ "etaxp",    0,  0,   0, QwtSymbol::Rect, QwtPlot::yRight,  "etaxp, etayp ",  ""});  
  curvespecs.push_back({ "etayp",    0,  0,   0, QwtSymbol::Rect, QwtPlot::yRight,  "etaxp, etayp ",  ""});  

  

  auto  plot = mainw_->addPlot(WindowId::TrackerDispPlt, plotspecs, legodata, dbspecs);
  plot->show();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

