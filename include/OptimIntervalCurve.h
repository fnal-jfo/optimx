//  =================================================================
//
//  OptimIntervalCurve.h
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

#ifndef OPTIMINTERVALCURVE_H
#define OPTIMINTERVALCURVE_H

#include <QwtPlotIntervalCurve>
#include <memory>

class QwtSymbol;

class QWT_EXPORT OptimIntervalCurve: public QwtPlotIntervalCurve {

public:
     enum UserCurveStyle
     {
       Line = 1
     };


    explicit OptimIntervalCurve( QString const& title = QString() );
    explicit OptimIntervalCurve( QwtText const& title );

    virtual ~OptimIntervalCurve();

    virtual int rtti() const;

    void setPaintAttribute( PaintAttribute, bool on = true );
    bool testPaintAttribute( PaintAttribute ) const;

    virtual void drawSeries( QPainter *p,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

    //virtual QRectF boundingRect() const;

    //virtual QwtGraphic legendIcon( int index, const QSizeF & ) const;

protected:

    void drawLine( QPainter *painter,
       const QwtScaleMap &xMap, const QwtScaleMap &yMap,
       const QRectF &canvasRect, int from, int to ) const;

    void drawSymbols( QPainter *painter, QwtSymbol const&,
       const QwtScaleMap &xMap, const QwtScaleMap &yMap,
       const QRectF &canvasRect, int from, int to ) const;
 
    virtual void drawTube( QPainter *,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

    virtual void drawSymbols( QPainter *, QwtIntervalSymbol const& symbol,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

 private:

    class PrivateData;
    std::unique_ptr<PrivateData> data_;
};

#endif
