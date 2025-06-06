//  =================================================================
//
//  OptimPlot.cpp
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
#include <memory>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include <OptimUserRtti.h>
#include <OptimApp.h>
#include <OptimLegoPlotItem.h>
#include <OptimLogoPlotItem.h>
#include <OptimPlot.h>
#include <OptimPlotCurve.h>
#include <OptimPlotZoomer.h>
#include <OptimPlotLegend.h>
#include <DistancePicker.h>
#include <OptimPlotLinearScaleEngine.h>
#include <FixedLimitsDialog.h>
#include <MeasuredDataDialog.h>
#include <EditMeasuredDataDialog.h>
#include <OptimMainWindow.h>
#include <OptimMessages.h>
#include <SQLSeriesData.h>
#include <Element.h>
#include <Utility.h>
#include <DataCurve.h>
#include <Globals.h>
#include <Conversions.h>
#include <CustomFileDialog.h>
#include <CompactLegend.h>
#include <sqlite/connection.hpp>

#include <QtDebug>
#include <QtGlobal>
#include <QApplication>
#include <QDateTime>
#include <QClipboard>
#include <QImage>
#include <QPainter>
#include <QImageWriter>
#include <QRegExp>
#include <QMdiSubWindow>
#include <QMenu>
#include <QDateTime>
#include <QBrush>
#include <QColor>
#include <QDir>
#include <QEvent>
#include <QFileInfo>
#include <QFileDialog>
#include <QMouseEvent>
#include <QLayout>
#include <QObject>
#include <QPalette>
#include <QPixmap>
#include <QPointer>
#include <QRect>
#if QT_VERSION >= 0x050000
#include <QRegularExpression>
#endif

#include <QwtPlotCanvas>
#include <QwtLegend>
#include <QwtLegendLabel>
#include <QwtPlotGrid>
#include <QwtPickerMachine>
#include <QwtPlotLayout>
#include <QwtPlotRenderer>
#include <QwtPlotZoomer>
#include <QwtScaleMap>
#include <QwtPlotTextLabel>
#include <QwtPlotScaleItem>
#include <QwtText>
#include <QwtScaleWidget>
#include <QwtInterval>
#include <QwtIntervalSample>
#include <QwtIntervalSymbol>

#include <OptimIntervalCurve.h>
#include <OptimIntervalSample.h>
#include <OptimIntervalSymbol.h>

using Utility::splitText;
using Utility::filterName;
using Utility::split;
using Utility::decodeExtLine;

#define FDATA_LEN 32 


std::vector<QwtSymbol::Style> OptimPlot::symbols_ = { QwtSymbol::Ellipse,    QwtSymbol::Rect,       QwtSymbol::Diamond,  
                                                      QwtSymbol::DTriangle,  QwtSymbol::UTriangle,  QwtSymbol::LTriangle,  
                                                      QwtSymbol::RTriangle,  QwtSymbol::Cross,      QwtSymbol::XCross, 
                                                      QwtSymbol::Star2,      QwtSymbol::Hexagon };

std::vector<std::string>       OptimPlot::colors_  = {  "blueviolet", "indianred",  "seagreen", 
                                                        "brown",       "magenta",   "olive",   
                                                        "yellowgreen" };


class ScaleEventFilter: public QObject { 

 public:

  ScaleEventFilter( OptimPlot* parent, QwtPlot::Axis axisid) 
    : QObject(parent), plot_(parent), axisw_(plot_->axisWidget(axisid)),
					     axisid_(axisid)  {} 

  bool eventFilter(QObject *watched, QEvent* event)
  {
    if ( axisw_ ==  qobject_cast<QwtScaleWidget*>(watched) ) {

      if ( event->type() != QEvent::MouseButtonPress ) return false;
      QMouseEvent* me =  static_cast<QMouseEvent*>(event);
      if ( me->button() == Qt::RightButton ) { 
	plot_->options();
	return true;
      }
      if ( me->button() == Qt::LeftButton ) { 
	plot_->fixedLimits(  axisid_ );
        return true;         
	
      }
    }
    return false; 
  }

