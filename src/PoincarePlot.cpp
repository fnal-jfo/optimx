//  =================================================================
//
//  PoincarePlot.cpp
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

#include <PoincarePlot.h>
#include <FixedLimitsDialog.h>
#include <QEvent>
#include <QMouseEvent>
#include <QwtPlotGrid>
#include <QwtScaleWidget>
#include <QwtInterval>
#include <OptimPlotZoomer.h>
#include <PoincarePlotCanvas.h>
#include <QMdiSubWindow>
#include <iostream>
#include <cstdio>
#include <QPixmap>


namespace {
class ScaleEventFilter: public QObject { 

 public:

  ScaleEventFilter( PoincarePlot* parent, QwtPlot::Axis axisid) 
    : QObject(parent), plot_(parent), axisw_(plot_->axisWidget(axisid)),
					     axisid_(axisid)  {} 

  bool eventFilter(QObject *watched, QEvent* event)
  {
    if ( axisw_ ==  qobject_cast<QwtScaleWidget*>(watched) ) {

      if ( event->type() != QEvent::MouseButtonPress ) return false;
      QMouseEvent* me =  static_cast<QMouseEvent*>(event);
      if ( me->button() == Qt::RightButton ) { 
	//plot_->options();
	return true;
      }
      if ( me->button() == Qt::LeftButton ) { 
	plot_->fixedLimits(  axisid_ );
        return true;         
	
      }
    }
    return false; 
  }

