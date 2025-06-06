//  =================================================================
//
//  OptimPlotZoomer.h
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

#ifndef OPTIMPLOTZOOMER_H
#define OPTIMPLOTZOOMER_H

#include <QwtPlotZoomer>
#include <QMap>

class QTimer;
class QMouseEvent;

class OptimPlotZoomer: public QwtPlotZoomer {

Q_OBJECT
  
public:

  OptimPlotZoomer (QWidget* w, bool doReplot=true );
  OptimPlotZoomer (int xAxis, int yAxis, QWidget* w, bool doReplot=true);
 ~OptimPlotZoomer();

  void zoom (QRectF const&);
  void zoom (int up);
  void setConstrainedAxis(int axis, bool set);
  bool isConstrainedAxis(int axis);
 
 protected:
 
  QPolygon adjustedPoints( QPolygon const& points) const;
  void widgetMouseDoubleClickEvent (QMouseEvent *);
  void widgetMousePressEvent (QMouseEvent* event);
  void drawRubberBand (QPainter * painter ) const; 

  QMap<int, bool> constraints_;

 private:

  QTimer*       timer_;
  QMouseEvent*  e_;
};

#endif //OPTIMPLOTZOOMER_H
