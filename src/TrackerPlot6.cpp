//  =================================================================
//
//  TrackerPlot6.cpp
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
#include <TrackerPlot6.h>
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

  TrackerPlot*       plot_;
  QwtScaleWidget*    axisw_;
  QwtAxis::Position  axisid_;
  PlotOptions        plot_opts_; 
};
    
} // namespace

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TrackerPlot6::TrackerPlot6(QWidget* parent) :
  QWidget(parent)
{
  glayout_ = new QGridLayout(this);
    
  plotxy_  = new  TrackerPlot(this); 
  plotxxp_ = new  TrackerPlot(this); 
  plotyyp_ = new  TrackerPlot(this); 
  plotzzp_ = new  TrackerPlot(this); 

  glayout_->addWidget(plotxxp_,0,0); 
  glayout_->addWidget(plotyyp_,0,1); 
  glayout_->addWidget(plotzzp_,1,0); 
  glayout_->addWidget(plotxy_, 1,1); 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerPlot6::clear()
{
 plotxy_->detachItems(Optim::Rtti_ScatterPlotItem);
 plotxxp_->detachItems(Optim::Rtti_ScatterPlotItem);
 plotyyp_->detachItems(Optim::Rtti_ScatterPlotItem);
 plotzzp_->detachItems(Optim::Rtti_ScatterPlotItem);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerPlot6::replot()
{
  plotxy_->replot(); 
  plotxxp_->replot(); 
  plotyyp_->replot(); 
  plotzzp_->replot(); 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerPlot6::setView(ViewType view)
{
  QwtPlot* plots[] = {plotxy_, plotxxp_, plotyyp_, plotzzp_};
  for (auto p:plots) {
    auto lst = p->itemList(Optim::Rtti_ScatterPlotItem);
    for (auto it: lst) {
      auto i = dynamic_cast<ScatterPlotItem*>(it);
      if (!i) return;
      if (i->viewType() == view ) {
	i->show();
      }
      else {
      i->hide();
      }
    }
 }
 replot();
 return;
}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerPlot6::enableZoomMode( bool on)
{}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerPlot6::fixedLimits( QwtAxis::Position axisid)
{}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerPlot6::setOptions(QwtAxis::Position axisid)
{}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerPlot6::setData(TrackingScatterData const& data)
{
   auto scatter = std::make_shared<TrackingScatterData>(data);

   struct Plot {  TrackerPlot* plot; PlaneType plane; };
   Plot plots[] = { {plotxxp_, PlaneType::X_Xp}, {plotyyp_,PlaneType::Y_Yp},
		    {plotzzp_, PlaneType::S_P}, {plotxy_, PlaneType::X_Y}};

   for ( auto& it : plots) {
     auto s = new ScatterPlotItem(scatter); 
     s->setPlane(it.plane);
     s->attach(it.plot);
     it.plot->setPlane(it.plane);
     it.plot->replot();
   }  

   //.......................................................................

   /*
   auto limits = [ &data](int i) { 
     double xmin = (*std::min_element(data.begin(), data.end(), [&i](auto const& lhs, auto const&  rhs){ return lhs[i]<rhs[i];}))[i];
     double xmax = (*std::max_element(data.begin(), data.end(), [&i](auto const& lhs, auto const&  rhs){ return lhs[i]<rhs[i];}))[i];
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
   

   auto hxy = std::make_unique<Histogram2D>(nx, xmin, xmax, ny, ymin, ymax); 
  

   // assign points to the histogram 
   for (auto const& state: data ) {
     (*hxy)(state[0], state[2]);
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


   for (int i=0; i<nx; ++i) {
     for (int j=0; j<ny; ++j) {
       vxy.push_back( double(hxy.at(i,j))/vmax);
     }
   };
   
  
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

   */
   return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TrackerPlot6::setData(PoincareScatterData const& data)
{
   auto scatter = std::make_shared<PoincareScatterData>(data);

   struct Plot {  TrackerPlot* plot; PlaneType plane; };
   Plot plots[] = { {plotxxp_, PlaneType::X_Xp}, {plotyyp_,PlaneType::Y_Yp},
		    {plotzzp_, PlaneType::S_P}, {plotxy_, PlaneType::X_Y}};

   for ( auto& it : plots) {
     auto s = new ScatterPlotItem(scatter); 
     s->setPlane(it.plane);
     s->attach(it.plot);
     it.plot->setPlane(it.plane);
     it.plot->replot();
   }  

   return;
}

/*



   
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

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
