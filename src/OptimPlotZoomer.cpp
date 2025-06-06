//  =================================================================
//
//  OptimPlotZoomer.cpp
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
#include <OptimPlotZoomer.h>

#include <QwtPlot> // for QwtPlot:Axes
#include <QwtPickerMachine>
#include <QwtPainter>

#include <QMouseEvent>
#include <QPoint>
#include <QPolygon>
#include <QRect>
#include <QTimer>
#include <QCoreApplication>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimPlotZoomer::OptimPlotZoomer(QWidget* w, bool doReplot)
  : QwtPlotZoomer(w,doReplot)
{
   constraints_[QwtPlot::xBottom] = false;
   constraints_[QwtPlot::xTop]    = false;
   constraints_[QwtPlot::yLeft]   = false;
   constraints_[QwtPlot::yRight]  = false;

   setMousePattern(QwtEventPattern::MouseSelect2, Qt::NoButton );
   setRubberBand(UserRubberBand);
   setTrackerMode(AlwaysOn);
   timer_ = new QTimer(this);
   timer_->setInterval(100);
   timer_->setSingleShot(true);
   // connect(timer_, &QTimer::timeout, [this]()
   //  { std::cout << " timeout " << std::endl; QCoreApplication::postEvent(parentWidget()->parentWidget(), e_ );}); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimPlotZoomer::OptimPlotZoomer(int xAxis, int yAxis, QWidget* w, bool doReplot)
  : QwtPlotZoomer(xAxis, yAxis, w, doReplot)
{
  constraints_[QwtPlot::xBottom] = false;
  constraints_[QwtPlot::xTop]    = false;
  constraints_[QwtPlot::yLeft]   = false;
  constraints_[QwtPlot::yRight]  = false;

  setMousePattern(QwtEventPattern::MouseSelect2, Qt::NoButton );
  setRubberBand(UserRubberBand);
  setTrackerMode(AlwaysOn);
  timer_ = new QTimer(this);
  timer_->setInterval(100);
  timer_->setSingleShot(true);
  // connect(timer_, &QTimer::timeout, 
  //  [this](){ std::cout << " timeout " << std::endl; QCoreApplication::postEvent(parentWidget()->parentWidget(), e_ );}); 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimPlotZoomer::~OptimPlotZoomer() {}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlotZoomer::widgetMouseDoubleClickEvent (QMouseEvent* event)
{
   if ( ( event->button()    == Qt::RightButton ) &&
	( event->modifiers() == Qt::NoModifier) ) {
    zoom(0);
    event->accept();
   }
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlotZoomer::widgetMousePressEvent (QMouseEvent* event)
{
  //std::cerr <<  "OptimPlotZoomer::widgetMousePressEvent" << std::endl;
  QwtPlotZoomer::widgetMousePressEvent(event);
  event->ignore();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  OptimPlotZoomer::zoom(QRectF const& rect)
{

  // constrain zoom function   
  
  QRectF brect(zoomBase());  // original zoom base rect
  QRectF crect(rect);        // constrained rect 

  if (constraints_[xAxis()] ) {
     crect.setLeft(brect.left());
     crect.setRight(brect.right());
  }
  if (constraints_[yAxis()] ) {
     crect.setTop(brect.top());
     crect.setBottom(brect.bottom());
  }
  
  QwtPlotZoomer::zoom(crect);
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  OptimPlotZoomer::zoom (int up)
{
   QwtPlotZoomer::zoom(up);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlotZoomer::setConstrainedAxis(int axis, bool set)
{
  constraints_[axis] = set; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool OptimPlotZoomer::isConstrainedAxis(int axis)
{
  return constraints_[axis];
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlotZoomer::drawRubberBand(QPainter * painter ) const
{
  // UserRubberBand is a customized rubber band
  // that uses a shaded rectangular region selection;

  static QColor color(Qt::gray);
  color.setAlpha(30);   
    
  if ( !isActive() || rubberBand() == NoRubberBand ||
        rubberBandPen().style() == Qt::NoPen )
   {
        return;
   }

   if ( rubberBand() == UserRubberBand ) {

     QPolygon pa = selection();

     QwtPickerMachine::SelectionType selectionType =
         QwtPickerMachine::NoSelection;

     if ( stateMachine() )
      selectionType = stateMachine()->selectionType();

     switch ( selectionType )
     {
      case QwtPickerMachine::RectSelection:
      { if ( pa.count() < 2 ) return;

	 QRect const rect = QRect( pa.first(), pa.last());
	 QwtPainter::drawRect( painter, rect );
	 QwtPainter::fillRect( painter, rect, color);

      }
      default:
            break;
     }
    
   }

   else {
     QwtPlotZoomer::drawRubberBand(painter);
   }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QPolygon OptimPlotZoomer::adjustedPoints( QPolygon const& points) const
{
  // adjust the selection for a constrained vertical zoom


  // determine if the zoom axis is active and constrained
  
  bool vcons = ( (constraints_[QwtPlot::yLeft]  && ( yAxis() == QwtPlot::yLeft))  ||
		 (constraints_[QwtPlot::yRight] && ( yAxis() == QwtPlot::yRight)) );

  bool hcons = ( (constraints_[QwtPlot::xBottom]  && (xAxis() == QwtPlot::xBottom))  ||
		 (constraints_[QwtPlot::xTop]     && (xAxis() == QwtPlot::xTop)) );

  // if there is no constraint, call base class version 

  if ( !(vcons || hcons) ) {
    return QwtPlotZoomer::adjustedPoints(points);
  }

  QPolygon adjusted;

  if ( points.size() == 2 )
    {
        int const width  = hcons ? canvas()->width()  : qAbs(points[1].x() - points[0].x());
        int const height = vcons ? canvas()->height() : qAbs(points[1].y() - points[0].y());

	int x0 =  hcons ? 0                  :  qMin(points[0].x(), points[1].x());
        int y0 =  vcons ? 0                  :  qMin(points[0].y(), points[1].y());

	QRect rect(x0, y0, qAbs(width), qAbs(height));

        adjusted += rect.topLeft();
        adjusted += rect.bottomRight();
    }
  return adjusted;

}

