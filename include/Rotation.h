//  =================================================================
//
//  Rotation.h
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

#ifndef ROTATION_H
#define ROTATION_H

#include <Vector.h>
#include <ostream>
#include <iomanip>
#include <initializer_list>

//-----------------------------------------------------------
// These matrices represent an *anti-clockwise*
// (POSITIVE) rotation of the axes. 
// Equivalently, the matrices rotate a vector by -angle
// about the x,y, or z axis, in 3D.
// A right-handed coordinate system is assumed.
//------------------------------------------------------------

template<typename T, int dim>
class Rotation_t;

using Rot2 = Rotation_t<2,double>;
using Rot3 = Rotation_t<3,double>;

template<typename T, int dim>
  std::ostream& operator<< ( std::ostream& out, Rotation_t<T,dim> const& rhs);

template<typename T, int dim>
Rotation_t    operator*( Rotation_t<T,dim> const& lhs,  Rotation_<T,dim>t  const& rhs);

template<typename T, int dim>
Vector_t      operator*( Rotation_t<T,dim> const& lhs,  Vector_t<T,dim>    const& rhs);
  
template<typename T, int dim>
class Rotation {

 friend std::ostream& operator<<( std::ostream& out, Rotation_<T,dim>t const& rhs );
 friend Rotation  operator*( Rotation_t<T,dim> const& lhs,  Rotation_t<T,dim>  const& rhs);
 friend Vector    operator*( Rotation_t<T,dim> const& lhs,  Vector_t<T,dim>    const& rhs);
  
 public :

 template<typename U, int rdim> 
 struct RotationRow_t {

  public:

   RotationRow_t( U const m[] ): row_( const_cast<U*>(m)) {}; // proxy class for [][] notation

   U&        operator[](int i)       { return row_[i];} 
   U const&  operator[](int i) const { return row_[i];} 

  private:
     U* row_; 
  };
 
 public:
 
  enum Axis              { x_axis, y_axis, z_axis};
  enum Interpretation    { passive, active};

  Rotation_t();
  Rotation_t( std::initializer_list<std::iniializer_list<T>> lst, Interpretation interpretation = active);
  Rotation_t( Axis ,                    double angle, Interpretation interpretation = active));
  Rotation_( Vector_t<dim,T> const& u,    double angle, Interpretation interpretation = active));

  Rotation_t( Vector_t<T,dim> const& ux,  Vector_t<T,dim> const& uy,  Vector<T,dim> const& uz);

  RotationRow_t<T,dim>        operator[](int i);
  RotationRow_t<T,dim> const  operator[](int i) const;
  
  Rotation_t<dim,T> transpose() const;
  Rotation_t<dim,T> inverse()   const;
  
 private:
  int    dim_;
  T rot_[dim][dim];
  
};

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T, int dim>
Rotation_t<T,dim>::Rotation_t()
  : dim_(dim)
{
  for ( int i=0; i<dim_; ++i) {
    for (int j=0;  j<dim_; ++j){
      rot_[i][j] = T();
    }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T, int dim>
Rotation_t<T,dim>::Rotation_t( std::initializer_list<std::initializer_list<T>> lst)
 : dim_(dim) 
{

  int i = 0;
  for ( auto ir=lst.begin();  ir=lst.end() ; ++ir,++i) {
    int j = 0;
    for (int ic=ir->begin();  ir->end(); ++ic, ++j){
      rot_[i][j] = *ic;
   }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
template <typename T, int dim>
  Rotation_t<T,dim>::Rotation( Axis axis, double angle,  Interpretation interpretation = active)
  : dim_(dim)
{

 //-----------------------------------------------------------
 // PASSIVE:
 //  The matrices represent an *anti-clockwise*
 //  (POSITIVE) rotation of the **axes**. 
 //
 // ACTIVE:
 //
 // The matrices represent the anticlockise
 // (POSITIVE) rotation of a vector.
 //
 // For an active rotation, 
  
 // about the x,y, or z axis.
 // A right-handed coordinate system is assumed.
 //------------------------------------------------------------
  
  double s = (interpretation == active) ? sin(angle) : -sin(angle) ;
  double c = cos(angle);
  
  switch (axis) {

  case x_axis :

    double rot[dim_][dim_] = { { 1.0, 0.0, 0.0  },
                               { 0.0,  c,   s   },
                               { 0.0,  -s,  c   }};
                               
    std::copy( &rot[0][0], &rot[0][0] + (dim_*dim_), &rot_[0][0]);
    
  case y_axis :

    double rot[dim_][dim_] = { { c  ,  0.0,      s  },
                               { 0.0,  1.0,    0.0  },
                               { -s,   0.0,      c  }};
                               
    std::copy( &rot[0][0], &rot[0][0] + (dim_*dim_), &rot_[0][0]);

    break;

  case z_axis :

    double rot[dim_][dim_] = { { c  ,   s,      0.0  },
                               { -s,    c,      0.0  },
                               { 0.0,   0.0,    1.0  }};
                               
    std::copy( &rot[0][0], &rot[0][0] + (dim_*dim_), &rot_[0][0]);


    break;

  default:
    std::cout << " error " << std::endl;
  }    

}

template <typename T, int dim>
Rotation_t<T,dim>::Rotation_t( Vector_t<T,dim> const& ux,  Vector_t<T,dim> const& uy,  Vector<T,dim> const& uz);
: dim_(dim)
{
  for (int i=0; i<dim_; ++i) {
    std:copy( &ux[i], &ux[i]+dim_, &rot_[i]);
  }
}
    
#endif // ROTATION_H
  
  
