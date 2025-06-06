//  =================================================================
//
//  Rotation.cpp
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
#include <ostream>
#include <iomanip>
#include <cmath>
#include <Rotation.h>
#include <numeric>
#include <Constants.h>

using Constants::PI;


std::ostream& operator<<( std::ostream& out, Vector const& rhs)
{
    out << "-----------------" << std::endl;
    for (int i=0; i<3; ++i) {
	out << std::setw(16) << rhs.vec_[i] << "  ";
    }
    out << "\n-----------------" << std::endl;

    return out;
}

Vector::Vector()
{ }

Vector::Vector( std::initializer_list<double> lst)
{
  int i = 0;
  for (auto it = lst.begin(); it != lst.end(); ++it) {
    vec_[i++] = *it;
  } 
}


Vector operator^( Vector const& lhs,  Vector const& rhs)
{
  return Vector { lhs.vec_[1]*rhs.vec_[2] - lhs.vec_[2]*rhs.vec_[1],
                  lhs.vec_[2]*rhs.vec_[0] - lhs.vec_[0]*rhs.vec_[2],
                  lhs.vec_[0]*rhs.vec_[1] - lhs.vec_[1]*rhs.vec_[0] };

}


Vector Vector::normalize() const
{
  Vector result(*this);
  return result/sqrt(result*result);
}  
  
  

double operator*( Vector const& lhs,  Vector const& rhs)
{
  return  lhs.vec_[0]*rhs.vec_[0] +  
          lhs.vec_[1]*rhs.vec_[1] +
          lhs.vec_[2]*rhs.vec_[2];

}

Vector&  operator+=( Vector& lhs,  Vector const& rhs)
{

  for (int i=0; i<3; ++i) {
    lhs.vec_[i] += rhs.vec_[i];
  }

  return lhs;
}

Vector&  operator-=( Vector& lhs,  Vector const& rhs)
{
  for (int i=0; i<3; ++i) {
    lhs.vec_[i] -= rhs.vec_[i];
  }

  return lhs;
}



std::ostream& operator<< ( std::ostream& out, Rotation const& rhs)
{
    out << "-----------------" << std::endl;
    for (int i=0; i<3; ++i) {
      for (int j=0; j<3; ++j) {
	out << std::setw(16) << rhs[i][j] << "  ";
       }
       out << std::endl;
    }

    out << "-----------------" << std::endl;

    return out;
}

Rotation::Rotation()
{}

Rotation::Rotation( Axis axis, double angle )
{

 //-----------------------------------------------------------
 // The matrices represent an *anti-clockwise*
 // (POSITIVE) rotation of the **axes**. 
 // Equivalently, the matrices rotate a vector by -angle
 // about the x,y, or z axis.
 // A right-handed coordinate system is assumed.
 //------------------------------------------------------------
  
  for (int i=0; i<3; ++i) {
    for (int j=0; j<3; ++j){
      rot_[i][j] = 0.0;
    }
  }

  double s = sin(angle);
  double c = cos(angle);
  
  switch (axis) {

  case x_axis :

    rot_[0][0] = 1.0;
    rot_[1][1] =  c;
    rot_[1][2] = -s;
    rot_[2][1] =  s;
    rot_[2][2] =  c;
    break;

  case y_axis :

    rot_[0][0] =  c;
    rot_[0][2] =  s;
    rot_[1][1] =  1.0;
    rot_[2][0] = -s;
    rot_[2][2] =  c;
    break;

  case z_axis :

    rot_[0][0] =  c;
    rot_[0][1] = -s;
    rot_[1][0] =  s;
    rot_[1][1] =  c;
    rot_[2][2] =  1.0;
    break;

  default:
    std::cout << " error " << std::endl;
  }    

}

Rotation::Rotation( Vector const& u, double angle)
{

 //rotation about an arbitrary axis.
  
 //-----------------------------------------------------------
 // The matrice represents an *anti-clockwise*
 // (POSITIVE) rotation of a vector about an axis defined by u. 
 // Equivalently, the matrix rotates a coordinate system by -angle
 // about the direction defined by u
 // A right-handed coordinate system is assumed.
 //------------------------------------------------------------

  Vector un = u/sqrt(u*u);
  
  double const& ux = un[0]; 
  double const& uy = un[1]; 
  double const& uz = un[2]; 

  double sn =  sin(angle);
  double cs =  cos(angle);
   
  rot_[0][0] = cs + ux*ux*(1-cs);  
  rot_[0][1] = ux*uy*(1-cs) + uz*sn;    
  rot_[0][2] = ux*uz*(1-cs) - uy*sn;


  rot_[1][0] = uy*ux*(1-cs) - uz*sn; 
  rot_[1][1] = cs + uy*uy*(1-cs);     
  rot_[1][2] = uy*uz*(1-cs)+ux*sn;

  rot_[2][0] = uz*ux*(1-cs) + uy*sn; 
  rot_[2][1] = uz*uy*(1-cs) - ux*sn;     
  rot_[2][2] = cs + uz*uz*(1-cs); 

}  

