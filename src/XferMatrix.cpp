//  =================================================================
//
//  XferMatrix.cpp
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

#include <Beamline.h>
#include <Element.h>
#include <Constants.h>
#include <Coordinates.h>
#include <RMatrix.h>
#include <TrackParam.h>
#include <cstdarg>
#include <cstdlib>
#include <iostream>

using Constants::C_DERV1;


XferMatrix::XferMatrix(const char* nm, char const* fnm) // 'X'
  : Element(nm,fnm) {} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

XferMatrix::XferMatrix(XferMatrix const& o)
  : Element(o), tmat_(o.tmat_)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void XferMatrix::toString(char* buf) const
{
 sprintf(buf,"Matrix, D_E[MeV]=%g  L[cm]=%g", G, L_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

XferMatrix* XferMatrix::split(int nslices) const // return a sliced element
{
  // XferMatrix cannot be split so we return a cloned element. 
  return this->clone();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix  XferMatrix::rmatrix()
{
  return this->tmat_;
}

RMatrix  XferMatrix::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{
  
  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;

  energy += G;
  tetaY  += B;

  return this->tmat_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void XferMatrix::setParameters( int np, double dat[], ... )
{


  va_list args;
  va_start(args, dat);

  // G  = dat[0];
  // L_ = dat[1];

  double* rdat[] = { &G, &L_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  
  RMatrix* tm = (va_arg(args, RMatrix*));    

  // NOTE: tm is usually a zero 6x6 matrix
  // it is initialized with a call to setMatrix (see below)
  
  tmat_  = (tm) ? (*tm) : tmat_; 
  
  B = S = T_ = 0.0;

  va_end(args);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void XferMatrix::setParameters( int np, std::vector<double> const& dat, ... )
{


  va_list args;
  va_start(args, dat);

  // G  = dat[0];
  // L_ = dat[1];

  double* rdat[] = { &G, &L_};
  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  
  RMatrix* tm = (va_arg(args, RMatrix*));    

  // NOTE: tm is usually a zero 6x6 matrix
  // it is initialized with a call to setMatrix (see below)
  
  tmat_  = (tm) ? (*tm) : tmat_; 
  
  B = S = T_ = 0.0;

  va_end(args);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void XferMatrix::setMatrix( RMatrix const& tm)
{ 
  // this is called to set the matrix coefficients.
  // Unlike other elements, the data (in this case the underlying matrix)
  // is NOT set when the constructor is invoked.   
  
  tmat_ = tm;

  auto symplectify_off = getenv ("OPTIMX_MATRIX_SYMPLECTIFY_OFF");
  
  if (symplectify_off) { 
    std::cerr << "Matrix symplectification is OFF" << std::endl; 
  }
  else {
    tmat_.symplectify();
  }

}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int XferMatrix::trackOnce( double ms,   double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		      RMatrix const& m1, Coordinates& v) const
{

  int status = 0; 
  if (status = backwardTest(prm, n_elem, n_turn, v)) return status;


  double EnrNew = Enr0;
  double tetaY  = 0.0;

  RMatrix tm   = Element::rmatrix(EnrNew, ms, tetaY, 0.0, 3);
  v.c    = tm*v.c;

  
  double capa = sqrt(sqrt( (2*(Enr0)*ms + Enr0 * Enr0)/(2.*EnrNew*ms+EnrNew*EnrNew)));
 
  // verify this ...  

  v[0] *= capa;
  v[1] *= capa;
  v[2] *= capa;
  v[3] *= capa;
  v[5] *= capa*capa*(ms+EnrNew)/(ms+ Enr0);

  Enr0  = EnrNew;

  done:	

  if (status = transAmpTest(prm, n_elem, n_turn, v )) return status; 

  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Beamline* XferMatrix::splitnew(int nslices) const  // return a sliced element as a beamline 
{
  Beamline* bml = new Beamline();
  XferMatrix* e = this->clone();
  e->L_      /= nslices;
  e->slices_  = nslices;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
RMatrix  XferMatrix::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{
  return Element::rmatrix( frame, energy, ms, st);
}
