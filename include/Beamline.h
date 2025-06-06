//  =================================================================
//
//  Beamline.h
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

#ifndef BEAMLINE_H
#define BEAMLINE_H

#include <vector>
#include <memory>
#include <iterator>
#include <RMatrix.h>
#include <Element.h>
#include <boost/iterator/iterator_adaptor.hpp>

class Beamline: public Element {

 public:


  template<class T>
    class deep_iterator: public element_private_access, public boost::iterator_adaptor<
   deep_iterator<T>
   , typename std::vector<T>::iterator
   , T
   , boost::forward_traversal_tag
   > {

  private:
    struct enabler {};  // a private type avoids misuse
    int count_;

 public:
      deep_iterator()
        : deep_iterator::iterator_adaptor_() {}

    explicit deep_iterator(typename std::vector<T>::iterator p)
      : deep_iterator::iterator_adaptor_(p), count_(1) {}

    template <class OtherValue>
    deep_iterator(
        deep_iterator<OtherValue> const& other
        , typename std::enable_if<std::is_convertible<OtherValue, T>::value
      , enabler>::type = enabler()
    )
    : deep_iterator::iterator_adaptor_(other.base()) {}

    int getCount() { return count_;}
    
  private:

    friend class boost::iterator_core_access;
    void increment() {
      int slices = getSlices(**this->base_reference());
      if (count_ == slices ) { 
            ++this->base_reference();
            count_ = 1;
          }
          else {
              ++count_;
          }
    };
 };

  deep_iterator<std::shared_ptr<Element>> deep_begin() { return (deep_iterator<std::shared_ptr<Element>>(beamline_.begin())); }
  deep_iterator<std::shared_ptr<Element>>  deep_end () { return (deep_iterator<std::shared_ptr<Element>>(beamline_.end())); }
    
  typedef std::vector<std::shared_ptr<Element>>::iterator             iterator; 
  typedef std::vector<std::shared_ptr<Element>>::const_iterator const_iterator;   

  std::shared_ptr<Element>      & operator[](int i)       { return beamline_[i]; }
  std::shared_ptr<Element> const& operator[](int i) const { return beamline_[i]; }

  Beamline& operator=( Beamline const& bml) {
    beamline_ = bml.beamline_;
    return *this;
  }  

  // .... virtual functions from the Element class  

  void setParameters( int np, std::vector<double> const&, ... );

  RMatrix   rmatrix( double& alfap,  double& energy, double ms, double& tetaY, double dalfa, int st) const;
  RMatrix rmatrixsc( double& alfap,  double& energy, double ms, double current, BeamSize& bs,double& tetaY, double dalfa, int st ) const;
  
  void  preTrack( double ms,    double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1) const;
  int  trackOnce( double ms,   double& Enr0,    int n_elem, int n_turn, TrackParam& prm, RMatrix const& m1, Coordinates& v) const;
  int  track( double ms, double& Enr0,  Coordinates& v, double& tetaY ) const; // track trajectory

  // ...................

  
  void append(std::shared_ptr<Element> e);
  void resize(size_t sz) {  beamline_.resize(sz); }
  void erase (iterator istart,  iterator iend) {  beamline_.erase(istart, iend); }
  void clear()           {  beamline_.clear(); }
   
  iterator        begin()       { return   beamline_.begin();  }
  const_iterator cbegin() const { return   beamline_.cbegin(); };
  iterator          end()       { return   beamline_.end();    }
  const_iterator   cend() const { return   beamline_.cend();   };

  void updateEdges();  // scan the beamline for edges associated with bends and adjust edge element gradient attributes

  int size() const { return beamline_.size(); }

  std::vector<std::shared_ptr<Element>> beamline_;   // beamline array // [ was: Elm]

  double alfap;
  double energy;
  double ms;
  double tetaY;
  double dalfa;

};


#endif // BEAMLINE_H
