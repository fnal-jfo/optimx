//  =================================================================
//
//  OptimPlotCurve.cpp
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
#include <OptimPlotCurve.h>
#include <QPainter>
#include <QwtPainter>
#include <QwtScaleMap>



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimPlotCurve::OptimPlotCurve ( QString const& title)
  : QwtPlotCurve(title), pxloffset_(1) 
{}

OptimPlotCurve::OptimPlotCurve ( QwtText const& title)
  :QwtPlotCurve(title), pxloffset_(1) 
{}


OptimPlotCurve::~OptimPlotCurve()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlotCurve::drawCurve(QPainter* painter, int style,
		QwtScaleMap const&  xMap,
		QwtScaleMap const&  yMap,
		QRectF      const&  canvasRect,
		int from, int to ) const
{
    switch ( style )
    {
    case Optim::HApertureCurve:
      pxloffset_= 0;   
      drawApertures( painter, xMap, yMap, canvasRect, from, to );
      break;
    case Optim::VApertureCurve:
      pxloffset_= 2;   
      drawApertures( painter, xMap, yMap, canvasRect, from, to );
      break;
     default:
      QwtPlotCurve::drawCurve(painter, style, xMap, yMap, canvasRect, from, to );
     break;
    }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
void OptimPlotCurve::drawApertures( QPainter *painter,
    QwtScaleMap const& xMap,
    QwtScaleMap const& yMap,
    QRectF      const&,
    int from, int to ) const
{
    painter->save();
    painter->setRenderHint( QPainter::Antialiasing, false );

    bool const doAlign = QwtPainter::roundingAlignment( painter );

    //double x0 = xMap.transform( 0.0 );
    //double y0 = yMap.transform( 0.0 );

    //if ( doAlign ) {
    //    x0 = qRound( x0 );
    //    y0 = qRound( y0 );
    //}

    QwtSeriesData<QPointF> const*series = data();

    for ( int i = from; i <= to; ++i )
    {
        QPointF const sample = series->sample( i );
        auto xi  = xMap.transform(  sample.x() );
        auto yi  = yMap.transform(  sample.y() );
        auto yim = yMap.transform( -sample.y() );

	if ( doAlign )
        {
            xi  =  qRound( xi  );
            yi  =  qRound( yi  );
            yim =  qRound( yim  );
        }
  
       	// vertical line 100 pixels 
         
	xi  += pxloffset_;
	
        QwtPainter::drawLine( painter, xi, yi,  xi, yi  - 1000 );
        QwtPainter::drawLine( painter, xi, yim, xi, yim + 1000 );
    }

    painter->restore();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimPlotCurve::rtti () const
{
  return  QwtPlotCurve::rtti();
}
