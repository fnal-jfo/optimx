//  =================================================================
//
//  Histogram2D.cpp
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
#include <Histogram.h>
#include <memory>
#include <boost/math/interpolators/cardinal_cubic_b_spline.hpp>
#include <iostream>

using namespace  boost::histogram::literals; // enables _c suffix
using interpolator_t  = boost::math::interpolators::cardinal_cubic_b_spline<double>;

// using  storage_t       = boost::histogram::storage_adaptor<std::vector<double>>;
// using  axis_t          = boost::histogram::axis::regular<>;
// using  axes_t          = std::tuple<axis_t>;
// using  histo_t         = boost::histogram::histogram<axes_t, storage_t>;
// using interpolator_t   = boost::math::interpolators::cardinal_cubic_b_spline<double>;
  

Histogram2D::Histogram2D( int nx, double xmin, double xmax,
                      int ny, double ymin, double ymax)
{
  h_ = std::make_shared<histo_t>(std::make_tuple(axis_t(nx,xmin,xmax),
						 axis_t(ny,ymin,ymax)) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Histogram2D::Histogram2D( Histogram2D const& o)
{
  h_ = o.h_;
}    

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Histogram2D::at(int i, int j) const
{
  return h_->at(i,j);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

size_t Histogram2D::size(int axis) const
{
   return h_->axis(axis).size();
}   

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::tuple<double,double> Histogram2D::range(int axis) const
{
   auto xaxis = h_->axis(axis);
   double xmin = xaxis.bin(0).lower();
   double xmax = xaxis.bin(xaxis.size()).lower();
   return std::make_tuple(xmin,xmax);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Histogram2D::max() const
{
  auto range = boost::histogram::indexed(*h_);
   
  // find the max bin count...

  return *std::max_element(range.begin(), range.end(), [](auto const& lhs, auto const& rhs){ return lhs< rhs;});

}   
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Histogram2D::index(int axis, double value) const
{
  auto ax = h_->axis(axis);
  int i = ax.index(value);
  return i;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Histogram2D::operator()( double x, double y)
{  
 (*h_)(x,y);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Histogram2D::binWidth(int axis) const
{
  return h_->axis(axis).bin(0).width(); // bin 0 is the first real bin. bin -1 is the "left overflow";  bin nx is the "right overflow" 
}   

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::tuple<double,double> Histogram2D::binRange(int axis, int index) const
  {
  return std::make_tuple(h_->axis(0).bin(index).lower(), h_->axis(0).bin(index).upper());
}   
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

size_t Histogram2D::sum() const
{
   return boost::histogram::algorithm::sum(*h_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void Histogram2D::reset()
{
  
 // reset all bin counts to 0.
 
 int nx = size(0);
 int ny = size(1);
 
 for( int i = -1; i <= nx; ++i) {
    for( int j = -1; j <= ny; ++j) {
      h_->at(i,j) = 0;
    }
 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Histogram1D Histogram2D::project( int axis)
{
  if (axis == 0) {
    return Histogram1D(boost::histogram::algorithm::project(*h_, 0_c));
  }
  else {
    return Histogram1D(boost::histogram::algorithm::project(*h_, 1_c));
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


boost::histogram::indexed_range<std::remove_reference_t<Histogram2D::histo_t>> Histogram2D::idxrange() const
{
  return boost::histogram::indexed(*h_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Histogram2D& Histogram2D::operator/=( double rhs )
{
  (*h_) /= rhs;
  return *this;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Histogram2D& Histogram2D::operator*=( double rhs )
{
  (*h_) *= rhs;
  return *this;

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