  PoincarePlot*      plot_;
  QwtScaleWidget*    axisw_;
  QwtPlot::Axis      axisid_;
  PlotOptions        plot_opts_; 
};
} // anonymous namespace 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PoincarePlot::PoincarePlot( QWidget* parent)
  : QwtPlot(parent), grid_(0), pixmap_(nullptr)
{

 setCanvas( new PoincarePlotCanvas(this));  
 QPalette palette;
 palette.setColor(QPalette::Window, QColor("white")); 
 canvas()->setPalette(palette);  

 grid_ = new QwtPlotGrid();
 grid_->setMajorPen(QColor("gray"), 0.0, Qt::DotLine); 	
 grid_->setMinorPen(QColor("gray"), 0.0, Qt::DotLine);
 grid_->attach( this);

 // event filters for pop-ups windows to change axis limits

  QwtScaleWidget* w = axisWidget(QwtPlot::yLeft);
  w->installEventFilter( new ScaleEventFilter(this, QwtPlot::yLeft ) );

  w = axisWidget(QwtPlot::yRight);
  w->installEventFilter( new ScaleEventFilter(this, QwtPlot::yRight ) );

  w = axisWidget(QwtPlot::xBottom);
  w->installEventFilter( new ScaleEventFilter(this,  QwtPlot::xBottom) );

  zoomer_ = new OptimPlotZoomer( xBottom, yLeft, canvas(), true); 
  zoomer_->setMousePattern(QwtPlotZoomer::MouseSelect1,Qt::LeftButton);
  zoomer_->setEnabled(false);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PoincarePlot::drawCanvas(QPainter* painter)
{
    QwtPlot::drawCanvas(painter);
#if 0
    if (!fpu_) return; 
    rewind(fpu_.get()); // rewind the data file 
    auto hmap = canvasMap(QwtPlot::xBottom);
    auto vmap = canvasMap(QwtPlot::yLeft);
    // read the entire turn-by-turn file  
    int status = 0;
    do { 
      double x,xp,y,yp,s,dpp;
      status = fscanf(fpu_.get()," %lg %lg %lg %lg %lg %lg", &x, &xp, y, &yp, &s, &dpp); 
      if (status == EOF) break;
      int ix = hmap.transform(x);
      int iy = vmap.transform(y); 
      painter->drawPoint( ix,  iy);
    } while( status != EOF);
#endif
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QPixmap* PoincarePlot::initPixmap()
{
  auto cnv = canvas();
  if (!pixmap_) {
    pixmap_ = std::make_unique<QPixmap>(cnv->width(), cnv->height());
  }
  else if ( (pixmap_->width()  != cnv->width()) ||
	    (pixmap_->height() != cnv->height()) ) {
   pixmap_ = std::make_unique<QPixmap>(cnv->width(), cnv->height());
  }    

  pixmap_->fill(Qt::white);
  return pixmap_.get();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PoincarePlot::fixedLimits( QwtPlot::Axis axisid)
{

  FixedLimitsDialog* dialog = findChild<FixedLimitsDialog*>("FixedLimitsDialog");
  if (!dialog) {
    dialog = new FixedLimitsDialog(this);
    dialog->setObjectName("FixedLimitsDialog");
  };
  
 QwtInterval interval = axisInterval(axisid);

 dialog->data_.minimum = interval.minValue();
 dialog->data_.maximum = interval.maxValue();
 
 switch (axisid) {
   case xBottom: 
    dialog->setWindowTitle("Horizontal Axis");
    dialog->data_.active = true;
    dialog->data_.active_box_enabled = false;
    dialog->data_.zoom       = plot_opts_.horZoomOn;
    dialog->data_.autoscale  = plot_opts_.auto_hor;
   break;
   case yLeft:
     dialog->setWindowTitle("Left  Vertical Axis");
     dialog->data_.active = plot_opts_.leftActive;
     dialog->data_.active_box_enabled = true;
     dialog->data_.zoom   = plot_opts_.leftZoomOn;   
     dialog->data_.autoscale  =  plot_opts_.auto_left;
   break;
   case yRight:
    dialog->setWindowTitle("Right Vertical Axis");
     dialog->data_.active = plot_opts_.rightActive;
     dialog->data_.active_box_enabled = true;
     dialog->data_.zoom   = plot_opts_.rightZoomOn;   
     dialog->data_.autoscale  =  plot_opts_.auto_right;
   break;
 default:
   break;
 }
  
 dialog->set();

 std::function<void()> callback;

 switch (axisid) {

   case xBottom:
     callback = [this,dialog]() { 
     plot_opts_.horZoomOn = dialog->data_.zoom;
     plot_opts_.min_hor   = dialog->data_.minimum;
     plot_opts_.max_hor   = dialog->data_.maximum;
     plot_opts_.auto_hor  = dialog->data_.autoscale;
     setOptions();
     };
     break;
     
   case yLeft:
     callback = [this,dialog]() {
     plot_opts_.leftZoomOn   = dialog->data_.zoom;
     plot_opts_.leftActive   = dialog->data_.active;
     plot_opts_.rightActive  = !plot_opts_.leftActive;
     plot_opts_.min_left     = dialog->data_.minimum;
     plot_opts_.max_left     = dialog->data_.maximum;
     plot_opts_.auto_left    = dialog->data_.autoscale;
     setOptions();
     };
     break;

   case yRight:
     callback = [this,dialog]() {
       plot_opts_.rightZoomOn   = dialog->data_.zoom;
       plot_opts_.rightActive   = dialog->data_.active;
       plot_opts_.leftActive    = !plot_opts_.rightActive;
       plot_opts_.min_right     = dialog->data_.minimum;
       plot_opts_.max_right     = dialog->data_.maximum;
       plot_opts_.auto_right    = dialog->data_.autoscale;
       setOptions();
     };
    break;
  default:
    break;
 };

 dialog->setCallback(callback);
 dialog->show();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void PoincarePlot::setOptions()
{
  // Axis limits ... 

  if (plot_opts_.auto_hor) { 
    setAxisAutoScale ( xBottom,  plot_opts_.auto_hor);
    QwtInterval interval = axisInterval(xBottom); 	
    plot_opts_.min_hor     = interval.minValue(); 
    plot_opts_.max_hor     = interval.maxValue();
  }
  else {
    setAxisScale (xBottom,  plot_opts_.min_hor, plot_opts_.max_hor, 0);
  }

  if (plot_opts_.auto_right) { 
    setAxisAutoScale ( yRight,  plot_opts_.auto_right);
    QwtInterval interval = axisInterval(yRight); 	
    plot_opts_.min_right     = interval.minValue(); 
    plot_opts_.max_right     = interval.maxValue();
  }
  else {
    setAxisScale (yRight,  plot_opts_.min_right, plot_opts_.max_right, 0);
  }

  if (plot_opts_.auto_left) { 
    setAxisAutoScale ( yLeft,  plot_opts_.auto_left);
    QwtInterval interval = axisInterval(yLeft); 	
    plot_opts_.min_left     = interval.minValue(); 
    plot_opts_.max_left     = interval.maxValue();
  }
  else {
    setAxisScale (yLeft,  plot_opts_.min_left, plot_opts_.max_left, 0);
  }

  // Zooming ...  

  zoomer_->setEnabled(plot_opts_.leftActive  && (plot_opts_.leftZoomOn  || plot_opts_.horZoomOn) );   // left zoom en/dis-abled 

  zoomer_->setConstrainedAxis( xBottom, !plot_opts_.horZoomOn);
  zoomer_->setConstrainedAxis( yLeft,   !plot_opts_.leftZoomOn);

  if (zoomer_) zoomer_->setZoomBase();
 

  replot();
}