Rotation::Rotation( Vector const& ux,  Vector const& uy, Vector const& uz, Rotation::Layout layout)
{

  Vector const* v[] = { &ux, &uy, &uz}; 
  
  if (layout == rows) {  // transpose = argument vectors as rows.  

    for ( int j=0; j<3; ++j) { 
      auto u  = v[j];
        for ( int i=0; i<3; ++i) { 
          rot_[i][j] = (*u)[i];
        }
    }

    return;
  }


  
  for ( int i=0; i<3; ++i) { // transpose = false argument vectors as cols.  
    auto u  = v[i];
    for ( int j=0; j<3; ++j) { 
      rot_[i][j] = (*u)[j];
    }
  }


}  


Rotation::RotationRow_t Rotation::operator[](int i)
{
  return RotationRow_t(rot_[i]);
}
  
Rotation::RotationRow_t const Rotation::operator[](int i) const
{
  return RotationRow_t(rot_[i]);
}


Rotation Rotation::transpose() const
{
  Rotation result;
  for (int i=0; i<3; ++i) {
    for (int j=0; j<3; ++j) {
      result.rot_[i][j] = rot_[j][i];
    }
  }
  return result;
}

Rotation Rotation::inverse() const
{
  return this->transpose();
}

  

Rotation operator*( Rotation const& lhs,  Rotation  const& rhs)
{

  Rotation result;

  for (int i=0; i<3; ++i) {
    for (int j=0; j<3; ++j) {
      result[i][j] = 0.0;
      for (int k=0; k<3; ++k) {
        result[i][j] += lhs[i][k]*rhs[k][j];
      }
    }
  }

  return result;
}  


Vector operator*( Rotation const& lhs,  Vector const& rhs )
{

  Vector result;

  for (int i=0; i<3; ++i){

     result[i] = 0.0;

     for (int k=0; k<3; ++k) {
       result[i] += lhs[i][k]*rhs[k];
     }
  }

  return result;
}


Vector operator*( Vector const& lhs,  double const& rhs)
{

  Vector result(lhs);

  for (int i=0; i<3; ++i) {
    result[i] *= rhs;
  }
  return result;
  
}




Vector operator/( Vector const& lhs,  double const& rhs)
{
  Vector result(lhs);

  for (int i=0; i<3; ++i) {
    result[i] /= rhs;
  }
  return result;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if  1
Rotation localToGlobal( double tx, double ty )
{
  
  //---------------------------------------------------------------
  // Establish the local frame at the  at the upstream element
  // face using the unit vector tangent to the reference trajectory.
  // The orientation of the  tangent vector is defined by the 2
  // angles tx and ty.
  //----------------------------------------------------------------

  Rotation roty(Rotation::y_axis, tx); 

  Vector uz = {0,0,1};
         uz = roty*uz;

  Vector ux = {1,0,0};
         ux = roty*ux;               // new local x axis 

  Rotation rotx( ux, ty);
	 ///uz = rotx * uz;               // rotation around local x-axis
 
  // at this point uz is a unit tangent vector to the trajectory,  in global coordinates.

  // we now determine the unit vectors ux and uy, which together with uz determine the
  // local coordinate frame.


  // ux  =  { uz[2], 0.0, -uz[0]};                     // (orthogonal to projection of the tangential vector in x-z plane)
  // ux  =  ux.normalize();                            // unit vector in local hor plane

  
	 ///Vector   uy  =  uz^ux;                            // local uy

   Vector uy = rotx * Vector{0,1,0};               // rotation around local x-axis

   return Rotation(ux, uy, uz, Rotation::rows);      // this is the **inverse** extrinsic (vector)  transformation matrix before local z-rotation    
                                                     // or equivalently, the transformation matrix from local to global coordinates    
}

#endif

#if 1
Rotation localToGlobalnew( double tx, double ty )
{
  
  Rotation roty(Rotation::y_axis, tx); 
  Rotation rotx(Rotation::x_axis, ty);  
  
  Rotation grot = roty*rotx; // intrinsic rotation of the coordinate system Y, X' 

  return grot.transpose();

}

#endif
