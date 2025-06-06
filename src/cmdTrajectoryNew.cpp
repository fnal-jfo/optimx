//  =================================================================
//
//  cmdTrajectoryNew.cpp
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

//-------------------------------------------------------------------
// OptiM   (c) FRA/Fermilab 
// Please refer to OptiM.Copyright.txt for copyright and license 
// information. All other rights reserved.
//
// cmdTrajectoryNew.cpp
// ------------------------------------------------------------------

#include <platform.h>
#include <Constants.h>
#include <GeneralPreferencesDialog.h>
#include <Element.h>
#include <Globals.h>
#include <RMatrix.h>
#include <Twiss.h>
#include <TrackParam.h>
#include <OptimApp.h>
#include <OptimEditor.h>
#include <OptimMainWindow.h>
#include <OptimMessages.h>
#include <OptimPlot.h>
#include <OptimTextEditor.h>

#include <iostream>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <fmt/format.h>

using Constants::PI;
using Constants::C_DERV1; 
using Constants::C_DERV3;
using Constants::C_CGS;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdTrajectoryNew ()
{
  Twiss vt;
  Coordinates v;
  
  if(interrupted_ ) { interrupted_ = false; return;}
  if( analyze(!analyzed_) ) return;

  RMatrix tm;
  double alfa;
  char buf[256];
  char const* cher[3]={"X","Y","X&Y"};
  
  if (getTrajParamFromFile(false, false, v)) return; 

  if( CtSt_.IsRingCh ){
    //.............................................................................................
    // find_tunes attempts to find the uncoupled tunes. irt !=0 if the lattice is unstable. 
    // why is this test necessary for a trajectory in ring mode ?
    // the only relevant issue here is whether or not the trajectory closes. !!FIXME !!!
    // A: apparently, the lattice functions are used later for normalizing the trajectory in units
    // of sigma (beam size). Aside from the fact that the sizes calculation should be performed
    // in 4D or even 6D, it does not really belong here.   
    // ............................................................................................

    findRMatrix(tm);
    int irt=find_tunes(tm, Length_, vt, &alfa); 
    if(irt){
      strcpy(buf,"Cannot close for ");
      strcat(buf, cher[irt-1]);
      OptimMessageBox::warning(this, "Close Error", buf, QMessageBox::Ok);
      return;
    }

    // ALWAYS do a linear closure (4D) 
    int ierr = closeTraject(v); // v[4] and v[5] remain unchanged 
    // if linear closure fails, just return.   
    if (ierr != 0) return; 

    // If we get here, attempt full closure; linear solution is used as initial guess for 4D case 
    if( NstTool.FullClosure) {

      if( !NstTool.Closure4D )  getTrajParamFromFile(false, false, v); // 6D: get initial guess for all coordinates from lattice file
     
	if ( closeTrajectFull(v)) {
         OptimMessageBox::warning(this, "Close Error", 
      				   "Cannot close trajectory. Make sure that Twiss parameters can be closed. If OK reduce the convergence coefficient",
			     QMessageBox::Ok);
      }
    }
    // NOTE: trajectory block is *NOT* updated; 

  } // Not a ring 
  else {
    setInitialBetas(vt);
  }

  TrajectoryNew (v, vt);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::TrajectoryNew(Coordinates& v, Twiss& vt)
{
  enum PlotType { NormalPlot, NormBySigma, TogetherWithEnvel };
  
  PlotType plot_type;
 
  if(NstTool.NormalPlot)        plot_type = NormalPlot;
  if(NstTool.NormBySigma)       plot_type = NormBySigma;
  if(NstTool.TogetherWithEnvel) plot_type = TogetherWithEnvel;

  int N = CtSt_.ArrayLen;
  std::vector<double> x(N+1);
  
  //y[0] = X
  //y[1] = Y
  //y[2] = sigma_x or total amplitude (betatron + dispersion)
  //y[3] = sigma_Y or total amplitude (betatron + dispersion )
  //y[4] = s: long distance from the reference particle  in cm 
  //y[5] = t: long distance from the reference particle  in ns 
  
  std::vector<std::vector<double> > y(6);
  for (int i=0; i<6;   ++i) {y[i].resize(N+ 1);};
  for (int k=0; k<N+1; ++k) {y[4][k] = y[5][k] = 0.0; };

  
  std::vector<double>               scraperS;
  std::vector<std::vector<double> > scraperPos(2);

  std::vector<LegoData> legodata;

  RMatrix tm, me;
  TrackParam prm;
  double dalfa;
  double EnrNew;
  double EnrNew0;

  int ns, brk = 0;

  x[0]=0.0;
  double ax = 0.0;
  double ay = 0.0;

  // set initial values at beamline input 

  if(NstTool.NormalPlot){
    y[0][0] = v[0];
    y[1][0] = v[2];
  }

  if(NstTool.NormBySigma){
    if(ex_<=0.0 || ey_<=0.0 ) {
      OptimMessageBox::warning(this,"Tool|Trajectory", "Cannot proceed because one of the emittances is equal to zero", QMessageBox::Ok);
      return;
    }
    if( CtSt_.PlotTotalSize) {
      ax = sqrt(ex_ * vt.BtX + vt.DsX * vt.DsX * dpp_ * dpp_);
      ay = sqrt(ey_ * vt.BtY + vt.DsY * vt.DsY * dpp_ * dpp_);
    }
    else {
      ax = sqrt(ex_ * vt.BtX);
      ay = sqrt(ey_ * vt.BtY);
    }
    
    y[0][0] = v[0]/ax;
    y[1][0] = v[2]/ay;
    y[2][0] = sqrt(y[0][0]*y[0][0] + y[1][0]*y[1][0]);
   }
   
  if(NstTool.TogetherWithEnvel) {

     y[0][0] = v[0];
     y[1][0] = v[2];

     if(CtSt_.PlotTotalSize){
       y[2][0] = sqrt(ex_ * vt.BtX + vt.DsX * vt.DsX * dpp_ * dpp_);
       y[3][0] = sqrt(ey_ * vt.BtY + vt.DsY * vt.DsY * dpp_ * dpp_);
     }
     else {
       y[2][0] = sqrt(ex_ * vt.BtX);
       y[3][0] = sqrt(ey_ * vt.BtY);
   }
 }

  
 double h         = Length_/N;
 tm.toUnity();
 double tetaY     = tetaYo0_;
 double Enr       = Ein;
 double Hrt       = Hr;
 int    nscrapers = 0;

 Coordinates vexact = v;

 double L  = 0.0;
 double Lp = 0.0;
 int k = 1;
 int i;   

 for( i=0; i<nelm_; ++i) {

     if(brk)break;
     auto ep       = beamline_[i];
     ep->preTrack( ms, Enr,  tetaY, i, prm, me); // does not change Enr
     EnrNew0 = Enr;
     ep->trackOnce(ms, EnrNew0, i, 0, prm, me, vexact); // changes EnrNew0
     char nm = ep->etype();
     ns = fabs(ep->length()/h)+1;
     if( nm == 'H') nscrapers++;
     if( nm == 'A' || nm == 'W' || nm == 'X' || nm=='Z' ) { ns=1; }

     auto e = std::shared_ptr<Element>( ep->split(ns) ); 

     // add element to lego plot 

     legodata.push_back( { L*0.01, ep->length()*0.01, ((e->G >=0.0) ? 1 :-1), ep->fullName()});
     L     +=  ep->length();

     // compute particle coordinates
     dalfa=0.;
     for( int j=0; j<ns; ++j) {
       switch( ep->etype() ) {
	 case 'E': 
         case 'X':  
         case 'W': 
         case 'A':
           {
	    EnrNew = Enr; 
            me = e->rmatrix(EnrNew, ms, tetaY, 0.0, e->checkEdge(j,ns));
	    v.c = me*v.c;
	    double capa = sqrt(sqrt((2.*Enr*ms+Enr*Enr)/(2.*EnrNew*ms+EnrNew*EnrNew)));
	    v[0] *= capa;	
            v[1] *= capa;	
            v[2] *= capa;	 
            v[3] *= capa;
            v[5] *= capa*capa*(ms+EnrNew)/(ms+Enr);
	    Enr = EnrNew;
	    Hrt = sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;
	   }
	    break;
	 case 'Q':
           me = e->rmatrix(Enr, ms, tetaY, 0.0, e->checkEdge(j,ns));
           v[0] -= ep->offsX();
           v[2] -= ep->offsY();
	   v.c = me*v.c;
           v[0] += ep->offsX();
           v[2] += ep->offsY();
	   break;
	 case 'B': 
         case 'D':
           me = e->rmatrix(dalfa, Enr, ms, tetaY, dalfa, e->checkEdge(j,ns)); // WHY is dalfa the 1st arg ???? 
           dalfa -= e->tilt();
	   v.c = me*v.c;
	   // apply additional kick due to dipole tilt error 
           if (ep->tiltErr() != 0.0) {
             double c = -cos(ep->tilt()/180.*PI) * (PI*ep->tiltErr()/180.0) * e->length()*e->B/Hrt;  // -cos(phi) * err * B 
	     double s =  sin(ep->tilt()/180.*PI) * (PI*ep->tiltErr()/180.0) * e->length()*e->B/Hrt;  //  sin(phi) * err * B 
             v[0] += 0.5*e->length()*s;
             v[1] += s;
             v[2] += 0.5*e->length()*c;
             v[3] += c;
           }
	   break;
	 case 'S':
	   Sextupole::sext_trans(e.get(), Hrt/(1.0+v[5]), &v, &v);
           me = e->rmatrix(Enr, ms, tetaY, 0.0, e->checkEdge(j,ns));
	   break;
         case 'M':
	   Multipole::multipole_trans(e.get(), Hrt, &v, &v);
           me = e->rmatrix(Enr, ms, tetaY, 0.0, e->checkEdge(j,ns));
	 case 'K':
	   {  double dtx = e->length()*e->B/Hrt*cos(PI*e->tilt()/180.);
	      double dty = e->length()*e->B/Hrt*sin(PI*e->tilt()/180.);
	      v[0] += e->length() * ( v[1] + 0.5 *dtx);
	      v[1] += dtx;
	      v[2] += e->length() * ( v[3] + 0.5 *dty);
	      v[3] += dty;
	   }
	   me = e->rmatrix(Enr, ms, tetaY, 0.0, e->checkEdge(j,ns));
	   break;
         case 'Z':
           v[0] += e->length() * v[1];
           v[2] += e->length() * v[3];
           v[5] += e->G*(Enr+ms)/(Enr*Enr+2.*Enr*ms);
           me = e->rmatrix(Enr, ms, tetaY, 0.0, e->checkEdge(j,ns));
           break;
	 default:
           me = e->rmatrix(Enr, ms, tetaY, 0.0, e->checkEdge(j,ns));
	   v.c = me*v.c; 
       }
       
      tm  = me * tm;
      Lp +=  e->length();

      if(Lp > Length_*k/(N-1) ) { // output plot data points only at resolution Length_/(N-1)  

	  x[k] = Lp *0.01; // x is in m  

	 if(NstTool.NormalPlot){
	   y[0][k] = v[0];
	   y[1][k] = v[2];
	   y[2][k] = 0;
	   y[3][k] = 0;
	   y[4][k] =  v[4]; // longitudinal position in cm
	   double bta = sqrt(1.0 - 1.0/((1.0+Enr/ms)*(1.0+Enr/ms)) );
           y[5][k] = -v[4]/(bta*C_CGS);                  // longitudinal position in s
           if (NstTool.longposns)    y[5][k] *=  1.0e9;  // longitudinal position in ns
           if (NstTool.longposrd)    y[5][k] *=  NstTool.rffrequency*1.0e6*(2*PI);            // longitudinal position in rad
	   if (NstTool.longposdeg)   y[5][k] *=  NstTool.rffrequency*1.0e6*(2*PI)*(180.0/PI); // longitudinal position in deg
	 }
	 
         else {
	   double capa  = Ein*(2.*ms+Ein)/(Enr*(2.*ms+Enr));
           double capaP = capa*(Enr+ms)/(Ein+ms);
           capa = sqrt(capa);
           double s1 = tm[0][1] / vt.BtX;
	   double sx = tm[0][0] - vt.AlX * s1;
           double s2 = tm[0][3] / vt.BtY;
	   double sy = tm[0][2] - vt.AlY * s2;
           ax = sqrt(capa*(ex_* vt.BtX *(sx*sx+s1*s1) + ey_* vt.BtY *(sy*sy+s2*s2)));
           s1 = tm[2][1] / vt.BtX;  sx = tm[2][0] - vt.AlX * s1;
           s2 = tm[2][3] / vt.BtY;  sy = tm[2][2] - vt.AlY * s2;
           ay=sqrt(capa*(ex_* vt.BtX *(sx*sx+s1*s1) + ey_* vt.BtY *(sy*sy+s2*s2)));
           if(CtSt_.PlotTotalSize){
	     s1 = capaP*fabs((tm[0][0]*vt.DsX + tm[0][1]*vt.DsXp +
	         tm[0][2]*vt.DsY + tm[0][3]*vt.DsYp + tm[0][5])*dpp_);
	     s2 = capaP*fabs((tm[2][0]*vt.DsX + tm[2][1]*vt.DsXp +
	    	 tm[2][2]*vt.DsY + tm[2][3]*vt.DsYp + tm[2][5])*dpp_);
             ax = sqrt(ax*ax+s1*s1);
             ay = sqrt(ay*ay+s2*s2);
           }
       }

       if(NstTool.NormBySigma){
	 y[0][k]=v[0]/ax;
	 y[1][k]=v[2]/ay;
	 y[2][k]=sqrt(y[0][k]*y[0][k] + y[1][k]*y[1][k]);
	 y[3][k]=0;
       }
       
       if(NstTool.TogetherWithEnvel){
	 y[0][k] = v[0];
	 y[1][k] = v[2];
	 y[2][k] = ax;
	 y[3][k] = ay;
       }
       
       ++k;
				  }
       if(fabs(v[0])>1000. || fabs(v[2])>1000. ||  v[5] <= -.9999999){
         brk=1;
	 OptimMessageBox::warning( this, "Tool|Trajectory", "Beam displacement is above threshold of 10 m or Energy became negative", QMessageBox::Ok);
         break;
       }

      if (k == N) break; 

     } // split element loop 
 
     v   = vexact; // at the end of an element, use the result from "exact" tracking (i.e. takes into account non-linearities). 
     Enr = EnrNew0; 

     if (k == N) break; 

 } // main element loop 

 legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 


   nscrapers = 0;
   for( int i=0; i<nelm_; ++i) { if( beamline_[i]->etype() =='H') ++nscrapers; }

   if(nscrapers != 0) {
     scraperS.resize(nscrapers);
     scraperPos[0].resize(nscrapers);
     scraperPos[1].resize(nscrapers);
   }

   nscrapers = 0;
   L         = 0.0; 

   for( int i=0; i<nelm_; ++i){
       auto ep = beamline_[i];
       char nm = toupper(ep->etype() );
       if( nm =='H'){
         scraperS[nscrapers] =  L*0.01; // cm to m
         scraperPos[0][nscrapers]                = ep->S;
         scraperPos[1][nscrapers]   = (ep->N==2) ? ep->S : ep->A;
         ++nscrapers;
       }
     L +=  ep->length();
  }
  

  PlotSpec plotspecs;
  plotspecs.title     = "Trajectory";
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  std::vector<double> y2p;
  std::vector<double> y2m;
  std::vector<double> y3p;
  std::vector<double> y3m;

  switch (plot_type) {

  case NormalPlot:
      //-----------------------------------------------------------------------------------------------------------------------------------
      //                     legendname xv       yv    n  axis             title                          bottom_title             color  
      //------------------------------------------------------------------------------------------------------------------------------------  

     if ( (nscrapers > 0)  &&  (CtSt_.PlotApertures) ) {
       curvespecs.push_back({ "H-apert",  &scraperS[0], &scraperPos[0][0],   nscrapers, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "", "cyan" });  
       curvespecs.push_back({ "V-apert",  &scraperS[0], &scraperPos[1][0],   nscrapers, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "", "magenta" });  
     }

      curvespecs.push_back({ "cX",    &x[0],   &y[0][0],  k, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "X / Y Positions [cm]", "" });
      curvespecs.push_back({ "cY",    &x[0],   &y[1][0],  k, QwtSymbol::NoSymbol,QwtPlot::yLeft,   "X / Y Positions [cm]", "" });  
      if ( NstTool.longposcm  ) curvespecs.push_back({ "cS",    &x[0],   &y[4][0],  k, QwtSymbol::NoSymbol, QwtPlot::yRight,  "S   Long. Position [cm]",  "" });  
      if ( NstTool.longposns  ) curvespecs.push_back({ "cT",    &x[0],   &y[5][0],  k, QwtSymbol::NoSymbol, QwtPlot::yRight,  "T   Long. Position [ns]",  "" });  
      if ( NstTool.longposdeg ) curvespecs.push_back({ "cPhi",  &x[0],   &y[5][0],  k, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Phi Long. Position [deg]", "" });  
      if ( NstTool.longposrd  ) curvespecs.push_back({ "cPhi",  &x[0],   &y[5][0],  k, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Phi Long. Position [rad]", "" });  
      break;

    case NormBySigma:
      curvespecs.push_back({ "cX",    &x[0],   &y[0][0],  k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "X / Y Positions [sigma]", "" });
      curvespecs.push_back({ "cY",    &x[0],   &y[1][0],  k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "X / Y Positions [sigma]", "" });  
      curvespecs.push_back({ "R",     &x[0],   &y[2][0],  k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "X / Y Positions [sigma]", "" });
      break;

    case TogetherWithEnvel:

     if ( (nscrapers > 0)  &&  (CtSt_.PlotApertures) ) {
       curvespecs.push_back({ "H-apert",  &scraperS[0], &scraperPos[0][0],   nscrapers,  QwtSymbol::NoSymbol, QwtPlot::yLeft,   "", "red" });  
       curvespecs.push_back({ "V-apert",  &scraperS[0], &scraperPos[1][0],   nscrapers,  QwtSymbol::NoSymbol, QwtPlot::yLeft,   "", "blue" });  

     }
     
     y2p = y[0] + y[2];
     y2m = y[0] - y[2];
     y3p = y[1] + y[3];
     y3m = y[1] - y[3];

     curvespecs.push_back({ "cX",     &x[0],   &y[0][0],  k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "X / Y Positions [cm]", "green" });
     curvespecs.push_back({ "cY",     &x[0],   &y[1][0],  k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "X / Y Positions [cm]", "black" });  
     curvespecs.push_back({ "aX+",    &x[0],   &y2p[0],   k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "X / Y Positions [cm]", "red"   });
     curvespecs.push_back({ "aX-",    &x[0],   &y2m[0],   k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "X / Y Positions [cm]", "red"   });
     curvespecs.push_back({ "aY+",    &x[0],   &y3p[0],   k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "X / Y Positions [cm]", "blue"  });  
     curvespecs.push_back({ "aY-",    &x[0],   &y3m[0],   k, QwtSymbol::NoSymbol, QwtPlot::yLeft,  "X / Y Positions [cm]", "blue"  });  
     
      break;
    default:
      break;
  };
  
  addPlot(WindowId::TracCh, plotspecs, legodata);

}

