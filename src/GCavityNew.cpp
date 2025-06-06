//  =================================================================
//
//  GCavityNew.cpp
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

#include <Structs.h>
#include <Losses.h>
#include <Element.h>
#include <Beamline.h>
#include <Cavity.h>
#include <Constants.h>
#include <RMatrix.h>
#include <TrackParam.h>
#include <OptimMessages.h>


using std::acosh;

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;
using Constants::C_CGS;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Beamline* GCavity::splitnew(int nslices) const  // return a sliced element as a beamline 
{
 // GCavity cannot be split so we return a cloned element. 
  Beamline* bml = new Beamline();
  GCavity* e      = this->clone();
  e->L_      /= 1;
  e->slices_  = 1;
  bml->append(std::shared_ptr<Element>(e));
  return bml;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix   GCavity::rmatrix( RMatrix_t<3>& frame, double& energy, double ms, int st) const
{   

  double P      = sqrt(energy * (energy + 2. * ms));
  double hr     = P/C_DERV1;
  double bt     = P/(energy+ms);
  double gamma1 = 1. + energy/ms;
  double s;
  double c;
  
  RMatrix mi;
  mi.toUnity();

  ODEparam p;
  int err, nstep;

  if(fabs(G)<1.e-18){
    mi[0][1] = mi[2][3] = this->L_;
    return mi;
  }

  p.fieldtbl = ext_dat_; // &ext_dat[N];

  p.ms      = ms;
  p.wavelen = T_; 
  nstep     = (*NStep_)*(1.0 + 4*sqrt( G / energy )); // CtSt_.NStep 
  err       = get_cav_phase(energy, G, &p, nstep);
  p.phase   = S*PI/180.;

  double En2;
  err     = get_matrix(&p, energy, &En2, mi, nstep);

  if(err){ 
	// ***FIXME***  OptimWarningMessage(this, "Runtime err. for ODE integr. in cavity", intgrerr[err], QMessageBox::Ok); 
    return mi;
  }

  s = sqrt(sqrt((En2*En2+2.*En2*ms)/(energy * energy + 2. * energy*ms))); // sqrt(p2/p1)

  mi[0][0] *= s;  // coupling terms are not scaled ???? 
  mi[0][1] *= s;
  mi[1][1] *= s;
  mi[1][0] *= s;

  mi[2][2] *= s;
  mi[2][3] *= s;
  mi[3][3] *= s;
  mi[3][2] *= s;

  mi[4][4] *= s;
  mi[4][5] *= s;
  mi[5][5] *= s;
  mi[5][4] *= s;

  s = mi[0][0]*mi[1][1] - mi[0][1]*mi[1][0] - 1.0;  
  c = mi[4][4]*mi[5][5] - mi[4][5]*mi[5][4] - 1.0;

  // **FIXME*** if( (fabs(s)>CtSt_.Accuracy)||(fabs(c)>CtSt_.AccuracyL)){
  // **FIXME**  sprintf(buf,"Poor accuracy(transverse %e and longitudinal %e of required %e and %e).Correct parameters in View/Control menu",
  //            fabs(s), fabs(c), CtSt_.Accuracy, CtSt_.AccuracyL);
  // *** FIXME *** OptimWarningMessage(this, "Runtime err. for ODE integr. in cavity ", buf, QMessageBox::Ok);
  //};

  if(fabs(mi[0][1]) > 1.0e-10)   mi[1][0] = mi[3][2] = (mi[0][0]*mi[1][1]-1.0)/mi[0][1]; 
  if(fabs(mi[4][5]) > 1.0e-10)   mi[5][4] = (mi[4][4]*mi[5][5]-1.)/mi[4][5];

  energy = En2;  

  return mi;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void GCavity::preTrack( RMatrix_t<3>& frame, double ms,    double Enr0, int n_elem, TrackParam& prm, RMatrix& m1) const
{
   prm.p0   = sqrt(2.*ms*Enr0+Enr0*Enr0);
   prm.Hr0  = prm.p0/C_DERV1;
   prm.vp0  = C_CGS*prm.p0/sqrt(prm.p0*prm.p0+ms*ms);
   
   prm.Efin = Enr0;
   m1       = rmatrix( frame, prm.Efin, ms, 3);
}