  OptimPlot*      plot_;
  QwtScaleWidget* axisw_;
  QwtPlot::Axis   axisid_;
  
};


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimPlot::OptimPlot( QWidget* parent, std::vector<LegoData> const& legodata) 
  : QwtPlot(parent),
    lego_(0),
    distance_picker_left_(0),
    distance_picker_right_(0),
    opts_dialog_(new  PlotOptionsDialog(this)),
    measured_data_dialog_(new MeasuredDataDialog(this)),
    edit_measured_data_dialog_(0),
    ndata_(0), name_ext_(""), len_data_(0), color_data_(0),
    cross_data_(0),
    YextM_(), Xext_(), Yext_(), YextP_(), scale_(1.0),
    Xoffs_(), elmNames_(), scraperPos_(2), data_label_(0),
    datatxt_(""), legend_(0),
    mdata_show_legend_(true),
    mdata_show_full_path_(false),
    mdata_data_only_(false),
    datetime_( QDateTime::currentDateTime()), 
    con_(Globals::preferences().con)
{
  ctor( parent, legodata);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::ctor(QWidget* parent, std::vector<LegoData> const& legodata)
{

 if (layout()) layout()->setSizeConstraint( QLayout::SetNoConstraint);

 distance_picker_left_ = new DistancePicker(xBottom, yLeft, canvas());
 distance_picker_left_->setEnabled(false);
  
 distance_picker_right_ = new DistancePicker(xBottom, yRight, canvas());
 distance_picker_right_->setEnabled(false);
  
 QPalette palette;
 palette.setColor(QPalette::Window, QColor("white")); 
 canvas()->setPalette(palette);  

 grid_ = new QwtPlotGrid();
 grid_->setMajorPen(QColor("gray"), 0.0, Qt::DotLine); 	
 grid_->setMinorPen(QColor("gray"), 0.0, Qt::DotLine);
 grid_->attach( this);
 
 setAxisScaleEngine(yLeft,  new OptimPlotLinearScaleEngine() );
 setAxisScaleEngine(yRight, new OptimPlotLinearScaleEngine() );

 zoomer1_ = new OptimPlotZoomer( xBottom, yLeft, canvas(), true); 
 zoomer1_->setMousePattern(QwtPlotZoomer::MouseSelect1,Qt::LeftButton);
 zoomer1_->setEnabled(false);

 zoomer2_ = new OptimPlotZoomer( xBottom, yRight, canvas(), true); 
 zoomer2_->setMousePattern(QwtPlotZoomer::MouseSelect1,Qt::LeftButton);
 zoomer2_->setEnabled(false);

 tracker1_ = new QwtPlotPicker( xBottom, yLeft,
				 QwtPlotPicker::NoRubberBand,
				QwtPlotPicker::AlwaysOn,  canvas());
 tracker1_->setEnabled(false);

 tracker2_ = new QwtPlotPicker( xBottom, yRight,
				 QwtPlotPicker::NoRubberBand,
				 QwtPlotPicker::AlwaysOn,  canvas());
 tracker2_->setEnabled(false);

 if (!legodata.empty()) createLego(legodata); 

 connect(this, SIGNAL( itemAttached(QwtPlotItem*, bool) ), 
	 this,   SLOT( resetZoomBase(QwtPlotItem*, bool) ));

  
 connect(zoomer1_, SIGNAL( zoomed(const QRectF& ) ), 
	 this,     SLOT(  plotIsZoomed(const QRectF&) ) );
 connect(zoomer2_, SIGNAL( zoomed(const QRectF& ) ), 
	 this,     SLOT(  plotIsZoomed(const QRectF&) ) );


 distance_picker_left_->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ShiftModifier); 
 distance_picker_left_->setStateMachine( (new QwtPickerDragLineMachine()) );

 distance_picker_right_->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ShiftModifier); 
 distance_picker_right_->setStateMachine( (new QwtPickerDragLineMachine()) );

 if (parent) {
  OptimMainWindow* mw = qobject_cast<OptimMainWindow*>(parent); 
  if (mw) { 
   connect( this, SIGNAL( attach() ),  mw, SLOT( attach() )); 
   connect( this, SIGNAL( detach() ),  mw, SLOT( detach() )); 
  }
 }


 // filters for pop-ups to change axis limits

 QwtScaleWidget* w = axisWidget(QwtPlot::yLeft);
 w->installEventFilter( new ScaleEventFilter(this, QwtPlot::yLeft ) );

 w = axisWidget(QwtPlot::yRight);
 w->installEventFilter( new ScaleEventFilter(this, QwtPlot::yRight ) );

 w = axisWidget(QwtPlot::xBottom);
 w->installEventFilter( new ScaleEventFilter(this,  QwtPlot::xBottom) );

 //.........................
 // set default plot options
 //.........................

 auto font = titleLabel()->font();
 font.setPointSize(10);
 titleLabel()->setFont(font);
 
 plot_opts_.auto_hor    = false; // if true,  limits are automatically determined; min_hor and max_hor are ignored.  
                                 // if false, min_hor and max_hor set by data
 plot_opts_.min_hor     = 0.0; 
 plot_opts_.max_hor     = 0.0; //1.0;
 plot_opts_.auto_left   = true; 
 plot_opts_.min_left    = 0.0;
 plot_opts_.max_left    = 1.0;
 plot_opts_.auto_right  = true;
 plot_opts_.min_right   = 0.0; 
 plot_opts_.max_right   = 1.0;
 plot_opts_.hide_lego   = false; 
 plot_opts_.bottom_lego = false;
 plot_opts_.top_lego    = true;
 plot_opts_.title            = QwtText("undefined");  
 plot_opts_.axis_title_hor   = QwtText("xBottom label");  
 plot_opts_.axis_title_left  = QwtText("yLeft label");  
 plot_opts_.axis_title_right = QwtText("yRight label");
 plot_opts_.leftActive   = true;
 plot_opts_.rightActive  = false;
 plot_opts_.leftZoomOn   = false;
 plot_opts_.rightZoomOn  = false;
 plot_opts_.horZoomOn    = true;

 axisWidget( QwtPlot::yLeft )->setMargin( 0 );
 axisWidget( QwtPlot::yRight)->setMargin( 0 );
 axisWidget( QwtPlot::xBottom)->setMargin( 0 );

 setOptions();  
 setDisplayStyle(Globals::preferences().plot_preferences.compact);

 QwtText timestamp(datetime_.toString(Qt::TextDate));
 timestamp.setRenderFlags(Qt::AlignRight | Qt::AlignTop);

 tsitem_ = new QwtPlotTextLabel();
 tsitem_->setText(timestamp);
 tsitem_->attach(this);
 update(); // this calls repaint() the call may be delayed to optimize performance. 

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimPlot::~OptimPlot()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::configureCurves( PlotSpec const& plotspec, bool clearplot, DBSpecs const& dbspecs)
{

  // configure the curves only. This function is usually called
  // when an existing plot is overwritten after a new calculation.
  
  static char const* colors[] = {"red", "green", "blue", "black" };

  QMdiSubWindow* sw = qobject_cast<QMdiSubWindow*>(parent());
  if (sw) {
    
    plot_opts_.title = QwtText(plotspec.title.c_str());
    //sw->setWindowTitle(plotspec.title.c_str());
    //setTitle( plotspec.title.c_str());
  }

  auto curvespecs = plotspec.curvespecs; 
  int cindex = 0;

  auto itq = dbspecs.queries.begin(); 

  for (auto it = curvespecs.begin(); it != curvespecs.end(); ++it) { 

    CurveSpec const& spec = *it; 
    QwtPlotCurve* curve =  dynamic_cast<QwtPlotCurve*>( findPlotItemByName( spec.legendname.c_str() ) );
    
      if (!curve) {

      //......................................................................
      // a KLUDGE to deal with amplitude plots.
      // This code should be revisited.   
      //
      // Check if we are dealing with an amplitude plot  

       if ( spec.legendname == std::string("Ax")) {
	 curve = dynamic_cast<QwtPlotCurve*>(findPlotItemByName( "Ax" ));
       }	   
       else if ( spec.legendname == std::string("Ax_tot")) {
	 curve = dynamic_cast<QwtPlotCurve*>(findPlotItemByName( "Ax_tot" ));
       }	   
       else if ( spec.legendname == std::string("Ay_tot")) {
	 curve = dynamic_cast<QwtPlotCurve*>(findPlotItemByName( "Ay_tot" ));
       }	   
       else if ( spec.legendname == std::string("Ay")) {
	 curve = dynamic_cast<QwtPlotCurve*>(findPlotItemByName( "Ay" ));
       }	   

         if (curve) {
 	   auto tmp=curve; removeItem(curve); delete tmp;
           curve = new OptimPlotCurve(spec.legendname.c_str());
	   curve->setPen ((!spec.color.empty() ? QColor(spec.color.c_str()):
		    QColor(colors[cindex++])),  Globals::preferences().plot_preferences.lwidth, Qt::SolidLine);

	   if ( spec.n != 0 ) {
               curve->setSamples( spec.xv, spec.yv, spec.n);  
           }
	   else {
	     curve->setSamples( new SQLSeriesData(*con_, *itq ));
	     ++itq;
           }

	   curve->setYAxis(spec.axis);
    
           if (spec.axis == QwtPlot::yLeft)  {
	     plot_opts_.axis_title_left  = QwtText(spec.vertical_title.c_str() );
           }
           if (spec.axis == QwtPlot::yRight) {
	     plot_opts_.axis_title_right = QwtText(spec.vertical_title.c_str() );
           }

           curve->attach(this);
           curve->setVisible(true);
        }
      //........................................................
      // curve does not exists. This is usually external data so we do not touch it. 
                  // exception: beam size plot

      //if( itq == dbspecs.queries.end() ) break;  // this is not needed since dbspecs.queries.size() == curves.size()         

       continue;
    }
    
    if (!clearplot) {
        bool visible = curve->isVisible(); 
        curve = new QwtPlotCurve(spec.legendname.c_str());
        curve->setVisible(visible);
    } 
     
    // if ( !curve->isVisible()) continue; // do not update curve when it is not visible 

    if ( spec.n != 0 ) {
      curve->setSamples( spec.xv, spec.yv, spec.n);  
    }
     else {
       curve->setSamples( new SQLSeriesData(*con_, *(itq++) ));
    }

    // set the hor scale limits only if they have never been set. 
    if ( plot_opts_.min_hor == 0.0 && plot_opts_.max_hor == 0.0) {
      plot_opts_.min_hor     = curve->minXValue(); 
      plot_opts_.max_hor     = curve->maxXValue(); ;
    }
    if (clearplot) continue;

    plot_opts_.min_hor     = curve->minXValue(); 
    plot_opts_.max_hor     = curve->maxXValue(); ;
    
    curve->setPen ((!spec.color.empty() ? QColor(spec.color.c_str()): QColor(colors[cindex])),
		               Globals::preferences().plot_preferences.lwidth, Qt::SolidLine);
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve->setYAxis(spec.axis);

    if ( spec.legendname == std::string("H-apert") ) {
      curve->setStyle( QwtPlotCurve::CurveStyle(Optim::HApertureCurve));
    };

    if ( spec.legendname == std::string("V-apert") ) {
      curve->setStyle( QwtPlotCurve::CurveStyle(Optim::VApertureCurve));
    };

    if (spec.axis == QwtPlot::yLeft)  {
      plot_opts_.axis_title_left  = QwtText(spec.vertical_title.c_str() );
    }
    if (spec.axis == QwtPlot::yRight) {
      plot_opts_.axis_title_right = QwtText(spec.vertical_title.c_str() );
    }
    
    curve->attach(this);

    if (spec.color.empty()) (++cindex) %= 4;

    // if( itq == dbspecs.queries.end() ) break;        
  }
  
 setOptions(); 

}
 //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::configure( PlotSpec const& plotspec, DBSpecs const& dbspecs)
{
 
  QMdiSubWindow* sw = qobject_cast<QMdiSubWindow*>(parent());
  sw->setWindowTitle(plotspec.title.c_str());

  // txt = true / generate textfile
  
 /* 
  struct CurveSpec { 
  char   const* legendname;
  double const* xv; 
  double const* yv; 
  int    const  n; 
  QwtPlot::Axis axis;
  union vertical_title_t { 
    char const*  left;
    char const*  right;
  } vertical_title;
  

  };

  struct PlotSpec { 
    char   const* title;
    char   const* bottom_title;
    std::list<CurveSpec> curvespecs;
  };

  */

  static char const* colors[] = {"red", "green", "blue", "black" };

  plot_opts_.title = QwtText(plotspec.title.c_str());
  plot_opts_.axis_title_hor = QwtText(plotspec.bottom_title.c_str());
  
  auto curvespecs = plotspec.curvespecs; 
  int cindex = 0;

  auto itq = dbspecs.queries.begin();

  for (auto it = curvespecs.begin(); it != curvespecs.end(); ++it) { 
    CurveSpec const& spec = *it; 
 
    OptimPlotCurve* curve = new OptimPlotCurve(spec.legendname.c_str());

    if ( spec.xv ) { // if a spec string is specified, use that  
      curve->setSamples( spec.xv, spec.yv, spec.n);  
    }
    else{ // if the spec string is a null ptr, this is DB spec 
      curve->setSamples( new SQLSeriesData(*con_, *(itq++) ));
    }

    if ( plot_opts_.min_hor == 0.0 && plot_opts_.max_hor == 0.0) {
      plot_opts_.min_hor     = curve->minXValue(); 
      plot_opts_.max_hor     = curve->maxXValue(); ;
    }

    curve->setPen ((!spec.color.empty() ? QColor(spec.color.c_str()):
    QColor(colors[cindex])),  Globals::preferences().plot_preferences.lwidth, Qt::SolidLine);
    curve->setYAxis(spec.axis);

    enableAxis( spec.axis );

    QwtScaleDraw* sdw = axisScaleDraw(QwtPlot::yLeft);
#if QWT_VERSION < 0x060200
    sdw->setPenWidth(Globals::preferences().plot_preferences.alwidth);
#else
    sdw->setPenWidthF(Globals::preferences().plot_preferences.alwidth);
#endif
    
    sdw = axisScaleDraw(QwtPlot::yRight);
#if QWT_VERSION < 0x060200
    sdw->setPenWidth(Globals::preferences().plot_preferences.alwidth);
#else
    sdw->setPenWidthF(Globals::preferences().plot_preferences.alwidth);
#endif
    
    sdw = axisScaleDraw(QwtPlot::xBottom);
#if QWT_VERSION < 0x060200
    sdw->setPenWidth(Globals::preferences().plot_preferences.alwidth);
#else
    sdw->setPenWidthF(Globals::preferences().plot_preferences.alwidth);
#endif
    
    if (spec.axis == QwtPlot::yLeft)  {
      plot_opts_.axis_title_left  = QwtText(spec.vertical_title.c_str() );
    }
    if (spec.axis == QwtPlot::yRight) {
      plot_opts_.axis_title_right = QwtText(spec.vertical_title.c_str() );
    }
    
    if ( spec.legendname == std::string("H-apert") ) {
      curve->setStyle( QwtPlotCurve::CurveStyle(Optim::HApertureCurve));
    }
    else if ( spec.legendname == std::string("V-apert") ) {
      curve->setStyle( QwtPlotCurve::CurveStyle(Optim::VApertureCurve));
    }
    else if (spec.symbol != QwtSymbol::NoSymbol ) {
	curve->setSymbol( new QwtSymbol(spec.symbol, QBrush( curve->pen().color()), curve->pen(),  QSize(5,5) ));
    }

    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve->attach(this);

    if (spec.color.empty()) (++cindex) %= 4;

    //if (itq ==  dbspecs.queries.end() ) break; 			 
  }
  
  setAxesTitleFont( (QFont("Sans", Globals::preferences().plot_preferences.fontsize, QFont::Normal)) );

  for (auto it  = curve_states_.begin();
            it != curve_states_.end(); ++it) {

     auto list = itemList();
     for (auto itc =  list.begin(); itc != list.end(); ++itc) {
       if ( dynamic_cast<OptimIntervalCurve*>( *itc ) ) continue;
       if ( (*itc)->title().text() == toQString(it->title) )  (*itc)->setVisible(it->visible);
     }    
  }

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

  setPreferences(); 
  setOptions(); 
  setDisplayStyle(Globals::preferences().plot_preferences.compact);
  replot();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  OptimPlot::setLegoVisible(bool visible )
{
  plot_opts_.hide_lego = !visible;

  if (!lego_) return;

  visible ? lego_->show() : lego_->hide();
  update(); 
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool  OptimPlot::legoIsVisible() const 
{
  if (!lego_) return false;  
  else return lego_->isVisible();
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  OptimPlot::createLego( std::vector<LegoData> const& legodata )
{
   if (lego_) {
     detachItems( QwtPlotItem::Rtti_PlotUserItem, true); //this deletes all the lego plot(s) 
     lego_ = 0;
   } 

   lego_ = new OptimLegoPlotItem(legodata); // should not create a new object here ! ***FIX ME***
   
   lego_->attach(this);

   canvas()->installEventFilter(lego_);
   canvas()->setMouseTracking(true);
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  OptimPlot::setLegoData( std::vector<LegoData> const& legodata )
{
  if (!lego_) return;
  
  lego_->updateData(legodata);
  
   if (plot_opts_.top_lego) { 
      lego_->setPosition(LegoPosition::Top); 
      setLegoVisible(true);
   }
   if (plot_opts_.bottom_lego) { 
     lego_->setPosition(LegoPosition::Bottom);
     setLegoVisible(true);
   }
   if (plot_opts_.hide_lego) {
     setLegoVisible(false);
   }
   if(legoIsVisible()) update(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  OptimPlot::enableCurve( const QVariant &itemInfo, bool on, int index)
{
  QwtPlotItem *plotItem = infoToItem( itemInfo );
  if ( !plotItem )  return;
  showCurve( plotItem, on);
}

void  OptimPlot::enableCurve( const QVariant &itemInfo, int index)
{
  QwtPlotItem *plotItem = infoToItem( itemInfo );
  if ( plotItem ) {
    showCurve( plotItem, !plotItem->isVisible());
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::showCurve( QwtPlotItem *item, bool on )
{
    item->setVisible( on );
    replot();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   OptimPlot::resetZoomBase( QwtPlotItem* item, bool attached)
{
  if ( item->rtti() == QwtPlotItem::Rtti_PlotCurve) {
    zoomer1_->setZoomBase();
    zoomer2_->setZoomBase(); 
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool   OptimPlot::event( QEvent* e)
{
#if  0  
  if (e->type() == QEvent::MouseButtonRelease) {
    std::cout << "OptimPlot:: QEvent::MouseButtonRelease " << std::endl;
  }
  if (e->type() == QEvent::MouseButtonPress) {
    std::cout << "OptimPlot:: QEvent::MouseButtonPress " << std::endl;
  }
  if (e->type() == QEvent::MouseButtonDblClick) {
    std::cout << "OptimPlot:: QEvent::MouseButtonDblClick " << std::endl;
  }
#endif
#if  0
-------------------------------------------------
    if ( QApplication::keyboardModifiers() & Qt::ControlModifier ) {
      if ( dynamic_cast<QMouseEvent*>(e)->button() == Qt::RightButton) {
	QMdiSubWindow* sw = qobject_cast<QMdiSubWindow*>(parent());
        if (sw)  {
	    QMenu* sysmenu =  sw->systemMenu();
	    sysmenu->exec(QCursor::pos());
	}
        return true; 
      } 
   }
    if ( !(zoomer1_->isEnabled() || zoomer1_->isEnabled()) ) {
      auto me = dynamic_cast<QMouseEvent*>(e);
      int xc = canvas()->x();
      int yc = canvas()->y();
     
      // construct cursor position in world coordinates   
      //QString label;
      //label.sprintf( "%g, %g", canvasMap(xBottom).invTransform( me->x()-xc ), canvasMap(yLeft).invTransform( me->y()-yc)  );
      //std::cout << "me->x() = " << me->x() << " " << canvasMap(xBottom).invTransform( me->x()-xc ) <<  std::endl;   
      //std::cout << "me->y() = " << me->y() << " " << canvasMap(yLeft).invTransform( me->y()-yc)    << std::endl;   
      ///std::cout << label.toUtf8().data() << std::endl;

    }
  }
--------------------------------------------------------------
#endif

// find the lego plot and send the event 

// if ( e->type() == QEvent::QEvent::MouseButtonPress && lego_ ) {
//   std::cerr << "OptimPlot received QEvent::MouseButtonPress" << std::endl; 
//QCoreApplication::sendEvent(lego_, e);
// }
 return QwtPlot::event(e);
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::editMeasuredData() 
{

  QStringList list;
  QwtPlotItemList curves = itemList( QwtPlotItem::RttiValues::Rtti_PlotIntervalCurve ); //  QwtPlotItem::RttiValues::Rtti_PlotCurve);
  for (auto it = curves.begin(); it != curves.end(); ++it) {
    //if  ( (*it)->title().text().contains(QRegExp("^Data_")) ) {
    if  ( (*it)->title().text().indexOf(QRegularExpression("^Data_")) != -1 ) {
       list.append((*it)->title().text());
    }
  }

  if ( !edit_measured_data_dialog_ ) {  
    edit_measured_data_dialog_ = new EditMeasuredDataDialog();
    edit_measured_data_dialog_->data_.show_legend    = true;
    edit_measured_data_dialog_->data_.show_full_path = false;
    edit_measured_data_dialog_->data_.data_only      = false;
    edit_measured_data_dialog_->data_.remove_set     = false;
    edit_measured_data_dialog_->data_.current_idx    = 0;
    edit_measured_data_dialog_->data_.setcolor       = false;
    edit_measured_data_dialog_->data_.setmarker      = false;
  }

  edit_measured_data_dialog_->setDataSets(list);
  edit_measured_data_dialog_->data_.remove_set  = false;
  edit_measured_data_dialog_->set();

  if ( edit_measured_data_dialog_->exec() == QDialog::Rejected) return;

  EditDataStruct& editdata = edit_measured_data_dialog_->data_;

  mdata_show_legend_     = editdata.show_legend;
  mdata_show_full_path_  = editdata.show_full_path;
  mdata_data_only_       = editdata.data_only;

  //if (dialog->data_.current_idx == "All Data Sets")

     OptimIntervalCurve* curve = 0;
     
     for (auto it = curves.begin(); it != curves.end(); ++it) {
       if  ( (*it)->title().text() == editdata.current_txt ) {
         curve = dynamic_cast<OptimIntervalCurve*>(*it);  
         break;
       }
     }

     
     // refresh the legend ( note: the legend is a single string with newline terminators )
   if ( editdata.remove_set ) {
     //QRegExp rx(editdata.current_txt +".*\\n");
     QRegularExpression rx(editdata.current_txt +".*\\n");
     //rx.setMinimal(true);  
     datatxt_.replace(rx,"");
     
     if (curve) { 
        removeItem( curve ); // note: removing an item does not delete it ! 
        delete curve;
     }
   }

   if ( editdata.setcolor ) {
     if (curve) { 
       OptimIntervalSymbol* symbol = new OptimIntervalSymbol(*curve->symbol());
       symbol->setPen(editdata.color, 1.0, Qt::SolidLine);
       symbol->setBrush(QColor(editdata.color));
       curve->setSymbol(symbol);
       QColor color(editdata.color);
       color.setAlpha(75);
       curve->setPen( QPen(color, 2, Qt::DotLine ) );
       curve->setBrush(color);
     }
   }

    
  // enable/disable display and full/short style

  if (editdata.show_legend ) {
     QString tmpstr( datatxt_);
     if (!editdata.show_full_path) {
#if QT_VERSION >= 0x050000
       QRegularExpression re(R"(((/.*/)|(\\.*\\)))");
#else
       QRegExp re(R"(((/.*/)|(\\.*\\)))");
#endif
       tmpstr.remove(re);
     };
     QwtText dataqwt( tmpstr );
     dataqwt.setRenderFlags( Qt::AlignLeft | Qt::AlignTop );
     if (data_label_) data_label_->setText(dataqwt);  
  }
  else {
     if (data_label_)  data_label_->setText(QwtText(""));  
  } 

  // enable/disable tube style display for data
  
  curves = itemList( QwtPlotItem::RttiValues::Rtti_PlotIntervalCurve );
  for (auto it = curves.begin(); it != curves.end(); ++it) {
    dynamic_cast<OptimIntervalCurve*>(*it)->setStyle( editdata.data_only ? QwtPlotIntervalCurve::NoCurve
						                          : QwtPlotIntervalCurve::CurveStyle(QwtPlotIntervalCurve::UserCurve+OptimIntervalCurve::Line));
  }

  replot();  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::addMeasuredData() 
{

  //---------------------------------------------------------------------------------
  // Synopsis: Display data read from an external file, overlayed on a standard plot. 
  //---------------------------------------------------------------------------------

  auto& dialog = measured_data_dialog_;
  if ( dialog->exec() == QDialog::Rejected) return;

  FilterDlgStr& NSt = dialog->data_;

  std::string FileName( NSt.fname.toUtf8().data() ); 

  FILE* fp = 0;
  if( FileName.length() == 0 ) {
     OptimMessageBox::warning(this, "Error opening file - ", "No filename specified.", QMessageBox::Ok);
     return;
   }
  if( !(fp=fopen(FileName.c_str(),"r")) ) {
     OptimMessageBox::warning(this, "Error opening file - ", splitText(FileName.c_str()), QMessageBox::Ok);
     return;
   }

  int Scol    = (NSt.Scol    > FDATA_LEN) ? (NSt.Scol    = FDATA_LEN -1) : NSt.Scol-1;
  int DataCol = (NSt.DataCol > FDATA_LEN) ? (NSt.DataCol = FDATA_LEN -1) : NSt.DataCol-1;
  int ErrpCol = (NSt.ErrpCol > FDATA_LEN) ? (NSt.ErrpCol = FDATA_LEN -1) : NSt.ErrpCol-1;
  int ErrmCol = (NSt.ErrmCol > FDATA_LEN) ? (NSt.ErrmCol = FDATA_LEN -1) : NSt.ErrmCol-1;
  int NameCol = (NSt.NameCol > FDATA_LEN) ? (NSt.NameCol = FDATA_LEN -1) : NSt.NameCol-1;
  int MaxCol  = 0;

  NSt.Color = ndata_%4 + 1;
  NSt.Cross = NSt.Color;
  
  color_data_ = (NSt.Color-1)  % colors_.size();
  cross_data_  = (NSt.Cross-1) % symbols_.size();
  scale_       = NSt.Scaling;
    
  QString datatxt(""); 

  OptimMainWindow* mw = qobject_cast<OptimMainWindow*>(window()); // ????    
  
  double offset = (NSt.RefToS0) ?  mw->so0_ : 0.0; 
  
  Xoffs_ = (NSt.SfromOptics) ? 0.0 : NSt.Xoffs;
 
  if(NSt.SfromOptics){
     if(NameCol < 0){
       OptimMessageBox::warning(this, "File error", "No element name column ", QMessageBox::Ok);
       fclose(fp);
       return;
     }
     if(NameCol>MaxCol)MaxCol = NameCol;
   } 
  else {
    if(Scol < 0){
      OptimMessageBox::warning(this, "File error", "No s-column", QMessageBox::Ok);
      fclose(fp);
      return;
    }
    if(Scol > MaxCol) MaxCol = Scol;
  }

  if(DataCol < 0){
    OptimMessageBox::warning(this, "File error", " No data column", QMessageBox::Ok);
    fclose(fp);
    return;
  }
  if(DataCol>MaxCol) MaxCol = DataCol;
  if( (ErrpCol >= 0) && (ErrmCol >= 0) ) {
     if(ErrpCol>MaxCol) MaxCol = ErrpCol;
     if(ErrmCol>MaxCol) MaxCol = ErrmCol;
  }
  ++MaxCol;
     
  char   buf[256];
  double dat[FDATA_LEN]; // FDATA_LEN maximum number of columns allowed in the data file   

  std::string elname;
  std::vector<double> s; // longitudinal position
    
  char  words[FDATA_LEN][32];
  char* pword[FDATA_LEN+1];
 
  // count the number of data points
  
  int npts = 0;

  while(fgets(buf, 255, fp)) {
    if(buf[0]=='#') continue;
    for(int i=0; i<FDATA_LEN; ++i) pword[i]=words[i];

    int ncols = split(buf,FDATA_LEN, pword);

    if (!npts) nc_ext_ = ncols;

    if( ncols  < 2) continue; // we need at least 2 colunms
 
    if( ncols < MaxCol) {
      OptimMessageBox::warning(this, "Read error", "A column index exceeds the number of available columns in the data file.", QMessageBox::Ok);
      fclose(fp);
      return;
    }
    if(NSt.SfromOptics) {
      elname =  std::string(NSt.NamePrefix) +  pword[NameCol];
 
      int j    = 0;
      for( j=0; j<mw->nelm_; ++j){
        if( elname == mw->beamline_[j]->fullName()) break;
      }
     
      if(  j == mw->nelm_ ) continue; // this means the element was not found 

      if( !filterName(pword[NameCol], NSt.filter, NSt.MatchCase) ) continue;
    }
    else {
      if( !((decodeExtLine(buf, dat, FDATA_LEN)>=2) && (dat[Scol] >= offset) && (dat[Scol] < (offset + mw->Length_)) )) continue;
      if( decodeExtLine(buf, dat, FDATA_LEN) < MaxCol) continue;
    }
    ++npts;
  } //while 

  fclose(fp);
  
  if(!npts){
    OptimMessageBox::warning(this,"Read error", "There is no data in the file or all data has been filtered out.", QMessageBox::Ok);
    return;
  }
  
  // set parameters and allocate memory
      
  len_data_ = npts;
  try {
     Xext_.resize(len_data_);
     Yext_.resize(len_data_); 
    YextP_.resize(len_data_);
    YextM_.resize(len_data_); 
  } 
  catch (std::bad_alloc &ba) { 
    OptimMessageBox::warning(this, "System error", "Not enough memory", QMessageBox::Ok);
    return;
  }
      
  name_ext_ = FileName;
  dcol_m_   = DataCol+1;

  // allocate memory for names and s coordinates

  if(NSt.SfromOptics){
    elmNames_.resize(len_data_);
    s.resize(mw->nelm_+1);
    s[0]=0.0;  

    for(int i=0; i<mw->nelm_-1; ++i) {
       s[i+1] = s[i] + mw->beamline_[i]->length();
    }
  }
  // read data

  fp = fopen(FileName.c_str(),"r");

  int row = 0;
  while (fgets(buf, 255, fp) ) {   

    if(buf[0]=='#') continue;

    for( int i=0; i<FDATA_LEN; ++i)  { pword[i] = words[i]; }

    if(split(buf,FDATA_LEN, pword)<2) continue;

    if(NSt.SfromOptics) { // obtain s position from element location in optics file  

      elname = std::string(NSt.NamePrefix) + pword[NameCol];

      int j = 0; 
      for(; j<mw->nelm_; ++j){
        if (elname == mw->beamline_[j]->fullName() )  break;
      }
      
      if(j == mw->nelm_) continue;  // element not found 
      
      elmNames_[row] = std::string(elname);
         Xext_[row] = s[j+1]*0.01; // convert s-position from cm to m (for plotting) 

      decodeExtLine(buf, dat, FDATA_LEN);
      if(!filterName(pword[NameCol], NSt.filter, NSt.MatchCase) ) continue;
    }
    else {
      if( !( (decodeExtLine(buf, dat, FDATA_LEN)>=2) &&
	     (dat[Scol] >=offset ) || (dat[Scol] < (offset + mw->Length_)) ) ) continue;

      if(decodeExtLine(buf, dat, FDATA_LEN) < MaxCol) continue;
      Xext_[row] = (dat[Scol]-offset)*0.01+Xoffs_;

    } // if(NSt.SfromOptics)

    Yext_[row]  = scale_ * dat[DataCol];
    YextM_[row] = scale_ * (( ErrmCol < 0) ? dat[DataCol] : dat[ErrmCol]);
    YextP_[row] = scale_ * (( ErrpCol < 0) ? dat[DataCol] : dat[ErrpCol]);

    ++row;

    if( row == len_data_ ) break;

  } // while  (fgets(buf, 255, fp) 

  fclose(fp);

  OptimIntervalCurve* mdata = new OptimIntervalCurve(QString("Data_")+QString("%1").arg(ndata_));
  mdata->setStyle( mdata_data_only_ ? OptimIntervalCurve::NoCurve : OptimIntervalCurve::CurveStyle(OptimIntervalCurve::UserCurve + OptimIntervalCurve::Line) );

  //mdata->setPen (QColor( colors_[color_data_].c_str() ),
  //		 Globals::preferences().plot_preferences.lwidth, Qt::NoPen);
 
  QColor color( colors_[color_data_].c_str() );
  color.setAlpha(75);
  mdata->setPen( QPen(color, 2, Qt:: DotLine) );
  mdata->setBrush( color );

  // prepare the plot data

  QVector<QwtIntervalSample> sv;

  for ( int i=0; i<npts; ++i) { 
   double x    =  Xext_[i];
   double ymin = YextM_[i];
   double ymax = YextP_[i];
   sv.push_back( QwtIntervalSample(x, QwtInterval(ymin, ymax) ) );
  };

  mdata->setSamples(sv);  

  if (!data_label_) { 
     data_label_ = new QwtPlotTextLabel();
     data_label_->setMargin(45);
     data_label_->attach(this);
  }

  datatxt_ += mdata->title().text() + " : " + QString(FileName.c_str()) + QString(" * %1").arg(scale_)+ QString("\n");

  if ( mdata_show_legend_ ) {
     QString tmpstr( datatxt_);
     if (!mdata_show_full_path_) {
#if QT_VERSION >= 0x050000
       QRegularExpression re(R"(((/.*/)|(\\.*\\)))");
#else
       QRegExp re(R"(((/.*/)|(\\.*\\)))");
#endif
       tmpstr.remove(re);
     };
     QwtText dataqwt( tmpstr );
     dataqwt.setRenderFlags( Qt::AlignLeft | Qt::AlignTop );
     data_label_->setText(dataqwt);  
  }
  else {
     data_label_->setText(QwtText(""));  
  } 

  
  //QwtIntervalSymbol* symbol = new QwtIntervalSymbol(QwtIntervalSymbol::Box);
  OptimIntervalSymbol* symbol = new OptimIntervalSymbol(OptimIntervalSymbol::Bar);
  symbol->setBrush(Qt::SolidPattern);
  QColor clr( colors_[color_data_].c_str() );
  symbol->setPen(clr, 1.0, Qt::SolidLine);  

  mdata->setSymbol( symbol );
  mdata->setYAxis(QwtPlot::yLeft);
  mdata->setRenderHint( QwtPlotItem::RenderAntialiased, true );
 
  mdata->attach(this);

  ++ndata_; // counter for the number of data sets read so far 

  replot();
  return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::fixedLimits( QwtPlot::Axis axisid)
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

void OptimPlot::options() 
{

  opts_dialog_->enableLegoOptions(lego_);
  
  opts_dialog_->data_ = plot_opts_;
  opts_dialog_->set();

  //if ( opts_dialog_->exec() == QDialog::Rejected) return;
  //plot_opts_ = opts_dialog_->data_;
  //setOptions();
  
  opts_dialog_->setCallback( [this](){ plot_opts_ = opts_dialog_->data_; setOptions();});  
  opts_dialog_->show();
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   OptimPlot::saveAs()
{

  QString filters = "pdf (*.pdf *.PDF) ;; svg (*.svg *.SVG) ;; ps (*.ps *.PS);; txt (*.txt *.TXT);;" ;   
  auto formats = QImageWriter::supportedImageFormats(); 
  for (auto it = formats.begin(); it != formats.end(); ++it) { 
    if (QString(*it).toUpper() == *it) continue;
    filters += (*it) + " (*." + *it + " *." + QString(*it).toUpper() + ")" + ";;"; 
  }

  QString filename = plot_opts_.title.text() + "-" + QDateTime::currentDateTime().toString(Qt::ISODate);
  //filename.replace(QRegExp("\\s+"),"_").replace(":","-");
  filename.replace(QRegularExpression("\\s+"),"_").replace(":","-");
  
  static CustomFileDialog* dialog = 0;
  if (!dialog) {
      dialog = new CustomFileDialog(0, "Save Plot As");
      dialog->setNameFilter(filters);
      dialog->setFileMode(QFileDialog::AnyFile);
      dialog->setViewMode(QFileDialog::Detail);
      dialog->setAcceptMode(QFileDialog::AcceptSave);
      dialog->setDirectory(QDir::current());
  }

#ifdef __linux__  
  QString extension = dialog->selectedNameFilter(); 
  QRegExp rx("^[^\\s]+");
  auto idx = rx.indexIn(extension);  
  auto len = rx.matchedLength();  
  dialog->selectFile(filename + "." + extension.left(len));
#else   
  dialog->selectFile(filename);
#endif
  

  if ( dialog->exec() != (QFileDialog::AcceptSave) ) return;

  filename = dialog->selectedFiles().first();
  if (filename == "") return;

  
  if ( QFileInfo(filename).suffix().toUpper() == "TXT") {
    this->setUpdatesEnabled(false);
    saveAsText(filename); 
    this->setUpdatesEnabled(true);
  } 
  else {  
    std::unique_ptr<QwtPlotRenderer> renderer( new  QwtPlotRenderer(0) ); 
    //renderer->setDiscardFlags( 	 QwtPlotRenderer::DiscardBackground  |  
    //                             QwtPlotRenderer::DiscardCanvasBackground );
    // renderer->setDiscardFlags( 	 QwtPlotRenderer::DiscardNone  );

    renderer->setDiscardFlags( 	 QwtPlotRenderer::DiscardBackground  );

    this->setUpdatesEnabled(false);
    this->setTitle(plot_opts_.title.text());

    renderer->renderDocument(this, filename,
	      QSizeF(Globals::preferences().plot_preferences.hsize*25.4,
		     Globals::preferences().plot_preferences.vsize*25.4),
		     Globals::preferences().plot_preferences.resolution); 
    //this->setTitle(""); 
    this->setUpdatesEnabled(true);
  } 
}   

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::saveAsText( QString const& file)
{

  std::ofstream os(file.toUtf8().data());  
  QwtPlotItemList const& curves = itemList(QwtPlotItem::Rtti_PlotCurve);
  
  int ncurves = 0;
  auto it = curves.begin();
  for ( ; it != curves.end(); ++it) { 
    if (!(*it)->isVisible()) continue;
    break; // first visible curve
  }

  // save pointer to first visible curve
  QwtPlotCurve const* c1 = dynamic_cast<QwtPlotCurve*>(*it);
    
  auto cv = dynamic_cast<QwtPlotCurve*>(*it);
  unsigned int npts = cv->dataSize();
  
  bool sampling_is_same = false; 
    
  for ( ; it != curves.end(); ++it) { 
    if (!(*it)->isVisible() ) continue;
    ++ncurves;
    auto cv = dynamic_cast<QwtPlotCurve*>(*it);
    sampling_is_same = (npts == cv->dataSize());
  }

  os << "#------------------------------------------------------------------------------" << std::endl;
  os << "# " <<  QDateTime::currentDateTime().toString(Qt::ISODate).toUtf8().data() << std::endl;
  os << "# " <<  plot_opts_.title.text().toUtf8().data() << std::endl;
  os << "# " <<  "Number of curves: " << ncurves << std::endl; 
  os << "#------------------------------------------------------------------------------" << std::endl;
    
  if ( ncurves == 0) return; 

  if (sampling_is_same)  {//CURVE SAMPLING IS NOT THE SAME FOR ALL CURVES; output table with multiple columns.
   
   os << "# s[m] "; 
   for (auto it = curves.begin(); it != curves.end(); ++it) { 
     if (!(*it)->isVisible() ) continue;
     os << (*it)->title().text().toUtf8().data() << "  ";  
    }

   os << std::endl;
   
   os.setf( std::ios_base::scientific | std::ios_base::left); 
   os.precision(5);

   for (int i=0; i< npts; ++i) { 
      os << std::setw(16) << c1->sample(i).rx() << " ";  
      for (auto it = curves.begin(); it != curves.end(); ++it) { 
        if (!(*it)->isVisible() ) continue;
        auto cv = dynamic_cast<QwtPlotCurve*>(*it);
	QPointF point =  cv->sample(i);
        os << std::setw(16) << point.ry() << " ";  
      }
      os << std::endl;
    }
  }
  else { //CURVE SAMPLING IS NOT THE SAME FOR ALL CURVES; output one table per curve.

    for (auto it = curves.begin(); it != curves.end(); ++it) { 
      auto cv = reinterpret_cast<QwtPlotCurve*>(*it); 
  
      os << "#------------------------------------------------------------------------------" << std::endl;
      os << "# " <<  cv->title().text().toUtf8().data() << std::endl;
      os << "#------------------------------------------------------------------------------" << std::endl;
    
      int n = cv->dataSize();
      os.setf( std::ios_base::scientific | std::ios_base::left); 
      os.precision(5); 
      for (int i=0; i<n; ++i) { 
        QPointF point =  cv->sample(i);
        os <<  std::setw(16) << point.rx() << "  " << std::setw(16) << point.ry() << std::endl;  
      }
    }
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  OptimPlot::clear()
{  

  // Save the state of the curves

  curve_states_.resize(0);

  auto list = itemList();
  for (auto it =  list.begin(); it != list.end(); ++it) {
    if ( dynamic_cast<DataCurve*>( *it ) ) continue;  // skip external data curves. 

    if ((*it)->rtti() == QwtPlotItem::Rtti_PlotCurve) {
      curve_states_.push_back(CurveState{  (*it)->title().text().toUtf8().data(), (*it)->isVisible() });
    }
  }

  // Clear all curves associated with computations.
  // External data curves are left intact.

  detachItems( QwtPlotItem::Rtti_PlotLegend,    true);  // remove and delete all legends

  QList<QwtPlotItem*> to_remove;

  list = itemList();
  for (auto it =  list.begin(); it != list.end(); ++it) {
    if ( dynamic_cast<DataCurve*>( *it ) )            continue;
    if ((*it)->rtti() == QwtPlotItem::Rtti_PlotCurve) { 
      to_remove.append( &(**it) );
    }
  }
 
  for (auto it = to_remove.begin(); it != to_remove.end(); ++it) {
    removeItem( *it ) ;  // remove and delete all existing no-data curves 
    delete ( *it );
  }
  
 // *** DO NOT delete the legend entries for the data points ++++ FIXME !

 // detachItems( QwtPlotItem::Rtti_PlotTextLabel, true);  // remove and delete all text
 // detachItems( QwtPlotItem::Rtti_PlotUserItem,  true);  // detach & delete lego 
 // lego_       =  0;
 // data_label_ =  0;      
 // datatxt_    = "";
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::replot() 
{
  QMdiSubWindow* sw = qobject_cast<QMdiSubWindow*>(parent());
  if (!sw) return;

  if (sw->property("attached") == QVariant()) { QwtPlot::replot();}
  if ( sw->property("attached").toBool() ) {
    setCanvasBackground(QBrush(QColor("white")));
    sw->setWindowTitle( sw->windowTitle().replace(" [Detached]", ""));
  }
  else { 
    setCanvasBackground(QBrush(QColor("whitesmoke")));
    sw->setWindowTitle( sw->windowTitle().replace(" [Detached]", "") +  " [Detached]");
  }
  QwtPlot::replot();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::plotIsZoomed(const QRectF& rect)
{

 // if the plot is zoomed, we need to update the
 // limit attributes.   

 QwtInterval interval = axisInterval(xBottom);
 plot_opts_.min_hor  = interval.minValue();
 plot_opts_.max_hor  = interval.maxValue();

 interval = axisInterval(yLeft);
 plot_opts_.min_left    = interval.minValue();
 plot_opts_.max_left    = interval.maxValue();

 interval = axisInterval(yRight);
 plot_opts_.min_right   = interval.minValue(); 
 plot_opts_.max_right   = interval.maxValue();


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimPlot::setAxesTitleFont(QFont const& font)
{
  QFont f(font);
  f.setWeight(QFont::Normal);

  plot_opts_.axis_title_left.setFont(f);
  plot_opts_.axis_title_right.setFont(f);
  //plot_opts_.axis_title_hor.setFont(f);
  plot_opts_.axis_title_hor = QwtText(""); //

  //plot_opts_.title.setFont(f);
   
  //setTitle( plot_opts_.title  );
  setAxisTitle( QwtPlot::yRight,   plot_opts_.axis_title_right );
  setAxisTitle( QwtPlot::yLeft,    plot_opts_.axis_title_left );
  setAxisTitle( QwtPlot::xBottom,  plot_opts_.axis_title_hor );

  QFont axisfont = axisWidget(QwtPlot::xBottom)->font();
  axisfont.setPointSize(font.pointSize());			
  axisfont.setWeight(QFont::Normal);			

  axisWidget(QwtPlot::xBottom)->setFont(axisfont);
  axisWidget(QwtPlot::yLeft)->setFont(axisfont);
  axisWidget(QwtPlot::yRight)->setFont(axisfont);


  replot();     
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::setPlotTitleFont(QFont const& font)
{
  titleLabel()->setFont(font);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::setPlotTitle(QString const& s)
{
  setTitle(s);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QSize OptimPlot::minimumSizeHint() 
{
  return QSize(0,0);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::setDisplayStyle(int flags)
{
  //  0x1 == enable compact scales 
  //  0x2 == enable compact legend 
  
  if ( legend())  insertLegend(0);             // legend() is managed by layout 
  if ( legend_ )  delete legend_;  legend_ = 0;// legend_  is NOT managed by layout
    
  if (flags & 0x2) { // compact legend  
 
    // We use an external QwtLegend with a transparent background on top the plot canvas
    // to achieve an effect similar to a QwtPlotLegendItem. The main motivation is that
    // QwtLegend is interactive (i.e. it responds to mouse clicks). 
     
     legend_ = new CompactLegend(canvas()); // new external legend (displayed on canvas)
     legend_->setDefaultItemMode(QwtLegendData::Checkable);
     //connect( legend_, SIGNAL(    clicked( QVariant const&, int)), 
     //         this,      SLOT(enableCurve( QVariant const&, int)) );   
     connect( legend_, SIGNAL(    checked( QVariant const&, bool, int)), 
              this,      SLOT(enableCurve( QVariant const&, bool, int)) );   
     legend_->setMaxColumns(0); // unlimited
     legend_->setAttribute(Qt::WA_TranslucentBackground);

     //legend_->setFrameShape(QFrame::Box);  

     connect(this,    SIGNAL( legendDataChanged( const QVariant&, const QList<QwtLegendData>& ) ),
	     legend_, SLOT  (      updateLegend( const QVariant&, const QList<QwtLegendData>& ) ) );
     updateLegend();
     legend_->show();
   }
   else {
     auto lgd = new OptimPlotLegend(this);
     //     connect( lgd,    SIGNAL(    clicked(QVariant const&, int)), 
     //       this,     SLOT(enableCurve(QVariant const&, int)) );   
     connect( lgd,    SIGNAL(    checked(QVariant const&, bool, int)), 
              this,     SLOT(enableCurve(QVariant const&, bool, int)) );   
     lgd->setDefaultItemMode(QwtLegendData::Checkable);
     lgd->setMaxColumns(0); // unlimited
     insertLegend(lgd, QwtPlot::BottomLegend);
     lgd->show();
   }

    
   //QFont font = axisWidget(QwtPlot::xBottom)->font();
   //font.setPointSize(6);			
   //axisWidget(QwtPlot::xBottom)->setFont(font);
   //axisWidget(QwtPlot::yLeft)->setFont(font);
   //axisWidget(QwtPlot::yRight)->setFont(font);
   
   if ( flags & 0x1 ) {

      plotLayout()->setCanvasMargin(0);
      plotLayout()->setAlignCanvasToScale(QwtPlot::yLeft,   true);
      plotLayout()->setAlignCanvasToScale(QwtPlot::yRight,  true);
      plotLayout()->setAlignCanvasToScale(QwtPlot::xBottom, true);
      plotLayout()->setAlignCanvasToScale(QwtPlot::xTop,    true);

      auto sc = axisScaleDraw(QwtPlot::yLeft);
      sc->enableComponent( QwtAbstractScaleDraw::Ticks,    false);
      sc->enableComponent( QwtAbstractScaleDraw::Backbone, false);
      sc->enableComponent( QwtAbstractScaleDraw::Labels,   true);

      sc = axisScaleDraw(QwtPlot::yRight);
      sc->enableComponent( QwtAbstractScaleDraw::Ticks,    false);
      sc->enableComponent( QwtAbstractScaleDraw::Backbone, false);
      sc->enableComponent( QwtAbstractScaleDraw::Labels,   true);

      sc = axisScaleDraw(QwtPlot::xBottom);
      sc->enableComponent( QwtAbstractScaleDraw::Ticks,    false);
      sc->enableComponent( QwtAbstractScaleDraw::Backbone, false);
      sc->enableComponent( QwtAbstractScaleDraw::Labels,   true);

      sc = axisScaleDraw(QwtPlot::xTop);
      sc->enableComponent( QwtAbstractScaleDraw::Ticks,    false);
      sc->enableComponent( QwtAbstractScaleDraw::Backbone, false);
      sc->enableComponent( QwtAbstractScaleDraw::Labels,   false);
 
      axisWidget(xBottom)->setTitle(plot_opts_.axis_title_hor);
      axisWidget(yLeft)->setTitle(plot_opts_.axis_title_left);
      axisWidget(yRight)->setTitle(plot_opts_.axis_title_right);

      auto scaleitems =  itemList( QwtPlotItem::Rtti_PlotScale);   

      for (auto it=scaleitems.begin(); it !=scaleitems.end(); ++it) {  
       (*it)->show();
      }
  }
  else {
    
   auto scaleitems =  itemList( QwtPlotItem::Rtti_PlotScale);   

   for (auto it=scaleitems.begin();
	     it !=scaleitems.end(); ++it) {  
     (*it)->hide();
   }
     
   auto sc = axisScaleDraw(QwtPlot::yLeft);
   sc->enableComponent( QwtAbstractScaleDraw::Ticks,    true);
   sc->enableComponent( QwtAbstractScaleDraw::Backbone, true);

   sc = axisScaleDraw(QwtPlot::yRight);
   sc->enableComponent( QwtAbstractScaleDraw::Ticks,    true);
   sc->enableComponent( QwtAbstractScaleDraw::Backbone, true);

   sc = axisScaleDraw(QwtPlot::xBottom);
   sc->enableComponent( QwtAbstractScaleDraw::Ticks,    true);
   sc->enableComponent( QwtAbstractScaleDraw::Backbone, true);

#if 0    
   if (!legend() && legend_) { delete legend_; legend_ = 0; }

   legend_ = new QwtLegend(this);
   connect( legend_, SIGNAL(clicked(const QVariant &, int)), 
             this,   SLOT(enableCurve(const QVariant &, int)) );   
   legend_->setDefaultItemMode(QwtLegendData::Checkable);
   insertLegend(legend_, QwtPlot::BottomLegend);

   axisWidget(xBottom)->setTitle(plot_opts_.axis_title_hor);
   axisWidget(yLeft)->setTitle(plot_opts_.axis_title_left);
   axisWidget(yRight)->setTitle(plot_opts_.axis_title_right);
#endif
   
  } // if(flags & 0x1) 

  setProperty("compact",flags);

  axisScaleDraw( QwtPlot::yLeft  )->enableComponent(QwtAbstractScaleDraw::Labels,  true );
  axisScaleDraw( QwtPlot::yRight )->enableComponent(QwtAbstractScaleDraw::Labels, true );
  axisScaleDraw( QwtPlot::xBottom)->enableComponent(QwtAbstractScaleDraw::Labels, true );
  
  updateLayout();

  //--------------------------------------------------------------------
  // We send a dummy resize event to the OptimPlot widget. This is done
  // because widget window geometry is not modified by updateLayout()
  // i.e. only the internal layout is modified. The CompactLegend
  // is managed independently and gets adjusted only when the
  // OptimPlot  widget geometry is modified. Perhaps implementing
  // a custom layout manager would be a better and cleaner
  // solution.
  //--------------------------------------------------------------------
  
  QSize sz    = size();
  QSize oldsz = sz;
  OptimApp::sendEvent(this, new  QResizeEvent(sz, oldsz) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

   
PlotOptions const&  OptimPlot::getPlotOpts() const
{
  return plot_opts_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::setPlotOpts(PlotOptions const& opts)
{
  plot_opts_ = opts;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::usePlotState(OptimPlot const* p)
{
  // use the state of a plot p to initialize. 
  // DataCurve elements are ignored.
  
  auto list = p->itemList();

  for (auto itc = list.begin(); itc != list.end(); ++itc) {
    if (  dynamic_cast<DataCurve*>( *itc ) )    continue;
    if ( !dynamic_cast<QwtPlotCurve*>( *itc ) ) continue;
    QwtPlotItem* cv = findPlotItemByName( (*itc)->title().text() ); 
    if (cv) {
       cv->setVisible( (*itc)->isVisible() );
    }
  }

  plot_opts_ = p->plot_opts_;
  setOptions();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::resizeEvent(QResizeEvent* e)
{
  QwtPlot::resizeEvent(e); // perform the resize first

  // now position the compact legend
  
  CompactLegend* w = qobject_cast<CompactLegend*>(legend_);
  if (w) { w->setGeometry( 0, canvas()->height()-40, canvas()->width(), 20);} 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::setPreferences()
{
    static QPalette pdefault = palette();
    QPalette p = palette();
    if (!Globals::preferences().plot_preferences.coloredframe) {
      setFrameStyle( QFrame::NoFrame );
      p.setColor(QPalette::Window, Qt::white);
      p.setBrush(QPalette::Window, QBrush(Qt::white));
      setPalette(p);
      setAutoFillBackground(true);
      ((QwtPlotCanvas*) canvas())->setLineWidth(0);
    }
    else {
      setFrameStyle( QFrame::NoFrame );
      setPalette(pdefault);
      setAutoFillBackground(true);
      ((QwtPlotCanvas*) canvas())->setLineWidth(0);
    };

    setDisplayStyle(Globals::preferences().plot_preferences.compact);

    if (Globals::preferences().plot_preferences.timestamp) {
	tsitem_->show();
    }
      else {
	tsitem_->hide();
    };
}  


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::setOptions()
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

  zoomer1_->setEnabled(plot_opts_.leftActive  && (plot_opts_.leftZoomOn  || plot_opts_.horZoomOn) );   // left zoom en/dis-abled 
  zoomer2_->setEnabled(plot_opts_.rightActive && (plot_opts_.rightZoomOn || plot_opts_.horZoomOn) );   // left zoom en/dis-abled 

  tracker1_->setEnabled(!zoomer1_->isEnabled() && plot_opts_.leftActive );
  tracker2_->setEnabled(!zoomer2_->isEnabled() && plot_opts_.rightActive);

  distance_picker_left_->setEnabled( plot_opts_.leftActive);
  distance_picker_right_->setEnabled( plot_opts_.rightActive);

  zoomer1_->setConstrainedAxis( xBottom, !plot_opts_.horZoomOn);
  zoomer1_->setConstrainedAxis( yLeft,   !plot_opts_.leftZoomOn);
  zoomer1_->setConstrainedAxis( yRight,  !plot_opts_.rightZoomOn);

  zoomer2_->setConstrainedAxis( xBottom, !plot_opts_.horZoomOn);
  zoomer2_->setConstrainedAxis( yLeft,   !plot_opts_.leftZoomOn);
  zoomer2_->setConstrainedAxis( yRight,  !plot_opts_.rightZoomOn);

  if (zoomer1_) zoomer1_->setZoomBase();
  if (zoomer2_) zoomer2_->setZoomBase();

  // title, labels ..

  // This is a kludge. The QwtPlot title is too wide. We use the window title instead.
  //-----------------------------------------------------------------------------------------------
  setTitle(plot_opts_.title);
    
  QMdiSubWindow* sw = qobject_cast<QMdiSubWindow*>(parent());
  if (sw) {
    //sw->setWindowTitle(plot_opts_.title.text());
  }
  // ----------------------------------------------------------------------------------------------
  
  plot_opts_.axis_title_right.setColor((plot_opts_.rightActive ? (QColor("black")) : (QColor("gray")) ) );
  plot_opts_.axis_title_left.setColor( (plot_opts_.leftActive ?  (QColor("black")) : (QColor("gray")) ) );
    
  setAxisTitle( QwtPlot::yRight, plot_opts_.axis_title_right );
  setAxisTitle( QwtPlot::yLeft,  plot_opts_.axis_title_left );
  
  // lego plot ...    

  if(lego_) {
    if (plot_opts_.top_lego)     lego_->setPosition(LegoPosition::Top);
    if (plot_opts_.bottom_lego)  lego_->setPosition(LegoPosition::Bottom); 
  }

  setLegoVisible( !plot_opts_.hide_lego );

  grid_->setYAxis( (plot_opts_.leftActive ?
				QwtPlot::yLeft : QwtPlot::yRight) ); 

  replot();
}
    
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QVariant OptimPlot::findLegendItemInfoByName(QString const& title)
{  
  auto lgd     = qobject_cast<QwtLegend*>(legend());

  if (!lgd) return QVariant(); // null 
  
  auto itemlst = lgd->contentsWidget()->children();

  QwtLegendLabel* w = 0;
  for ( auto cit = itemlst.begin(); cit != itemlst.end(); ++cit) {
    w = qobject_cast<QwtLegendLabel*>(*cit);
    if (!w)  continue;                       // Item is not a legend widget
    if ( title == w->text().text() ) {
      break; // found
    }    
    w = 0;   // not found
  }

  return w ? lgd->itemInfo(w) : QVariant(); 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QwtPlotItem* OptimPlot::findPlotItemByName(QString const& title)
{
  auto itemlst = itemList();

  for ( auto cit = itemlst.begin(); cit != itemlst.end(); ++cit) {
    if ( title == (*cit)->title().text() ) return *cit;
  }

  return 0; // not found
}     

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::removeDuplicateCurves()
{
   auto lgd     = qobject_cast<QwtLegend*>(legend());
   if (!lgd) return;

   QList<QwtPlotItem*> infolst;
   auto children = lgd->contentsWidget()->children(); // the legend widget children 

   std::for_each( children.begin(), children.end(),  [&lgd, &infolst](QObject* o){
                                                      auto p = lgd->itemInfo( qobject_cast<QwtLegendLabel*>(o)).value<QwtPlotItem*>();
						      if (p) infolst.append(p); });

   QwtPlotItemList lst =  itemList(QwtPlotItem::Rtti_PlotCurve);

   auto last = std::remove_if( lst.begin(), lst.end(), [&infolst](QwtPlotItem* item){ if (!infolst.contains(item) ) {
	                                                                                item->attach(0);
										        delete item;
										        return true; 
                                                                                      }
                                                                                      return false;});
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlot::copyToClipboard()
{
  setTitle( QwtText(plot_opts_.title) );
  bool         axis_enabled  = axisEnabled(QwtPlot::yRight);
  QwtInterval  axis_interval = axisInterval(QwtPlot::yRight);
  QwtText      axis_title    = axisTitle(QwtPlot::yRight);

  if (!axis_enabled) enableAxis(QwtPlot::yRight);
  if (!axis_enabled) axisScaleDraw(QwtPlot::yRight)->enableComponent(QwtAbstractScaleDraw::Labels,   false);
  if (!axis_enabled) setAxisTitle(QwtPlot::yRight, "");

#if QT_VERSION < 0x050000
  QImage bitmap(size(), QImage::Format_ARGB32);
  bitmap.fill(Qt::transparent);
  QPainter painter(&bitmap);
  render(&painter, QPoint(), QRegion(), QWidget::DrawChildren);
  QClipboard* clipboard = QApplication::clipboard();
  clipboard->clear();
  clipboard->setImage(bitmap);
#else
  QPixmap pixmap = grab();
  QClipboard* clipboard = QApplication::clipboard();
  clipboard->clear();
  clipboard->setPixmap(pixmap);  
#endif

  //setTitle( QwtText(""));
  if (axis_enabled) enableAxis(QwtPlot::yRight);
  if (axis_enabled) axisScaleDraw(QwtPlot::yRight)->enableComponent(QwtAbstractScaleDraw::Labels,   true);
  if (axis_enabled) setAxisTitle(QwtPlot::yRight, axis_title);

}

