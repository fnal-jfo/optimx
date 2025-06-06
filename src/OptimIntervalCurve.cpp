//  =================================================================
//
//  OptimIntervalCurve.cpp
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
#include <OptimIntervalCurve.h>
#include <OptimIntervalSymbol.h>
#include <OptimIntervalSample.h>
#include <QwtScaleMap>
#include <QwtClipper>
#include <QwtPainter>
#include <QwtSymbol>
#include <QwtInterval>
#include <string.h>
#include <QPainter>

class OptimIntervalCurve::PrivateData {
public:
  CurveStyle style;
};

OptimIntervalCurve::OptimIntervalCurve( QwtText const&title )
  : QwtPlotIntervalCurve( title ),  data_( new PrivateData()) 
{}

OptimIntervalCurve::OptimIntervalCurve( QString const& title )
  : QwtPlotIntervalCurve( title ), data_( new PrivateData()) 
{}

OptimIntervalCurve::~OptimIntervalCurve()
{}

int OptimIntervalCurve::rtti() const
{
    return QwtPlotIntervalCurve::Rtti_PlotIntervalCurve;
}

void OptimIntervalCurve::drawSeries( QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRectF &canvasRect, int from, int to ) const
{
    if ( to < 0 )
        to = dataSize() - 1;

    if ( from < 0 )
        from = 0;

    if ( from > to )
        return;
   
    switch ( style() )
    {
        case (UserCurve + Line):
            drawLine( painter, xMap, yMap, canvasRect, from, to );
            if ( symbol() && symbol()->style() != QwtIntervalSymbol::NoSymbol ) 
            {
	      drawSymbols( painter, *symbol(), xMap, yMap, canvasRect, from, to );
            }
            break;

        default:
	  QwtPlotIntervalCurve::drawSeries(painter, xMap, yMap, canvasRect, from, to );
    }


}

void OptimIntervalCurve::drawLine( QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRectF &canvasRect, int from, int to ) const
{
    const bool doAlign = QwtPainter::roundingAlignment( painter );

    painter->save();

    const size_t size = to - from + 1;
    QPolygonF polygon( size );
    QPointF *points = polygon.data();

    for ( uint i = 0; i < size; i++ )
    {
        QPointF &point = points[i];

        QwtIntervalSample const intervalSample = sample( from + i );
        if ( orientation() == Qt::Vertical )
        {
            double x = xMap.transform( intervalSample.value );
            double y = yMap.transform( 0.5*( intervalSample.interval.minValue()  + intervalSample.interval.maxValue() ) );
            point.rx() = x;
            point.ry() = y;
	}
        else
        {
            double y  = yMap.transform( intervalSample.value );
            double x  = xMap.transform( 0.5*(intervalSample.interval.minValue() + intervalSample.interval.maxValue()));
            point.rx() = x;
            point.ry() = y;
        }
    }

    if ( pen().style() != Qt::NoPen )
    {
        painter->setPen( pen() );
        painter->setBrush( Qt::NoBrush );

        QwtPainter::drawPolyline( painter, points, size );
    }

    painter->restore();
}

void OptimIntervalCurve::drawTube( QPainter* painter,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const
{
  QwtPlotIntervalCurve::drawTube( painter, xMap, yMap, canvasRect, from, to );
}

void OptimIntervalCurve::drawSymbols( QPainter* painter, QwtIntervalSymbol const& symbol, 
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const
{
  QwtPlotIntervalCurve::drawSymbols( painter, symbol, xMap, yMap, canvasRect, from, to );
}
