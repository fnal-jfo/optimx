//  =================================================================
//
//  OptimLegoPlotItem.cpp
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
#include <algorithm>
#include <iterator>

#include <OptimApp.h>
#include <OptimLegoPlotItem.h>

#include <QMouseEvent>
#include <QPoint>
#include <QPrinter>
#include <QPainter>
#include <QToolTip>
#include <QwtPainter>
#include <QwtPlot>
#include <QwtScaleMap>

#include <map>
#include <string>

static std::map<char,std::string> Element_key = 
     {
      {'O', "drift"      }, 
      {'I', "instrument" }, 
      {'B', "CF-bend"    }, 
      {'D', "CF-bend"    }, 
      {'G', "m-edge"     }, 
      {'R', "ES-bend"    }, 
      {'Q', "quad"       }, 
      {'L', "ES-quad"    }, 
      {'S', "sextupole"  }, 
      {'M', "mpole"      }, 
      {'C', "solenoid"   },
      {'K', "corrector"  },
      {'Z', "L-corrector"},
      {'A', "sw-cavity"  },
      {'W', "sw-cavity"  },
      {'E', "es-accel"   },
      {'F', "Li-lens"    },
      {'X', "xfer-matrix"},
      {'H', "aperture"   }  };    


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimLegoPlotItem::OptimLegoPlotItem (std::vector<LegoData> const& legodata, 
                                      QwtText const& title)
  : QwtPlotItem(title), legodata_(legodata), top_(true) 
{
  setZ(100000.0);
  setItemInterest(QwtPlotItem::ScaleInterest); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimLegoPlotItem::updateData( std::vector<LegoData> const& legodata )
{
  legodata_ = legodata;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimLegoPlotItem::~OptimLegoPlotItem()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimLegoPlotItem::draw(QPainter* painter, QwtScaleMap const& xMap, 
                             QwtScaleMap const& yMap, QRectF const& canvasRect) const
{

  if (legodata_.size() == 0) return;
  
  painter->save();
   
   //const double x1 = canvasRect.left();
   //const double x2 = canvasRect.right();

   const int height =  canvasRect.height();
   //  const double dy     =  height*0.01;
     
   int hbend    =  5; // qMax(height/80, 5);    

   hbend *= 2;
   
   int hquad    =  hbend*2;     

   hquad    /=  1.5;
   
   int sgn      = 1; // focusing Element sign  
   int vpos     =  canvasRect.y() + ( top_ ? 0.05*height : 0.95*height);

   int txorg = xMap.transform(legodata_.front().start );
   

    //QwtPainter::drawLine(painter, QPoint(x1, vpos),  QPoint(x2,vpos));

   for ( auto it : legodata_ ) {
     
        char etype = toupper(it.name[0]);

        double length = it.length;
        double start  = it.start;
 
        // Elements with negative length are highlighted in yellow.
	// The length is displayed as negative on the logo plot. 
  
        if (length < 0) { 
          //length = fabs(length); 
          QPen pen = painter->pen();  
          painter->setPen("yellow");
	  
	  painter->drawRect(  xMap.transform(start), vpos-hquad,  xMap.transform(length)- txorg, 2*hquad );
	  painter->fillRect(  xMap.transform(start), vpos-hquad,  xMap.transform(length)- txorg, 2*hquad , (QColor("yellow")) );
	  painter->setPen(pen);
      }
      
        int xc = xMap.transform(start);
        int lc = xMap.transform(length)-txorg;

	switch (etype) {
        case 'Q':
        case 'L': // quads 
	  {sgn = it.sgn; 
           int yh    = vpos - sgn*hquad;          
           painter->drawRect(xc,  yh, lc, sgn*hquad );
	   painter->fillRect(xc,  yh, lc, sgn*hquad , (QColor("red")) );
          }
	  break;
	case 'B' :  // bending
	case 'D' :  // bending
	case 'R' :  // bending
          { int yh    = vpos - hbend/2;
            painter->drawRect(xc, yh,lc, hbend );
            painter->fillRect(xc, yh,lc, hbend , (QColor("blue")) );
          }
	  break;
        case 'I' :  // bpm
	  { int yh    = vpos - hbend/2;
            painter->drawRect(xc, yh,lc, hbend );
            painter->fillRect(xc, yh,lc, hbend , (QColor("black")) );
          }
	  break;
        case 'C' :  // solenoid
	  { int yh    = vpos - hbend/2;
            painter->drawRect(xc, yh,lc, hbend );
            painter->fillRect(xc, yh,lc, hbend , (QColor("brown")) );
          }
	  break;
        case 'W' : // cavity
	  { int yh    = vpos - hbend/2;
	     painter->drawRect(xc, yh,lc, hbend );
	     painter->fillRect(xc, yh,lc, hbend , (QColor("cyan")) );
          }
	  break;
        case 'A' :  // cavity
	  { int yh    = vpos - hbend/2;
            painter->drawRect(xc, yh,lc, hbend );
            painter->fillRect(xc, yh,lc, hbend , (QColor("cyan")) );
          }
        case 'K' :  // cavity
	  { int yh    = vpos - hbend/2;
            painter->drawRect(xc, yh,lc, hbend );
            painter->fillRect(xc, yh,lc, hbend , (QColor("brown")) );
          }
	  break;
        case 'S' :  // sextupole
	  { sgn = it.sgn; 
            int yh    = vpos - hbend/2;
	    painter->drawRect(xc, yh, lc, hbend );
	    painter->fillRect(xc, yh, lc, hbend , (QColor("green")) );
          }
	  break;
        case 'X' :  // xfer matrix
	  {int yh    = vpos - hbend/2;
	    painter->drawRect(xc, yh, lc, hbend );
	    painter->fillRect(xc, yh, lc, hbend , (QColor("gray")) );
          }
	  break;
        case 'M' :  // multipole
	  {int yh    = vpos - hbend;
	    painter->drawRect(xc, yh, lc+2, hbend*2 );
	    painter->fillRect(xc, yh, lc+2, hbend*2 , (QColor("red")) );
          }
	  break;
        case 'O': //  drift
        default :
          painter->drawLine( QPoint(xc, vpos),  QPoint( xc+lc ,vpos));
    }
   }
   painter->restore();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int OptimLegoPlotItem::rtti() const 
{
  return Rtti_PlotUserItem;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool OptimLegoPlotItem::eventFilter(QObject *watched, QEvent *event)
{
  
  // Intercept mouse move events. Get x mouse position in 
  // physical coordinates and use it to determine Elements nearby.
  // Use tooltip to display Element info.  
  //

  QwtPlot* plt = plot();

  if (watched !=  plt->canvas()) return false;
  
  QMouseEvent*      mevent         = 0;
  QPoint pos;

  QwtScaleMap xMap = plt->canvasMap(QwtPlot::xBottom);


  QRect rect = plt->canvas()->geometry();

  int const height = rect.height();
  int const dy     = height*0.02; // tolerance range for tooltip is +/- dy

  int  vpos = top_ ?  height*0.05 : height*0.95;
  int  xpos = 0;
  int  ypos = 0;
  
  rect.setBottom(vpos + dy);
  rect.setTop(vpos - dy);
    
  switch ( event->type() ) { 
    case QEvent::MouseMove:
      //case QEvent::MouseButtonPress:
      mevent     = static_cast<QMouseEvent*>(event); 
      xpos = mevent->pos().x();
      ypos = mevent->pos().y();
      //if ( ( ypos > vpos + dy ) || (ypos < vpos - dy ) ) break;
      if ( !rect.contains( xpos,ypos) ) break;
      QToolTip::showText(QPoint(mevent->globalX(),mevent->globalY()), (QString("s = %1 [m]").arg(xMap.invTransform(xpos)) + findElement(xMap.invTransform(xpos))),
			 plt->canvas(), rect, 1000000);
      event->accept();
      return true;
    default:
      break;
  }

  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QString OptimLegoPlotItem::findElement(double x) const
{  

  // double start;       // Element start     [m]
  // double length;      // Element length    [m] 
  // std::string name;   // Element full name 
  // return: string for tooltip

  int index = 0;

  if (legodata_.empty()) return QString("");

  auto it = std::lower_bound(legodata_.begin(), legodata_.end(), x, 
                             []( LegoData const& data, double x){ return data.start < x; } );

  // the Element found is the first such that its start pos is NOT < x (i.e. downstream face pos >= x) .
  // This is the "next" Element.  

  QString next    =  (it == legodata_.end())   ? "END"   : it->name.c_str(); 
  QString etypen(  next != "END" ?  Element_key[ toupper(it->name.c_str()[0]) ].c_str() : "Marker" ); 

  QString current =  (it == legodata_.begin()) ? "START" : (--it)->name.c_str();
  QString etype(  current != "START" ? Element_key[ toupper(it->name.c_str()[0]) ].c_str() : "Marker" ); 
  auto itc = it; // save the pointer to the current element  
  
  unsigned idx = std::distance(legodata_.begin(), itc) + 1;

  QString previous =  (it == legodata_.begin()) ? "START" : (--it)->name.c_str();
  QString etypep( previous != "START" ? Element_key[ toupper(it->name.c_str()[0]) ].c_str() : "Marker" ); 

  return QString("\n") + QString("index: %1\n").arg(idx) + etype  + " " +
         current  + QString("\n") + QString("Next: ") + etypen + " " + next +
         "\nPrevious: " + etypep + " " + previous;   

}   
   
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimLegoPlotItem::setPosition(LegoPosition position)
{
  top_ = (position == LegoPosition::Top);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LegoPosition OptimLegoPlotItem::position() const
{
  return top_ ? LegoPosition::Top : LegoPosition::Bottom;
}
