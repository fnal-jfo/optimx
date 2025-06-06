//  =================================================================
//
//  Histogram1D.cpp
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

using namespace  boost::histogram::literals; // enables _c suffix
using interpolator_t  = boost::math::interpolators::cardinal_cubic_b_spline<double>;

// using  storage_t       = boost::histogram::storage_adaptor<std::vector<double>>;
// using  axis_t          = boost::histogram::axis::regular<>;
// using  axes_t          = std::tuple<axis_t>;
// using  histo_t         = boost::histogram::histogram<axes_t, storage_t>;
// using interpolator_t   = boost::math::interpolators::cardinal_cubic_b_spline<double>;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Histogram1D::Histogram1D(int nx, double xmin, double xmax)
{
  h_ = std::make_shared<histo_t>(std::make_tuple(boost::histogram::axis::regular<>(nx,xmin,xmax)));
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Histogram1D::Histogram1D( Histogram1D const& o)
{
  h_ = o.h_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Histogram1D::Histogram1D( histo_t const& h)
{
  h_ = std::make_shared<histo_t>(h);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

size_t Histogram1D::size() const
{
   auto xaxis = h_->axis(0);
   return xaxis.size();
}   

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Histogram1D::reset()
{  
  int nx = size();
 
 for( int i = -1; i <= nx; ++i) {
      h_->at(i) = 0.0;
 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Histogram1D::index(double value) const
{
  auto ax = h_->axis(0);
  int i = ax.index(value);
  return i;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Histogram1D::at(int idx) const
{
  return h_->at(idx);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

size_t Histogram1D::sum() const
{
  return boost::histogram::algorithm::sum(*h_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Histogram1D::operator()(double x)
{
  (*h_)(x);
}   

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Histogram1D::binWidth() const
{
  return h_->axis(0).bin(0).width(); // bin 0 is the first real bin. bin -1 is the "left overflow";  bin nx is the "right overflow" 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Histogram1D::max() const
{ 
  auto range = boost::histogram::indexed(*h_);
   
  // find the max bin count...

  return *std::max_element(range.begin(), range.end(), [](auto const& lhs, auto const& rhs){ return lhs< rhs;});
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::tuple<double,double> Histogram1D::binRange(int index) const
{
  return std::make_tuple(h_->axis(0).bin(index).lower(), h_->axis(0).bin(index).upper());
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::tuple<double,double> Histogram1D::range() const
{
   auto xaxis = h_->axis(0);
   double xmin = xaxis.bin(0).lower();
   double xmax = xaxis.bin(xaxis.size()).lower();
   return std::make_tuple(xmin,xmax);
}   

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
