//  =================================================================
//
//  TrackerPlot.cpp
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

#include <Globals.h>
#include <TrackerPlot.h>
#include <FixedLimitsDialog.h>
#include <QEvent>
#include <QMouseEvent>
#include <QwtPlotGrid>
#include <QwtScaleWidget>
#include <QwtPlotScaleItem>
#include <QwtPlotLayout>
#include <OptimPlotZoomer.h>
#include <QMdiSubWindow>
#include <array>
#include <iostream>
#include <OptimUserRtti.h>
#include <ScatterPlotItem.h>
#include <QGridLayout>
#include <QwtAxis>
#include <Histogram.h>
/*

  struct PlotOptions {
  bool    auto_hor; 
  double  min_hor; 
  double  max_hor; 
  bool    auto_left; 
  double  min_left; 
  double  max_left; 
  bool    auto_right; 
  double  min_right; 
  double  max_right; 
  bool    hide_lego; 
  bool    bottom_lego;
  bool    top_lego;
  QwtText title;   
  QwtText axis_title_hor;   
  QwtText axis_title_left;   
  QwtText axis_title_right;
  bool    leftActive;
  bool    rightActive;
  bool    leftZoomOn;
  bool    rightZoomOn;
  bool    horZoomOn;
};

*/

namespace {
class ScaleEventFilter: public QObject { 

 public:

  ScaleEventFilter( TrackerPlot* parent, QwtAxis::Position axisid) 
    : QObject(parent), plot_(parent), axisw_(plot_->axisWidget(axisid)),
					     axisid_(axisid)  {} 

  bool eventFilter(QObject *watched, QEvent* event)
  {
    if ( axisw_ ==  qobject_cast<QwtScaleWidget*>(watched) ) {

      if ( event->type() != QEvent::MouseButtonPress ) return false;
      QMouseEvent* me =  static_cast<QMouseEvent*>(event);
      if ( me->button() == Qt::RightButton ) { 
	// plot_->options(); // FIXME
	return true;
      }
      if ( me->button() == Qt::LeftButton ) { 
	plot_->fixedLimits(  axisid_ );
        return true;         
	
      }
    }
    return false; 
  }

