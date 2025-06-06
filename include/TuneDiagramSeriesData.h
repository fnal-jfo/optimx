//  =================================================================
//
//  TuneDiagramSeriesData.h
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

#ifndef TUNEDIAGRAMSERIESDATA_H
#define TUNEDIAGRAMSERIESDATA_H

#include <vector>
#include <QwtSeriesData>


class TuneDiagramSeriesData: public QwtSeriesData<QPointF> {

public:
  
  TuneDiagramSeriesData( int m, int n, int k, 
			 double& Qxmin, double& Qxmax, double& Qymin, double& Qymax);
 ~TuneDiagramSeriesData();
  
  size_t  size()            const;
  QPointF sample( size_t i) const;
  QRectF  boundingRect()    const;

private:
  
  int m_;
  int n_;
  int k_;
 
  double &  Qxmin_;
  double &  Qxmax_;
  double &  Qymin_;
  double &  Qymax_;

  mutable std::vector<QPointF> points_;

};

#endif  // TUNEDIAGRAMSERIESDATA_H
