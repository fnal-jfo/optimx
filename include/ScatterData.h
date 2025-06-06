//  =================================================================
//
//  ScatterData.h
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

#ifndef SCATTERDATA_H
#define SCATTERDATA_H

#include <tuple>
#include <memory>
#include <vector>
#include <cstdio>
#include <Bunch.h>
#include <Optim.h>
#include <QPointF>
#include <QwtSeriesData>

// .......................................
  

class AbstractScatterData : public QwtSeriesData<QPointF> {
 
 public:

  struct State {
    double state[6]; // coordinates
    bool   lost;     // true if particle is lost
    int    id;       // id that may be used for color contour
                     // ( for a valid point id>0; id<0 is used as end marker)
    double operator[](int i) const {return state[i];}    
  };
  
  struct Limits {
    double xmin;
    double xmax;
    double xpmin;
    double xpmax;
    double ymin;
    double ymax;
    double ypmin;
    double ypmax;
    double smin;
    double smax;
    double dppmin;
    double dppmax;
  };

  AbstractScatterData()          = default;
  virtual ~AbstractScatterData() = 0; 
  
  Limits limits();
  
  virtual State       start() const = 0;
  virtual State       next()  const = 0;
  virtual bool        done()  const = 0;

  virtual size_t      size()           const;  
  virtual QPointF     sample(size_t i) const  = 0;  
  virtual QRectF      boundingRect ()  const  = 0;
  virtual void 	      setRectOfInterest (QRectF const &rect) = 0;

  void        setView(ViewType view);  
  ViewType    view() const;

protected:
  
  mutable State                   state_; // the current state -- this is not an array  
  mutable int                     idx_;   // index of the current state 
  int                             size_;  
  ViewType                        view_;

  static constexpr double scaling_[] = {1.0, 1000.0, 1.0, 1000.0, 1.0, 1000.0};  

 private:

  void computeSize();

};

//..........................................................................................

class PoincareScatterData: public AbstractScatterData {

 public:

  PoincareScatterData(std::shared_ptr<FILE>& fp, ViewType view = ViewType::input); 
  virtual ~PoincareScatterData() {}
  
  virtual State       start() const;
  virtual State       next()  const;
  virtual bool        done()  const;
  
  size_t      size()           const;  
  QPointF     sample(size_t i) const;  
  QRectF      boundingRect ()  const;
  void 	      setRectOfInterest (QRectF const &rect);

private:

  std::shared_ptr<FILE>   fp_;    // file handle to temporary file 
  mutable bool            eof_;   // true if end of file reached
};


//..........................................................................................

class TrackingScatterData: public AbstractScatterData {

 public:

  TrackingScatterData(Bunch& bunch);
  TrackingScatterData(Coordinates const* coordinates, int N,  ViewType view = ViewType::input);
  virtual ~TrackingScatterData() {}

  virtual State       start() const;
  virtual State       next()  const;
  virtual bool        done()  const;

  size_t      size()           const;  
  QPointF     sample(size_t i) const;  
  QRectF      boundingRect ()  const;
  void 	      setRectOfInterest (QRectF const &rect);

 private:

  Bunch              bunch_;
  Coordinates const* coords_;
  int                N_;
};


#endif //  SCATTERDATA_H
