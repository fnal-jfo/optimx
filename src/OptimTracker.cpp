//  =================================================================
//
//  OptimTracker.cpp
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
#include <omp.h>
#include <Constants.h>
#include <Globals.h>
#include <BeamMoments.h>
#include <Element.h>
#include <OptimCalc.h>
#include <OptimExceptions.h>
#include <OptimMainWindow.h>
#include <OptimMdiArea.h>
#include <OptimMdiSubWindow.h>
#include <OptimMessages.h>
#include <OptimPlot.h>
#include <OptimTextEditor.h>
#include <OptimTracker.h>
#include <OptimUserRtti.h>
#include <RMatrix.h>
#include <SymMatrix.h>
#include <Cholesky.h>
#include <ScatterData.h>
#include <ScatterPlotItem.h>
#include <Structs.h>
#include <Tracker3DSeriesData.h>
#include <TrackerPlot.h>
#include <TrackerSaveDistributionDialog.h>
#include <TrackingParametersDialog.h>
#include <TrackParam.h>
#include <Twiss.h>
#include <Utility.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QFileDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMenuBar>
#include <QProgressDialog>
#include <QSignalMapper>
#include <QTextEdit>

#include <fmt/format.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

using Utility::gauss;
using Utility::decodeExtLine;
using Utility::strcmpr;
using Utility::filterName;
using Utility::Filter7;

using Constants::PI;
using Constants::C_DERV1;

#define LSTR 1024
 
// .................................................................
// loss_  array :  0 (no loss); 1(ampl. > 10 m); 2(lost on aperture) 
// .................................................................

// In Qt app, Ctrl-W is predefined. The default behavior is to close QMdiSubwindows.  


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


static char const* colors[] = { "black", "red", "blue", "darkGreen", "magenta", "darkMagenta", "darkCyan", "darkBlue"} ; 

