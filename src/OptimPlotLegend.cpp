//  =================================================================
//
//  OptimPlotLegend.cpp
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
#include <OptimPlotLegend.h>
#include <OptimLegendLabel.h>
#include <QMouseEvent>
#include <QScrollBar>
#include <algorithm>
#include <QwtLegendLabel>
#include <QwtTextLabel>


OptimPlotLegend::OptimPlotLegend(QWidget* parent)
  : QwtLegend(parent)
{
  horizontalScrollBar()->hide();
  verticalScrollBar()->hide();
}        


OptimPlotLegend::~OptimPlotLegend()
{
  // this is used for dbg
  // std::cout << "OptimPlotLegend::~OptimPlotLegend()" << std::endl;
}


QWidget* OptimPlotLegend::createWidget 	( QwtLegendData const&  data ) 	const
 {

    Q_UNUSED( data ); // this macro is a qt kludge
                      // to disable unused variable warning message; 
                      // it has no other effect
    
    OptimLegendLabel *label = new OptimLegendLabel();
    label->setItemMode( defaultItemMode() );

    connect( label, SIGNAL( clicked() ),       SLOT( itemClicked() ) );
    connect( label, SIGNAL( checked( bool ) ), SLOT( itemChecked( bool ) ) );

    label->setChecked(true);

    return label;

 }

void  OptimPlotLegend::updateLegend (const QVariant &itemInfo, const QList< QwtLegendData > &data)
{

  // itemInfo: usually a ptr to a PlotItem e.g. QwtPlotCurve.
  // data:     list of all data necessary to build the legend items, usually legenddata from the plotitem 

  auto lst = contentsWidget()->children();

  
  auto isPresent = [&lst]( QString const& s) {  
    for (auto it = lst.begin(); it !=lst.end();  ++it) {
      QwtLegendLabel* w = 0;
      if (  w = qobject_cast<QwtLegendLabel*>(*it) ) {
      if (   w->text().text() == s ) { return true;} 
      }
    }
    return false;
  };

  bool tst = data.empty() ? false : isPresent( data.front().title().text() );
  if (tst) return;
  
  QwtLegend::updateLegend(itemInfo, data);
}



