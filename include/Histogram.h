//  =================================================================
//
//  Histogram.h
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
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <boost/histogram.hpp>
#include <tuple>

namespace std {
  template <typename T>  class shared_ptr;
}  

class Histogram1D {

public:  

  using  storage_t       = boost::histogram::storage_adaptor<std::vector<double>>;
  using  axis_t          = boost::histogram::axis::regular<>;
  using  axes_t          = std::tuple<axis_t>;
  using  histo_t         = boost::histogram::histogram<axes_t,   storage_t>;
 

  using  iterator        = histo_t::iterator;
  using  const_iterator  = histo_t::const_iterator;
  
  Histogram1D(int nx, double xmin, double xmax);
  Histogram1D(Histogram1D const& h);
  Histogram1D(histo_t const& h);
  
  size_t sum() const;

  void operator()( double);
  double at(int i) const;

  double max() const;
  size_t  size() const;

  std::tuple<double,double> range() const;

  int    index(double value) const;
  double binWidth() const;

  std::tuple<double,double> binRange(int index=0) const;
   
  void reset();
  
 private:

  std::shared_ptr<histo_t> h_;
  
};

typedef Histogram1D Histogram;

//...............................................................................................

class Histogram2D {

 public:

  using  storage_t       = boost::histogram::storage_adaptor<std::vector<double>>;
  using  axis_t          = boost::histogram::axis::regular<>;
  using  axes_t          = std::tuple<axis_t,axis_t>;
  using  histo_t         = boost::histogram::histogram<axes_t, storage_t>;
   
  using  iterator        = histo_t::iterator;
  using  const_iterator  = histo_t::const_iterator;


  Histogram2D( int nx, double xmin, double xmax,
             int ny, double ymin, double ymax);
  
  Histogram2D( Histogram2D const& h);


  Histogram2D& operator/=( double rhs );

  Histogram2D& operator*=( double rhs );

  double at(int i, int j) const;

  size_t  size(int axis) const;

  std::tuple<double,double> range(int axis) const;

  Histogram1D project( int axis);

  double max() const;

  int index(int axis, double value) const;

  void operator()( double, double);

  double binWidth(int axis)   const;

  size_t sum() const;

  std::tuple<double,double> binRange(int axis, int index=0) const;
  
  void reset();	     

  boost::histogram::indexed_range<std::remove_reference_t<Histogram2D::histo_t>> idxrange() const; 

  
 private:
   
   std::shared_ptr<histo_t> h_;
    
};

#endif // HISTOGRAM_H
