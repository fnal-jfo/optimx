//  =================================================================
//
//  OptimPlotMarker.h
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

#ifndef OPTIMPLOTMARKER_H
#define OPTIMPLOTMARKER_H

#include <QwtPlotMarker>


class OptimPlotMarker:public QwtPlotMarker {

 public:

  struct ResonanceData { int i; int j; int k; double angle;};

  OptimPlotMarker(ResonanceData const& resdata, QString const &title=QString());
  OptimPlotMarker(ResonanceData const& resdata, QwtText const& title);
 
  void 	draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &) const;
  void  updateScaleDiv(QwtScaleDiv const& xScaleDiv, QwtScaleDiv const& yScaleDiv); 

 private:

  std::pair<double,double> visibleRange(  ResonanceData const& rdata, double xmin, double xmax, double ymin, double ymax); // determine the range of x for the visible part of the line.   

  void drawLabel( QPainter *painter, QRectF const& canvasRect,
		  QPointF  const& pos ) const;

  ResonanceData rdata_; // angle w/r to horizontal [in degrees]
};



#endif //OPTIMPLOTMARKER_H
