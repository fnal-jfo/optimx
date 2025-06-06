//  =================================================================
//
//  cmdTrackerPlotIntensity.cpp
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

#include <Constants.h>
#include <BeamMoments.h>
#include <Globals.h>
#include <Element.h>
#include <RMatrix.h>
#include <LegoData.h>
#include <OptimMainWindow.h>
#include <OptimMdiArea.h>
#include <OptimMdiSubWindow.h>
#include <OptimMessages.h>
#include <OptimPlot.h>
#include <OptimTrackerNew.h>
#include <TrackerParameters.h>
#include <OptimUserRtti.h>
#include <RMatrix.h>
#include <ScatterData.h>
#include <ScatterPlotItem.h>
#include <Losses.h>
#include <Structs.h>
#include <SQLSeriesData.h>
#include <TrackParam.h>
#include <Twiss.h>
#include <Utility.h>
#include <QMdiArea>
#include <fmt/format.h>
#include <vector>


#undef emit
#include <sqlite/execute.hpp>
#include <sqlite/private/private_accessor.hpp>
#include <sqlite/query.hpp>
#include <sqlite3.h>

using Constants::PI;
using Constants::C_DERV1;

void OptimTrackerNew::cmdTrackerPlotIntensity()
{

  std::vector<LegoData> legodata;
  double   L = 0.0;

  if ( dataspec_ == TrackerParameters::all ) { 
    legodata.push_back( { 0.0, 0.0, 1 , std::string("START") } );

    for(int i=0; i <mainw_->beamline_.size(); ++i) {
      auto ep = mainw_->beamline_[i];
      legodata.push_back( { L*0.01, ep->length()*0.01, ((ep->G>=0 ) ? 1 :-1) , ep->fullName() });
      L     +=  ep->length();
    }
  
    legodata.push_back( { L*0.01,  0,  1,   std::string("END") } ); 
  }

  std::string sql;

  
  PlotSpec plotspecs;
  plotspecs.title     = "Tracker: Particle Losses";
  plotspecs.bottom_title = (dataspec_ == TrackerParameters::all) ? "S [m]": "Turn no";
  auto& curvespecs = plotspecs.curvespecs;

  std::list<std::string> qlist;

  if ( dataspec_ == TrackerParameters::all ) {
    sql = std::string("SELECT pathlen, avg(nlost) FROM Moments GROUP BY eidx ORDER BY eidx;");
    curvespecs.push_back( {"NLost",  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,  "NLost/turn",  ""} );
  }
  else {
    sql = fmt::format("SELECT turn, nlost FROM Moments WHERE (eidx ==0) ORDER BY turn;");
    curvespecs.push_back( {"NLost",  0,  0,   0, QwtSymbol::Rect,  QwtPlot::yLeft,  "NLost",  ""} );
  }

  qlist.insert(qlist.end(), sql);
 
  DBSpecs dbspecs;
  
  dbspecs.dbfile            = "./moments.db"; 
  dbspecs.init_custom_fncts = 0;

  for ( auto const& q: qlist) {
    dbspecs.queries.push_back(sql);  
  }

  auto  plot = mainw_->addPlot(WindowId::TrackerIntensityPlt, plotspecs, legodata, dbspecs);
  plot->show();

  }
