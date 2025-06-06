//  =================================================================
//
//  TuneDiagramSeriesData.cpp
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

#include <TuneDiagramSeriesData.h>
#include<iostream>
#include <algorithm>
#include <vector>


struct target_less
{
    template<class It>
    bool operator()(It const &a, It const &b) const 
  { return ( ( a->x() < b->x()) && ( a->y() < b->y() )); }
};

struct target_equal
{
    template<class It>
    bool operator()(It const &a, It const &b) const 
  { return ( (a->x() == b->x())  &&  (a->y() == b->y()) ); }
};

template<class It> It uniquify(It begin, It const end)
{
    std::vector<It> v;
    v.reserve(static_cast<size_t>(std::distance(begin, end)));
    for (It i = begin; i != end; ++i)
    { v.push_back(i); }
    std::sort(v.begin(), v.end(), target_less());
    v.erase(std::unique(v.begin(), v.end(), target_equal()), v.end());
    std::sort(v.begin(), v.end());
    size_t j = 0;
    for (It i = begin; i != end && j != v.size(); ++i)
    {
        if (i == v[j])
        {
            using std::iter_swap; iter_swap(i, begin);
            ++j;
            ++begin;
        }
    }
    return begin;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagramSeriesData::TuneDiagramSeriesData(int m, int n, int k, 
                                             double& Qxmin, double& Qxmax, double& Qymin, double& Qymax)  
  : QwtSeriesData<QPointF>(),  m_(m), n_(n), k_(k), 
    Qxmin_(Qxmin),  Qxmax_(Qxmax),   Qymin_(Qymin),   Qymax_(Qymax) 
{
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneDiagramSeriesData::~TuneDiagramSeriesData()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

size_t TuneDiagramSeriesData::size() const
{
   return 2;
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QPointF TuneDiagramSeriesData::sample( size_t i) const
{

  double qx = 0.0;
  double qy = 0.0;

  //  m Qx + n Qy = k  ; n*m != 0  

  if ( i == 0 ) { 

    points_.clear();

    if (n_ == 0 ) { // vertical line
      qx = double(k_)/double(m_); qy = Qymin_;
      points_.push_back( QPointF(qx, qy) ); 
      qx = double(k_)/double(m_); qy = Qymax_;
      points_.push_back( QPointF(qx, qy) );
      return points_[0]; 
    }
    
    if (m_ == 0 ) { // horizontal line
      qx = Qxmin_; qy = double(k_)/double(n_);
      points_.push_back( QPointF(qx, qy) ); 
      qx = Qxmax_; qy = double(k_)/double(n_);
      points_.push_back( QPointF(qx, qy) );
      return points_[0]; 
    }
   
    //  ... intersection with the (vertical) line Qx = Qxmin   
    
    qx =  Qxmin_ ;
    qy =  (double(k_)/double(n_)) - (double(m_)/double(n_))*Qxmin_ ;  

    if ( (qy >= Qymin_) &&  (qy <= Qymax_) )  points_.push_back( QPointF(qx, qy) );

    // ... intersection with Qy = Qymin 

    qx  = (double(k_)/double(m_)) - (double(n_)/double(m_))*Qymin_;
    qy  = Qymin_;

    if ( (qx >= Qxmin_) &&  (qx <= Qxmax_) )    points_.push_back( QPointF(qx, qy) );
 
    //  intersection with the line Qx = Qxmax   
      
    qx =  Qxmax_;
    qy =  (double(k_)/double(n_)) - (double(m_)/double(n_))*Qxmax_;

    if ( (qy >= Qymin_) &&  (qy <= Qymax_) )  points_.push_back( QPointF(qx, qy) );
    
    // intersection with Qy = Qymax

    qx  = (double(k_)/double(m_)) - (double(n_)/double(m_))*Qymax_;
    qy  = Qymax_;

    if ( (qx >= Qxmin_) &&  (qx <= Qxmax_) )  points_.push_back( QPointF(qx, qy) );
 
    // if there are more than 2 intersections, with the sides of the rectangle of interest, this
    // means that the line passes through a rectangle vertex. We get rid of the duplicates.  

    if ( points_.size() > 2 ) points_.erase( uniquify(points_.begin(), points_.end()), points_.end());

    // if qx, qy is outside the rectangle of interest
    // we just compute (0,  qy) and  (qx, 0) 

    if (n_*m_ != 0 && points_.size() <= 1) {
       qx = 0.0 ;  
       qy = double(k_)/double(n_);
       points_.push_back( QPointF(qx, qy) );
       qy = 0.0 ;  
       qx = double(k_)/double(m_);
       points_.push_back( QPointF(qx, qy) ); 
    }


    return points_[0];
  }
  else {
    return points_[1];
  }  
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QRectF TuneDiagramSeriesData::boundingRect() const
{
  return QRectF( QPointF( Qxmin_, Qymax_), QPointF( Qxmax_, Qymin_) ); 

  //return qwtBoundingRect(*this); 

}