OptimTracker::OptimTracker (QWidget* parent, Qt::WindowFlags flags) // V7
  :  QMainWindow(parent,flags), inputscatter_(0), outputscatter_(0)
{

  int status = 0;

  setWindowFlags(Qt::WindowStaysOnTopHint);
  mainw_  = qobject_cast<OptimMainWindow*>( parent );
  setWindowTitle("OptiM Tracker");

  
    parametersAct_= new QAction( "Parameters",            this);
  distributionAct_= new QAction( "Distribution",          this);
          readAct_= new QAction( "Read Input Distribution",     this);
          saveAct_= new QAction( "Save Output Distribution",    this);
           xxpAct_= new QAction( "x-x'"    ,              this);
           yypAct_= new QAction( "y-y'",                  this);
            xyAct_= new QAction( "x-y",                   this);
          sdppAct_= new QAction( "s-dp/p",                this);
         inputAct_= new QAction( "Input",                 this);
        outputAct_= new QAction( "Output",                this);
    emittancesAct_= new QAction( "Emittances",            this);
     intensityAct_= new QAction( "Intensity",             this);
         trackAct_= new QAction( "Track !",               this);

  	poincareAct_= new QAction( "Poincare Plot",         this);
  

    distributionAct_->setEnabled(false);
            readAct_->setEnabled(false);
            saveAct_->setEnabled(false);
             xxpAct_->setEnabled(false);
             yypAct_->setEnabled(false);
              xyAct_->setEnabled(false);
            sdppAct_->setEnabled(false);
	   inputAct_->setEnabled(false);
	  outputAct_->setEnabled(false);
      emittancesAct_->setEnabled(false);
       intensityAct_->setEnabled(false);
           trackAct_->setEnabled(false);
        poincareAct_->setEnabled(false);

             xxpAct_->setCheckable(true);
             yypAct_->setCheckable(true);
              xyAct_->setCheckable(true);
            sdppAct_->setCheckable(true);
	   inputAct_->setCheckable(true);
	  outputAct_->setCheckable(true);


     QActionGroup* views = new  QActionGroup(this);
     views->addAction(xyAct_);
     views->addAction(xxpAct_);
     views->addAction(yypAct_);
     views->addAction(sdppAct_);
     views->setExclusive(true);

     QActionGroup* input_output = new  QActionGroup(this);
     input_output->addAction(inputAct_);
     input_output->addAction(outputAct_);
     input_output->setExclusive(true);
    
  parametersMenu     =  menuBar()->addMenu(tr("Setup"));
  phaseSpaceViewMenu =  menuBar()->addMenu(tr("Views"));
  fileMenu           =  menuBar()->addMenu(tr("File"));
  plotsMenu          =  menuBar()->addMenu(tr("Plots"));

  fileMenu->addAction(readAct_);
  fileMenu->addAction(saveAct_);

  phaseSpaceViewMenu->addAction( inputAct_);
  phaseSpaceViewMenu->addAction( outputAct_);
  phaseSpaceViewMenu->addSeparator();
  phaseSpaceViewMenu->addAction( xxpAct_);
  phaseSpaceViewMenu->addAction( yypAct_);
  phaseSpaceViewMenu->addAction( xyAct_ );
  phaseSpaceViewMenu->addAction( sdppAct_ );

  plotsMenu->addAction(emittancesAct_);
  plotsMenu->addAction(intensityAct_);

  parametersMenu->addAction(parametersAct_);
  parametersMenu->addAction(distributionAct_);
  menuBar()->addAction( trackAct_ );
  menuBar()->addAction( poincareAct_ );

  connect(trackAct_,         SIGNAL(triggered()), this, SLOT(cmdTracking()) );
  connect(distributionAct_,  SIGNAL(triggered()), this, SLOT(cmdInitialize()) );
  connect(parametersAct_,    SIGNAL(triggered()), this, SLOT(cmdParameters()) );
  connect(saveAct_,          SIGNAL(triggered()), this, SLOT(cmdSave()) );
  connect(readAct_,          SIGNAL(triggered()), this, SLOT(cmdRead()) );
  connect(inputAct_,         SIGNAL(triggered()), this, SLOT(cmdInput())  ); 
  connect(outputAct_,        SIGNAL(triggered()), this, SLOT(cmdOutput()) ); 
  connect(emittancesAct_,    SIGNAL(triggered()), this, SLOT(cmdPlotEmittance()) ); 
  connect(intensityAct_,     SIGNAL(triggered()), this, SLOT(cmdPlotIntensity()) ); 
  connect(poincareAct_,      SIGNAL(triggered()), this, SLOT(cmdPoincare()) );
 
  connect( mainw_,  SIGNAL(trackerPlotPositions()),    this,  SLOT(cmdTrackerPlotPositions())    ); 
  connect( mainw_,  SIGNAL(trackerSavePositions()),    this,  SLOT(cmdTrackSavePos())   ); 
  connect( mainw_,  SIGNAL(trackerPlotDistribution()), this,  SLOT(cmdTrackerPlotDistribution()) ); 

  QSignalMapper* signalMapper = new QSignalMapper (this);

  connect( xxpAct_,     SIGNAL(triggered()), signalMapper, SLOT(map()) ); 
  connect( yypAct_,     SIGNAL(triggered()), signalMapper, SLOT(map()) ); 
  connect( xyAct_ ,     SIGNAL(triggered()), signalMapper, SLOT(map()) ); 
  connect( sdppAct_,    SIGNAL(triggered()), signalMapper, SLOT(map()) ); 

  signalMapper->setMapping(xxpAct_,  int(PlaneType::X_Xp));
  signalMapper->setMapping(yypAct_,  int(PlaneType::Y_Yp));
  signalMapper->setMapping(xyAct_,   int(PlaneType::X_Y));
  signalMapper->setMapping(sdppAct_, int(PlaneType::S_P));
   
  connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(cmdSwitchView(int))) ;

  plot_ = new TrackerPlot();
  setCentralWidget(plot_);
  
  strcpy(TrSv_.Filter,"*");
  TrSv_.MatchCase = false;
  TrSv_.x  = true;
  TrSv_.y  = true;
  TrSv_.s  = false;
  TrSv_.px = false;
  TrSv_.py = false;
  TrSv_.ps = false;

  TrGs_.Nbin =   50;
  TrGs_.Xin  =  -2.0;
  TrGs_.Xfin =   2.0;

  TrGs_.X  = true;
  TrGs_.Y  = false;
  TrGs_.S  = false;
  TrGs_.Px = false;
  TrGs_.Py = false;
  TrGs_.Ps = false;

  N_      =  500;			
  nturn_  =  1;
  xmin_   = -1.;		
  xmax_   =  1.;
  ymin_   = -1.;		
  ymax_   =  1.;
  xpmin_  = -1.; 		
  xpmax_  =  1.;
  ypmin_  = -1.;		
  ypmax_  =  1.;
  pmin_   = -1.;		
  pmax_   =  1.;
  smin_   = -1.;		
  smax_   =  1.;
  rmin_   =  0.;			
  rmax_   =  6.;
  sigmaP_ =  0.0001; 
  sigmaS_ =  0.1; 
  alphaL_ =  0.0;
  plane_   =  PlaneType::X_Y;
  view_at_elem_   = 0;
  view_elem_      = 0;
  TrackFast_      = false;
  PrintResults_   = true;
  IncrementTurns_ = false;
  strcpy(TrackFilter_, "ip*");
  MatchCase_      = false;
  p_elm_view_.resize(0) ;
  TotalTurnsTracked_ = 0;
 
   // Save track data to file
   
   ExScStr_.dX       = 0.0;
   ExScStr_.dY       = 0.0;
   ExScStr_.dS       = 0.0;
   ExScStr_.dPX      = 0.0;
   ExScStr_.dPY      = 0.0;
   ExScStr_.dPS      = 0.0;
   ExScStr_.dSigmaX  = 0.0;
   ExScStr_.dSigmaY  = 0.0;
   ExScStr_.dSigmaS  = 0.0;
   ExScStr_.dSigmaPX = 0.0;
   ExScStr_.dSigmaPY = 0.0;
   ExScStr_.dSigmaPS = 0.0;

   for (int i=0; i<7; ++i) { 
     TrPlot_.X1[i] = TrPlot_.X2[i] = false;
   }
   TrPlot_.X1[6] = TrPlot_.X2[0] = true;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimTracker::~OptimTracker ()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::cmdParameters() //V7
{

  TrackParamStr st;
  TrackingParametersDialog* dialog = 0;
  
  if (!dialog) {
    dialog = new TrackingParametersDialog(0);
  }

  st.NTurn           = nturn_;
  st.NElem           = view_at_elem_;
  strcpy(st.Filter, TrackFilter_);
  st.FastTracking    = TrackFast_;
  st.PrintResults    = PrintResults_;
  st.IncrementTurns  = IncrementTurns_;
  st.MatchCase       = MatchCase_;

  dialog->data_ = st;
  dialog->set();  

  if( dialog->exec() == QDialog::Rejected) return;
  st =  dialog->data_;
  
  nturn_         = st.NTurn;			
  view_at_elem_  = st.NElem;
  strcpy(TrackFilter_, st.Filter);
  TrackFast_     = st.FastTracking;
  PrintResults_  = st.PrintResults;
  IncrementTurns_= st.IncrementTurns;
  MatchCase_     = st.MatchCase;

  if(nturn_<1) {nturn_=1;}

  if(view_at_elem_ < -1)            view_at_elem_ = -1;
  if(view_at_elem_ > mainw_->nelm_) view_at_elem_ =  0;

   // find Elements to be filtered

  if( (view_at_elem_ == -1) && (nturn_>1)){
     total_elm_view_ = 0;
   
     for(int i=0; i< mainw_->nelm_; ++i) {
       char const* fnm = mainw_->beamline_[i]->fullName();
       if( filterName(fnm, TrackFilter_, MatchCase_)) total_elm_view_++;
     }
     if(total_elm_view_ == 0 ){
       OptimMessageBox::warning(this, "Tracker Error", "Element selection is empty.", QMessageBox::Ok);
       return;
     }  
     else { 
       OptimInformationMessage(this, "Tracker ", (QString().setNum(total_elm_view_) + QString(" Elements selected.")).toUtf8().data(), QMessageBox::Ok);
     }
     p_elm_view_.resize(total_elm_view_);
     int j=0;
     for(int i=0; i<mainw_->nelm_; ++i) {
       char const* fnm = (i==0) ? "START" : mainw_->beamline_[i]->fullName();
       if( filterName(fnm, TrackFilter_, MatchCase_)) p_elm_view_[j++] = i;
    }
   } else 
   {
     total_elm_view_ = 1;
   }

   TotalTurnsTracked_ = 0;

   if(nturn_ ==1 ){
     history_.resize(1);
     history_[0].resize(mainw_->nelm_+1);
   }
   else{
     history_.resize(total_elm_view_);
     for(int i=0; i<total_elm_view_; ++i) { history_[i].resize(nturn_+1); }
   }

  //.............................................................................
  // In Poincare mode, use a temporary file to store the turn-by-turn data.
  // the deleter removes the file.

  char* envdbg = std::getenv("OPTIMX_POINCARE_PROTO");
  if (true) {
    std::string fnm = std::string( tmpnam(0) );
    FILE* fp = fopen(fnm.c_str(), "w+");
    fpu_ = std::shared_ptr<FILE>(fp, [fnm](FILE* p){if(p){ fclose(p); remove(fnm.c_str()); p=0;};});
  }
  else {
    fpu_ = nullptr;
  }
  //...............................................................................................


  distributionAct_->setEnabled(true);
            readAct_->setEnabled(true);
            saveAct_->setEnabled(false);
             xxpAct_->setEnabled(false);
             yypAct_->setEnabled(false);
              xyAct_->setEnabled(false);
            sdppAct_->setEnabled(false);
	   inputAct_->setEnabled(false);
      emittancesAct_->setEnabled(false);
       intensityAct_->setEnabled(false);
           trackAct_->setEnabled(false);
        poincareAct_->setEnabled(false);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::cmdInitialize() // V7
{
   TrackChStr st;
   BunchParam bunch;

   static TrackerDistributionDialog* dialog = 0;
   
   if (!dialog) {
     st.Npart = N_;
     st.Rmin  = rmin_;
     st.Rmax  = rmax_;
     dialog = new TrackerDistributionDialog(0);
     dialog->data_ = st;
     dialog->set();
   }

   bool done = true;
   do{

       if (dialog->exec() != QDialog::Accepted) return;
       st = dialog->data_;    
 
       N_     = st.Npart;
       rmin_  = st.Rmin;
       rmax_  = st.Rmax;

       if (rmax_>6.0)         {rmax_ = 6.;        done = false;}
       if (rmax_<0.5)         {rmax_ = 0.5;       done = false;}
       if (rmin_<0.0)         {rmin_ = 0.;        done = false;}
       if ((rmax_-rmin_)<0.5){rmin_= rmax_-0.5;   done = false;}
       
       if(!done) OptimMessageBox::warning(this, "Error in parameters", "Rmax and Rmin are out of range", QMessageBox::Ok);

   } while(!done);

   if(mainw_->interrupted_){ mainw_->interrupted_= false;  return; }

   //if(mainw_->analyzed_) { if(mainw_->analyze(false,1)) return; }
   //else { if( mainw_->analyze(true,1)) return; }

   if(mainw_->analyze( !mainw_->analyzed_, 1) ) return; // first arg true : parse parameters 
 
   if(mainw_->GetSpaceChargeParam(false, bunch)) return;

   sigmaS_ = bunch.L;
   sigmaP_ = bunch.Dp;
   alphaL_ = bunch.alpha;
   DltS_  =  bunch.sOffest;

   if (alphaL_ >  1.0)  alphaL_ =  0.9999;
   if (alphaL_ < -1.0)  alphaL_ = -0.9999;

   vin_.resize(N_);
   vfin_.resize(N_);

   TotalTurnsTracked_ = 0;
   
   if( setInitCoordinates() ) {
       vin_.resize(0);
      vfin_.resize(0);
      return;
   }

   for(int i=0; i<N_; ++i) { 
      auto& particle = vin_[i];  
      particle.lost   = 0;  
      particle.pid    = i;  
      particle.weight = 1.0; 
   }

    distributionAct_->setEnabled(true);
            readAct_->setEnabled(true);
            saveAct_->setEnabled(true);
             xxpAct_->setEnabled(true);
             yypAct_->setEnabled(true);
              xyAct_->setEnabled(true);
            sdppAct_->setEnabled(true);
	   inputAct_->setEnabled(true);
           trackAct_->setEnabled(true);
        poincareAct_->setEnabled(true);
      emittancesAct_->setEnabled(false);
       intensityAct_->setEnabled(false);

   view_elem_=0;


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimTracker::setInitCoordinates() // V7
{

  Twiss4D v;
  if(mainw_->Get4DBetasFromFile(false, false, v)) return 1;

  // contruct the moment matrix from the coupled 4D lattice functions
   std::cout << "initial lattice functions:\n" << v << std::endl;

   double nu1 =  (2*PI) * v.qx;  
   double nu2  = (2*PI) * v.qy;
   
   //.....................................................
   // Method for sampling a random vector x from the N-dimensional multivariate normal distribution
   // with mean vector mu and covariance matrix cov = M 
   // (1) Find *any* real matrix A such that A A^T = M. When M is positive-definite,
   //     the Cholesky decomposition is typically used. An alternative is to use the matrix A = U Lambda obtained
   //     from a spectral decomposition M = U Lambda U^{-1} of M. The former approach is computationally more straightforward but the matrix
   //     A changes for different orderings of the elements of the random vector, while the latter approach gives matrices that are
   //     related by simple re-orderings.
   //     In theory both approaches give equally good ways of determining a suitable matrix A, but there are differences in computation time.
   // (2) Let z = (z1, …, zN)^T be a vector whose components are N independent standard normal variates
   // (3) Let x be mu + Az. This has the desired distribution due to the affine transformation property.
   //.........................................................................................................

#if 1

   // using the spectral decomposition. The real eigenvectors are given by (3.13)
   // in Lebedev & Bogcaz FERMILAB-PUB-10-383-AD

   
   RMatrix_t<4,double> L;
   L[0][0] =    sqrt(v.btx1*v.e1);
   L[1][0] = -v.alfx1*sqrt(v.e1/v.btx1);
   L[2][0] =  sqrt(v.bty1*v.e1)*cos(v.teta1);
   L[3][3] = (fabs(v.bty1) <1e-8 ) ? 0.0 : sqrt(v.e1/v.bty1)*(v.u*sin(v.teta1)-v.alfy1*cos(v.teta1));

   L[1][0] = 0.0;
   L[1][1] = (v.u-1.)*sqrt(v.e1/v.btx1);
   L[2][1] =  sqrt(v.bty1*v.e1)*sin(v.teta1);
   L[3][1] = (fabs(v.bty1)<1e-8) ? 0.0 : -sqrt(v.e1/v.bty1)*(v.alfy1*sin(v.teta1) + v.u*cos(v.teta1));

   L[0][2] =  sqrt(v.btx2*v.e2)*cos(v.teta2);
   L[1][2] = (fabs(v.btx2)<1e-8) ? 0.0 : sqrt(v.e2/v.btx2)*(v.u*sin(v.teta2)-v.alfx2*cos(v.teta2));
   L[2][2] =  sqrt(v.bty2 * v.e2);
   L[3][2] = -v.alfy2*sqrt(v.e2/v.bty2);

   L[0][3] =  sqrt(v.btx2*v.e2)*sin(v.teta2);
   L[1][3] = (fabs(v.btx2)<1e-8) ? 0.0 : -sqrt(v.e2/v.btx2)*(v.alfx2*sin(v.teta2) + v.u*cos(v.teta2));
   L[2][3] = 0.0;
   L[3][3] = (v.u-1.)*sqrt(v.e2/v.bty2);

   // NOTE: here the matrix L is NEITHER symmtric NOR lower triangular
   
#endif



#if 0

   // using the Cholesky factorization of the 2nd order moment matrix.
   // The 2nd order moment matrix is given in Appendix A, (p21)
   // in Lebedev & Bogcaz FERMILAB-PUB-10-383-AD


   SymMatrix_t<4,double> M;

  /*11*/ M[0][0] =   v.e1*v.btx1 + v.e2*v.btx2;

  /*21*/ M[1][0] =  -v.e1*v.alfx1 - v.e2*v.alfx2;
  /*22*/ M[1][1] =   v.e1*((1-v.u)*(1-v.u) + v.alfx1*v.alfx1)/v.btx1 + v.e2*(v.u*v.u* + v.alfx2*v.alfx2)/v.btx2;  

  /*31*/ M[2][0] =   v.e1*sqrt(v.btx1*v.bty1)*cos(nu1) + v.e2*sqrt(v.btx2*v.bty2)*cos(nu2);

  /*32*/ M[2][1] =  -v.e1*sqrt(v.bty1/v.btx1)*( (1-v.u)*sin(nu1) + v.alfx1*cos(nu1) ) + v.e2*sqrt(v.bty2/v.btx2)*( v.u*sin(nu2) - v.alfx2*cos(nu2));

  /*33*/ M[2][2] =   v.e1*v.bty1 + v.e2*v.bty2;

  /*41*/ M[3][0]  =  v.e1*sqrt(v.btx1/v.bty1)*(    v.u*sin(nu1) - v.alfy1*cos(nu1))
	               - v.e2*sqrt(v.btx2/v.bty2)*((1-v.u)*sin(nu2) + v.alfy2*cos(nu2));

  /*42*/ M[3][1]  =  v.e1*( (v.alfy1*(1-v.u)-v.alfx1*v.u)*sin(nu1) + (v.u*(1-v.u) + v.alfx1*v.alfy1)*cos(nu1) )/sqrt(v.btx1*v.bty1) 
	               + v.e2*( (v.alfx2*(1-v.u)-v.alfy2*v.u)*sin(nu2) + (v.u*(1-v.u) + v.alfx2*v.alfy2)*cos(nu2) )/sqrt(v.btx2*v.bty2);			  
  /*43*/ M[3][2]  = -v.e1*v.alfy1 - v.e2*v.alfy2;
  /*44*/ M[3][3]  =  v.e1*(v.u*v.u + v.alfy1*v.alfy1)/v.bty1 + v.e2*((1-v.u)*(1-v.u) + v.alfy2*v.alfy2)/v.bty2;

   auto L = cholesky(M); // NOTE: here the matrix M is symmetric and L is lower triangular

#endif


  std::cout << " L = \n" << L << std::endl; 
  std::cout << " L* L.transpose() = \n" <<   L* L.transpose()<< std::endl; 

  double vi[6];

  int k=0;
  while (k<N_) {
    double s = 0.0;
    for(int i=0; i<6; ++i){
      do {
	vi[i] = gauss();
      } while (	fabs(vi[i]) > 3.5); // cut at 3.5-sigma
      s     += vi[i]*vi[i];
    }

    s = sqrt(s);

    if(s < rmin_ || s > rmax_) continue; // s = radius in normalized 6D phase space

    // generate 4D distribution
    
    for(int i=0; i<4; i++) {
      vin_[k][i] = 0.0;
      for(int j=0; j<4; ++j) {
	//vin_[k][i] += M[i][j]*vi[j];
	vin_[k][i] += L[i][j]*vi[j];
      }
    }

    // generate long distribution
    
    vin_[k][4] = (sqrt(1.0-alphaL_*alphaL_)*vi[4] + vi[5]*alphaL_)*sigmaS_;
    vin_[k][5] = vi[5]*sigmaP_;

    // add dispersion contribution to transverse coordinates
    
    vin_[k][0] += v.dx  * vi[5]*sigmaP_;
    vin_[k][1] += v.dxp * vi[5]*sigmaP_;
    vin_[k][2] += v.dy  * vi[5]*sigmaP_;
    vin_[k][3] += v.dyp * vi[5]*sigmaP_;


    ++k;
  } // while

   inputscatter_ = new ScatterPlotItem( std::shared_ptr<TrackingScatterData>( new TrackingScatterData(&vin_[0], N_ )));
   inputscatter_->attach(plot_);
   cmdInput();

   double ms     = mainw_->ms;
   double tetaY  = mainw_->tetaYo0_;
   double Enr    = mainw_->Ein;
   double gamma  = 1.0+Enr/ms;
   BeamMoments mom(gamma, vin_, N_, false);
   
   return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::cmdRead()
{
  unsigned int const ncols = 9; // max number of columns read from a distribution file. 
                                // the minimum is 6 (the 6 phase space coordinates)

  double dat[ncols];
  char   buf[1024];
  
  
  QString FileName = QFileDialog::getOpenFileName ( 0, "Read Tracking Data", ".");
  
  if (FileName == "") return;
  FILE *fp = 0; 
  if( !(fp=fopen(FileName.toUtf8().data(),"r") ) ){
    OptimMessageBox::warning(this, "Error - ",  FileName.toUtf8().data(), QMessageBox::Ok);
    return;
  }

  std::unique_ptr<FILE, int(*)(FILE*)> fpu(fp,&std::fclose); 

  fgets(buf, 255, fp);
  if(!strcmpr("OptiM Track Data", buf)){

errtc:	
    OptimMessageBox::warning( this, "This is not a valid OptiM Track Data file.", FileName.toUtf8().data(), QMessageBox::Ok);
    return;
  }

  // Validate the file. There must be at least 6 entries
  // (phase space coordinates) on each line. 

  int np = 0;
  while(fgets(buf, 255, fp)!= 0 ){
    if(buf[0]=='#')  continue;
    if(decodeExtLine(buf, dat, 6) == 6 ) ++np;
    else { goto errtc; }
  }

  N_ = np;

  static ExtraScatterDialog* dialog = 0;

   if (!dialog) {
     dialog = new ExtraScatterDialog(0);
   }

   // dialog->set();
   if ( dialog->exec() == QDialog::Rejected ) return;
   
   std::vector<double> dx(6);
   dx[0] = dialog->data_.dX;
   dx[1] = dialog->data_.dPX*0.001;;
   dx[2] = dialog->data_.dY;
   dx[3] = dialog->data_.dPY*0.001;;
   dx[4] = dialog->data_.dS;
   dx[5] = dialog->data_.dPS*0.001;;

   std::vector<double> dsx(6);
   
   dsx[0] =  dialog->data_.dSigmaX;
   dsx[1] =  dialog->data_.dSigmaPX;
   dsx[2] =  dialog->data_.dSigmaY;
   dsx[3] =  dialog->data_.dSigmaPY;
   dsx[4] =  dialog->data_.dSigmaS;
   dsx[5] =  dialog->data_.dSigmaPS;

    vin_.resize(N_);
   vfin_.resize(N_);
   
   if (nturn_<=1) { 
     history_.resize(mainw_->nelm_+1);
   }
   else { 
     history_.resize(nturn_+1);
   }
   //------------------------------------------
   // Rewind and read particle
   // number and weight if they are supplied. 
   // otherwise, assign default values.
   // Add user-specified offsets
   // and rms widths.   
   //------------------------------------------
   
   rewind(fp);
   fgets(buf, 255, fp);

   int i = 0;
   //int k = 0;

   while(fgets(buf, 255, fp)!= 0){
     if(buf[0]=='#') continue;
     if ( int k = decodeExtLine(buf, dat, ncols)  ){
       for(int j=0; j<6; ++j){ 
         vin_[i][j] = dat[j]+dx[j] + dsx[j]*gauss(); 
       }
       vin_[i].pid        = (k>=7) ? dat[6]      : i;
       vin_[i].weight     = (k>=8) ? dat[7]      : 1.0;
       vin_[i].lost       = (k>=ncols) ? int(dat[8]) : 0;
       ++i;
     }
   }

   view_elem_ = 0;

   inputscatter_ = new ScatterPlotItem( std::shared_ptr<TrackingScatterData>( new TrackingScatterData(&vin_[0], N_)));
   inputscatter_->attach(plot_);

   cmdInput(); // switch to input view

    distributionAct_->setEnabled(true);
            readAct_->setEnabled(true);
            saveAct_->setEnabled(true);
             xxpAct_->setEnabled(true);
             yypAct_->setEnabled(true);
              xyAct_->setEnabled(true);
            sdppAct_->setEnabled(true);
	   inputAct_->setEnabled(true);
	  outputAct_->setEnabled(false);
      emittancesAct_->setEnabled(false);
       intensityAct_->setEnabled(false);
           trackAct_->setEnabled(true);
        poincareAct_->setEnabled(true);

   return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::cmdSave()
{

  TrackerSaveDistributionDialog* dialog = 0;
  if (!dialog) {
    dialog = new TrackerSaveDistributionDialog(this);
  }  

  if ( dialog->exec() == QDialog::Rejected ) return;

  FILE *fp = 0;

  QString FileName = QFileDialog::getSaveFileName ( 0, "Save Tracking Data", ".");
  if (FileName == "" ) return; 
  
  if((fp = fopen(FileName.toUtf8().data(),"w"))== 0 ){
     OptimMessageBox::warning(this, "Error opening file - ", FileName.toUtf8().data(), QMessageBox::Ok);  
    return;
  }

  fprintf(fp,"OptiM Track Data\n");
    
  static char const info_all[] = "# x[cm]  x'  y[cm]  y'  s[cm]  dp/p  label  weight  code  elm  pass\n";   
  static char const info[]     = "# x[cm]  x'  y[cm]  y'  s[cm]  dp/p  label  weight\n";   

  fprintf(fp,  (dialog->data_.saveall ? info_all: info));
  
  Coordinates* v = view_elem_ ? &vfin_[0] : &vin_[0];

  for( int i=0; i<N_; ++i){
    if ((!dialog->data_.saveall) && (v[i].lost != 0) ) continue;
    for(int j=0; j<6; ++j) fprintf(fp,"%g  \t",v[i][j]);
    fprintf(fp,"%d \t%g ",v[i].pid, v[i].weight);

    if (dialog->data_.saveall) fprintf(fp,"\t%d \t%d \t%d", v[i].lost, v[i].nelem, v[i].npass);

    fprintf(fp,"\n");
  }
  fclose(fp);
 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int
OptimTracker::trackParticleExact(Element const* ep, double Enr0,  double tetaY,
				 Bunch& v, int N, int n_turn, int n_elem) // V7
{

  RMatrix m1;
  TrackParam prm;
  double enr;
  int    rt = 0;
  static char const *msg[]={
    "Beam contains only one particle !",
    "All particles have the same longitudinal coordinate.",
    "Spline error for original wake computation (err 3).",
    "Spline error for fitted wake computation (err 4).",
    "Spline error for fitted wake computation (err 5).",
    "Particle is outside the range of the wake data provided in the file."
  };
  
  if( toupper(ep->name()[0])=='Y'){  
    // wake field
    rt = trackWake(ep, v, N, Enr0, mainw_->ms, &(mainw_->ext_dat[ep->N]));
    if(rt) {
      OptimMessageBox::warning(this, "Tracking err. in wake elm.", msg[rt-1], QMessageBox::Ok);
      	return 1;
    }
    return 0;
  }

  ep->preTrack(mainw_->ms, Enr0, tetaY, n_elem, prm, m1);
  //std::cout << ep->name() << " " << ep->str() << std::endl;
  //std::cout << "n_elem = " << n_elem << "\n" << prm << std::endl;

  
int kloop = 0;
int kcond = 0;

int Np =  v.size();
#pragma omp parallel for if( parallel_tracking_) 
 for(int j=0; j<Np; ++j) {

    //#pragma omp atomic capture
    //kcond  = kloop++;

    enr = Enr0;
    if (v[j].lost != 0 ) continue; // do not track lost particle. 
    ep->trackOnce(mainw_->ms, enr, n_elem, n_turn,  prm,  m1, v[j] );

    //#pragma omp critical(processevents) 
    //{
    //   if (kcond%1000 != 0) { QCoreApplication::processEvents(); }
    //}
  }

  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimTracker::trackWake(Element const* ep, Bunch& v, int N, double Enr0, double ms, ExtData* p) // V7
{
  if ( N_<2 ) return 1; // only one particle in the beam
  
  double P0 = sqrt(2.*ms*Enr0+Enr0*Enr0)*1e6;
  // Find min and max s
  double smin = v[0][4];
  double smax = smin;

  for(int i=0; i<N_; ++i){
     auto& particle = v[i];
     if(smin > particle[4]) smin =particle[4];
     if(smax < particle[4]) smax = particle[4];
   }
   if( smax<=smin ) return 2;                    // all particles are in the same point
   if( (smax - smin) > p->x[p->n-1] ) return 6;  // a particle outside of wake range
 
  // building reference long. coordinates (binning)

   int nb = 20+(int)exp(0.33333*log(double(N)));

   std::vector<double> sb(nb);
   std::vector<double> wx(nb);
   std::vector<double> wy(nb);
   std::vector<double> vx(nb);
   std::vector<double> vy(nb);

   int rt=0;
   double deltaS=(smax-smin)/(nb-7);
   for(int i=0; i<nb; ++i){
      sb[i]=smin+(i-3)*deltaS;
      wx[i]=0.;
      wy[i]=0.;
   }
   double x,y;
   // computing wake at reference coordinates
   for(int i=0; i<N; ++i) {
      for( int n=0; n<nb; ++n) {
        x = v[i][4] - sb[n];
        if(x<0.0) continue;
        if(splint( &p->x[0], &p->y[0], &p->v[0], p->n, x, &y)){rt=3; goto rttw;} //spline err
        switch(ep->plane()){
   	  case 0:  // T - both transverse
      	    wx[n] += v[i][0]*ep->B*y/(N*P0);
      	    wy[n] += v[i][2]*ep->B*y/(N*P0);
      	    break;
   	  case 1:  // X - plane
      	    wx[n] += v[i][0]*ep->B*y/(N*P0);
      	    break;
   	  case 2:  // Y - plane
      	    wx[n] += v[i][2]*ep->B*y/(N*P0);
      	    break;
   	  case 3:  // Longitudinal
      	    wx[n] -= ep->B*y/(N*P0);
      	    break;
        }
       }
     }
     // Filtering and setting spline
     Filter7(&wx[0], nb);
     spline(&sb[0], &wx[0], nb, &vx[0]);
     if(!ep->plane()){
     Filter7(&wy[0], nb);
     spline(&sb[0], &wy[0], nb, &vy[0]);}
     // Computing corrections
     for(int i=0; i<N; ++i) {
       if(splint(&sb[0], &wx[0], &vx[0], nb, v[i][4], &x)){rt=4; goto rttw;}//spline err
       switch(ep->plane()){
   	 case 0:  // T - both transverse
   	   if(splint(&sb[0], &wy[0], &vy[0], nb, v[i][4], &y)){rt=5; goto rttw;}//spline err
      	   v[i][1] +=x;
      	   v[i][3] +=y;
      	   break;
   	 case 1:  // X - plane
      	   v[i][1] +=x;
      	   break;
   	 case 2:  // Y - plane
      	   v[i][3] +=x;
      	   break;
   	 case 3:  // Longitudinal
      	   v[i][5] +=x;
      	   break;
      }
   }

rttw:
   return rt;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::trackParticleThroughElem(char nm, double Hrt, Element const *ep,
        Bunch& v, RMatrix const& me, int N, int k, int i,
	double dPdS, double dPdE, double capa, double dtx, double dty, double dSdP) // V7
{
  double x, y, s, c;
  
  for(int j=0; j<N_; ++j) {

    auto& particle = v[j];

    int ret = 0;

    if (! j%1000 == 0) { QCoreApplication::processEvents(); }

    if( particle.lost !=0 ) continue;

    switch( nm ) {
      	case 'A': 
        case 'W': 
          particle[5] += dPdS*particle[4];
        case 'E': // electrostatic acceleration 
        case 'X': // matrix
          particle.c = me*particle.c;
          particle[0] *= capa;
	  particle[1] *= capa;
          particle[2] *= capa;
	  particle[3] *= capa;
          particle[5] *= capa*capa;
          break;
        case 'S':
	  Sextupole::sext_trans(ep, Hrt, &particle, &particle);
          break;
        case 'M':
	  Multipole::multipole_trans(ep, Hrt, &particle, &particle);
          break;
        case 'K':
          particle[0] += ep->length() * ( particle[1] + 0.5 *dtx);
          particle[1] += dtx;
          particle[2] += ep->length() * ( particle[3] + 0.5 *dty);
          particle[3] += dty;
          break;
        case 'Z':
          particle[0] += ep->length() * particle[1];
          particle[2] += ep->length() * particle[3];
          particle[5] += ep->G*dPdE;
          break;
        case 'H': // aperture
          if( (ep->S <= 0.0) || (ep->A <= 0.0) )break;
          s = sin(PI*ep->tilt()/180.);  // aperture cooordinate rotation 
	  c = cos(PI*ep->tilt()/180.);
          x = (particle[0] - ep->B)*c + (particle[2] - ep->G)*s;
          y =-(particle[0] - ep->B)*s + (particle[2] - ep->G)*c;
          ret = 0;
          switch (ep->N){
            case 1:  // Ellipse
              x /= ep->S;   
              y /= ep->A;

              if((x*x+y*y) > 1.0) ret=1;

              break;
            case 2:  // Hyperbole
              if( (fabs(2.*x*y)>(ep->S*ep->S)) || (x*x+y*y>ep->A*ep->A))ret=1;
              break;
	      
            default: // Rectangle
              if( (fabs(x) > ep->S) || (fabs(y) > ep->A)) ret=1;
              break;
          }
          if(ret){
            particle.lost  = 2;
            particle.nelem = i+1;
            particle.npass = k; 
	  }
          break;
        default:
          particle.c = me*particle.c;
      }

      particle[4] += dSdP*particle[5];

      if(fabs(particle[0]) > 1000.0 || fabs(particle[2]) > 1000.0 ) {
      	particle.lost  = 1;
        particle.nelem = i+1;
        particle.npass = k;
      }
  } // for 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::getHistory(BeamMoments* p_hist, double gamma, Twiss const& twiss, Bunch const& v) // V7
{
  // Computes moments, emittances and other parameters of the particle distribution v (input)   
  // Results are returned via p_hist (output)
  
  double bg   = sqrt(gamma*gamma - 1.0);
  double btx  = twiss.BtX;
  double bty  = twiss.BtY;
  double alfx = twiss.AlX;
  double alfy = twiss.AlY;
  double gamx = (1. + twiss.AlX * twiss.AlX)/twiss.BtX;
  double gamy = (1. + twiss.AlY * twiss.AlY)/twiss.BtY;
  
  p_hist->emitX()   = 0.0;
  p_hist->emitY()   = 0.0;
  p_hist->sigmaDP() = 0.0;
  p_hist->intensity   = 0.0;
  
  p_hist->Xmax() = p_hist->Ymax() = 0.0;
  p_hist->Xav()  = p_hist->Yav()  = p_hist->Sav() = 0.;
  p_hist->PXav() = p_hist->PYav() = p_hist->PSav() = 0.;

  double S  = 0;
  double S0 = 0;

  int np = 0;
  for(int j=0; j<N_; ++j){
    auto& particle = v[j];

    S0 +=  particle.weight;
    if( particle.lost) continue;
    S  += particle.weight;

    if(p_hist->Xmax() < fabs(particle[0])) p_hist->Xmax() = fabs(particle[0]);
    if(p_hist->Ymax() < fabs(particle[2])) p_hist->Ymax() = fabs(particle[2]);

    double weight = particle.weight;
    p_hist->Xav()  += (particle[0] * weight);
    p_hist->PXav() += (particle[1] * weight);
    p_hist->Yav()  += (particle[2] * weight);
    p_hist->PYav() += (particle[3] * weight);
    p_hist->Sav()  += (particle[4] * weight);
    p_hist->PSav() += (particle[5] * weight);
    ++np;
   }

  if (np !=0) {
    p_hist->Xav()  /= S;
    p_hist->Yav()  /= S;
    p_hist->Sav()  /= S;
    p_hist->PXav() /= S;
    p_hist->PYav() /= S;
    p_hist->PSav() /= S;
  }

   p_hist->intensity=S/S0;

  //------------------------------------------------------------------------------

 #if 1  
     for(int j=0; j<N_; ++j){

       auto& particle = v[j];

       if( particle.lost)continue;

       double x      =  (particle[0]-p_hist->Xav())  - (twiss.DsX *(particle[5]-p_hist->PSav()));
       double xp     =  (particle[1]-p_hist->PXav()) - (twiss.DsXp*(particle[5]-p_hist->PSav()));
       p_hist->emitX() += (xp*xp*btx + 2.*x*xp*alfx + x*x*gamx) * particle.weight;

       double y        =  (particle[2]-p_hist->Yav()) - (twiss.DsY* (particle[5]-p_hist->PSav()));
       double yp       =  (particle[3]-p_hist->PYav())- (twiss.DsYp*(particle[5]-p_hist->PSav()));
       p_hist->emitY() += (yp*yp*bty + 2.*y*yp*alfy + y*y*gamy)*particle.weight;

       p_hist->sigmaDP() += (particle[5]-p_hist->PSav())*(particle[5]-p_hist->PSav())*particle.weight;

   }

   if(np != 0){

     // NOTE; the factor arises from the fact that  1/r [int_0^r r dr] = 1/2 r^2/r = r/2
     //       here, r is the action
     p_hist->emitX() /= (2*S); // unnormalized emittance // *= bg/(2*S);  // normalized emittance    
     p_hist->emitY() /= (2*S); // unnormalized emittance // *= bg/(2*S);  // normalized emittance
     p_hist->sigmaDP() = sqrt(p_hist->sigmaDP()/S);
   }


   // ==================================================================

#endif
#if 0
     double sig[6][6]; 
     for (int i=0; i<6; ++i) {
        for (int j=0; j<6; ++j) {
	  sig[i][j] = 0.0;
	}
     } 

     for(int j=0; j<N_; ++j){

      auto& particle = v[j];
       
      if( particle.lost)continue;

      double w = particle.weight;
   
      double x      =  (particle[0]-p_hist->Xav) - twiss.DsX *(particle[5]-p_hist->PSav);
      sig[0][0]    +=  x*x*w;

      double xp     =  (particle[1]-p_hist->PXav)- twiss.DsXp*(particle[5]-p_hist->PSav);
      sig[1][1]    +=  xp*xp*w;

      sig[0][1]    +=  x*xp*w;
      sig[1][0]     =  sig[0][1];

      double y      =  (particle[2]-p_hist->Yav) - twiss.DsY* (particle[5]-p_hist->PSav);
      sig[2][2]    +=  y*y*w;

      double yp     =  (particle[3]-p_hist->PYav)- twiss.DsYp*(particle[5]-p_hist->PSav);
      sig[3][3]    +=  yp*yp*w;

      sig[2][3]    +=  y*yp*w;
      sig[3][2]     =   sig[2][3];


      double s      =  (particle[4]-p_hist->Sav);
      sig[4][4]    +=  s*s*w;

      double dpp    =  (particle[5]-p_hist->PSav);
      sig[5][5]    +=  dpp*dpp*w;

      sig[5][4]    +=  s*dpp*w;
      sig[4][5]     =  sig[5][4];


   }

 
   double epsx =  sqrt(sig[0][0]* sig[1][1] -  sig[1][0]* sig[0][1]);
   double epsy =  sqrt(sig[2][2]* sig[3][3] -  sig[2][3]* sig[3][2]);
   double epsz =  sqrt(sig[4][4]* sig[5][5] -  sig[4][5]* sig[5][4]);


   // S = total particle weight. If all weights = 1, S = no of particles at location

   if( (np != 0) ) {
     p_hist->emitX    = epsx*(bg/S);   
     p_hist->emitY    = epsy*(bg/S);
     p_hist->sigmaDP =  sqrt( sig[5][5]/S);
}

 #endif
   // ==================================================================


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimTracker::cmdTracking( bool poincare) // V7
{

  parallel_tracking_  = Globals::preferences().parallel_tracking;

  if(mainw_->interrupted_ ) { mainw_->interrupted_ = false; return;}
  if (mainw_->analyze(!mainw_->analyzed_,1)) return;

  poincareAct_->setEnabled(poincare);

  // clear all existing scatter plot data 
  plot_->detachItems(Optim::Rtti_ScatterPlotItem);

  // input data 
  inputscatter_ = new ScatterPlotItem( std::shared_ptr<TrackingScatterData>( new TrackingScatterData(&vin_[0], N_)));
  inputscatter_->attach(plot_);
  outputscatter_ = 0;

  RMatrix me;
  RMatrix tm;
  Twiss twiss;

  double EnrNew, dtx, dty, capa, dPdS, dSdP, dalfa;
  char nm, buf[LSTR+1];
  int i, ie, kin;

  std::complex<double> ev[4][4];
  int nline;

  // Print tracking status update to the text window
  // Note: the output window is created by the dialog only if requested 

  auto TrackOutCh = mainw_->getAttachedSubWin(WindowId::TrackOutCh);

  OptimTextEditor* editor = 0;
  if (PrintResults_) { 
    if (!TrackOutCh){
      TrackOutCh = mainw_->createAttachedSubWin( (editor = new OptimTextEditor()), WindowId::TrackOutCh);  
      editor->setWindowTitle("Tracking Results");
    }
    else {
      editor = qobject_cast<OptimTextEditor*>(TrackOutCh->widget());
      if (mainw_->CtSt_.ClearText) editor->clear();
    } 
    QCoreApplication::processEvents();
  }
  
  if (PrintResults_) {
    TrackOutCh->raise();
    TrackOutCh->setGeometry( mainw_->mdiArea->geometry().width()/2, 0,
				    mainw_->mdiArea->geometry().width()/2,  mainw_->mdiArea->geometry().height());
    TrackOutCh->show();
  }


  int Nelm_f = (view_at_elem_> 0 ) ?  view_at_elem_ : mainw_->nelm_;

  double ms     = mainw_->ms;
  double tetaY  = mainw_->tetaYo0_;
  double Enr    = mainw_->Ein;
  double gamma  = 1.+Enr/ms;
  double dPdE   = (Enr+ms)/(Enr*Enr+2.*Enr*ms);
  double Hrt    = sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;

  //....................................................................
  // If this is the FIRST turn, make new final distribution the same as
  // the initial distribution. The "Final" distribution is tracked.
  // The initial distribution remains *UNCHANGED*.
  // TotalTurnsTracked_ is the cumulative no of turns. It is initialized
  // in the initialize() function.
  //....................................................................
  
  //for(int i=0; i<N_; ++i) loss_[i].lost = 0; // THIS SHOULD NOT BE NEEDED !!

  if (TotalTurnsTracked_ == 0 ) {
    for( int j=0; j<N_; ++j) for(int i=0; i<6; i++) { vfin_[j][i] = vin_[j][i];}
  }  
 
   Bunch& v = vfin_;

   twiss.BtX  = mainw_->BetaXin;        twiss.BtY  = mainw_->BetaYin;
   twiss.AlX  = mainw_->AlfaXin;        twiss.AlY  = mainw_->AlfaYin;
   twiss.DsX  = mainw_->DispXin;        twiss.DsY  = mainw_->DispYin;
   twiss.DsXp = mainw_->DispPrimeXin;   twiss.DsYp = mainw_->DispPrimeYin;

   if( mainw_->CtSt_.IsRingCh ){
     mainw_->findRMatrix(tm);
     i = find_tunes(tm, 100., twiss, &dalfa);
     if(i){
       OptimMessageBox::warning(this, "Close Error", "Cannot close for X or Y", QMessageBox::Ok);
       return;
     }
   }
  
   twiss.nuX = 0.0;			 
   twiss.nuY = 0.0;
   
   twiss.eigenvectors(ev);
   
   if( (nturn_>1) && (view_at_elem_ ==-1)) {
     kin = -1;
   }
   else {
     kin = 0;
     getHistory(&history_[0][0], gamma, twiss, v);
     history_[0][0].s=0.0;
   }

  progress_bar_ = new QProgressDialog(0);
  progress_bar_->setWindowModality(Qt::ApplicationModal);
  connect ( progress_bar_, SIGNAL( canceled()), this, SLOT( cancel()) ) ;  
  progress_bar_->resize(400, 150);
  progress_bar_->setWindowTitle("Tracking Progress");
  progress_bar_->setLabelText(QString("Tracking %1 particles %2 time(s) through the lattice.\n Using %3 thread(s).").arg(N_).arg(nturn_).arg(parallel_tracking_ ? omp_get_max_threads() : 1));
  progress_bar_->setMinimumDuration(4000); 

  omp_set_dynamic(0);           // Explicitly disable dynamic teams
  // omp_set_num_threads(4);    // to enforce a specific maximum

  for(int k=kin; k<nturn_; ++k) {
     ie = 0;
     for(int i=0; i <mainw_->nelm_; ++i) { 
       auto ep     = mainw_->beamline_[i];
       nm     = toupper(ep->name()[0]);
       dSdP   = ep->length()/(gamma*gamma);
       EnrNew = Enr;
       
         switch(nm){
	   case 'A': // pill-box cavity 
	   case 'W': // general rf cavity
            dPdS = dPdE*2.*PI*ep->G/ep->tilt()*sin(PI/180.*ep->S);
	   case 'E': // electrostatic acc
	   case 'X': // xfer matrix 
            me = ep->rmatrix( EnrNew, ms, tetaY, 0.0, 3);
            capa = sqrt(sqrt((2.*Enr*ms+Enr*Enr)/(2.*EnrNew*ms+EnrNew*EnrNew)));
            break;
	   case 'K': //transverse corrector
            dtx = ep->length()*ep->B/Hrt*cos(PI*ep->tilt()/180.);
            dty = ep->length()*ep->B/Hrt*sin(PI*ep->tilt()/180.);
          default:
            me = ep->rmatrix(Enr, ms, tetaY, 0.0, 3);
            break;
         }
       
         ep->propagateLatticeFunctions(me, twiss, ev);

         if( TrackFast_ ) { // TrackFast_ == Track using transfer matrices. Also include correctors
	                    // and update energy when going through accelerating elements.    
	     trackParticleThroughElem(nm, Hrt, ep.get(), v, me, N_, k+1, i, dPdS, dPdE, capa, dtx, dty, dSdP);
	 }
	 else {
	   // NOTE: if TrackFast_==false, there is no need to propagate lattice functions (is this true?)
	   if(trackParticleExact(ep.get(), Enr, tetaY, v, N_, k+1, i)) { mainw_->interrupted_ = true; return; }
	 }
	 
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
         } // switch

       if(nturn_==1) {
         getHistory( &history_[0][i+1], gamma, twiss, v);
         history_[0][i+1].s=history_[0][i].s+ep->length()*0.01;
       }
       else{
         if(view_at_elem_ ==-1){
           if(i==p_elm_view_[ie]){
             getHistory(&history_[ie][k+1], gamma, twiss, v);
             history_[ie][k+1].s=k+1;
             ++ie;
           }
         }
      	 if((view_at_elem_>0)&&(view_at_elem_==(i+1))){
           getHistory(&history_[0][k+1], gamma, twiss, v);
           history_[0][k+1].s=k+1;
         }
       }
       
       if((k==nturn_-1)&&(i==Nelm_f-1))break;
       
       if (mainw_->interrupted_) {
	   mainw_->interrupted_ = OptimQuestionMessage(this,  "Tracking", "Do you want to interrupt tracking  ?",
						       QMessageBox::Yes| QMessageBox::No) == QMessageBox::Yes;
	   break;
       }
  
       progress_bar_->setValue(int( double( (i+1) + (k *mainw_->nelm_) )/double(mainw_->nelm_*nturn_)  * 100.0 ));  
       QCoreApplication::processEvents();


       } // elements for loop

     if (mainw_->interrupted_) break; //  interrupted in the inner loop, we need to break out of the outer loop.
 
     TotalTurnsTracked_++;

     if(nturn_>1){
        if(view_at_elem_ == 0){ getHistory(&history_[0][k+1], gamma, twiss, v); history_[0][k+1].s=k+1; }
      	if(k==kin) { 
           if(fabs((mainw_->Ein -Enr)/mainw_->Ein)>1.e-12){
             sprintf(buf,
              "On the first turn, the beam was accelerated by %e percent. This can cause instability in further tracking. Do you want to proceed ?",
              (mainw_->Ein -Enr)/mainw_->Ein*0.01);
	      if(OptimQuestionMessage(this,  "Tracking", buf, QMessageBox::Yes| QMessageBox::No) == QMessageBox::No )return;
	      
           }
	}
        if( (twiss.BtX > 1.0e8) || (twiss.BtY >1.0e8)){
	  OptimQuestionMessage(this, "Tracking", "Beta-functions are above threshold. Calculations stopped",QMessageBox::Yes|QMessageBox::No);
          break;
        }
   	
      if(PrintResults_){
          QTextCursor cursor = editor->textCursor();
	  cursor.select(QTextCursor::LineUnderCursor);
	  cursor.removeSelectedText();
	  sprintf(buf,"Tracked %d of %d turns.", k+1, nturn_);
      	  editor->insertPlainText(buf);
      }
     }

     if( IncrementTurns_){ 
        if(mainw_->analyze( false,k+2) ) return;
     }


     //........................................................
     // In Poincare mode, store turn-by-turn state coordinates
     if (poincare) { 
        for (int i=0; i< N_; ++i ) {
	  auto& particle = v[i];
	   fmt::print(fpu_.get(),"{:12.6g} {:12.6g} {:12.6g} {:12.6g} {:12.6g} {:12.6g} {:12d} \n",
	  	                      particle[0],particle[1],
		                      particle[2],particle[3],
		                      particle[4],particle[5], i);
	};
     } // if poincare
  }; // for int k=nturns ... 

  //....................................................

  progress_bar_->setValue(100);

   view_elem_=1;

   outputscatter_ = poincare ?  new ScatterPlotItem( std::shared_ptr<PoincareScatterData>( new PoincareScatterData(fpu_ ))) :
                                new ScatterPlotItem( std::shared_ptr<TrackingScatterData>( new TrackingScatterData(&vfin_[0], N_)));

   outputscatter_->attach(plot_);

   if(PrintResults_){
     QTextCursor cursor = editor->textCursor();
     cursor.select(QTextCursor::LineUnderCursor);
     cursor.removeSelectedText();
     i = (nturn_==1) ? Nelm_f : (TotalTurnsTracked_-1);
     editor->insertPlainText("EmitX_N[cm] \tEmitY_N[cm] \tXmax[cm] \tYmax[cm] \tdP/P          \tIntensity");
     editor->insertPlainText("\n");
     sprintf(buf,"%g  \t%g  \t%f  \t%f  \t%g  \t%f \n", history_[0][i].emitX(), history_[0][i].emitY(),
	     history_[0][i].Xmax(), history_[0][i].Ymax(), history_[0][i].sigmaDP(), history_[0][i].intensity);
     sprintf(buf, "Total Turns Tracked %d", TotalTurnsTracked_);
     editor->insertPlainText(buf);
     editor->insertPlainText("\n");
     editor->document()->setModified(false);
    }

    cmdOutput(); // switch to output view
    
    distributionAct_->setEnabled(true);
            readAct_->setEnabled(true);
            saveAct_->setEnabled(true);
             xxpAct_->setEnabled(true);
             yypAct_->setEnabled(true);
              xyAct_->setEnabled(true);
            sdppAct_->setEnabled(true);
	   inputAct_->setEnabled(true);
	  outputAct_->setEnabled(true);
      emittancesAct_->setEnabled(true);
       intensityAct_->setEnabled(true);

    mainw_->mdiArea->setActiveSubWindow(mainw_->Tracker_.data());;    

    mainw_->interrupted_  =  false;   
    
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::cmdSwitchView( PlaneType view)
{
  plane_ = view;

  switch (plane_) {
  case PlaneType::X_Y:
    xyAct_->setChecked(true);
    break;
  case PlaneType::X_Xp:
    xxpAct_->setChecked(true);
    break;
  case PlaneType::Y_Yp:
    yypAct_->setChecked(true);
    break;
  case PlaneType::S_P:
    sdppAct_->setChecked(true);
    break;
 }
  plot_->setPlane(view);
  plot_->replot(); 

  setWindowFlags(windowFlags()| Qt::WindowStaysOnTopHint);
  show();

  return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::cmdInput()
{

  setWindowTitle( "Optim Tracker: Input Distribution"); 
  if (outputscatter_) outputscatter_->hide();
  inputscatter_->show();
  cmdSwitchView(plane_); 
  inputAct_->setChecked(true);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::cmdOutput()
{
  setWindowTitle( "Optim Tracker: Output Distribution"); 
  if (outputscatter_) outputscatter_->show();
  inputscatter_->hide();
  cmdSwitchView(plane_); 
  outputAct_->setChecked(true);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimTracker::cmdTrackSavePos() // V7
{

  if (history_.size() == 0 ) {
      OptimMessageBox::warning(this, "Tracking", "There is no tracking history to save.", QMessageBox::Ok); 
      return;
  }

  TracSaveStruct TrSv;

  static TrackSavePositionsDialog* dialog = 0;
  if (!dialog) { 
    dialog = new  TrackSavePositionsDialog(0);
  }
  if (dialog->exec() == QDialog::Rejected ) return;
   
   
  FILE* fp = fopen(dialog->data_.fileName,"w");

  if(!fp){ 
      OptimMessageBox::warning(this, "Tracking", "Cannot open file for writing", QMessageBox::Ok); 
      return;
  }

  TrSv = dialog->data_;
  
  if(nturn_ == 1) {
     // Write legend into the first line
     fprintf(fp,"#");
     if(TrSv.x)   fprintf(fp," _X\t");
     if(TrSv.y)   fprintf(fp," _Y\t");
     if(TrSv.s)   fprintf(fp," _S\t");
     if(TrSv.px)  fprintf(fp," _PX\t");
     if(TrSv.py)  fprintf(fp," _PY\t");
     if(TrSv.ps)  fprintf(fp," _PS\t");
     fprintf(fp,"\n");
      
     // write data

     for( int j=0; j<= mainw_->nelm_; ++j) {
       if(j==0) { 
	 fprintf(fp,"%d \t%e \tStart ", 0,  history_[0][0].s*100.0);
       }
       else { 
         char const* fnm = mainw_->beamline_[j-1]->fullName();
         if ( !filterName(fnm, TrSv.Filter, TrSv.MatchCase)) continue;
         fprintf(fp,"%d \t%e \t%s ",  j, history_[0][j].s*100.0, fnm);
       }
       if(TrSv.x)  fprintf(fp,"\t%e ",  history_[0][j].Xav());
       if(TrSv.y)  fprintf(fp,"\t%e ",  history_[0][j].Yav());
       if(TrSv.s)  fprintf(fp,"\t%e ",  history_[0][j].Sav());
       if(TrSv.px) fprintf(fp,"\t%e ",  history_[0][j].PXav());
       if(TrSv.py) fprintf(fp,"\t%e ",  history_[0][j].PYav());
       if(TrSv.ps) fprintf(fp,"\t%e ",  history_[0][j].PSav());
       fprintf(fp,"\n");
     }
  }
  else { // more than 1 turn 
       switch (view_at_elem_){
      	 case -1:
      	   // Write legend into the first line
           fprintf(fp,"#");
      	   for(int i=0; i<total_elm_view_; ++i) {
             char const* fnm = ( i == 0 ) ? "START" : mainw_->beamline_[p_elm_view_[i]]->fullName();
      	     if(!filterName(fnm, TrSv.Filter, TrSv.MatchCase)) continue;
      	     fprintf(fp,"%s", fnm);
      	     if(TrSv.x)  fprintf(fp," _X\t");
      	     if(TrSv.y)  fprintf(fp," _Y\t");
      	     if(TrSv.s)  fprintf(fp," _S\t");
      	     if(TrSv.px) fprintf(fp," _PX\t");
      	     if(TrSv.py) fprintf(fp," _PY\t");
      	     if(TrSv.ps) fprintf(fp," _PS\t");
      	   }
      	   fprintf(fp,"\n");
      	   // write data
      	   for(int j=0; j<TotalTurnsTracked_; ++j) {
      	      for(int i=0; i <total_elm_view_; ++i) {
                 char const* fnm = ( i ==0 ) ? "START" : mainw_->beamline_[p_elm_view_[i]]->fullName();
	         if (!filterName(fnm, TrSv.Filter, TrSv.MatchCase)) continue;
      	         if(TrSv.x)fprintf(fp,"%e \t",  history_[i][j].Xav());
      	         if(TrSv.y)fprintf(fp,"%e \t",  history_[i][j].Yav());
      	         if(TrSv.s)fprintf(fp,"%e \t",  history_[i][j].Sav());
      	         if(TrSv.px)fprintf(fp,"%e \t", history_[i][j].PXav());
      	         if(TrSv.py)fprintf(fp,"%e \t", history_[i][j].PYav());
      	         if(TrSv.ps)fprintf(fp,"%e \t", history_[i][j].PSav());
              }
              fprintf(fp,"\n");
      	   }
           break;
 
     	 default:
	  if(view_at_elem_) {
	    char const* fnm = ( view_at_elem_ == 0 ) ? "START" : mainw_->beamline_[view_at_elem_-1]->fullName();
  	      fprintf(fp,"#%s[%d]", fnm, view_at_elem_);
	   }
           else fprintf(fp,"#LatticeEnd");
           if(TrSv.x)  fprintf(fp," _X\t");
           if(TrSv.y)  fprintf(fp," _Y\t");
           if(TrSv.s)  fprintf(fp," _S\t");
           if(TrSv.px) fprintf(fp," _PX\t");
           if(TrSv.py) fprintf(fp," _PY\t");
           if(TrSv.ps) fprintf(fp," _PS\t");
      	   fprintf(fp,"\n");

	   // write data
      	  
           for(int j=0; j<TotalTurnsTracked_; ++j) {

	     if(TrSv.x)  fprintf(fp,"%e \t",  history_[0][j].Xav());
	     if(TrSv.y)  fprintf(fp,"%e \t",  history_[0][j].Yav());
	     if(TrSv.s)  fprintf(fp,"%e \t",  history_[0][j].Sav());
	     if(TrSv.px) fprintf(fp,"%e \t",  history_[0][j].PXav());
	     if(TrSv.py) fprintf(fp,"%e \t",  history_[0][j].PYav());
	     if(TrSv.ps) fprintf(fp,"%e \t",  history_[0][j].PSav());
              fprintf(fp,"\n");
           }
           break;
       } // switch
   }  // else
   fclose(fp);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::cmdSwitchView( int view)
{
  cmdSwitchView( static_cast<PlaneType>(view) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::cmdPlotIntensity() // V7
{   

  int n;
  char buf[50];

  OptimPlot*     plot   = 0;
  auto TrackerIntensityPlt     = mainw_->getAttachedSubWin(WindowId::TrackerIntensityPlt);
  if (!TrackerIntensityPlt) { 
    TrackerIntensityPlt = mainw_->createAttachedSubWin( (plot = new OptimPlot(mainw_) ), WindowId::TrackerIntensityPlt);
  }
  else {
    plot = qobject_cast<OptimPlot*>( TrackerIntensityPlt->widget() );
    if (mainw_->CtSt_.ClearPlot) { 
      plot->clear();
    }
  }


  double xmin = 0.0;
  double xmax = 0.0;
  
  if(nturn_ ==1) {
     xmax = mainw_->Length_*.01;
     if( (view_at_elem_ > 0) && (view_at_elem_ < mainw_->nelm_) ) { 
        n =view_at_elem_ + 1;
     } 
     else {
        n = mainw_->nelm_+1;
     }
   }
   else {
      xmax = nturn_;
      n = nturn_+1;
   }
   
   double yminL = 0.0;
   double yminR = 0.0;

   double ymaxL = 1.1;
   double ymaxR = 2.*history_[0][0].sigmaDP();

   //s[0]=0;   s[1]=1;    // points to a scale (right or left)
   //s[2]=1;   s[3]=1;
   
   std::vector<double> xv(n);
   std::vector<double> intensity(n);
   std::vector<double> dpp(n);

   for(int i=0; i<n; ++i){
      xv[i] = (nturn_ ==1 ) ? history_[0][i].s : i;
      intensity[i] = history_[0][i].intensity;
      dpp[i]       = history_[0][i].sigmaDP();
   }


  PlotSpec plotspecs;
  plotspecs.title     = "Tracker: Beam intensity & Momentum spread";
  plotspecs.bottom_title = "S [m] or Turn No";

  auto& curvespecs = plotspecs.curvespecs;
  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  
  curvespecs.push_back({ "Intensity",    &xv[0],   &intensity[0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft,    "Intensity",    0     });
  curvespecs.push_back({ "dp/p",         &xv[0],   &dpp[0],       n, QwtSymbol::NoSymbol, QwtPlot::yRight,   "dp/p",         0     });

  plot->configure(plotspecs);
  plot->replot();
  plot->show();

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::cmdTrackerPlotPositions() // V7
{

  if (history_.size() == 0 ) {
      OptimMessageBox::warning(this, "Tracking", "There is no tracking history.", QMessageBox::Ok); 
      return;
  }

  char buf[50];

  static TrackPositionsDialog* dialog = 0;
  if ( !dialog) {
     dialog = new  TrackPositionsDialog(0);
     dialog->data_ = TrPlot_;
     dialog->set();
  }  
  
  if (dialog->exec() == QDialog::Rejected ) return;
  TrPlot_ =  dialog->data_;
  
  static const unsigned int  Xav = offsetof(BeamMoments, uavg[0]); // Xav());
  static const unsigned int PXav = offsetof(BeamMoments, uavg[1]); // PXav());
  static const unsigned int  Yav = offsetof(BeamMoments, uavg[2]); // Yav());
  static const unsigned int PYav = offsetof(BeamMoments, uavg[3]); // PYav());
  static const unsigned int  Sav = offsetof(BeamMoments, uavg[4]); // Sav());
  static const unsigned int PSav = offsetof(BeamMoments, uavg[5]); // PSav());
  static const unsigned int s    = offsetof(BeamMoments, s);
  static int const jmp[] = {  Xav, PXav,  Yav,  PYav,  Sav, PSav, s };

  // determine the offsets within the structure
  int i1=0, i2=0;  
  for(int i=0; i<7; ++i) {
    if(TrPlot_.X1[i]) i1 = i;   // X1 bool array for selecting what is plotted on ver axis 
    if(TrPlot_.X2[i]) i2 = i;   // X2 bool array for selecting what is plotted on hor axis 
  }

  static char const* nm[] = {"X[cm]", "Px[mrad]", "Y[cm]", "Py[mrad]", "S[cm]", "dP/P[mrad]", "Nturn", "s[m]"};

  if(nturn_>1 && view_at_elem_ ==-1) {
     char const* fnm = (p_elm_view_[0] == 0) ? "START" : mainw_->beamline_[p_elm_view_[0]]->fullName();
     sprintf(buf,"%s versus %s at Element %s ",
	     nm[i2], nm[i1], fnm);
  }
  else {
    // for a single turn, use distance along line instead of turn no. The axis label is changed. 
    sprintf(buf,"%s versus %s", nm[(i2==6 && nturn_==1) ? 7 : i2 ], nm[(i1==6 && nturn_==1) ? 7 : i1]);
  }

  OptimPlot*     plot   = 0;
  auto TrackerCentroidPlt = mainw_->getAttachedSubWin(WindowId::TrackerCentroidPlt ); 
  if (!TrackerCentroidPlt) { 
    TrackerCentroidPlt = mainw_->createAttachedSubWin( (plot = new OptimPlot(mainw_) ), WindowId::TrackerCentroidPlt);
  }
  else {
    plot = qobject_cast<OptimPlot*>( TrackerCentroidPlt->widget() );
    if (mainw_->CtSt_.ClearPlot) { 
      plot->clear();
    } 
  }
  TrackerCentroidPlt->raise();
  
  int n=0;
  if(nturn_ ==1) {
    if( view_at_elem_ >0 && view_at_elem_ < mainw_->nelm_) n = view_at_elem_+1;
    else n = mainw_->nelm_+1;
  }
  else {
    n = nturn_+1;
  };
   std::vector<double> xv(n);
   std::vector<double> yv(n);
   
   for(int i=0; i< n; ++i) {
     char* base  = reinterpret_cast<char*>(&history_[0][i]);
     xv[i] = *reinterpret_cast<double*>(base+jmp[i1]);
     yv[i] = *reinterpret_cast<double*>(base+jmp[i2]);
   }

  PlotSpec plotspecs;
  std::string title;
  title= std::string("Centroid Plot: ") +  nm[(i2==6 && nturn_==1) ? 7 : i2] + " vs "+ nm[(i1==6 && nturn_==1) ? 7 : i1];
  plotspecs.title        = title.c_str();
  plotspecs.bottom_title = nm[(i1==6 && nturn_==1) ? 7 : i1];
  std::string lgd (nm[(i2==6 && nturn_==1) ? 7 : i2]);
  lgd.erase(lgd.find('[')); // omit units in the legend.  

  auto& curvespecs = plotspecs.curvespecs;
  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             vertical_title    color            vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  
  curvespecs.push_back({ lgd.c_str(),    &xv[0],   &yv[0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft,  nm[(i2==6 && nturn_==1) ? 7 : i2], 0 });

  plot->configure(plotspecs);
  plot->replot();
  plot->show();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimTracker::cancel()
 {
   mainw_->interrupted_ = true;
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTracker::cmdPoincare()
{
  // enable poincare plot and track;
  // replace tracker plot with a poincare plot ?? 
  cmdTracking( true );
  trackAct_->setEnabled(false);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

