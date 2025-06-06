//  =================================================================
//
//  ScatterPlotItem.cpp
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

#include <ScatterPlotItem.h>
#include <ScatterData.h>
#include <iostream>
#include <algorithm>
#include <OptimUserRtti.h>
#include <QwtPlot>
#include <QwtPainter>
#include <QwtScaleMap>
#include <QPainter>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ScatterPlotItem::ScatterPlotItem ( std::shared_ptr<AbstractScatterData> data,
				   QwtText const& title)
  : data_(data), plane_(PlaneType::X_Y)
{
  setItemAttribute (AutoScale, true);  
  computeBoundingRect();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ScatterPlotItem::ScatterPlotItem (ScatterPlotItem const& s)
  : data_(s.data_), view_(s.view_), plane_(s.plane_)
{
  setItemAttribute (AutoScale, true);  
  computeBoundingRect();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ScatterPlotItem::~ScatterPlotItem()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ScatterPlotItem::setView( ViewType view )
{
  view_ = view;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ScatterPlotItem::setPlane( PlaneType plane )
{
  plane_ = plane; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ScatterPlotItem::draw(QPainter* painter, QwtScaleMap const& xMap, 
                           QwtScaleMap const& yMap, QRectF const& canvasRect) const
{
  if (!data_) return;
  
    painter->save();
    painter->setPen(Qt::blue);
    painter->setBrush(Qt::blue);

    int ix=0, iy=1;

    switch(plane_) {
    case PlaneType::X_Xp:
      ix = 0 ;
      iy = 1 ;
      break;
    case PlaneType::Y_Yp:
      ix = 2 ;
      iy = 3 ;
      break;
    case PlaneType::S_P:
      ix = 4 ;
      iy = 5 ;
      break;
    case PlaneType::X_Y:
    default:
      ix = 0 ;
      iy = 2 ;
    }

    // draw only the particles that are not lost  

    AbstractScatterData::State s = data_->start();
    while (!data_->done()) {
      if (s.lost) {
	data_->next();
	continue;
      };
      double ixc = xMap.transform(s.state[ix] );
      double iyc = yMap.transform(s.state[iy] );  
      QRectF rect(ixc-1.0, iyc-1.0, 2.0, 2.0);  // last 2 args: ellipse diameters 
      QwtPainter::drawEllipse( painter, rect);
      s = data_->next();
    };

    // done painting particles that are not lost ... 

    // Now, change pen/brush color and draw only the lost particles  

    painter->restore();

    painter->save();
    painter->setPen(Qt::red);
    painter->setBrush(Qt::red);
    
    s = data_->start();
    while (!data_->done()){
      if (!s.lost) {
	data_->next();
	continue;
      }
      int ixc = xMap.transform(s.state[ix]);
      int iyc = yMap.transform(s.state[iy]); 
      QRectF rect(ixc-1.0, iyc-1.0, 2.0, 2.0);   
      QwtPainter::drawEllipse( painter, rect);
      s = data_->next();
    }; 


    // done painting lost particles ... 
    painter->restore();
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ScatterPlotItem::drawSeries(QPainter* painter, QwtScaleMap const& xMap, 
				 QwtScaleMap const& yMap, QRectF const& canvasRect, int from, int to ) const
{
  return draw(painter, xMap, yMap, canvasRect);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int ScatterPlotItem::rtti() const 
{
  return Optim::Rtti_ScatterPlotItem;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ScatterPlotItem::computeBoundingRect ()
{
 auto lm = data_->limits(); 

 brect_xy_   = QRectF(lm.xmin, lm.ymin,    (lm.xmax-lm.xmin), (lm.ymax -lm.ymin)   );
 brect_xxp_  = QRectF(lm.xmin, lm.xpmin,   (lm.xmax-lm.xmin), (lm.xpmax-lm.xpmin)  );
 brect_yyp_  = QRectF(lm.ymin, lm.ypmin,   (lm.ymax-lm.ymin), (lm.ypmax-lm.ypmin)  );
 brect_sdpp_ = QRectF(lm.smin, lm.dppmin,  (lm.smax-lm.smin), (lm.dppmax-lm.dppmin));

}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QRectF ScatterPlotItem::boundingRect () const
{
  switch(plane_) {
  case PlaneType::X_Xp:
      return brect_xxp_;
      break;
  case PlaneType::Y_Yp:
      return brect_yyp_;
      break;
  case PlaneType::S_P:
      return brect_sdpp_;
      break;
  case PlaneType::X_Y:
  default:
      return brect_xy_;
  }
}

void    ScatterPlotItem::setRectOfInterest( QRectF const& )
{
  // pure abstract in QAbstractSeriesStore
  // not used 
  // std::cout <<   "ScatterPlotItem::bsetRectOfInterest" << std::endl;
}    
  
QRectF   ScatterPlotItem::dataRect() const
{
  // pure abstract in QAbstractSeriesStore
  std::cout << "ScatterPlotItem::dataRect" << std::endl;
  return boundingRect();
}


size_t   ScatterPlotItem::dataSize() const
{
  // pure abstract in QAbstractSeriesStore
  return data_->size();
}

ViewType  ScatterPlotItem::viewType() const
{
  return data_->view();
}
