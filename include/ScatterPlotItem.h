//  =================================================================
//
//  ScatterPlotItem.h
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

#ifndef SCATTERPLOTITEM_H
#define SCATTERPLOTITEM_H

#include <cstdio>
#include <vector>
#include <memory>
#include <algorithm>
#include <tuple>
#include <QwtPlotSeriesItem>
#include <QwtText>
#include <QString>
#include <Structs.h>
#include <ScatterData.h>
#include <Optim.h>

class QwtScaleMap;
class Coordinates;
class LossStruct;

enum class ViewType: int;
enum class PlaneType: int;

class ScatterPlotItem: public QwtPlotSeriesItem 
{ 
 public:

  ScatterPlotItem ( std::shared_ptr<AbstractScatterData> data,  QwtText const& title=QwtText());
  ScatterPlotItem ( ScatterPlotItem const& s);

 ~ScatterPlotItem();

  ViewType viewType() const;
  void     setView(ViewType  view);
  void     setPlane(PlaneType plane);

  int    rtti()              const;
  QRectF boundingRect ()     const;

  void draw(QPainter *painter, QwtScaleMap const& xMap,
	    QwtScaleMap const& yMap, QRectF const& canvasRect) const;
  
  void drawSeries( QPainter *painter, QwtScaleMap const& xMap,
		   QwtScaleMap const& yMap, QRectF const& canvasRect, int from, int to) const;

 protected:

  void    setRectOfInterest( QRectF const& ); // abstract interface from QAbstractSeriesStore
  QRectF  dataRect() const;                   // abstract interface from QAbstractSeriesStore 
  size_t  dataSize() const;                   // abstract interface from QAbstractSeriesStore


 private:

  void computeBoundingRect();
  

  ViewType            view_;   
  PlaneType          plane_;   
  QRectF          brect_xy_;
  QRectF         brect_xxp_;
  QRectF         brect_yyp_;
  QRectF        brect_sdpp_;

  std::shared_ptr<AbstractScatterData> data_;

}; 


#endif // SCATTERPLOTITEM_H
