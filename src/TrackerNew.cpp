//  =================================================================
//
//  TrackerNew.cpp
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
#include <iomanip>
#include <vector>

#include <Constants.h>
#include <ChromaControlDialog.h>
#include <Tracker.h>
#include <TrackParam.h>
#include <Element.h>
#include <Structs.h>
#include <Twiss.h>
#include <OptimMainWindow.h>
#include <OptimPlot.h>
#include <OptimApp.h>
#include <OptimExceptions.h>
#include <OptimMessages.h>
#include <PolyRegression.h>
#include <complex>
#include <sstream>
#include <QwtPlotTextLabel>
#include <QwtPlotCurve>

using Constants::C_DERV1;
using Constants::PI;


int Tracker::trackParticleExact( Element const& element, double ms, double Enr0,  double tetaY, Coordinates& v, int n_turn, int n_elem)
{

  // n_turn    is the turn    number. it is used by the loss array. We set it to zero,   
  // n_element is the element number. it is used by the loss array. We set it to zero.  

  RMatrix m1;
  TrackParam prm;

  int    rt = 0;

  element.preTrack(ms, Enr0, tetaY, n_elem, prm, m1);  // this computes the transfer matrix m1 
  double enr = Enr0;
  element.trackOnce(ms, enr, n_elem, n_turn,  prm,  m1, v);

  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::chroma4D(double& chroma1, double& chroma2 )
{
  // compute first order chromaticities for mode 1 and mode 2

   int     nsteps = 1;
   double dppstep = 0.0001;
   double nu1v[3];
   double nu2v[3];

   using std::setw;
  
   bool CompAtExcitedOrb  = CtSt_.CompAtExcitedOrb;
   bool IsRingCh          = CtSt_.IsRingCh ;

   CtSt_.CompAtExcitedOrb = true;
   CtSt_.IsRingCh         = true;

   if(interrupted_){ interrupted_=false; return;} 
   int status = analyze(!analyzed_);
   if (status) {
     // restore state
     CtSt_.CompAtExcitedOrb = CompAtExcitedOrb;
     CtSt_.IsRingCh         = IsRingCh;
     return;
   }  
   auto restore = [](int* p){ QGuiApplication::restoreOverrideCursor(); delete p;};
   std::unique_ptr<int,decltype(restore)> cursor_guard(new int, restore); 

   QGuiApplication::setOverrideCursor(Qt::WaitCursor);
   
   int ir;
   char err[256];
   Twiss4D v;
   RMatrix tm;
   std::complex<double> lambda[4], ev[4][4];
   double dpp0 = 0.0;

   Coordinates v_anlz_tmp = v_anlz;
   bool Closure4D   = NstTool.Closure4D;
   
   // calculate transfer matrix and tunes

   try {
     findRMatrix(tm);
     if ( (ir = RMatrix_t<4,double>(tm).findEigenValues(lambda, ev)) !=0 ) {
       OptimMessageBox::warning(this, "4D View", "Cannot close lattice.", QMessageBox::Ok);
       throw OptimRuntimeException( "Cannot close lattice." );
     }
     else{
       v.betatronFromEV(ev);
       getDisp4D(tm, v, err);
     }

     double nu1 = -std::arg(lambda[0])/(2*PI); 
     double nu2 = -std::arg(lambda[2])/(2*PI); 

     while (nu1 < 0.0) ++nu1;
     while (nu2 < 0.0) ++nu2;

     double nu10 = nu1;
     double nu20 = nu2;
     dpp0 = v_anlz[5];

   // ....................................................................................

     
   Closure4D   = NstTool.Closure4D;
   for (int i=-nsteps; i<=nsteps; ++i) {
     CtSt_.CompAtExcitedOrb = true;

     if(getTrajParamFromFile(false, false, v_anlz)) return; // to get dpp
     v_anlz[5] = dpp0 + 0.0001*i; // dpp step size 

     NstTool.Closure4D = false;
     getTrajParamFromFile(false, true, v_anlz); // update the parameter block (Reprint==false, Update==true )
     NstTool.Closure4D =  Closure4D;

     if(analyze(true)) throw OptimRuntimeException( "Cannot parse lattice." ); // Reprint == false

     findRMatrix(tm);
     if ( (ir = RMatrix_t<4,double>(tm).findEigenValues(lambda, ev)) !=0 ) {
       OptimMessageBox::warning(this, "4D View", "Cannot close lattice.", QMessageBox::Ok);
       throw OptimRuntimeException( "Cannot close lattice." );
     }
     else{
       v.betatronFromEV(ev);
       getDisp4D(tm, v, err);
     }

     nu1 = -std::arg(lambda[0])/(2*PI);
     nu2 = -std::arg(lambda[2])/(2*PI);

     while (nu1 <0.0) ++nu1;
     while (nu2 <0.0) ++nu2;

     nu1v[i+nsteps] = nu1; // array index  0 <= i <= 2*nsteps   
     nu2v[i+nsteps] = nu2; 
    }
   } // try;
   
   // restore initial state

   catch(std::exception const& e) {
     CtSt_.CompAtExcitedOrb = CompAtExcitedOrb; // restore orginal setting
     CtSt_.IsRingCh         = IsRingCh;
     v_anlz =  v_anlz_tmp;
     NstTool.Closure4D = false;
     getTrajParamFromFile(false, true, v_anlz); // update the parameter block (Reprint==false, Update==true )
     NstTool.Closure4D = Closure4D;
     return;
   }
   // attempt to identify modes

   bool swapon = false;
   for ( int i=1; i<3; ++i) {  
     double d1  =  fabs(nu1v[i]-nu1v[i-1]); 
     double d2  =  fabs(nu2v[i]-nu2v[i-1]); 
     double d1s =  fabs(nu2v[i]-nu1v[i-1]); 
     double d2s =  fabs(nu1v[i]-nu2v[i-1]); 
     swapon = (( d1 > d1s) && (d2 >d2s));
     if (swapon) { std::swap(nu1v[i], nu2v[i]); } 
   }  

   // centered difference
   chroma1 = (nu1v[nsteps+1]-nu1v[nsteps-1])/(2*dppstep);
   chroma2 = (nu2v[nsteps+1]-nu2v[nsteps-1])/(2*dppstep);

   // chromap1 = (nu1[nsteps+1]-2*nu1[nsteps]+nu1[nsteps-1])/(dppstep*dppstep);
   // chromap2 = (nu2[nsteps+1]-2*nu2[nsteps]+nu2[nsteps-1])/(dppstep*dppstep);
   
   CtSt_.CompAtExcitedOrb = CompAtExcitedOrb; // restore orginal setting
   CtSt_.IsRingCh         = IsRingCh;
   v_anlz =  v_anlz_tmp;
   NstTool.Closure4D = false;
   getTrajParamFromFile(false, true, v_anlz); // update the parameter block (Reprint==false, Update==true )
   NstTool.Closure4D = Closure4D;
   return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdChroma4D()
{

  using std::setw;
  
  // generate tables of tunes and tune shift vs dpp 

   static ChromaControlDialog* dialog = 0;

   if (!dialog) {
     dialog = new  ChromaControlDialog(this);
     dialog->data_.nsamples        = 10;
     dialog->data_.stepsize        = 0.0001;
     dialog->data_.isring =  CtSt_.IsRingCh;
     dialog->set();
   }

   dialog->setRingMode(CtSt_.IsRingCh);

   if (QDialog::Rejected == dialog->exec()) return;
   dialog->set();

   int     nsteps = dialog->data_.nsamples;
   double dppstep = dialog->data_.stepsize;

   std::vector<double> dppv;
   std::vector<double> nu1v;
   std::vector<double> nu2v;
   
   using std::setw;
  
   bool CompAtExcitedOrb  = CtSt_.CompAtExcitedOrb;
   bool IsRingCh          = CtSt_.IsRingCh ;

   CtSt_.CompAtExcitedOrb = true;
   CtSt_.IsRingCh         = dialog->data_.isring;

   if(interrupted_){ interrupted_=false; return;} 
   if(analyze(!analyzed_)) return;

   int ir;
   char err[256];
   Twiss4D v;
   RMatrix tm;
   std::complex<double> lambda[4], ev[4][4];
   double dpp0 = 0.0;

   Coordinates v_anlz_tmp = v_anlz;
   bool Closure4D   = NstTool.Closure4D;
   
   // calculate transfer matrix and tunes

   try {
     findRMatrix(tm);
     if ( (ir = RMatrix_t<4,double>(tm).findEigenValues( lambda, ev)) !=0 ) {
       OptimMessageBox::warning(this, "4D View", "Cannot close lattice.", QMessageBox::Ok);
       throw OptimRuntimeException( "Cannot close lattice." );
     }
     else{
       v.betatronFromEV(ev);
       getDisp4D(tm, v, err);
     }

     double nu1 = -std::arg(lambda[0])/(2*PI); 
     double nu2 = -std::arg(lambda[2])/(2*PI); 

     while (nu1 < 0.0) ++nu1;
     while (nu2 < 0.0) ++nu2;

     double nu10 = nu1;
     double nu20 = nu2;
     dpp0 = v_anlz[5];

     //std::cout << "#  dpp  nu1  nu2  nu1-nu10 nu2-nu20" << std::endl; 

     // ....................................................................................

     QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

     bool CompAtExcitedOrbSaved = CtSt_.CompAtExcitedOrb;
     
     for (int i=-nsteps; i<=nsteps; ++i) {
       CtSt_.CompAtExcitedOrb = true;

       if(getTrajParamFromFile(false, false, v_anlz)) return; // to get dpp
       v_anlz[5] = dpp0 + dppstep*i; // dpp step size 
       dppv.push_back(v_anlz[5]);
     
       Closure4D   = NstTool.Closure4D;
       NstTool.Closure4D = false;
       getTrajParamFromFile(false, true, v_anlz); // update the parameter block (Reprint==false, Update==true )
       NstTool.Closure4D =  Closure4D;

       if(analyze(true)) {//  argument Reprint == true
         QGuiApplication::restoreOverrideCursor();
          throw OptimRuntimeException( "Parser error." );
       }
     
       findRMatrix(tm);
       if ( (ir = RMatrix_t<4,double>(tm).findEigenValues(lambda, ev)) !=0 ) {
         OptimMessageBox::warning(this, "4D View", "Cannot close lattice.", QMessageBox::Ok);
         throw OptimRuntimeException( "Cannot close lattice." );
       }
       else{
         v.betatronFromEV(ev);
         //getDisp4D(tm, v, err); // NOT NEEDED ?? 
       }

       nu1 = -std::arg(lambda[0])/(2*PI);
       nu2 = -std::arg(lambda[2])/(2*PI);

       while (nu1 <0.0) ++nu1;
       while (nu2 <0.0) ++nu2;
  
       nu1v.push_back(nu1);
       nu2v.push_back(nu2);

       }
     } // try;

     catch(std::exception const& e) {
       // restore initial state
       CtSt_.CompAtExcitedOrb = CompAtExcitedOrb; // restore orginal setting
       CtSt_.IsRingCh         = IsRingCh;
       v_anlz =  v_anlz_tmp;
       NstTool.Closure4D = false;
       getTrajParamFromFile(false, true, v_anlz); // update the parameter block (Reprint==false, Update==true )
       NstTool.Closure4D = Closure4D;
       QGuiApplication::restoreOverrideCursor();
       return;
    }

   // attempt to identify modes

   bool swapon = false;
   for ( int i=1; i< nu1v.size(); ++i) {  
     double d1  =  fabs(nu1v[i]-nu1v[i-1]); 
     double d2  =  fabs(nu2v[i]-nu2v[i-1]); 
     double d1s =  fabs(nu2v[i]-nu1v[i-1]); 
     double d2s =  fabs(nu1v[i]-nu2v[i-1]); 
     swapon = (( d1 > d1s) && (d2 >d2s));
     if (swapon) std::swap(nu1v[i], nu2v[i]); 
   }  
   
  PolynomialRegression<double> regression;
  std::vector<double> coeffs1(4); // 3rd order 
  regression.fitIt( dppv, nu1v, 3, coeffs1);
  std::stringstream ss;
  ss << "Third Order Polynomial Regression\n\n"
     << "Mode 1: " << " a0 = " << setw(12) << coeffs1[0] 
     << " a1 = " << setw(12) << coeffs1[1] 
     << " a2 = " << setw(12) << coeffs1[2] 
     << " a3 = " << setw(12) << coeffs1[3] << '\n';
  
  std::vector<double> coeffs2(4); // 3rd order 
  regression.fitIt( dppv, nu2v, 3, coeffs2);
  ss << "Mode 2: " << " a0 = " << setw(12) << coeffs2[0] 
                   << " a1 = " << setw(12) << coeffs2[1] 
                   << " a2 = " << setw(12) << coeffs2[2] 
	           << " a3 = " << setw(12) << coeffs2[3] << std::endl;
   
  PlotSpec plotspecs;
  plotspecs.title        = "Fractional Tunes vs dp/p";
  plotspecs.bottom_title = "dp/p";
  auto& curvespecs = plotspecs.curvespecs;

  auto fit = [](std::vector<double> const& a, double dpp) { return a[0] + a[1]*dpp + a[2]*dpp*dpp + a[3]*(dpp*dpp*dpp);};
  
  std::vector<double> nu1pv(nu1v.size());
  std::vector<double> nu2pv(nu2v.size());

  std::transform( dppv.begin(),  dppv.end(), nu1pv.begin(), [&fit, &coeffs1](double x) { return fit(coeffs1,x); }); 
  std::transform( dppv.begin(),  dppv.end(), nu2pv.begin(), [&fit, &coeffs2](double x) { return fit(coeffs2,x); }); 
  
  OptimPlot* plot = 0;
  //if ( (plot = findChild<OptimPlot*>("ChromaticityPlot")) ) delete plot->parentWidget();

  std::vector<LegoData> empty;
  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  
  curvespecs.push_back({ "nu1",    &dppv[0], &nu1v[0],  (int) dppv.size(), QwtSymbol::Ellipse,  QwtPlot::yLeft,  "Tune", "" });  
  curvespecs.push_back({ "nu1p",   &dppv[0], &nu1pv[0], (int) dppv.size(), QwtSymbol::NoSymbol, QwtPlot::yLeft,  "Tune", "" });  
  curvespecs.push_back({ "nu2",    &dppv[0], &nu2v[0],  (int) dppv.size(), QwtSymbol::Ellipse,  QwtPlot::yLeft,  "Tune", "" });  
  curvespecs.push_back({ "nu2p",   &dppv[0], &nu2pv[0], (int) dppv.size(), QwtSymbol::NoSymbol, QwtPlot::yLeft,  "Tune", "" });  

  plot->setObjectName("ChromaticityPlot");

  auto sw = getAttachedSubWin(WindowId::Chroma); 
  if (!sw) { 
    plot = addPlot(WindowId::Chroma, plotspecs,  empty);
  }
  else {
    plot = qobject_cast<OptimPlot*>( sw->widget() );
  }

    
  //plot->configure(plotspecs, dbspecs); 
  plot->setObjectName("ChromaticityPlot");
  
  QwtPlotTextLabel* lbl = new  QwtPlotTextLabel();
  QwtText txt(ss.str().c_str());
  txt.setRenderFlags( Qt::AlignHCenter | Qt::AlignTop ); 
  QFont font;
  font.setBold( true );
  txt.setFont( font );
  lbl->setText( txt); 
  lbl->attach(plot);

  QwtPlotItemList lst  = plot->itemList( QwtPlotItem::Rtti_PlotCurve);		

  QPen pen1,pen2;
  
  for ( auto it: lst ) {
    QwtPlotCurve* c = dynamic_cast<QwtPlotCurve*>(it);
    if (!c) continue;
    if ( c->title().text() == "nu1") { c->setStyle(QwtPlotCurve::NoCurve); pen1 = c->pen();}
    if ( c->title().text() == "nu2") { c->setStyle(QwtPlotCurve::NoCurve); pen2 = c->pen();}
  }

  for ( auto it: lst ) {
    QwtPlotCurve* c = dynamic_cast<QwtPlotCurve*>(it);
    if (!c) continue;
    if ( c->title().text() == "nu1p") { c->setPen(pen1);}
    if ( c->title().text() == "nu2p") { c->setPen(pen2);}

  }
  // normal return - restore initial state
  CtSt_.CompAtExcitedOrb = CompAtExcitedOrb; // restore orginal setting
  CtSt_.IsRingCh         = IsRingCh;
  v_anlz =  v_anlz_tmp;

  NstTool.Closure4D = false;
  getTrajParamFromFile(false, true, v_anlz); // update the parameter block (Reprint==false, Update==true )
  NstTool.Closure4D = Closure4D;

  plot->replot(); 
  plot->show();
  QGuiApplication::restoreOverrideCursor();

  return;
}

  //-----------------------------------------------
  /*
  auto sw = getAttachedSubWin(WindowId::Chroma); 
  if (!sw) { 
    sw = createAttachedSubWin( (plot = new OptimPlot(this, (CtSt_.PlotBoxes ? legodata : empty) )), id);
    plot->configure(plotspecs, dbspecs); 
  }
  else {
    plot = qobject_cast<OptimPlot*>( sw->widget() );
  }

  if (CtSt_.ClearPlot ) { // eliminate all but one of the curves (QwtPlotCurve)
    plot->removeDuplicateCurves();
  }; 
    
  plot->configureCurves(plotspecs,CtSt_.ClearPlot, dbspecs);  // refresh the curves only   

  plot->setLegoData(CtSt_.PlotBoxes ? legodata : empty);  // legodata needs to be reset because it may have changed.
                                                          // speed could be improved by refreshing only when hor axis
                                                          // is changed. The user may also simply hide the lego plot.  

  // enable only the plots that were previously enabled. 

  sw->raise(); 
  mdiArea->setActiveSubWindow(previous);

  // initialize the new plot using an existing (detached) one when the latter exists.  
  
  auto dw = getLatestDetached(id);  
  OptimPlot*  dpw = dw ? qobject_cast<OptimPlot*>(dw->widget()) : 0;
  if ( dpw ) {
    plot->usePlotState(dpw);
  }  

  plot->replot(); 
  plot->show();
  //addPlot(WindowId::Chroma, plotspecs, legodata);
}
  */

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdCompaction4D()
{

  std::cout << "void OptimMainWindow::cmdCompaction4D()" << std::endl;
  // double ms  
  double tetaY  = tetaYo0_;
  double Enr    = Ein;
  double gamma  = 1.+Enr/ms;
  double dPdE   = (Enr+ms)/(Enr*Enr+2.*Enr*ms);
  double Hrt    = sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;

  double dSdP;
  double dPdS;
  double capa;
  double dtx;
  double dty;
  RMatrix me;

  //Coordinates vin = { 0.2710, 0.0,  0.0,0.0, 0.0, 0.001};
  Coordinates vin = { 0.0, 0.0,  0.0,0.0, 0.0, 0.001};
  Coordinates v   = vin;

  auto vnorm = []( Coordinates& v,  Coordinates& vin ) {
    return sqrt( (v[0]-vin[0])*(v[0]-vin[0]) + (v[1]-vin[1])*(v[1]-vin[1]) +
                 (v[2]-vin[2])*(v[2]-vin[2]) + (v[3]-vin[3])*(v[3]-vin[3]) );
  };
  
  std::cout << "beamline_.size() = " << beamline_.size() << std::endl;

 again:
   
#if 1
  for( auto& ep : beamline_ ) {

    char nm     = ep->etype();

    dSdP   = ep->length()/(gamma*gamma);
    double EnrNew = Enr;
       
    #if 0
    switch(nm){
      case 'A': 
      case 'W':
          dPdS = dPdE*2.*PI*ep->G/ep->T*sin(PI/180.*ep->S);
      case 'E': 
      case 'X':
        me = ep->rmatrix( EnrNew, ms, tetaY, 0.0, 3);
        capa = sqrt(sqrt((2.*Enr*ms+Enr*Enr)/(2.*EnrNew*ms+EnrNew*EnrNew)));
        break;
      case 'K':
         dtx = ep->L*ep->B/Hrt*cos(PI*ep->T/180.);
         dty = ep->L*ep->B/Hrt*sin(PI*ep->T/180.);
        default:
         me = ep->rmatrix(Enr, ms, tetaY, 0.0, 3);
       break;
     }
       
    //ep->propagateLatticeFunctions(me, twiss, ev);
    #endif

    //std::cout << "tracking through : " << ep->name << std::endl;

    Tracker::trackParticleExact(*ep, ms, Enr, tetaY, v, 0, 0);
       
     #if 0
    switch(nm){
         case 'E': 
         case 'X': 
         case 'A': 
         case 'W':
           Enr   = EnrNew;
   	   gamma = 1.0+Enr/ms;
   	   dPdE  = (Enr+ms)/(Enr*Enr+2.*Enr*ms);
           Hrt   = sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;
         default:
           break;
       }
    #endif
    
   } // for ep 


  std::cout << "vnorm(v,vin)  =" << vnorm(v,vin) << std::endl;

  std::cout <<  "vin[0] = " << vin[0] << std::endl; 
  std::cout <<  "vin[1] = " << vin[1] << std::endl; 
  std::cout <<  "vin[2] = " << vin[2] << std::endl; 
  std::cout <<  "vin[3] = " << vin[3] << std::endl; 
  std::cout <<  "vin[4] = " << vin[4] << std::endl; 
  std::cout <<  "vin[5] = " << vin[5] << std::endl; 

  std::cout <<  "v[0] = " << v[0] << std::endl; 
  std::cout <<  "v[1] = " << v[1] << std::endl; 
  std::cout <<  "v[2] = " << v[2] << std::endl; 
  std::cout <<  "v[3] = " << v[3] << std::endl; 
  std::cout <<  "v[4] = " << v[4] << std::endl; 
  std::cout <<  "v[5] = " << v[5] << std::endl; 

  static int iter  = 0;
  if (iter++ >10000) return; 
  vin = vin + 0.005*(v-vin);
  goto again;
#endif

  v = { 0.0, 0.0,  0.0,0.0, 0.0,0.001};
  closeTraject(v);
  
  std::cout <<  "v[0] = " << v[0] << std::endl; 
  std::cout <<  "v[1] = " << v[1] << std::endl; 
  std::cout <<  "v[2] = " << v[2] << std::endl; 
  std::cout <<  "v[3] = " << v[3] << std::endl; 
  std::cout <<  "v[4] = " << v[4] << std::endl; 
  std::cout <<  "v[5] = " << v[5] << std::endl; 
  
#if 0
  if (vnorm(v,vin) > 1.0e-8 ) {
    vin = vin + 0.05*(v-vin);
    goto again;
  };
#endif
  
  

}