  TrackerPlot*       plot_;
  QwtScaleWidget*    axisw_;
  QwtAxis::Position  axisid_;
  PlotOptions        plot_opts_; 
};


} // anonymous namespace 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackerPlot::TrackerPlot( QWidget* parent)
  : QwtPlot(parent), grid_(0)
{
 setCanvasBackground( (QBrush( (QColor("white"))) ) );

 grid_ = new QwtPlotGrid();
 grid_->setMajorPen(QColor("gray"), 0.0, Qt::DotLine); 	
 grid_->setMinorPen(QColor("gray"), 0.0, Qt::DotLine);
 grid_->attach( this);

 // event filters for pop-ups windows to change axis limits

  QwtScaleWidget* w = axisWidget(QwtAxis::YLeft);
  w->installEventFilter( new ScaleEventFilter(this, QwtAxis::YLeft ) );

  w = axisWidget(QwtAxis::YRight);
  w->installEventFilter( new ScaleEventFilter(this, QwtAxis::YRight ) );

  w = axisWidget(QwtAxis::XBottom);
  w->installEventFilter( new ScaleEventFilter(this,  QwtAxis::XBottom) );

  zoomer_ = new OptimPlotZoomer( QwtAxis::XBottom, QwtAxis::YLeft, canvas(), true); 
  zoomer_->setMousePattern(QwtPlotZoomer::MouseSelect1,Qt::LeftButton);
  zoomer_->setEnabled(false);

  //--------------
  // compact axes 
  //--------------

  // The [QwtPlotScaleItem]s are scales that replace the defaults ones.
  // The tick marks on the new scales are drawn inside of the canvas to save space. 
  // we turn off the labels on the new scales and use the labels associated
  // with the default scales.
  // Note: the position on the canvas specified in the constructor (2nd arg) is
  // overridden by setBorderDistance();        
  
  QwtPlotScaleItem* yLeftScaleItem   = new QwtPlotScaleItem(QwtScaleDraw::RightScale, 10);
  QwtPlotScaleItem* yRightScaleItem  = new QwtPlotScaleItem(QwtScaleDraw::LeftScale, 1000);
  QwtPlotScaleItem* xBottomScaleItem = new QwtPlotScaleItem(QwtScaleDraw::TopScale, 0);
  QwtPlotScaleItem* xTopScaleItem    = new QwtPlotScaleItem(QwtScaleDraw::BottomScale, 1000);
  
  std::array<QwtPlotScaleItem*,4> scales = { yLeftScaleItem, yRightScaleItem, xBottomScaleItem, xTopScaleItem};
  for (auto item : scales) {
    item->scaleDraw()->enableComponent(QwtAbstractScaleDraw::Backbone, true);
    item->scaleDraw()->enableComponent(QwtAbstractScaleDraw::Ticks,    true);
    item->scaleDraw()->enableComponent(QwtAbstractScaleDraw::Labels,   false);
#if QWT_VERSION < 0x060200
    item->scaleDraw()->setPenWidth(Globals::preferences().plot_preferences.alwidth);
#else
    item->scaleDraw()->setPenWidthF(Globals::preferences().plot_preferences.alwidth);
#endif
    item->setBorderDistance(0); // this puts a new axis exactly on top
                                // of the old one i.e. the scale backbone is on the canvas edge   
    item->attach(this);
  }
  // this is a trick to get a right vertical and top horizontal window border. 
  yRightScaleItem->setBorderDistance(1);
  xBottomScaleItem->setBorderDistance(1);
 
  plotLayout()->setCanvasMargin(0);

  QwtAxis::Position axes[] = {QwtAxis::YLeft, QwtAxis::YRight, QwtAxis::XTop, QwtAxis::XBottom};

  // turn off drawing of the default scales.
  // but keep default scales labels
  
  for (auto id : axes) {
      plotLayout()->setAlignCanvasToScale(id,true);
      auto sc = axisScaleDraw(id);
      sc->enableComponent( QwtAbstractScaleDraw::Ticks,    false);
      sc->enableComponent( QwtAbstractScaleDraw::Backbone, false);
      sc->enableComponent( QwtAbstractScaleDraw::Labels,   true);
  };
    
  // show the new compact scales.
  
  auto scaleitems =  itemList( QwtPlotItem::Rtti_PlotScale);   
  for (auto it=scaleitems.begin(); it !=scaleitems.end(); ++it) {  
       (*it)->show();
  }

  // initialize the plot options 
  plot_opts_ = std::vector<PlotOptions>(4);
  for (int i=0; i <4; ++i ) {
    auto& opts = plot_opts_[i];
    opts.horZoomOn  = true;
    opts.auto_hor   = true;
    opts.leftZoomOn = true;
    opts.auto_left  = true;

  }

  // use sensible font and font size for axes labels
  
   QFont font = axisWidget(QwtPlot::yLeft)->font();
   //font.setPointSize(80);
   //axisWidget(QwtPlot::yLeft)->setFont(font);


   for (auto& axis: axes) {
     auto atitle = axisTitle(axis);
     atitle.setFont(font);
     setAxisTitle(axis,atitle);
   }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerPlot::fixedLimits( QwtAxis::Position axisid)
{

  FixedLimitsDialog* dialog = findChild<FixedLimitsDialog*>("FixedLimitsDialog");
  if (!dialog) {
    dialog = new FixedLimitsDialog(this);
    dialog->setObjectName("FixedLimitsDialog");
  };
  
 QwtInterval interval = axisInterval(axisid);

 dialog->data_.minimum = interval.minValue();
 dialog->data_.maximum = interval.maxValue();

 int vt = static_cast<int>(view_type_);
 
 switch (axisid) {
 case QwtAxis::XBottom: 
    dialog->setWindowTitle("Horizontal Axis");
    dialog->data_.active = true;
    dialog->data_.active_box_enabled = false;
    dialog->data_.zoom       = plot_opts_[vt].horZoomOn;
    dialog->data_.autoscale  = plot_opts_[vt].auto_hor;
   break;
 case QwtAxis::YLeft:
     dialog->setWindowTitle("Left  Vertical Axis");
     dialog->data_.active = true;
     dialog->data_.active_box_enabled = false;
     dialog->data_.zoom        = plot_opts_[vt].leftZoomOn;   
     dialog->data_.autoscale  =  plot_opts_[vt].auto_left;
   break;
 default:
   break;
 }
  
 dialog->set();

 std::function<void()> callback;

 switch (axisid) {

 case QwtAxis::XBottom:
     callback = [this,dialog, vt]() { 
     plot_opts_[vt].horZoomOn = dialog->data_.zoom;
     plot_opts_[vt].min_hor   = dialog->data_.minimum;
     plot_opts_[vt].max_hor   = dialog->data_.maximum;
     plot_opts_[vt].auto_hor  = dialog->data_.autoscale;
     setOptions(QwtAxis::XBottom);
     };
     break;
     
 case QwtAxis::YLeft:
     callback = [this,dialog,vt]() {
     plot_opts_[vt].leftZoomOn   = dialog->data_.zoom;
     plot_opts_[vt].leftActive   = dialog->data_.active;
     plot_opts_[vt].rightActive  = !plot_opts_[vt].leftActive;
     plot_opts_[vt].min_left     = dialog->data_.minimum;
     plot_opts_[vt].max_left     = dialog->data_.maximum;
     plot_opts_[vt].auto_left    = dialog->data_.autoscale;
     setOptions(QwtAxis::YLeft);
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

void TrackerPlot::enableZoomMode( bool on)
{
    zoomer_->setEnabled( on );

    if(on) zoomer_->setZoomBase();
    zoomer_->zoom(0); //  reset zoom base 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerPlot::setPlane(PlaneType t)
{
  // set the current view type.
  
  view_type_ = t;
  auto lst = itemList(Optim::Rtti_ScatterPlotItem); 
  for ( auto it = lst.begin(); it != lst.end(); ++it) {
    auto item =  dynamic_cast<ScatterPlotItem*>(*it);
    if (!item) continue;
    item->setPlane(t);
  }  

  switch (t) {
  case PlaneType::X_Y:
    setAxisTitle (QwtAxis::XBottom, "X [cm]");   
    setAxisTitle (QwtAxis::YLeft,   "Y [cm]");
    break;
  case PlaneType::X_Xp:
    setAxisTitle (QwtAxis::XBottom, "X      [cm]");   
    setAxisTitle (QwtAxis::YLeft,   "Xprime [mrad]"); 
    break;
  case PlaneType::Y_Yp:
    setAxisTitle (QwtAxis::XBottom, "Y      [cm]");   
    setAxisTitle (QwtAxis::YLeft,   "Yprime [mrad]"); 
    break;
  case PlaneType::S_P:
    setAxisTitle (QwtAxis::XBottom, "S      [cm]");   
    setAxisTitle (QwtAxis::YLeft,   "dP/P * 1000");  
    break;
 }

  setOptions(QwtAxis::XBottom);
  setOptions(QwtAxis::YLeft);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerPlot::setOptions(QwtAxis::Position axisid)
{
  // Axis limits ... 

  using QwtAxis::XBottom;
  using QwtAxis::YLeft;
  
  int vt = static_cast<int>(view_type_);
  switch ( axisid ) {
    
  case xBottom:
    setAxisAutoScale ( XBottom,  plot_opts_[vt].auto_hor);
    if (plot_opts_[vt].auto_hor) { 
      QwtInterval interval = axisInterval(XBottom); 	
      plot_opts_[vt].min_hor     = interval.minValue(); 
      plot_opts_[vt].max_hor     = interval.maxValue();
    }
      else {
      setAxisScale (XBottom,  plot_opts_[vt].min_hor, plot_opts_[vt].max_hor, 0);
    }

    break;

  case yLeft:
   setAxisAutoScale ( yLeft,  plot_opts_[vt].auto_left);
   if (plot_opts_[vt].auto_left) { 
      QwtInterval interval = axisInterval(YLeft); 	
      plot_opts_[vt].min_left     = interval.minValue(); 
      plot_opts_[vt].max_left     = interval.maxValue();
    }
    else {
      setAxisScale (YLeft,  plot_opts_[vt].min_left, plot_opts_[vt].max_left, 0);
    };
  break;

  default:
    break;
  }; 
  
  // Zooming ...  

  zoomer_->setConstrainedAxis( xBottom, !plot_opts_[vt].horZoomOn); 
  zoomer_->setConstrainedAxis( yLeft,   !plot_opts_[vt].leftZoomOn);

  enableZoomMode(plot_opts_[vt].horZoomOn || plot_opts_[vt].leftZoomOn);
  
  // *** for now, we do not change the titles for each view. This is done in the tracker code. 

  replot();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
/***
void TrackerPlot::set(QwtAxis::Position axisid)
{
   // function to determine the min/max valuesfor each coordinates. 
   // arg  0 <= i <= 5  i=0 is x i=1 is x' etc ...  

   auto limits = [ this](int i) { 
           double xmin = (*std::min_element(&vin_[0], &vin_[N_], [&i](auto const& lhs, auto const&  rhs){ return lhs[i]<rhs[i];}))[i];
           double xmax = (*std::max_element(&vin_[0], &vin_[N_], [&i](auto const& lhs, auto const&  rhs){ return lhs[i]<rhs[i];}))[i];
           return std::make_pair(xmin,xmax);
   };
   
   auto [xmin,     xmax]  = limits(0);     
   auto [xpmin,   xpmax]  = limits(1);     
   auto [ymin,     ymax]  = limits(2);     
   auto [ypmin,   ypmax]  = limits(3);     
   auto [smin,     smax]  = limits(4);     
   auto [dppmin, dppmax]  = limits(5);     

   double zmin = 1.0e-5;
   double zmax = 1.0;

   int nx = 100;
   int ny = 100;
   
   Histogram2D  hxy(nx, xmin, xmax, ny, ymin, ymax); 
  
   // assign points to the histogram 
   for (int i=0; i<N_; ++i) {
     hxy(vin_[i][0], vin_[i][2]);
   }

   // determine the max bin size 
   double vmax = double(hxy.max());
  
   // copy histo data into a vector
   QVector<double> vxy;

   for (auto&& x : hxy.idxrange()) {
    auto const i = x.index(0); // current index along first axis
    auto const j = x.index(1); // current index along second axis
    //auto const b0 = x.bin(0);  // current bin interval along first axis
    //auto const b1 = x.bin(1);  // current bin interval along second axis
    auto const v = *x;         // "dereference" to get the bin value
    vxy.push_back( double(hxy.at(i,j))/vmax);
   };

   /*
   for (int i=0; i<nx; ++i) {
     for (int j=0; j<ny; ++j) {
       vxy.push_back( double(hxy.at(i,j))/vmax);
     }
   };
   */
/*   
   QwtMatrixRasterData* mraster = new QwtMatrixRasterData();
   mraster->setValueMatrix(vxy, ny);

   mraster->setInterval(Qt::XAxis, QwtInterval(xmin, xmax));
   mraster->setInterval(Qt::YAxis, QwtInterval(ymin, ymax));
   mraster->setInterval(Qt::ZAxis, QwtInterval(zmin, zmax));
   QwtInterval zInterval = QwtInterval(zmin, zmax);
   mraster->setResampleMode(QwtMatrixRasterData::BilinearInterpolation);

   QwtPlotSpectrogram* spectro = new  QwtPlotSpectrogram();
   spectro->setData(mraster);
   spectro->setColorMap(new JetLogColorMap());

   setAxisScaleEngine(QwtPlot::yRight, new QwtLogScaleEngine());
   setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
   setAxisVisible(QwtPlot::yRight );
   spectro->attach(plot);
   replot();
   return;
}
*/
