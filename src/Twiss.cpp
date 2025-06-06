//  =================================================================
//
//  Twiss.cpp
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

#include <utility>
#include <ostream>
#include <algorithm>
#include <Twiss.h>
#include <Vector.h>
#include <Structs.h>
#include <RMatrix.h>
#include <Constants.h>
#include <OptimCalc.h>
#include <stdexcept>

using Constants::PI;


Twiss::Twiss()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Twiss::Twiss( Twiss const& o)
  : BtX(o.BtX), AlX(o.AlX), 
    BtY(o.BtY), AlY(o.AlY),
    nuX(o.nuX), nuY(o.nuY),
    DsX(o.DsX), DsXp(o.DsXp), 
    DsY(o.DsY), DsYp(o.DsYp)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Twiss& Twiss::operator=( Twiss rhs)
{
   std::swap(BtX,  rhs.BtX);
   std::swap(AlX,  rhs.AlX);
   std::swap(BtY,  rhs.BtY);
   std::swap(AlY,  rhs.AlY);
   std::swap(nuX,  rhs.nuX);
   std::swap(nuY,  rhs.nuY);
   std::swap(DsX,  rhs.DsX);
   std::swap(DsXp, rhs.DsXp); 
   std::swap(DsY,  rhs.DsY);
   std::swap(DsYp, rhs.DsYp); 

    return *this;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Twiss& Twiss::operator=( Vector_t<6,double> const& rhs) // for setting dispersion  
{
   DsX  = rhs[0];
   DsXp = rhs[1]; 
   DsY  = rhs[2];
   DsYp = rhs[3]; 

   return *this;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Twiss::operator Vector_t<6,double>() const // conversion operator for dispersion 
{
  return Vector_t<6,double>{  DsX,  DsXp, DsY, DsYp, 0.0, 1.0 };
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
void Twiss::eigenvectors( std::complex<double> ev[][4]) const
{

  //--------------------------------------------------------------
  // Returns eigenvectors obtained from from Twiss parametrization
  //--------------------------------------------------------------

  ev[0][0] = {sqrt(BtX), 0.0};

  ev[0][1] = {-AlX/sqrt(BtX), -1.0/sqrt(BtX)};

  ev[0][2] = {0.0, 0.0};
  ev[0][3] = {0.0, 0.0};
  ev[2][0] = {0.0, 0.0};
  ev[2][1] = {0.0, 0.0};

  ev[2][2] = {sqrt(BtY), 0.0};

  ev[2][3] = { -AlY/sqrt(BtY), -1.0/sqrt(BtY) };

   for(int i=0; i<4; i++){ 

      ev[1][i] = conj(ev[0][i]);
      ev[3][i] = conj(ev[2][i]);
   }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Twiss::btfTransform( RMatrix const& tm, std::complex<double> ev[][4] )
{

 // btf: "beta functions" transformation
 //
 // ev : eigenvector matrix (eigenvectors are ROWS) 
 // This algorithm assumes that the lattice is *globally* decoupled.
 // However, local coupling is allowed.


  for(int i=0; i<4; ++i){

    ev[1][i] = {0.0, 0.0}; // those rows (eigenvectors) are redundant, so we use them
    ev[3][i] = {0.0, 0.0}; // to store the results (propagated) vectors

      for(int j=0; j<4; ++j) {
   	ev[1][i] += tm[i][j]*ev[0][j];
      	ev[3][i] += tm[i][j]*ev[2][j];
      }
   }
   
   double dnux = -arg(ev[1][0]/ev[0][0])/(2.*PI);  // phase factor propagated/initial  

   //if ( dnux < -1.0e-13 )  { dnux +=0.5; }

   double dnuy = -arg(ev[3][2]/ev[2][2])/(2.*PI);

   //if ( dnuy < -1.0e-13 ) { dnuy +=0.5;  }

   // overwrite initial with  propagated vectors  
   for(int i=0; i<4; ++i) {
     ev[0][i] = ev[1][i];
     ev[2][i] = ev[3][i];
   }

   Twiss4D v4;
   v4.betatronFromEV(ev);

   nuX += dnux;
   nuY += dnuy;

   // FIXME: VERIFY THESE FORMULAS !  
   BtX = v4.btx1 /(1-v4.u);  // u should be 0 when there is no coupling 
   BtY = v4.bty2 /(1-v4.u);
   AlX = v4.alfx1/(1-v4.u);
   AlY = v4.alfy2/(1-v4.u);

   // propagate the dispersion;

   (*this) = tm * Vector_t<6,double>(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::ostream& operator<<( std::ostream& os, Twiss const& t)
{
  os <<  "----------------------------" << "\n";
  os <<  " BtX  = " << t.BtX   << "\n";
  os <<  " AlX  = " << t.AlX   << "\n";
  os <<  " nuX  = " << t.nuX   << "\n";
  os <<  " BtY  = " << t.BtY   << "\n"; 
  os <<  " AlY  = " << t.AlY   << "\n"; 
  os <<  " nuY  = " << t.nuY   << "\n";
  os <<  " DsX  = " << t.DsX   << "\n";
  os <<  " DsXp = " << t.DsXp  << "\n";
  os <<  " DsY  = " << t.DsY   << "\n"; 
  os <<  " DsYp = " << t.DsYp  << "\n";
  os <<  "----------------------------" << "\n";
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Twiss4D::Twiss4D()
  :   btx1(0.0), alfx1(0.0),
      bty1(0.0), alfy1(0.0),
     teta1(0.0),
      btx2(0.0), alfx2(0.0),
      bty2(0.0), alfy2(0.0),
     teta2(0.0),     u(0.0),
        qx(0.0),    qy(0.0),
        dx(0.0),   dxp(0.0),
        dy(0.0),   dyp(0.0),
        e1(0.0),    e2(0.0)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Twiss4D::Twiss4D(Twiss4D const& o)
  :  btx1(o.btx1),
     alfx1(o.alfx1),
     bty1(o.bty1),
     alfy1(o.alfy1),
     teta1(o.teta1),
     btx2(o.btx2),
     alfx2(o.alfx2),
     bty2(o.bty2),
     alfy2(o.alfy2),
     teta2(o.teta2),
     u(o.u),
     qx(o.qx),
     qy(o.qy),
     dx(o.dx),
     dxp(o.dxp),
     dy(o.dy), 
     dyp(o.dyp),
     e1(o.e1),
     e2(o.e2)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Twiss4D& Twiss4D::operator=(Twiss4D rhs)
{

  std::swap(btx1,  rhs.btx1);
  std::swap(alfx1, rhs.alfx1);
  std::swap(bty1,  rhs.bty1);
  std::swap(alfy1, rhs.alfy1);
  std::swap(teta1, rhs.teta1);
  std::swap(btx2,  rhs.btx2);
  std::swap(alfx2, rhs.alfx2);
  std::swap(bty2,  rhs.bty2);
  std::swap(alfy2, rhs.alfy2);
  std::swap(teta2, rhs.teta2);
  std::swap(u,     rhs.u);
  std::swap(qx,    rhs.qx);
  std::swap(qy,    rhs.qy);
  std::swap(dx,    rhs.dx);
  std::swap(dxp,   rhs.dxp);
  std::swap(dy,    rhs.dy);
  std::swap(dyp,   rhs.dyp);
  std::swap(e1,    rhs.e1);
  std::swap(e2,    rhs.e2);

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Twiss4D& Twiss4D::operator=( Vector_t<6,double> const& rhs) // for setting dispersion  
{
   dx  = rhs[0];
   dxp = rhs[1]; 
   dy  = rhs[2];
   dyp = rhs[3]; 

   return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Twiss4D::operator Vector_t<6,double>() const // conversion operator for dispersion 
{
  return Vector_t<6,double>{  dx,  dxp, dy, dyp, 0.0, 1.0 };
}  


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Twiss4D::betatronFromEV(std::complex<double> const ev[][4] )
{
 // --------------------------------------------------------------------
 // Obtain the lattice parametrization from the
 // eigenvectors of the transfer matrix. 
 // For details, see FERMILAB-PUB-10-383-AD pp. 10-11
 //
 // NOTE: only the ev[0][i] and ev[2][i] are used !!!!
 //       since    ev[1][i] and ev[3][i] are complex conjugates
 //                and contain no additionla information
 //
 //        in other words ev[1][i] and ev[3][i] are IGNORED.
 //
 // The eigenvectors are assumed to be **normalized** so that
 //            v_k* U v_l = -2i delta_kl  
 // where:
 // 
 // i = imaginary  delta_kl = Kronecker's delta
 //
 // --------------------------------------------------------------------

  std::complex<double> ev_tmp[4][4];
  
  // copy the function argument
  for (int i=0; i<4; ++i) {
     for (int j=0; j<4; ++j) {
       ev_tmp[i][j] = ev[i][j];
    }
  };

  // sort the eigenvectors
  if (std::abs(ev_tmp[0][0]) < std::abs(ev_tmp[2][0]))  {
    for ( int i=0; i<4; ++i) {
      auto it0 = &ev_tmp[0][i];
      auto it2 = &ev_tmp[2][i];
      std::iter_swap(it0,it2);
    }
  };

  //......................................................

  
   double d = std::abs(ev_tmp[0][0]);

   btx1 = d*d;

   std::complex<double> s = ev_tmp[0][1]/ev_tmp[0][0]*btx1;

   alfx1 = -real(s);

   double utmp = imag(s) + 1.0;

   d           = std::abs(ev_tmp[0][2]); // magnitude of complex 
   bty1        = d*d;

   if(bty1 > 1.0e-12){
     s     = ev_tmp[0][3]/ev_tmp[0][2]*bty1;
     alfy1 = -real(s);
     u     = -imag(s);
   } 
   else {
     alfy1 = 0.0;
     u     = 0.0;
   }
   
   teta1 = arg(ev_tmp[0][2]/ev_tmp[0][0]);
    
   // ..................................................................................
   // check if the parameters u1 and u2 are related by u2 = 1-u1 as it should
   // for symplectic vectors. If not, this likely means that one of the elements
   // is not symplectic.
   // .................................................................................. 
   //double eps = 0.5*(fabs(u) + fabs(utmp)) > 1.0 ?  fabs((u - utmp)/(0.5*(fabs(u) + fabs(utmp)))) : fabs(u - utmp);
   //if( eps > 1.0e-4 && (0.5*(fabs(u) + fabs(utmp)) > 1.0e-4 ) ) {
   if( fabs(u - utmp) > 1.0e-5 ) {
      std::cout <<  "fabs(u - utmp) = " <<  fabs(u - utmp) << std::endl;
      std::cout <<  "fabs(u)    = " <<  fabs(u) << std::endl;
      std::cout <<  "fabs(utmp) = " <<  fabs(utmp) << std::endl;
      throw std::runtime_error("Lattice is unstable (Internal err.4 at 4D calc.)");
   }

   // ..................................................................................
   
   // vector 2
   d        = abs(ev_tmp[2][2]);
   bty2  = d*d;
   s        = ev_tmp[2][3]/ev_tmp[2][2]*bty2;
   alfy2 = -real(s);
   d        = abs(ev_tmp[2][0]);
   btx2  = d*d;

   if(btx2 > 1.0e-8) {
     s        = ev_tmp[2][1]/ev_tmp[2][0]*btx2;
     alfx2 = -real(s);
   } 
   else {
     alfx2 = 0.0;
   }

   teta2 = arg(ev_tmp[2][0]/ev_tmp[2][2]);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void Twiss4D::eigenvectors( std::complex<double> ev[][4]) const
{
  //--------------------------------------------------------------
  // Returns eigenvectors obtained from from Twiss parametrization
  // For details, see FERMILAB-PUB-10-383-AD pp. 10-11
  //--------------------------------------------------------------

  ev[0][0] = { sqrt(btx1), 0.0 };
  ev[0][1] = {-alfx1/sqrt(btx1) , (u-1.0)/sqrt(btx1) };
  ev[0][2] = sqrt(bty1)*std::complex<double>(cos(teta1), sin(teta1));

  ev[0][3] = (bty1 > 1.0e-8) ? std::complex<double>(-alfy1, -u) * std::complex<double>(cos(teta1), sin(teta1)) / sqrt(bty1)
                                : std::complex<double>(0.,0.);
  ev[2][2] = { sqrt(bty2), 0.0};
  ev[2][3] = { -alfy2/sqrt(bty2), (u-1.0)/sqrt(bty2) };
  ev[2][0] = sqrt(btx2)*std::complex<double>(cos(teta2), sin(teta2));
   
  ev[2][1] = (btx2 > 1.0e-8) ? std::complex<double>(-alfx2, -u) * std::complex<double>( cos(teta2), sin(teta2)) / sqrt(btx2)
                             :  std::complex<double>(0.0,0.0);   

  for(int i=0; i<4; ++i){
      ev[1][i] = conj(ev[0][i]);
      ev[3][i] = conj(ev[2][i]);
  }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::ostream& operator<<( std::ostream& os, Twiss4D const& t4d)
{
  os <<  "----------------------------" << "\n";
  os <<  " btx1  = " << t4d.btx1   << "\n";
  os <<  " alfx1 = " << t4d.alfx1  << "\n";
  os <<  " bty1  = " << t4d.bty1   << "\n"; 
  os <<  " alfy1 = " << t4d.alfy1  << "\n"; 
  os <<  " teta1 = " << t4d.teta1  << "\n"; 
  os <<  " btx2  = " << t4d.btx2   << "\n";
  os <<  " alfx2 = " << t4d.alfx2  << "\n";
  os <<  " bty2  = " << t4d.bty2   << "\n"; 
  os <<  " alfy2 = " << t4d.alfy2  << "\n";
  os <<  " teta2 = " << t4d.teta2  << "\n";
  os <<  " u     = " << t4d.u      << "\n"; 
  os <<  " qx    = " << t4d.qx     << "\n";
  os <<  " qy    = " << t4d.qy     << "\n";
  os <<  " dx    = " << t4d.dx     << "\n"; 
  os <<  " dxp   = " << t4d.dxp    << "\n";
  os <<  " dy    = " << t4d.dy     << "\n";
  os <<  " dyp   = " << t4d.dyp    << "\n";
  os <<  " e1    = " << t4d.e1     << "\n";
  os <<  " e2    = " << t4d.e2     << "\n";
  os <<  "----------------------------" << "\n";

  double kappax   =  t4d.btx2/t4d.btx1; 
  double kappay   =  t4d.bty1/t4d.bty2; 
  double kappax2  =  kappax*kappax;
  double kappay2  =  kappay*kappay;
  double kappaxy2 =  kappax2*kappay2;
  
  //double Ax = (kappax < 1.0e-12) ? 0.0 : kappax * t4d.alfx1 - t4d.alfx2/kappax;
  //double Ay = (kappay < 1.0e-12) ? 0.0 : kappay * t4d.alfy1 - t4d.alfy2/kappay;
  double Ax = kappax * t4d.alfx1 - t4d.alfx2/kappax;
  double Ay = kappay * t4d.alfy1 - t4d.alfy2/kappay;

  //double tmp  =  (kappax*kappax*kappay*kappay)*(kappax*kappax*kappay*kappay)*(1.0 + (Ax*Ax - Ay*Ay)/(kappax*kappax- kappay*kappay)*(1.0-kappax*kappax* kappay*kappay)));
  double tmp  =  (1.0 + (Ax*Ax-Ay*Ay)/(kappax2-kappay2)*(1.0-kappaxy2));
									      
  double up   = (fabs(tmp) < 1.0e-12) ?  -kappaxy2: -kappaxy2 + (kappax*kappay) * sqrt(tmp)/(1-kappaxy2);
  double um   = (fabs(tmp) < 1.0e-12) ?  -kappaxy2: -kappaxy2 - (kappax*kappay) * sqrt(tmp)/(1-kappaxy2); 

  double allowed = ((Ax*Ax-Ay*Ay)/(kappax2-kappay2))*(1.0-kappax2*kappay2);  // should be >= -1
  
  os << " allowed =  " << allowed << std::endl; 
  os << " kappax  =  " << kappax  << std::endl; 
  os << " kappay  =  " << kappay  << std::endl;
  os << " Ax = "      <<  Ax  << std::endl; 
  os << " Ay = "      <<  Ay  << std::endl; 
  os << " tmp = "     <<  tmp << std::endl; 
  os << " up   = "    <<  up  << std::endl; 
  os << " um   = "    <<  um  << std::endl; 

  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Twiss4D operator-(Twiss4D const& lhs, Twiss4D const& rhs)
{

  Twiss4D result(rhs);

  result.btx1  = lhs.btx1 -   rhs.btx1; 
  result.alfx1 = lhs.alfx1  - rhs.alfx1; 
  result.bty1  = lhs.bty1   - rhs.bty1; 
  result.alfy1 = lhs.alfy1  - rhs.alfy1; 
  result.teta1 = lhs.teta1  - rhs.teta1; 

  result.btx2  = lhs.btx2   - rhs.btx2; 
  result.alfx2 = lhs.alfx2  - rhs.alfx2; 
  result.bty2  = lhs.bty2   - rhs.bty2; 
  result.alfy2 = lhs.alfy2  - rhs.alfy2;
  result.teta2 = lhs.teta2  - rhs.teta2; 

  result.u     = lhs.u      - rhs.u; 
  
  result.dx    = lhs.dx     - rhs.dx; 
  result.dxp   = lhs.dxp    - rhs.dxp; 
  result.dy    = lhs.dy     - rhs.dy; 
  result.dyp   = lhs.dyp    - rhs.dyp;

  
  return result;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

		  
Twiss4D operator+(Twiss4D const& lhs, Twiss4D const& rhs)
{

  Twiss4D result(rhs);
  
  result.btx1  = lhs.btx1   + rhs.btx1; 
  result.alfx1 = lhs.alfx1  + rhs.alfx1; 
  result.bty1  = lhs.bty1   + rhs.bty1; 
  result.alfy1 = lhs.alfy1  + rhs.alfy1; 
  result.teta1 = lhs.teta1  + rhs.teta1; 

  result.btx2  = lhs.btx2   + rhs.btx2; 
  result.alfx2 = lhs.alfx2  + rhs.alfx2; 
  result.bty2  = lhs.bty2   + rhs.bty2; 
  result.alfy2 = lhs.alfy2  + rhs.alfy2;
  
  result.teta2 = lhs.teta2  + rhs.teta2; 
  result.u     = lhs.u      + rhs.u; 
  
  result.dx    = lhs.dx     + rhs.dx; 
  result.dxp   = lhs.dxp    + rhs.dxp; 
  result.dy    = lhs.dy     + rhs.dy; 
  result.dyp   = lhs.dyp    + rhs.dyp;

  return result;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

		  
Twiss4D operator*(double lhs, Twiss4D const& rhs)
{

  Twiss4D result(rhs);
  
  result.btx1  = lhs*rhs.btx1; 
  result.alfx1 = lhs*rhs.alfx1; 
  result.bty1  = lhs*rhs.bty1; 
  result.alfy1 = lhs*rhs.alfy1; 
  result.teta1 = lhs*rhs.teta1; 

  result.btx2  = lhs*rhs.btx2; 
  result.alfx2 = lhs*rhs.alfx2; 
  result.bty2  = lhs*rhs.bty2; 
  result.alfy2 = lhs*rhs.alfy2;
  result.teta2 = rhs.teta2; 

  result.u     = lhs*rhs.u; 
  
  result.dx    = lhs*rhs.dx; 
  result.dxp   = lhs*rhs.dxp; 
  result.dy    = lhs*rhs.dy; 
  result.dyp   = lhs*rhs.dyp;

  
  return result;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const Twiss4D::norm()
{

  double maxbetax = std::max(fabs(btx1),    fabs(btx2)); 
  double maxalfx  = std::max(fabs(alfx1),   fabs(alfx2)); 
  double maxbetay = std::max(fabs(bty1),    fabs(bty2)); 
  double maxalfy  = std::max(fabs(alfy1),   fabs(alfy2)); 
  double maxteta  = std::max(fabs(teta1),   fabs(teta2)); 
  double maxu     = fabs(u);  
  
  double maxdx    = fabs(dx);
  double maxdxp   = fabs(dxp);
  double maxdy    = fabs(dy);
  double maxdyp   = fabs(dyp);

  // the weight on teta is set to 0.0; This may not be the best choice 
  
  double vec[] = { maxbetax,   maxalfx,   maxbetay,  maxalfy,  0.0 * maxteta,  maxu,  maxdx,   maxdxp,   maxdy,  maxdyp};

  return *std::max_element( const_cast<double*>(&vec[0]), const_cast<double*>(&vec[0])+ sizeof(vec)/sizeof(double));
				  
}
		  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Twiss4D::getU4Dp(double phase, int& ret) const
{
 
  double cpx  = sqrt(btx2/btx1);
  double cpy  = sqrt(bty1/bty2);
  double Ax   = cpx*cpx*alfx1-alfx2;
  double Ay   = cpy*cpy*alfy2-alfy1;
  double c    = cos(phase);
  double s    = sin(phase);
  double cpx2 = cpx*cpx;
  double cpy2 = cpy*cpy;
  double U    = (cpx2+cpy2-2.)*cpx*cpy+(2.*cpx2*cpy2-cpx2-cpy2)*c-(Ax*(cpy2-1.)+Ay*(cpx2-1.))*s;
  double t    = (cpx2-1.)*(cpx2-1.)*cpy2+(cpy2-1.)*(cpy2-1.)*cpx2+2.*(cpx2-1.)*(cpy2-1.)*cpx*cpy*c;

  if(fabs(t)<1.e-30){ret=1; return 0.5;}

  U = cpx*cpy*U/t;
  return U;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Twiss4D::getU4Dm( double phase, int& ret) const
{
  double cpx  = sqrt(btx2/btx1);
  double cpy  = sqrt(bty1/bty2);
  double Ax   = cpx*cpx*alfx1-alfx2;
  double Ay   = cpy*cpy*alfy2-alfy1;
  double c    = cos(phase);
  double s    = sin(phase);
  double cpx2 = cpx*cpx;
  double cpy2 = cpy*cpy;
  double U    = (cpx2+cpy2+2.)*cpx*cpy-(2.*cpx2*cpy2+cpx2+cpy2)*c-(Ax*(cpy2+1.)-Ay*(cpx2+1.))*s;
  double t    = (cpx2+1.)*(cpx2+1.)*cpy2+(cpy2+1.)*(cpy2+1.)*cpx2-2.*(cpx2+1.)*(cpy2+1.)*cpx*cpy*c;
  
  if (fabs(t)<1.e-30) {ret=1; return 0.5;}
  
  U = cpx*cpy*U/t;
  return U;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

