//  =================================================================
//
//  OptimLogoPlotItem.cpp
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
#include <algorithm>

#include <OptimLogoPlotItem.h>
#include <QPoint>
#include <QPrinter>
#include <QPainter>
#include <QwtPainter>
#include <QwtScaleMap>
#include <QwtPlot>



OptimLogoPlotItem::OptimLogoPlotItem (QPixmap const& pixmap, 
                                      QwtText const& text)
  : QwtPlotItem(text), pixmap_(pixmap)
{ setZ(100000.0); }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimLogoPlotItem::~OptimLogoPlotItem()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimLogoPlotItem::draw(QPainter* painter, QwtScaleMap const& xMap, 
                             QwtScaleMap const& yMap, QRectF const& canvasRect) const
{

   painter->save();
   
   double const height =  canvasRect.height();
   double const width  =  canvasRect.width();
   double const x0 =  canvasRect.x();
   double const y0  = canvasRect.y();

   double scale =  width/(7.5*pixmap_.width());  
   painter->drawPixmap(QRect(x0 + width  - scale*pixmap_.width(),
			     y0 + height - scale*pixmap_.height(),
			     scale*pixmap_.width(), 
			     scale*pixmap_.height()), pixmap_);

   painter->restore();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int OptimLogoPlotItem::rtti() const 
{
  return Rtti_PlotUserItem+1;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

