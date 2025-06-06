//  =================================================================
//
//  DistancePicker.cpp
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

#include <DistancePicker.h>

#include <QwtPlotCanvas>
#include <QwtPickerMachine>
#include <QwtText>
#include <QPainter>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DistancePicker::DistancePicker( QWidget* canvas )
: QwtPlotPicker( canvas )
{
  ctor(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


DistancePicker::DistancePicker( int xAxis, int yAxis, QWidget* canvas)
  :  QwtPlotPicker( xAxis, yAxis, canvas ) 
{
  ctor(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DistancePicker::ctor()
{
  /*
    We don't have a picker for a line, but the rectangle
    selection is also a selection of 2 points. So all we
    have to do is to paint a line rubberband.
  */

  setStateMachine( new QwtPickerClickRectMachine );
  setRubberBand( PolygonRubberBand );
  setTrackerMode( QwtPicker::ActiveOnly );

  // Disable keyboard handling
  setKeyPattern(QwtEventPattern::KeySelect1, Qt::Key_unknown);
  setKeyPattern(QwtEventPattern::KeySelect2, Qt::Key_unknown);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DistancePicker::drawRubberBand(QPainter* painter) const
{
  painter->drawPolygon( selection() );
}
     
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QwtText  DistancePicker::trackerText (const QPoint& pos) const
{
  QPolygon const& polygon = selection();
     
  if ( polygon.size() != 2 ) return QwtText();
     
  QLineF const line(invTransform(polygon[0]), invTransform(polygon[1]));
     
  //QwtText text( QString::number(line.length()) );
  QwtText text( "dx = " + QString::number(line.dx()) + ", dy = " +  QString::number(line.dy()) );
     
  QColor bg(Qt::white);
  bg.setAlpha(180);
  text.setBackgroundBrush(QBrush(bg));
     
  return text;
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

