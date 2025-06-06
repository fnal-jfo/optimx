//  =================================================================
//
//  OptimPlotMarker.cpp
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
#include <cmath>
#include <set>
#include <utility>
#include <random>
#include <chrono>
#include <Constants.h>
#include <OptimPlotMarker.h>
#include <QPainter>
#include <QwtScaleMap>
#include <QwtSymbol>
#include <QwtPlot>
#include <QwtText>

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator (seed);

using Constants::PI; 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimPlotMarker::OptimPlotMarker( ResonanceData const& rdata, QString const &title )
  : QwtPlotMarker(title), rdata_(rdata)
{
  setItemInterest(QwtPlotItem::ScaleInterest, true);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


OptimPlotMarker::OptimPlotMarker(  ResonanceData const& rdata, QwtText const &title )
  : QwtPlotMarker(title), rdata_(rdata)
{
  setItemInterest(QwtPlotItem::ScaleInterest, true);
}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimPlotMarker::draw(QPainter* painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF& canvasRect) const
{
  QPointF const pos( xMap.transform( xValue() ), 
		     yMap.transform( yValue() ) );
#if 0
    // draw lines
    drawLines( painter, canvasRect, pos );

    // draw symbol
    if (  symbol() &&
	  ( symbol()->style() != QwtSymbol::NoSymbol ) )
    {
        QSizeF const sz = d_data->symbol->size();

        QRectF const clipRect = canvasRect.adjusted( 
            -sz.width(), -sz.height(), sz.width(), sz.height() );

        if ( clipRect.contains( pos ) )
	  symbol()->drawSymbol( painter, pos );
    }
#endif
    drawLabel( painter, canvasRect, pos );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0 
void OptimPlotMarker::drawLines( QPainter *painter, QRectF const &canvasRect, QPointF const& pos ) const
{
    if ( d_data->style == NoLine )
        return;

    bool const doAlign = QwtPainter::roundingAlignment( painter );

    painter->setPen(linePen())
    if ( d_data->style == QwtPlotMarker::HLine ||
        d_data->style == QwtPlotMarker::Cross )
    {
        double y = pos.y();
        if ( doAlign )
            y = qRound( y );

        QwtPainter::drawLine( painter, canvasRect.left(),
            y, canvasRect.right() - 1.0, y );
    }
    if ( d_data->style == QwtPlotMarker::VLine ||
        d_data->style == QwtPlotMarker::Cross )
    {
        double x = pos.x();
        if ( doAlign )
            x = qRound( x );

        QwtPainter::drawLine( painter, x,
            canvasRect.top(), x, canvasRect.bottom() - 1.0 );
    }
}

#endif

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimPlotMarker::drawLabel( QPainter *painter, QRectF const& canvasRect,
				 QPointF  const& pos ) const
{

  QFont thefont = painter->font();
  thefont.setWeight(QFont::Black);
  
  if ( label().isEmpty() )
    return;

    Qt::Alignment align = labelAlignment();
    QPointF alignPos = pos;

    QSizeF symbolOff( 0, 0 );

    switch ( lineStyle() )
    {
        case QwtPlotMarker::VLine:
        {
            // In VLine-style the y-position is pointless and
            // the alignment flags are relative to the canvas

	  if ( labelAlignment() & Qt::AlignTop )
            {
                alignPos.setY( canvasRect.top() );
                align &= ~Qt::AlignTop;
                align |= Qt::AlignBottom;
            }
	  else if ( labelAlignment() & Qt::AlignBottom )
            {
                // In HLine-style the x-position is pointless and
                // the alignment flags are relative to the canvas

                alignPos.setY( canvasRect.bottom() - 1 );
                align &= ~Qt::AlignBottom;
                align |= Qt::AlignTop;
            }
            else
            {
                alignPos.setY( canvasRect.center().y() );
            }
            break;
        }
        case QwtPlotMarker::HLine:
        {
	  if ( labelAlignment() & Qt::AlignLeft )
            {
                alignPos.setX( canvasRect.left() );
                align &= ~Qt::AlignLeft;
                align |= Qt::AlignRight;
            }
            else if ( labelAlignment() & Qt::AlignRight )
            {
                alignPos.setX( canvasRect.right() - 1 );
                align &= ~Qt::AlignRight;
                align |= Qt::AlignLeft;
            }
            else
            {
                alignPos.setX( canvasRect.center().x() );
            }
            break;
        }
        default:
        {
	  if ( symbol() &&
		  ( symbol()->style() != QwtSymbol::NoSymbol ) )
            {
	      symbolOff = symbol()->size() + QSizeF( 1, 1 );
                symbolOff /= 2;
            }
        }
    }

    qreal pw2 = linePen().widthF() / 2.0;
    if ( pw2 == 0.0 )
         pw2 = 0.5;

    const int spacing = this->spacing();

    const qreal xOff = qMax( pw2, symbolOff.width() );
    const qreal yOff = qMax( pw2, symbolOff.height() );

    const QSizeF textSize = label().textSize( thefont );

#if 0
    if ( align & Qt::AlignLeft )
    {
        alignPos.rx() -= xOff + spacing;
        if ( labelOrientation() == Qt::Vertical )
             alignPos.rx() -= textSize.height();
        else
            alignPos.rx() -= textSize.width();
    }
    else if ( align & Qt::AlignRight )
    {
        alignPos.rx() += xOff + spacing;
    }
    else
    {
      if ( labelOrientation() == Qt::Vertical )
             alignPos.rx() -= textSize.height() / 2;
        else
             alignPos.rx() -= textSize.width() / 2;
    }

    if ( align & Qt::AlignTop )
    {
        alignPos.ry() -= yOff + spacing;
        if ( labelOrientation() != Qt::Vertical )
             alignPos.ry() -= textSize.height();
    }
    else if ( align & Qt::AlignBottom )
    {
        alignPos.ry() += yOff + spacing;
        if ( labelOrientation() == Qt::Vertical )
             alignPos.ry() += textSize.width();
    }
    else
    {
      if ( labelOrientation() == Qt::Vertical )
             alignPos.ry() += textSize.width() / 2;
        else
            alignPos.ry() -= textSize.height() / 2;
    }
#endif


    painter->translate( alignPos.x(), alignPos.y() );
    //if ( labelOrientation() == Qt::Vertical )
    //     painter->rotate( -90.0 );

    // the minus sign below arises because in screen coordinates  Y increases from top to bottom.
    double dx  =  (plot()->transform(QwtPlot::xBottom, 1.0) - plot()->transform(QwtPlot::xBottom, 0.0));
    double dy  = -(plot()->transform(QwtPlot::yLeft,   1.0) - plot()->transform(QwtPlot::yLeft,   0.0));

    // the angle on the canvas is modified because the x and y scale aspect ratio is not 1.         
    double angle = atan(tan(rdata_.angle*PI/180.0)*dy/dx ) * 180.0/PI;
    painter->rotate(-angle);
    
    const QRectF textRect( 0, 0, textSize.width(), textSize.height() );
    label().draw( painter, textRect );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::pair<double,double> OptimPlotMarker::visibleRange(  ResonanceData const& rdata, double qxmin, double qxmax, double qymin, double qymax )
{
  // determine the range of x for the visible part of the line.   

  if (qxmin > qxmax) std::swap(qxmin,qxmax);
  if (qymin > qymax) std::swap(qymin,qymax);
  
  std::set<double> limits;
  
  int nx = rdata.i;
  int ny = rdata.j;
  int nz = rdata.k;

  double qx;
  double qy;

  //  horizontal line
  
  if (nx == 0 ) {
      if ( (qy >= qymin) &&  (qy <= qymax) ) return std::make_pair(qxmin,qxmax); 
      return std::make_pair(-1.0, -1.0); 
    }
    
  //  vertical line

   if (ny == 0 ) {
     qx = double(nz)/double(nx);  
     if ( (qx >= qxmin) &&  (qx <= qxmax) ) return std::make_pair(qx,qx); 
     return  std::make_pair(-1.0,-1.0); 
   }
   
   //  ... case 1: intersection with the (vertical) line Qx = Qxmin   
    
   qy =  (double(nz) - double(nx)*qxmin )/double(ny);  
   if ( (qy >= qymin) &&  (qy <= qymax) )  limits.insert(qxmin);

   // ... case2: intersection with Qy = Qymin 

   qx  = (double(nz) - double(ny)*qymin)/double(nx); 
   if ( (qx >= qxmin) &&  (qx <= qxmax) )  limits.insert(qx);
 
   //  ... case3: intersection with the line Qx = Qxmax   
      
   qy =  (double(nz) - double(nx)*qxmax )/double(ny);  
   if ( (qy >= qymin) &&  (qy <= qymax) )  limits.insert(qxmax);
    
   // ... case 4: intersection with Qy = Qymax

   qx  = (double(nz) - double(ny)*(qymax))/double(nx); 
   if ( (qx >= qxmin) &&  (qx <= qxmax) )  limits.insert(qx);

   if (  limits.size() < 2) return std::make_pair( -1.0, -1.0);  // the line has no visible part  
   if ( fabs(*limits.begin() - *(--limits.end()) ) < 1.0e-3 ) return std::make_pair( -1.0, -1.0); // line intersects with a single vertex 
     
   return std::make_pair( *limits.begin(),  *(--limits.end())); 
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimPlotMarker::updateScaleDiv( QwtScaleDiv const& xScaleDiv, QwtScaleDiv const&  yScaleDiv)
{
  // move the position of the markers randomly along the visible lines.  

  auto canvasRect = plot()->canvas()->rect();

  
  double xmin = plot()->invTransform(QwtPlot::xBottom, canvasRect.left());
  double xmax = plot()->invTransform(QwtPlot::xBottom, canvasRect.right());
  double ymin = plot()->invTransform(QwtPlot::yLeft,   canvasRect.bottom());
  double ymax = plot()->invTransform(QwtPlot::yLeft,   canvasRect.top());

  //double frac = (rdata_.i == 0) ? 0.025 : double(rand()% 1000)/double(1000); // a random number between 0 and 1;

    auto vrange = visibleRange( rdata_, xmin, xmax, ymin, ymax); // determine the range of x for the visible part of the line.   
  xmin = vrange.first;
  xmax = vrange.second;
  if (xmax < xmin) std::swap(xmin,xmax); // FIXME is this needed ??? 
  if ((xmin < 0.0) && ( xmax < 0.0 )) return;

  std::normal_distribution<double> ndist( 0.5, 0.125 ); // (sigma, mu) 

  double frac = 0.0;
  do {
    frac = ndist(generator); // a random number between 0 and 1;
  } while (frac < 0.0 || frac > 1.0 );
    
  //double frac = (rdata_.i == 0) ? 0.025 : ndist(generator); // a random number between 0 and 1;
  frac = (rdata_.i == 0) ? 0.025 : frac;
  
  double xpos = xmin + (xmax-xmin)*fabs(frac); 		 
  double ypos = (rdata_.j != 0 ) ?  (rdata_.k - rdata_.i*xpos) /rdata_.j :  ymin +  0.025*(ymax-ymin);
  setValue(xpos, ypos ); // FIXME (case j=0)
}
