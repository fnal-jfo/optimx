//  =================================================================
//
//  SQLSeriesData.cpp
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

#include <QtDebug>
#include <iostream>
#include <sstream>
#include <SQLSeriesData.h>
#include <Globals.h>
#undef emit
#include <sqlite/connection.hpp>
#include <sqlite/query.hpp>
#include <sqlite/result.hpp>
#include <sqlite/ext/variant.hpp>
#include <sqlite/private/private_accessor.hpp>
#include <sqlite3.h>

// ......................................................
// SQLite supports multiple connections for reading only,
// SQLite Series data is for reading only; 
//
//........................................................

SQLSeriesData::SQLSeriesData( std::string const& dbfname, std::string const& sqlquery , void (*custom_functions)(sqlite::connection& con))

  : QwtSeriesData<QPointF>()
{
   sqlite::connection con(dbfname);
   int status = 0;
   if (custom_functions) {
     custom_functions(con); 
   }
   init(con, sqlquery);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

SQLSeriesData::SQLSeriesData(sqlite::connection& con, std::string const& sqlquery) 
  : QwtSeriesData<QPointF>()
{
  init(con, sqlquery);    // get the data
}
  
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

SQLSeriesData::~SQLSeriesData()
{}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void SQLSeriesData::init( sqlite::connection& con, std::string const& sqlquery)
{
  
   sqlite::query q(con, sqlquery);
   auto res = q.emit_result();

   //std::cout << sqlquery << std::endl; 

   sqlite::type tp[2];
   tp[0] = res->get_column_type(0);
   tp[1] = res->get_column_type(1);

   auto getval = [&res, &tp](int col) { using namespace sqlite;
                                        double result = 0.0;
                                        switch (tp[col]) {
                                         case integer:
					   result = double(res->get_int(col));
					   break;
                                          case real:
					   result = res->get_double(col);
					   break;
					  default:
					   result = 0.0;  
                                        };
					return result;
                                      };

   points_.push_back({getval(0), getval(1)}); 

   double xmin =  points_.back().x();
   double xmax =  xmin;
   double ymin =  points_.back().y();
   double ymax =  ymin;

   int k = 0;
   res->next_row();
   
   do {
     double x = getval(0);
     double y = getval(1);

     xmin = std::min(x,xmin);
     xmax = std::max(x,xmax);
     ymin = std::min(y,ymin);
     ymax = std::max(y,ymax);

     ++k;

     points_.push_back({x,y});
     
   } while(res->next_row()); 

   brect_ = QRectF( QPointF(xmin,ymin), QPointF(xmax,ymax)); 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QRectF  SQLSeriesData::boundingRect() const
{
  return brect_; 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

size_t  SQLSeriesData::size() const
{
  return points_.size();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QPointF SQLSeriesData::sample( size_t i ) const
{
  return QPointF( points_[i] );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

  // virtual void 	setRectOfInterest (const QRectF &rect)

