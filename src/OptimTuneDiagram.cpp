//  =================================================================
//
//  OptimTuneDiagram.cpp
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
#include <iomanip>
#include <memory>
#include <set>
#include <tuple>

#include <QString>
#include <Constants.h>
#include <OptimTuneDiagram.h>
#include <TuneDiagramSeriesData.h>
#include <TuneDiagramDialog.h>
#include <iostream>
#include <cmath>
#include <QwtText>
#include <QwtPlot>
#include <QwtSymbol>
#include <QwtPlotCurve>
#include <QwtPlotGrid>
#include <QwtPlotZoomer>
#include <QwtPlotRenderer>
#include <QwtPlotLayout>
#include <OptimPlotMarker.h>

using Constants::PI; 

bool isInside( int i,  int j, int k,
	       double  qxmin, double qxmax,
	       double  qymin, double qymax ) 
{

  // --------------------------------------------
  // Determine whether the resonance line (ix + jy = k)
  // is inside (i.e. cuts through) a rectangular region
  // of tune space.
  // ---------------------------------------------
  
  int plus  = 0; 
  int minus = 0;
  int zeros = 0;
  
  double eps1 = ( i*qxmin + j*qymin - k);
  (eps1 >= 0.0) ? ++plus : ++minus; 
  (eps1 == 0.0) ? ++zeros : zeros; 

  double eps2 = ( i*qxmax + j*qymin - k);
  (eps2 >= 0.0) ? ++plus : ++minus; 
  (eps2 == 0.0) ? ++zeros : zeros; 

  double eps3 = ( i*qxmax + j*qymax - k);
  (eps3 >= 0.0) ? ++plus : ++minus; 
  (eps3 == 0.0) ? ++zeros : zeros; 

  double eps4 = ( i*qxmin + j*qymax - k);
  (eps4 >= 0.0) ? ++plus : ++minus; 
  (eps4 == 0.0) ? ++zeros : zeros; 

  
  bool inside = !( (plus == 4) || (minus == 4) ); 
  if (zeros == 2 ) inside = true; 
  
  return inside;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int gcd( int a, int b)
{
  // find greatest common divider
  
   return b == 0 ? a : gcd(b, a % b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void gcd_normalize(int& in, int& jn, int& kn)
{
  //-------------------------------------------- 
  // normalize the equation coefficients so that
  // they are co-primes.
  //--------------------------------------------
  
  int cmnfactor = gcd( in, jn);
  cmnfactor     = gcd( cmnfactor, kn);
  in /= cmnfactor;
  jn /= cmnfactor;
  kn /= cmnfactor;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimTuneDiagram::OptimTuneDiagram (QWidget* parent)
  :  QwtPlot(parent)
{
  
  setCanvasBackground( (QColor("white")) ); 
  QwtPlotGrid* grid = new QwtPlotGrid();
  grid->setMajorPen( QColor("gray"), 1.0, Qt::DotLine);
  grid->setMinorPen( QColor("gray"), 1.0, Qt::DotLine);
  grid->enableXMin(true);
  grid->enableYMin(true);

  grid->attach(this);

  zoomer_ = new QwtPlotZoomer(canvas());
  plotLayout()->setAlignCanvasToScales(true);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimTuneDiagram::~OptimTuneDiagram()
{ }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTuneDiagram::setup( TuneDiagramDialog::TuneDialogData const* diagramdata )
{ 

  auto comp = [](std::tuple<int,int,int> const& lhs,
	         std::tuple<int,int,int> const& rhs)->bool {
						        int lhs0 = std::get<0>(lhs);  
						        int rhs0 = std::get<0>(rhs);  
							if (lhs0 !=  rhs0) return lhs0 <  rhs0;
						        int lhs1 = std::get<1>(lhs);  
						        int rhs1 = std::get<1>(rhs);  
                                                        if (lhs1 !=  rhs1) return lhs1 <  rhs1;
        					        int rhs2 = std::get<2>(rhs);  
						        int lhs2 = std::get<2>(lhs);  
	                                                return lhs2 <  rhs2;
						    };

  std::set<std::tuple<int,int,int>,decltype(comp)> resonance_lines(comp);
						    
  auto exists = [&resonance_lines](int i, int j, int k){ auto found = resonance_lines.find( std::make_tuple(i,j,k));
							 return !( found ==  resonance_lines.end());
                                                       };
  
  // qxmin  ... qymax are declared static because the TuneDiagramSeriesData objects
  // hold these values by reference. 

  static double qxmin             = 0.0;               
  static double qxmax             = 1.0;             
  static double qymin             = 0.0;              
  static double qymax             = 1.0;             
  qxmin                           = diagramdata->qxmin;               
  qxmax                           = diagramdata->qxmax;             
  qymin                           = diagramdata->qymin;              
  qymax                           = diagramdata->qymax;             
  int    minorder                 = diagramdata->minorder;           
  int    maxorder                 = diagramdata->maxorder;           
  bool   sumresonances            = diagramdata->sumresonances;     
  bool   couplingresonances       = diagramdata->couplingresonances; 
  bool   captions                 = diagramdata->captions;       

  //int    qxintervals              = diagramdata->qxintervals;
  //int    qyintervals              = diagramdata->qyintervals;
  double qx                       = diagramdata->qx;
  double qy                       = diagramdata->qy;

 
  auto position = []( double x, int i, int j, int k) { return (j==0) ? 0.4 : ( double(k) - x*double(i) )/double(j);};     
  
  detachItems 	( QwtPlotItem::Rtti_PlotCurve,  true ); // delete the existing curves, if any   
  detachItems 	( QwtPlotItem::Rtti_PlotMarker, true ); // delete the existing markers, if any   

  setAxisScale( QwtPlot::xBottom, qxmin, qxmax); 
  setAxisScale( QwtPlot::yLeft,   qymin, qymax); 

  //int nplots = 0;

  int in = 0;
  int jn = 0;
  int kn = 0;

  auto curve     = new QwtPlotCurve("Operating Point");
  curve->setSamples( &qx, &qy, 1);
  auto symbol = new QwtSymbol(QwtSymbol::Star2);
  curve->setZ(100); // on top
  symbol->setSize(15);
  symbol->setColor(QColor("red"));
  curve->setSymbol( symbol);
  curve->attach(this);
  
   int maxtune = std::max(qxmax,qymax); 
   for (int i = 0; i <= maxorder; ++i ) { 
    for (int j = 0; j <= maxorder; ++j ) { 
      for (int k = 0; k <= 20*maxtune*maxorder; ++k ) {  // what should be the max value of k ??? 

        if ( i == 0 && j == 0 && k == 0)   continue; 

        in = i;
        jn = j;
        kn = k;
	
	gcd_normalize(in, jn, kn);  
	
	if (  in+jn == 0       ) continue;   
	if (  in+jn < minorder ) continue;   
	if (  in+jn > maxorder ) continue;   
  
        TuneDiagramSeriesData* data  = 0; 
        QwtPlotCurve*          curve = 0;  


       if (sumresonances) {
	 if (  isInside( in,jn,kn, qxmin, qxmax,  qymin, qymax) &&  !exists(in,jn,kn) ) { // is the line in*x +jn*y = kn inside the rectangle ?  
	   resonance_lines.insert( std::make_tuple(in,jn,kn) );
	   auto data   =  new TuneDiagramSeriesData( in, jn, kn, qxmin, qxmax, qymin, qymax); 
           curve       =  new QwtPlotCurve();
           curve->setPen( QColor("blue"), 2.0/log(1.3 + abs(in)+abs(jn) ), Qt::SolidLine); // line thickness decreases with resonance order 
           curve->setSamples(data);
           curve->attach(this);
            if ( captions ) {
	     double length = sqrt( (qxmax-qxmin)*(qxmax-qxmin) + (qymax-qymin)*(qymax-qymin) );
	     double xpos = 0.327; // qxmin + length * double(in)/sqrt(double((in+jn)*(in+jn))) * 0.5; 
             double angle = (jn==0) ? 90.0 :  atan2(double(-in), double(jn))*180.0/PI;
	     auto marker =  new OptimPlotMarker({in,jn,kn, angle});

	     QString qxlabel;
	     if (in != 0)  qxlabel = (in == 1 ? QString("qx") : QString("%1 qx ").arg(in));
	     QString qylabel;
	     if (jn != 0)  qylabel = (jn == 1 ? QString("qy") : QString("%1 qy").arg(jn));

	     if (in == 0 ) {
	       marker->setLabel( qylabel + QString(" = %1").arg(kn) );
	     }
	     else if (jn == 0 ) {
	       marker->setLabel( qxlabel + QString(" = %1").arg(kn) );
             }
	     else {
	       marker->setLabel( qxlabel + " + " + qylabel + QString(" = %1").arg(kn) );
	     }	       
	     marker->setValue(xpos, position(xpos,in, jn, kn));
	     marker->attach(this);
          }
	} 
       }
       
       if ((in == 0) || (jn ==0 )) continue; // main sum resonances, no coupling resonances with same integers 


	if (couplingresonances ) {
	 if ( isInside( in,-jn,kn, qxmin, qxmax,  qymin, qymax) && !exists(in,-jn,kn) ) {
	   resonance_lines.insert( std::make_tuple(in,-jn,kn) );
	   auto data   =  new TuneDiagramSeriesData( in, -jn, kn, qxmin, qxmax, qymin, qymax); 
	   curve       =  new QwtPlotCurve();
	   curve->setPen( QColor("magenta"), 2.0/log(1.3+abs(i)+abs(j)), Qt::DashLine);
           curve->setSamples(data);
           curve->attach(this);
           if ( (in!=0) && (jn!=0) && captions ) {
	     double length = sqrt( (qxmax-qxmin)*(qxmax-qxmin) + (qymax-qymin)*(qymax-qymin) );
	     double xpos = 0.27; // qxmin + length * double(in)/sqrt(double((in+jn)*(in+jn))) * 0.5; 
             double angle = atan2(double(in), double(jn))*180.0/PI;
	     auto marker =  new OptimPlotMarker({in,-jn, kn, angle});
	     QString qxlabel;
	     qxlabel = (in == 1 ? QString("qx") : QString("%1 qx ").arg(in));
	     QString qylabel;
	     qylabel = (jn == 1 ? QString("- qy") : QString(" - %1 qy").arg(jn));
	     marker->setLabel( qxlabel + qylabel + QString(" = %1").arg(kn) );
	     marker->setValue(xpos, position(xpos,in,-jn, kn));
	     marker->attach(this);
          }
	 }
         if ( (k!=0 ) && isInside( -in,jn,kn, qxmin, qxmax,  qymin, qymax) && !exists(-in,jn,kn) ) { // eliminate redundant case -nx x + ny y = 0 (same as nx x - ny y = 0)
	   resonance_lines.insert(std::make_tuple(-in,jn,kn) );
	   auto data = new TuneDiagramSeriesData( -in, jn, kn, qxmin, qxmax, qymin, qymax);
	   curve     = new QwtPlotCurve();
	   curve->setPen( QColor("magenta"), 2.0/log(1.3+abs(i)+abs(j)), Qt::DashLine);
           curve->setSamples(data);
           curve->attach(this);
             if ( (in!=0) && (jn!=0) && captions ) {
	     double length = sqrt( (qxmax-qxmin)*(qxmax-qxmin) + (qymax-qymin)*(qymax-qymin) );
	     double xpos = 0.1; // qxmin + length * double(in)/sqrt(double((in+jn)*(in+jn))) * 0.5; 
             double angle = atan2(double(in), double(jn))*180.0/PI;
	     auto marker =  new OptimPlotMarker({-in,jn,kn,angle});
	     
	     marker->setLabel( (in == 1 ? QString(" -qx") : QString("-%1 qx").arg(in))
			     + (jn == 1 ? QString(" + qy = %1").arg(kn) : QString(" + %1 qy = %2").arg(jn).arg(kn)));
	     marker->setValue(xpos, position(xpos,-in, jn, kn));
	     marker->attach(this);
          }
         }	   
       } // couplingresonances
      } // for int k
    } // for int j
  } //for int i

  //std::cout << "nplots = " << nplots << std::endl;

  zoomer_->setZoomBase(); 
  replot();  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void   OptimTuneDiagram::saveAs()
{
  std::unique_ptr<QwtPlotRenderer> renderer(new  QwtPlotRenderer(0)); 
  renderer->setDiscardFlags( 	 QwtPlotRenderer::DiscardBackground  |  
                                 QwtPlotRenderer::DiscardCanvasBackground );

  renderer->exportTo(this, ".", QSizeF(6*25.4, 4*25.4), 300); 


}   
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
