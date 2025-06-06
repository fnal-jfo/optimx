//  =================================================================
//
//  RMatrix.cpp
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

#include <RMatrix.h>
#include <Utility.h>
#include <Constants.h>

#include <stdexcept>


using Constants::PI;

inline double degToRad( double degs) { return (degs/180)*PI; }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
RMatrix_t<6> RMatrix_t<6>::m_tilt(double alfa)
{  

  // returns 6x6 matrix [result] set to rotation matrix around z-axis 
  // by angle alpha
  
  RMatrix result;
  
  for(int i=0; i<6; ++i) {
     for(int j=0; j<6; ++j) {
         result[i][j]=0.0;
     }
  }

  /*-------------------
  //  c  0  s  0  0  0
  //  0  c  0  s  0  0
  // -s  0  c  0  0  0
  //  0 -s  0  c  0  0
  //  0  0  0  0  1  0
  //  0  0  0  0  0  1
  -------------------*/
    
  double s = sin(alfa);
  double c = cos(alfa);
  
  // by columns ... 
  result[0][0] =  c;
  result[2][0] = -s;

  result[1][1] =  c;
  result[3][1] = -s;

  
  result[0][2] = s;
  result[2][2] = c;

  result[1][3] = s;
  result[3][3] = c;

  result[4][4] = 1.0;

  result[5][5] = 1.0;


  return result;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
RMatrix_t<6> RMatrix_t<6>::tilt(double alfa) const
{
  using Constants::PI;
  RMatrix result(*this);
  
  alfa   *= (PI/180.0);
  auto tl = m_tilt(alfa);

  result  = result * tl;
       tl = m_tilt(-alfa);

  result = tl* result;

  return result;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


#if 0
template <>
RMatrix_t<2> RMatrix_t<2>::operator*(  RMatrix_t<2> const& rhs  ) const
{

  RMatrix_t<2> result;

  result[0][0]    = (*this)[0][0] * rhs[0][0] + (*this)[0][1] * rhs[1][0];
  result[0][1]    = (*this)[0][0] * rhs[0][1] + (*this)[0][1] * rhs[1][1];
  result[1][0]    = (*this)[1][0] * rhs[0][0] + (*this)[1][1] * rhs[1][0];
  result[1][1]    = (*this)[1][0] * rhs[0][1] + (*this)[1][1] * rhs[1][1];

  return result; 
}
#endif

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// local to global coordinate transformation (rotation) angles in DEGREES 
// (note: inverse i.e. global to local transformationis the transposed matrix)  

template<>
RMatrix_t<3>  RMatrix_t<3>::localToGlobal(double psi, double phi, double theta)
{

  psi   = degToRad(psi);
  phi   = degToRad(phi);
  theta = degToRad(theta);
  
  double sn = sin(psi);
  double cs = cos(psi);
  
  double m1[][3] = { {cs, -sn, 0},  {sn, cs, 0},  {0,0,1} };
  RMatrix_t<3> m_psi(m1);

  sn = sin(psi);
  cs = cos(psi);
  
  double m2[][3] = { {1, 0, 0},  {0, cs, sn},  {0,-sn, cs} };
  RMatrix_t<3> m_phi(m2);

  sn = sin(theta);
  cs = cos(theta);

  double m3[][3] = { {cs, 0, sn},  {0, 1, 0},  {-sn, 0, cs} };
  RMatrix_t<3> m_theta(m3);
  
  return m_theta*m_phi* m_psi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
RMatrix_t<6>  RMatrix_t<6>::localToGlobal(double psi, double phi, double theta)
{

  psi   = degToRad(psi);
  phi   = degToRad(phi);
  theta = degToRad(theta);
  
  double sn = sin(psi);
  double cs = cos(psi);
  
  double m1[6][6] = { {cs,  0.0, -sn,   0.0,   0.0, 0.0},
                      {0.0, cs,   0.0, -sn,    0.0, 0.0},
                      {sn,  0.0,  cs,   0.0,   0.0, 0.0},
                      {0.0, 0.0,  0.0,  0.0,   1.0, 0.0},
                      {0.0, 0.0,  0.0,  0.0,   0.0, 1.0}
                    };

  RMatrix_t<6> m_psi(m1);
  
  sn = sin(psi);
  cs = cos(psi);
  
  double m2[6][6] = { { 1.0,  0.0,  0.0,  0.0,  0.0,  0.0 },
                      { 0.0,  1.0,  0.0,  0.0,  0.0,  0.0 },
                      { 0.0,  0.0,  cs,   0.0,  sn,   0.0 },
                      { 0.0,  0.0,  0.0,  cs,   0.0,  sn  },
                      { 0.0,  0.0, -sn,   0.0,  cs,   0.0 },
                      { 0.0,  0.0,  0.0, -sn,   0.0, -cs  }
                    };
  
  RMatrix_t<6> m_phi(m2);

  sn = sin(theta);
  cs = cos(theta);

  double m3[6][6] =    {  {  cs,  0.0,  0.0, 0.0, sn, 0.0  },
                          {  0.0, cs,   0.0, 0.0, 0.0, sn  },
                          {  0.0, 0.0,  1.0, 0.0, 0.0, 0.0 },
                          {  0.0, 0.0,  0.0, 1.0, 0.0, 0.0 },
                          { -sn,  0.0,  0.0, 0.0, cs,  0.0 },
                          { 0.0, -sn,   0.0, 0.0, 0.0, cs  }
                       };


RMatrix_t<6> m_theta(m3);
  
return m_theta*m_phi* m_psi;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

