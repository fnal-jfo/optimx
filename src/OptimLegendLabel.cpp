//  =================================================================
//
//  OptimLegendLabel.cpp
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

#include <QtDebug>
#include <QEvent>
#include <OptimLegendLabel.h>
#include <QMouseEvent>
#include <QScrollBar>
#include <qdrawutil.h>
#include <qstyleoption.h>
#include <QToolTip>
#include <QPainter>
#include <iostream>

static const int ButtonFrame = 2;
static const int Margin = 2;

static QSize buttonShift( const QwtLegendLabel *w )
{
    QStyleOption option;
    option.initFrom( w );

    const int ph = w->style()->pixelMetric(
        QStyle::PM_ButtonShiftHorizontal, &option, w );
    const int pv = w->style()->pixelMetric(
        QStyle::PM_ButtonShiftVertical, &option, w );
    return QSize( ph, pv );
}

OptimLegendLabel::OptimLegendLabel(QWidget* parent)
  : QwtLegendLabel(parent)
{
  auto f = font();
  f.setPointSize(6);
  setFont(f);
  setAttribute(Qt::WA_Hover);
}        


OptimLegendLabel::~OptimLegendLabel()
{
  // this is used for dbg
  // std::cout << "OptimPlotLegend::~OptimPlotLegend()" << std::endl;
}


void OptimLegendLabel::mousePressEvent (QMouseEvent* event)
{
  
  auto bt = event->button();
  if (bt == Qt::RightButton)  {
    qDebug()<< "Right Button ";

    return;
  }
  
  //  std::cout << "OptimLegendLabel::mousePressEvent" << std::endl;
  QwtLegendLabel::mousePressEvent(event);
  //std::cout << "isChecked " << isChecked() << std::endl;
     
}

void OptimLegendLabel::paintEvent (QPaintEvent* e)
{
    const QRect cr = contentsRect();

    QPainter painter( this );
    painter.setClipRegion( e->region() );

    if ( isDown() )
    {
      //qDrawWinButton( &painter, 0, 0, width(), height(),
      //palette(), true );
        qDrawPlainRect( &painter, 0, 0, width(), height(),
			QColor("black"), 1 );
    }

    painter.save();

    if ( isDown() )
    {
        const QSize shiftSize = buttonShift( this );
        painter.translate( shiftSize.width(), shiftSize.height() );
    }

    painter.setClipRect( cr );

    drawContents( &painter );

    if ( !icon().isNull() )
    {
        QRect iconRect = cr;
        iconRect.setX( iconRect.x() + margin() );
        if (itemMode() != QwtLegendData::ReadOnly )
            iconRect.setX( iconRect.x() + ButtonFrame );

        iconRect.setSize( icon().size() );
        iconRect.moveCenter( QPoint( iconRect.center().x(), cr.center().y() ) );

        painter.drawPixmap( iconRect, icon() );
    }

    painter.restore();
}

bool  OptimLegendLabel::event (QEvent* e)
{
  if (e->type() == QEvent::HoverEnter) {
    //QHoverEvent* h = static_cast<QHoverEvent*>(e); 
    //std::cout << "Hover Enter" << std::endl;
    //QToolTip::showText(  this->mapToGlobal(QPoint(0,0)), "Axis: " , this);
  }
  if (e->type() == QEvent::HoverLeave) {
    //QHoverEvent* h = static_cast<QHoverEvent*>(e); 
    //QToolTip::hideText();
    //std::cout << "Hover Leave" << std::endl;
  }
  
  return QwtLegendLabel::event(e);   

}  
