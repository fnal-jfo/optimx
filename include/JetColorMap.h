//  =================================================================
//
//  JetColorMap.h
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
#ifndef JETCOLORMAP_H
#define JETCOLORMAP_H

#include <QwtColorMap>
#include <QwtInterval>
#include <QVector>

// QwtLinearColorMap appears to have an interpolation bug, probably related
// to integer arithmetic. For this reason, the code below is disabled. 

/**************************************************************************

class JetColorMap : public QwtLinearColorMap {

  public:
  
   JetColorMap(QwtColorMap::Format f = QwtColorMap::RGB);
  ~JetColorMap();
   QRgb rgb( QwtInterval const& interval, double value) const;

};

class JetLogColorMap : public JetColorMap {

  public:
    JetLogColorMap(QwtColorMap::Format f = QwtColorMap::RGB);
   ~JetLogColorMap(); 

  QRgb rgb( QwtInterval const& interval, double value) const;

};


**************************************************************************/

class JetLinearColorMap : public QwtColorMap {

  public:

  struct BreakPoint {
    double s;
    double r;
    double g;
    double b;
  };  

   JetLinearColorMap(QwtColorMap::Format f = QwtColorMap::RGB);
  ~JetLinearColorMap();
   QRgb rgb( QwtInterval const& interval, double value) const;

  protected:

  double  step_;
  QVector<BreakPoint> v_;
  
};

class JetLogColorMap : public JetLinearColorMap {

  public:
    JetLogColorMap(QwtColorMap::Format f = QwtColorMap::RGB);
   ~JetLogColorMap(); 

  QRgb rgb( QwtInterval const& interval, double value) const;

};

#endif // JETCOLORMAP_H
