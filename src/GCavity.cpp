//  =================================================================
//
//  GCavity.cpp
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
#include <Element.h>
#include <Beamline.h>
#include <Cavity.h>
#include <Constants.h>
#include <RMatrix.h>
#include <TrackParam.h>
#include <OptimMessages.h>
#include <Coordinates.h>


using std::acosh;

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;
using Constants::C_CGS;

GCavity::GCavity(const char* nm, char const* fnm)
  : Element(nm,fnm), ext_dat_(0) {} //  fieldtbl_(0) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

GCavity::GCavity(GCavity const& o)
  : Element(o),
    ext_dat_(o.ext_dat_),
    NStep_(o.NStep_),
    e_(&o.ext_dat_->x[0],
       &o.ext_dat_->y[0],
       ext_dat_->n ) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void GCavity::toString(char* buf) const
{
  //***FIXME***
  double energy     = 0.0;
  double energy_old = 0.0;
  sprintf(buf,"  D_E[MeV]=%g  E_out[MeV]=%g", energy-energy_old, energy);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

GCavity* GCavity::split(int nslices) const // return a sliced element
{
  // GCavity cannot be split so we return a cloned element. 
  return this->clone();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

RMatrix GCavity::rmatrix(double& alfap, double& energy, double ms, double& tetaY, double dalfa, int st) const
{

  alfap = 0.0;
  
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

void GCavity::setParameters( int np, double dat[], ... ) 
{
  double Ndum = 0.0;
  double* rdat[] = { &L_, &G, &S, &T_, &Ndum };

  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  if (Ndum !=0.0) N = floor(Ndum);
  
  //L_ = dat[0];
  //G  = dat[1];
  //S  = dat[2];
  //T_ = dat[3];
  //N = floor(dat[4]);
  //B = 0.0;

  va_list args;
  va_start(args, dat);
   
  ext_dat_ = (va_arg(args, ExtData const*));  //  &ext_dat_[N];
  NStep_   = (va_arg(args, int*          ));  //  CtSt_.NStep    

  
  // *** FIXME *** ext_dat_ and NStep_ are connected to external stuff in
  // a rather kludgy manner. This needs to be cleaned up.

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void GCavity::setParameters( int np, std::vector<double> const& dat, ... ) 
{
  double Ndum = 0.0;
  double* rdat[] = { &L_, &G, &S, &T_, &Ndum };

  for (int i=0; i<np; ++i) {
    *rdat[i] = dat[i];
  }
  if (Ndum !=0.0) N = floor(Ndum);
  
  //L_ = dat[0];
  //G  = dat[1];
  //S  = dat[2];
  //T_ = dat[3];
  //N = floor(dat[4]);
  //B = 0.0;

  va_list args;
  va_start(args, dat);
   
  ext_dat_ = (va_arg(args, ExtData const*));  //  &ext_dat_[N];
  NStep_   = (va_arg(args, int*          ));  //  CtSt_.NStep    

  
  // *** FIXME *** ext_dat_ and NStep_ are connected to external stuff in
  // a rather kludgy manner. This needs to be cleaned up.

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

INTEGRERR GCavity::get_energy(ODEparam *p, double Enr1, double *Enr2, int n)
{

  double*  xp = 0;
  double** yp = 0;
  double ystart[2];

  int nstep;

  double x1 = p->fieldtbl->x[0];
  double x2 = p->fieldtbl->x[p->fieldtbl->n-1];

  ystart[0] = 0.0;
  ystart[1] = sqrt(Enr1*(Enr1+2.0*p->ms));

  INTEGRERR err = odeintfs(x1, x2, 2,ystart, n, &nstep, 0, xp, yp, LongitEquations, p);

  *Enr2     =  sqrt(p->ms*p->ms +ystart[1]*ystart[1]) - p->ms;

  return err;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#define FUDGE 6.

INTEGRERR GCavity::get_cav_phase(double En1, double dEn, ODEparam *p, int m)
{
  INTEGRERR err;
  double fi0, En2, dfi, Ep, Em, a, deltafi;
  double e, x, fi;
  int i, n;

  //  Get estimate of the cavity phase
  
  double len   = p->fieldtbl->x[p->fieldtbl->n-1]-p->fieldtbl->x[0];
  double k     = 2.*PI/p->wavelen;
  double gamma = 1.0 + (En1+dEn/2)/p->ms;
  double beta  = sqrt(1.0 - 1.0/gamma/gamma);
  double ac    = 0.0;  
  double as    = 0.0;
 
  for ( int i=0; i <=p->fieldtbl->n; ++i) {
     x = p->fieldtbl->x[0] + len*i/p->fieldtbl->n;
     if(splint(&p->fieldtbl->x[0], &p->fieldtbl->y[0], &p->fieldtbl->v[0], p->fieldtbl->n, x, &e)) {
        return SPLINE_ERR;
     }
     fi  = k*len*i/p->fieldtbl->n/beta;
     ac += e*cos(fi);      
     as += e*sin(fi);
  }

  fi0 = atan2(as,ac);
  ac  = ac*len/p->fieldtbl->n;
  as  = as*len/p->fieldtbl->n;
 
  p->ampl = (dEn<En1) ? dEn/sqrt(ac*ac+as*as) : En1/sqrt(ac*ac+as*as);

  p->phase  = 0.0;
  dfi       = 0.3;
  p->phase0 = fi0;   // it will keep the phase for the on-crest acceleration
  n         = m/8;

  err = get_energy(p, En1, &En2, n);  if(err)return err;

  i         = 0;     // Integration at first stage of phase determination
  do{
      p->phase0 = fi0+dfi;
      err       = get_energy(p, En1, &Ep, n);  if(err)return err;

      p->phase0 = fi0-dfi;
      err       = get_energy(p, En1, &Em, n);  if(err)return err;

      a         = 2.*En2-Ep-Em;
      
      if(a>0.0) {
      	deltafi = 0.5*dfi*(Ep-Em)/a;
        if(fabs(deltafi)<dfi){
	   fi0 += deltafi; dfi /=FUDGE;
	}
        else  {
	  fi0 += 1.5*copysign(dfi,deltafi);
	}
      }
      else {
	fi0 += 0.5*copysign(dfi,Ep-Em);
      }
      p->phase0 = fi0;
      err       = get_energy(p, En1, &En2, n);  if (err!=0) return err;

      if(i++ > 50) return No_Conv_Phase;

   } while(fabs(deltafi)>.2);
  
   for(int j=1; j>=0; --j){

      n    =  m/(1.+7.*j); //Integration at first stage of phase determination
      err  =  get_energy(p, En1, &En2, n);  if(err)return err;

      do {
	p->ampl   = p->ampl*dEn/(En2-En1);
	err       = get_energy(p, En1, &En2, n);  if(err)return err;

	p->phase0 = fi0+dfi;
	err       = get_energy(p, En1, &Ep, n);  if(err)return err;

	p->phase0 = fi0-dfi;
        err       = get_energy(p, En1, &Em, n);  if(err)return err;

	a         = 2.0*En2-Ep-Em;

	if (a>0.0){
	  deltafi = 0.5*dfi*(Ep-Em)/a;
	  if(fabs(deltafi)<dfi) {
	     fi0 += deltafi; dfi /=FUDGE;
	  }
	  else  {
	    fi0 += 1.5*copysign(dfi,deltafi);
	  }
	}
	else {
	  fi0 += 0.5*copysign(dfi,Ep-Em);
	}
	p->phase0 = fi0;
	err       = get_energy(p, En1, &En2, n);  if(err)return err;

	if(i++ > 50) return No_Conv_Phase;
      } while(fabs(deltafi)>1.e-7);

      dfi = dfi*FUDGE*.95;
    }

   if(fabs((En2-En1)/dEn) >1.e-8){
      err = get_energy(p, En1, &En2, n);  
      if( err )return err;
      p->ampl=p->ampl*dEn/(En2-En1);
    }
    return err;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

INTEGRERR GCavity::get_matrix(ODEparam *p, double Enr1, double *Enr2, RMatrix& m, int n)
{
  INTEGRERR err;
  double  *xp  = 0;
  double  **yp = 0;
  double  ylong[2];

  int nstep, i;
  
  m.toZero();

  double x1    = p->fieldtbl->x[0];
  double x2    = p->fieldtbl->x[p->fieldtbl->n-1];
  double p_in  = sqrt(Enr1*(Enr1+2.*p->ms));
  double beta1 = p_in/sqrt(p->ms*p->ms+p_in*p_in);

  // First trajectory

  double ystart[]= {0.0,  p_in, 1.0, 0.0};
  
  if ( (err=odeintfs(x1, x2, 4,ystart, n, &nstep, 0, xp, yp, TransvEquations, p)) ) return err;

  m[0][0] = m[2][2] = ystart[2];
  m[1][0] = m[3][2] = ystart[3]/ystart[1];

  // Second trajectory
  ystart[0] = 0.;
  ystart[1] = p_in;
  ystart[2] = 0;
  ystart[3] = 0.001*p_in;

  if ( ((err=odeintfs(x1, x2, 4,ystart, n, &nstep, 0, xp, yp, TransvEquations, p))) ) return err;
 
  m[0][1] = m[2][3] = ystart[2]*1000.;
  m[1][1] = m[3][3] = ystart[3]/ystart[1]*1000.;

  *Enr2        = sqrt(p->ms*p->ms + ystart[1]*ystart[1]) - p->ms;
  double beta2 = ystart[1]/sqrt(p->ms*p->ms+ystart[1]*ystart[1]);

  // First trajectory for longitudinal plane
  double delta_t = 0.0002*p->wavelen/beta1;
  ylong[0]       = delta_t;
  ylong[1]       = p_in;

  if ( (err=odeintfs(x1, x2, 2, ylong, n, &nstep, 0, xp, yp, LongitEquations, p)) ) return err;
 
  m[4][4] =  beta2*(ylong[0]-ystart[0])/(beta1*delta_t);    
  m[5][4] = -(ylong[1]-ystart[1])/ystart[1]/(beta1*delta_t);

  // Second trajectory for longitudinal plane
  ylong[0] = 0.0;
  ylong[1] = p_in*1.001;

  if ( (err = odeintfs(x1, x2, 2, ylong, n, &nstep, 0, xp, yp, LongitEquations, p)) ) return err; 

  m[4][5] = -1000.*beta2*(ylong[0]-ystart[0]);
  m[5][5] =  1000.*(ylong[1]-ystart[1])/ystart[1];

  return err;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

INTEGRERR GCavity::LongitEquations(double x, double *y, double *dy, ODEparam *p)
{

  double e;
  
  if(splint( &p->fieldtbl->x[0], &p->fieldtbl->y[0], &p->fieldtbl->v[0], p->fieldtbl->n, x, &e)) {
    return SPLINE_ERR;
  }
 
  double beta  = y[1]/sqrt(p->ms*p->ms+y[1]*y[1]);
  dy[0] = 1.0/beta;
  dy[1] = p->ampl*e/beta*cos(2.*PI/p->wavelen*y[0]+p->phase-p->phase0);

  return NO_ERR;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//equations for linear transverse motion: y[0]=v*t, y[1]=p, y[2]=x, y[3]=p_tr/p

INTEGRERR GCavity::TransvEquations(double x, double *y, double *dy, ODEparam *p)
{

  double e, et, k, s, beta, cs, si;
   
  if(splint( &p->fieldtbl->x[0], &p->fieldtbl->y[0], &p->fieldtbl->v[0], p->fieldtbl->n, x, &e)) {
     return SPLINE_ERR;
  }
  if(dsplint( &p->fieldtbl->x[0], &p->fieldtbl->y[0], &p->fieldtbl->v[0], p->fieldtbl->n, x, &et)) {
     return SPLINE_ERR;
  }
  
  k     = 2.0*PI/p->wavelen;
  s     = k*y[0]+p->phase-p->phase0;
  cs    = cos(s);
  si    = sin(s);
  beta  = y[1]/sqrt(p->ms*p->ms+y[1]*y[1]);

  dy[0] = 1.0/beta;
  dy[1] = p->ampl*e/beta*cs;
  dy[2] = y[3]/y[1];
  dy[3] = 0.5*p->ampl*y[2]*(k*e*si-cs*et/beta);

  return NO_ERR;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


//equations for linear transverse motion: y[0]=v*t, y[1]=p, y[2]=x, y[3]=p_tr/p

INTEGRERR GCavity::dynamics(double x, double *y, double *dy, ODEparam *p)
{

  double e  = e_(x);     // interpolated electric field 
  double et = e_(x,1);   // de/dx ? 
  
  double k     = 2.*PI/wavelen_;
  double s     = k*y[0]+(phase_-phase0_);
  double cs    = cos(s);
  double si    = sin(s);
  double beta  = y[1]/sqrt(ms_*ms_+y[1]*y[1]);

  dy[0] = 1.0/beta;
  dy[1] = p->ampl*e/beta*cs;
  dy[2] = y[3]/y[1];
  dy[3] = 0.5*ampl_*y[2]*(k*e*si-cs*et/beta);

  dy[4] = 1.0/beta;
  dy[5] = ampl_*e/beta*cos(2.*PI/wavelen_*y[0]+(phase_-phase0_));


  return NO_ERR;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void GCavity::preTrack(double ms, double Enr0,  double tetaY, int n_elem, TrackParam& prm, RMatrix& m1 ) const
{
  
   prm.p0   = sqrt(2.*ms*Enr0+Enr0*Enr0);
   prm.Hr0  = prm.p0/C_DERV1;
   prm.vp0  = C_CGS*prm.p0/sqrt(prm.p0*prm.p0+ms*ms);
   
   prm.Efin = Enr0;
   m1       = Element::rmatrix( prm.Efin, ms, tetaY, 0.0, 3);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int GCavity::trackOnce( double ms,   double& Enr0, int n_elem, int n_turn, TrackParam& prm,
		      RMatrix const& m1, Coordinates& v ) const
{

  int status = 0; 
  if ( (status = backwardTest(prm, n_elem, n_turn, v)) ) return status;

  double EnrNew = prm.Efin;
  double tetaY  = 0.0;
  
  RMatrix tm = Element::rmatrix(EnrNew, ms, tetaY, 0.0, 3); // IS IT NECESSARY TO RECOMPUTE THE MATRIX HERE ????
  v.c = tm*v.c;

  double capa = sqrt(sqrt((2.* Enr0 * ms + Enr0 * Enr0)/(2.*EnrNew*ms+EnrNew*EnrNew)));

  v[0] *= capa;
  v[1] *= capa;
  v[2] *= capa;
  v[3] *= capa;
  v[5] *= capa*capa*(ms+EnrNew)/(ms+ Enr0);

  Enr0 = EnrNew;

 done:	

  if ( (status = transAmpTest(prm, n_elem, n_turn, v)) ) return status; 

  return 0;
}

