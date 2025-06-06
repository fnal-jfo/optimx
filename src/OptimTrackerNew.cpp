//  =================================================================
//
//  OptimTrackerNew.cpp
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
#include <fstream>
#include <sstream>
#include <omp.h>
#include <memory>
#include <spdlog/spdlog.h>

#include <Constants.h>
#include <BeamMoments.h>
#include <Globals.h>
#include <Element.h>
#include <Histogram.h>
#include <RMatrix.h>
#include <Cholesky.h>
//#include <OptimExceptions.h>
#include <OptimMainWindow.h>
#include <OptimMdiArea.h>
#include <OptimMdiSubWindow.h>
#include <OptimMessages.h>
#include <OptimPlot.h>
#include <OptimTextEditor.h>
#include <OptimTrackerNew.h>
#include <OptimUserRtti.h>
#include <RMatrix.h>
#include <ScatterData.h>
#include <ScatterPlotItem.h>
#include <Structs.h>
#include <SQLSeriesData.h>
//#include <Tracker3DSeriesData.h>
#include <TrackerParameters.h>
#include <TrackerPlot.h>
#include <TrackerPlot6.h>
#include <TrackerSaveDistributionDialog.h>
#include <TrackerDistributionDialogNew.h>
#include <TrackingParametersNewDialog.h>
#include <TrackParam.h>
#include <Twiss.h>
#include <Utility.h>
#include <JetColorMap.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QCheckBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPalette>
#include <QProgressDialog>
#include <QPushButton>
#include <QSignalMapper>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVector>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>
#include <QwtPlotCurve>
#include <QwtPlotSpectrogram>
#include <QwtMatrixRasterData>
#include <QwtLogScaleEngine>

#undef emit
#include <sqlite/execute.hpp>
#include <sqlite/private/private_accessor.hpp>
#include <sqlite/query.hpp>
#include <sqlite3.h>

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

OptimTrackerNew::OptimTrackerNew (QWidget* parent, Qt::WindowFlags flags) // V7
  :  QMainWindow(parent,flags), inputscatter_(0), outputscatter_(0),
     con_(Globals::preferences().con)
{

  int status = 0;
  //sqlite3_db_config(sqlite::private_accessor::get_handle(con_),SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION,1, &status);
  //  std::cerr <<  "sqlite3_db_config returns " << status  << std::endl; 
  BeamMoments::init_moments_table(*con_);

  setWindowFlags(Qt::WindowStaysOnTopHint);
  mainw_  = qobject_cast<OptimMainWindow*>( parent );
  setWindowTitle("OptiM Tracker");

  
     parametersAct_= new QAction( "Parameters",            this);
   distributionAct_= new QAction( "Distribution",          this);
           readAct_= new QAction( "Read Input Distribution",     this);
        saveOutAct_= new QAction( "Save Input Distribution",     this);
         saveInAct_= new QAction( "Save Output Distribution",    this);
    saveMomentsAct_= new QAction( "Save Moments",          this);
          inputAct_= new QAction( "Input",                 this);
         outputAct_= new QAction( "Output",                this);
eigenemittancesAct_= new QAction( "EigenEmittances",       this);
        momentsAct_= new QAction( "Moments",               this);
   correlationsAct_= new QAction( "Correlations",          this);
     latticeFunAct_= new QAction( "Lattice Functions",     this);
      centroidsAct_= new QAction( "Centroids",             this);
     dispersionAct_= new QAction( "Dispersion",            this);
      intensityAct_= new QAction( "Intensity",             this);
       trackNewAct_= new QAction( "Track",                 this);
       poincareAct_= new QAction( "Poincare Plot",         this);
  

  QFrame*       frame          = new QFrame();
  QHBoxLayout*  layout         = new QHBoxLayout(frame);   
  QPushButton*  track_button   = new QPushButton("Track");
  QCheckBox*    fast_checkbox  = new QCheckBox("Use Matrices");
  
  track_button->setEnabled(false);
  QPalette palette = QGuiApplication::palette();
  //QPalette palette(Qt::red, QPalette::Window);

  //palette.setBrush(QPalette::Disabled, QPalette::Window, QBrush(Qt::gray));
  palette.setColor(QPalette::Active,   QPalette::Button, Qt::red);
  //palette.setBrush(QPalette::Active,   QPalette::Window, QBrush(Qt::red));
  //palette.setColor(QPalette::Button, Qt::red);

  track_button->setPalette(palette);
  track_button->setAutoFillBackground(true);
			   
  layout->addWidget(fast_checkbox);
  layout->addWidget(track_button);
    
  menuBar()->setCornerWidget(frame, Qt::TopRightCorner);
  

    distributionAct_->setEnabled(false);
            readAct_->setEnabled(false);
         saveOutAct_->setEnabled(false);
          saveInAct_->setEnabled(false);
     saveMomentsAct_->setEnabled(false);
	   inputAct_->setEnabled(false);
	  outputAct_->setEnabled(false);
         momentsAct_->setEnabled(false);
    correlationsAct_->setEnabled(false);
 eigenemittancesAct_->setEnabled(false);
      latticeFunAct_->setEnabled(false);
      dispersionAct_->setEnabled(false);
       intensityAct_->setEnabled(false);
        trackNewAct_->setEnabled(true);
        poincareAct_->setEnabled(false);

	   inputAct_->setCheckable(true);
	  outputAct_->setCheckable(true);


     QActionGroup* input_output = new  QActionGroup(this);
     input_output->addAction(inputAct_);
     input_output->addAction(outputAct_);
     input_output->setExclusive(true);
    
  parametersMenu     =  menuBar()->addMenu(tr("Setup"));
  phaseSpaceViewMenu =  menuBar()->addMenu(tr("Views"));
  fileMenu           =  menuBar()->addMenu(tr("File"));
  plotsMenu          =  menuBar()->addMenu(tr("Plots"));

  fileMenu->addAction(readAct_);
  fileMenu->addAction(saveInAct_);
  fileMenu->addAction(saveOutAct_);
  fileMenu->addAction(saveMomentsAct_);

  phaseSpaceViewMenu->addAction( inputAct_);
  phaseSpaceViewMenu->addAction( outputAct_);
  phaseSpaceViewMenu->addSeparator();

  plotsMenu->addAction(eigenemittancesAct_);
  plotsMenu->addAction(momentsAct_);
  plotsMenu->addAction(centroidsAct_);
  plotsMenu->addAction(correlationsAct_);
  plotsMenu->addAction(latticeFunAct_);
  plotsMenu->addAction(dispersionAct_);
  plotsMenu->addAction(intensityAct_);

  parametersMenu->addAction(parametersAct_);
  parametersMenu->addAction(distributionAct_);
  menuBar()->addAction( poincareAct_ );

  track_button->setEnabled(true);
  fast_checkbox->setChecked(false);

  connect(track_button,        &QPushButton::clicked,    [this, track_button,  frame](){ if (trackNewAct_->isEnabled()) {
	                                                                                     //trackNewAct_->setEnabled(false);
											     trackNewAct_->trigger();
											     // trackNewAct_->setEnabled(true);
                                                                                          };
                                                                                       });
  connect(trackNewAct_,        &QAction::changed,        [this, track_button,  frame](){ track_button->setEnabled(trackNewAct_->isEnabled());} );
  connect(fast_checkbox,       &QCheckBox::checkStateChanged, [this, fast_checkbox, frame](int state){ TrackFast_ = (state ? true : false); });

  connect(trackNewAct_,        SIGNAL(triggered()), this, SLOT(cmdTrackingNew()) );
  connect(distributionAct_,    SIGNAL(triggered()), this, SLOT(cmdInitialize()) );
  connect(parametersAct_,      SIGNAL(triggered()), this, SLOT(cmdParameters()) );
  connect(saveOutAct_,         &QAction::triggered, [this](){ cmdSave(0);} );
  connect(saveInAct_,          &QAction::triggered, [this](){ cmdSave(1);} );
  connect(saveMomentsAct_,     &QAction::triggered, [this](){ cmdTrackerSaveMoments();} );
  connect(readAct_,            SIGNAL(triggered()), this, SLOT(cmdRead()) );
  connect(inputAct_,           SIGNAL(triggered()), this, SLOT(cmdInput())  ); 
  connect(outputAct_,          SIGNAL(triggered()), this, SLOT(cmdOutput()) ); 
  connect(eigenemittancesAct_, SIGNAL(triggered()), this, SLOT(cmdTrackerPlotEigenEmittances()) ); 
  connect(momentsAct_,         SIGNAL(triggered()), this, SLOT(cmdTrackerPlotMoments()) ); 
  connect(centroidsAct_,       SIGNAL(triggered()), this, SLOT(cmdTrackerPlotCentroids()) ); 
  connect(correlationsAct_,    SIGNAL(triggered()), this, SLOT(cmdTrackerPlotCorrelations()) ); 
  connect(latticeFunAct_,      SIGNAL(triggered()), this, SLOT(cmdTrackerPlotLatticeFunctions()) ); 
  connect(dispersionAct_,      SIGNAL(triggered()), this, SLOT(cmdTrackerPlotDispersion()) ); 
  connect(intensityAct_,       SIGNAL(triggered()), this, SLOT(cmdTrackerPlotIntensity()) ); 
  connect(poincareAct_,        SIGNAL(triggered()), this, SLOT(cmdPoincare()) );
 
  connect( mainw_,  SIGNAL(trackerPlotPositions()),    this,  SLOT(cmdTrackerPlotPositions())    ); 
  connect( mainw_,  SIGNAL(trackerSavePositions()),    this,  SLOT(cmdTrackerSavePositions())   ); 
  connect( mainw_,  SIGNAL(trackerPlotDistribution()), this,  SLOT(cmdTrackerPlotDistribution()) ); 

  plot6_ = new TrackerPlot6(0);

  setCentralWidget(plot6_);
  
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

   track_button->update();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimTrackerNew::~OptimTrackerNew ()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimTrackerNew::setInitCoordinates()
{
  Twiss4D v;
  int status = 0;
  if( (status = mainw_->Get4DBetasFromFile(false, false, v)) ) return status;
  status = setInitCoordinates(v);
  return status;  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimTrackerNew::setInitCoordinates(Twiss4D const& v) // V7
{
  using namespace boost::histogram;
  
  auto optimx_logger = spdlog::get("optimx_logger");
 
  // construct the moment matrix from the coupled 4D lattice functions

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

   bool bunch_init_uses_eigenvectors = false;  // false => use CHOLESKI decomposition of Moments matrix 
   
   RMatrix_t<4,double> L;

   if ( bunch_init_uses_eigenvectors) { 

     SPDLOG_LOGGER_INFO( optimx_logger, "Generating initial distribution using spectral decomposition.");

     // using the spectral decomposition. The real eigenvectors are given by (3.13)
     // in Lebedev & Bogcaz FERMILAB-PUB-10-383-AD
     // NOTE: in this case the matrix L is NEITHER symmetric NOR lower triangular

   
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
   }
   else {  

     // using the Cholesky factorization of the 2nd order moments matrix.
     // The 2nd order moment matrix is given in Appendix A, (p21)
     // in Lebedev & Bogcaz FERMILAB-PUB-10-383-AD

     SPDLOG_LOGGER_INFO( optimx_logger, "Generating initial distribution using Cholesky decomposition.");
   

     SymMatrix_t<4,double> M;

     double term = 0.0; // dummy term
     /*11*/ M[0][0] =   v.e1*v.btx1 + v.e2*v.btx2;

     /*21*/ M[1][0] =  -v.e1*v.alfx1 - v.e2*v.alfx2;

     /*22*/ M[1][1]  = (abs(v.btx1) > 1.0e-12 ? v.e1*((1-v.u)*(1-v.u) + v.alfx1*v.alfx1)/v.btx1: 0.0);  
            M[1][1] += (abs(v.btx2) > 1.0e-12 ? v.e2*(v.u*v.u* + v.alfx2*v.alfx2)/v.btx2 :  0.0);   

     /*31*/ M[2][0] =   v.e1*sqrt(v.btx1*v.bty1)*cos(nu1) + v.e2*sqrt(v.btx2*v.bty2)*cos(nu2);

     /*32*/ M[2][1]  =  -v.e1*sqrt(v.bty1/v.btx1)*( (1-v.u)*sin(nu1) + v.alfx1*cos(nu1) );
            M[2][1] +=  (abs(v.btx2) > 1.0e-12 ? v.e2*sqrt(v.bty2/v.btx2)*( v.u*sin(nu2) - v.alfx2*cos(nu2)) : 0.0);

     /*33*/ M[2][2] =   v.e1*v.bty1 + v.e2*v.bty2;

     /*41*/ M[3][0]  =  (abs(v.bty1) > 1.0e-12 ?   v.e1*sqrt(v.btx1/v.bty1)*(    v.u*sin(nu1) - v.alfy1*cos(nu1))   : 0.0);
            M[3][0] +=  (abs(v.bty2) > 1.0e-12 ?	- v.e2*sqrt(v.btx2/v.bty2)*((1-v.u)*sin(nu2) + v.alfy2*cos(nu2))   : 0.0);

     /*42*/ M[3][1]  =  (abs(v.btx1*v.bty1) > 1.0e-12 ? v.e1*( (v.alfy1*(1-v.u)-v.alfx1*v.u)*sin(nu1)
 							      + (v.u*(1-v.u) + v.alfx1*v.alfy1)*cos(nu1) )/sqrt(v.btx1*v.bty1) : 0.0 ); 
            M[3][1] +=  (abs(v.btx2*v.bty2) > 1.0e-12 ? v.e2*( (v.alfx2*(1-v.u)-v.alfy2*v.u)*sin(nu2)
							      + (v.u*(1-v.u) + v.alfx2*v.alfy2)*cos(nu2) )/sqrt(v.btx2*v.bty2) : 0.0 );			  

     /*43*/ M[3][2]  = -v.e1*v.alfy1 - v.e2*v.alfy2;

     /*44*/ M[3][3]   =   (abs(v.bty1) > 1.0e-12 ? v.e1*(v.u*v.u + v.alfy1*v.alfy1)/v.bty1         : 0.0 );
            M[3][3]  +=   (abs(v.bty2) > 1.0e-12 ? v.e2*((1-v.u)*(1-v.u) + v.alfy2*v.alfy2)/v.bty2 : 0.0 );


     auto Ltmp = cholesky(M); // NOTE: here the matrix M is symmetric and L is lower triangular
     for (int i=0;i<4;++i) {
      for (int j=0;j<4;++j) {
   	 L[i][j]=Ltmp[i][j];
       }
     }  
   }

   SPDLOG_LOGGER_INFO( optimx_logger, fmt::format("beta_x1 [specified] = {:12.5g}", v.btx1) );
   SPDLOG_LOGGER_INFO( optimx_logger, fmt::format("beta_y1 [specified] = {:12.5g}", v.bty1) );
   SPDLOG_LOGGER_INFO( optimx_logger, fmt::format("beta_x2 [specified] = {:12.5g}", v.btx2) );
   SPDLOG_LOGGER_INFO( optimx_logger, fmt::format("beta_y2 [specified] = {:12.5g}", v.bty2) );
   

  double vi[6];

  int k=0;
  while (k<N_) {
    double s = 0.0;
    for(int i=0; i<6; ++i) {
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
	vin_[k][i] += L[i][j]*vi[j];
	vin_[k].lost = 0;
      }
    }

    // generate long distribution
    
    vin_[k][4] = (sqrt(1.0-alphaL_*alphaL_)*vi[4] + vi[5]*alphaL_)*sigmaS_; // [sqrt(1-a_L^2) s + a_L*dpp] sigma_s 
    vin_[k][5] = vi[5]*sigmaP_;

    // add dispersion contribution to transverse coordinates
    
    vin_[k][0] += v.dx  * vi[5]*sigmaP_;
    vin_[k][1] += v.dxp * vi[5]*sigmaP_;
    vin_[k][2] += v.dy  * vi[5]*sigmaP_;
    vin_[k][3] += v.dyp * vi[5]*sigmaP_;
    
    ++k;
  } // while

   auto scatterdata = std::make_shared<TrackingScatterData>(&vin_[0], N_, ViewType::input);
   inputscatter_ = new ScatterPlotItem( scatterdata);
   plot6_->setData(*scatterdata);

  
   /**
   // function to determine the min/max valuesfor each coordinates. 
   // arg  0 <= i <= 5  i=0 is x i=1 is x' etc ...  

   auto limits = [ this](int i) { 
           double xmin = (*std::min_element(&vin_[0], &vin_[N_], [&i](auto const& lhs, auto const&  rhs){ return lhs[i]<rhs[i];}))[i];
           double xmax = (*std::max_element(&vin_[0], &vin_[N_], [&i](auto const& lhs, auto const&  rhs){ return lhs[i]<rhs[i];}))[i];
           return std::make_pair(xmin,xmax);
   };
   
   auto [xmin,     xmax]  = limits(0);     
   auto [xpmin,   xpmax]  = limits(1);     
   auto [ymin,     ymax]  = limits(2);     
   auto [ypmin,   ypmax]  = limits(3);     
   auto [smin,     smax]  = limits(4);     
   auto [dppmin, dppmax]  = limits(5);     

   double zmin = 1.0e-5;
   double zmax = 1.0;

   int nx = 100;
   int ny = 100;
   
   Histogram2D  hxy(nx, xmin, xmax, ny, ymin, ymax); 
  
   // assign points to the histogram 
   for (int i=0; i<N_; ++i) {
     hxy(vin_[i][0], vin_[i][2]);
   }

   // determine the max bin size 
   double vmax = double(hxy.max());
  
   // copy histo data into a vector
   QVector<double> vxy;

   for (auto&& x : hxy.idxrange()) {
    auto const i = x.index(0); // current index along first axis
    auto const j = x.index(1); // current index along second axis
    //auto const b0 = x.bin(0);  // current bin interval along first axis
    //auto const b1 = x.bin(1);  // current bin interval along second axis
    auto const v = *x;         // "dereference" to get the bin value
    vxy.push_back( double(hxy.at(i,j))/vmax);
   };

   **/

   /*
   for (int i=0; i<nx; ++i) {
     for (int j=0; j<ny; ++j) {
       vxy.push_back( double(hxy.at(i,j))/vmax);
     }
   };
   */

   /*
   QwtMatrixRasterData* mraster = new QwtMatrixRasterData();
   mraster->setValueMatrix(vxy, ny);

   mraster->setInterval(Qt::XAxis, QwtInterval(xmin, xmax));
   mraster->setInterval(Qt::YAxis, QwtInterval(ymin, ymax));
   mraster->setInterval(Qt::ZAxis, QwtInterval(zmin, zmax));
   QwtInterval zInterval = QwtInterval(zmin, zmax);
   mraster->setResampleMode(QwtMatrixRasterData::BilinearInterpolation);

   QwtPlotSpectrogram* spectro = new  QwtPlotSpectrogram();
   spectro->setData(mraster);
   spectro->setColorMap(new JetLogColorMap());
   QwtPlot* plot = new QwtPlot(0);
   plot->setAxisScaleEngine(QwtPlot::yRight, new QwtLogScaleEngine());
   plot->setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
   plot->setAxisVisible(QwtPlot::yRight );
   
   spectro->attach(plot);
   plot->show();
   plot->replot();
   
   cmdInput();
   plot_->replot();
   
   double ms     = mainw_->ms;
   double tetaY  = mainw_->tetaYo0_;
   double Enr    = mainw_->Ein;
   double gamma  = 1.0+Enr/ms;
   BeamMoments mom(gamma, vin_, N_, false);
   */
   return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimTrackerNew::setInitCoordinatesDelta()
{   

  double vi[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  int k=0;
  while (k<N_) {
    for(int i=0; i<4; i++) {
      vin_[k][i] = 0.0;
      for(int j=0; j<4; ++j) {
	vin_[k][i] = vi[j];
	vin_[k].lost = 0;
      }
    }

    // generate long distribution
    
    vin_[k][4] = vi[4];
    vin_[k][5] = vi[5];

    // add dispersion contribution to transverse coordinates
    
    ++k;
  } // while

   auto scatterdata = std::make_shared<TrackingScatterData>(&vin_[0], N_, ViewType::input);
   inputscatter_ = new ScatterPlotItem(scatterdata);
   plot6_->setData(*scatterdata);

   cmdInput();

   double ms     = mainw_->ms;
   double tetaY  = mainw_->tetaYo0_;
   double Enr    = mainw_->Ein;
   double gamma  = 1.0+Enr/ms;
   BeamMoments mom(gamma, vin_, N_, false);
   
   return 0;

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTrackerNew::cmdParameters() 
{

  TrackerParameters st;
  TrackingParametersNewDialog* dialog = 0;
  
  if (!dialog) {
    dialog = new TrackingParametersNewDialog(0);
  }

  st.nturns         = nturn_;
  st.ielm            = view_at_elem_;
  strcpy(st.Filter, TrackFilter_);
  st.FastTracking    = TrackFast_;
  st.PrintResults    = PrintResults_;
  st.IncrementTurns  = IncrementTurns_;
  st.MatchCase       = MatchCase_;
  st.dataspec        = TrackerParameters::all;

  dialog->set(st);  

  if( dialog->exec() == QDialog::Rejected) return;
  st =  dialog->data_;
  
  nturn_         = st.nturns;			
  view_at_elem_  = st.ielm-1; // internally, element indices start at 0 
  strcpy(TrackFilter_, st.Filter);
  TrackFast_     = st.FastTracking;
  PrintResults_  = st.PrintResults;
  IncrementTurns_= st.IncrementTurns;
  MatchCase_     = st.MatchCase;
  dataspec_      = st.dataspec;         

  if(nturn_<1) {nturn_=1;}

  // find Elements to be filtered

  //if (nturn_>1) {
      
  switch (dataspec_) {      

  case TrackerParameters::selection:
      for(int i=0; i< mainw_->nelm_; ++i) {
           char const* fnm = mainw_->beamline_[i]->fullName();
           if( filterName(fnm, TrackFilter_, MatchCase_)) {elm_selection_[i] =  std::string(fnm);};
       }
       if( elm_selection_.size() == 0 ){
         OptimMessageBox::warning(this, "Tracker Error", "Element selection is empty.", QMessageBox::Ok);
         return;
       }  
       else {
	 // elm_selection is a std::map
         // show a table of selected elements 
	 elm_selection_.display();
 	 OptimInformationMessage(this, "Tracker ", (QString().setNum(elm_selection_.size()) + QString(" elements selected.")).toUtf8().data(), QMessageBox::Ok);
       }
       break;
  case TrackerParameters::specific:     
       { char const* fnm = mainw_->beamline_[view_at_elem_]->fullName();
         elm_selection_[view_at_elem_] =  std::string(fnm);
	 elm_selection_.display();
         dataspec_ = TrackerParameters::selection;  // TREAT specific as selection in what follows
       }
       break;
  case TrackerParameters::all :
  default:
       break;
  };
  //}

   TotalTurnsTracked_ = 0;

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

  BeamMoments::init_moments_table(*Globals::preferences().con);

    distributionAct_->setEnabled(true);
            readAct_->setEnabled(true);
         saveOutAct_->setEnabled(false);
          saveInAct_->setEnabled(false);
     saveMomentsAct_->setEnabled(false);
	   inputAct_->setEnabled(false);
 eigenemittancesAct_->setEnabled(false);
         momentsAct_->setEnabled(false);
    correlationsAct_->setEnabled(false);
      latticeFunAct_->setEnabled(false);
      dispersionAct_->setEnabled(false);
       intensityAct_->setEnabled(false);
        trackNewAct_->setEnabled(false);
        poincareAct_->setEnabled(false);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTrackerNew::cmdInitialize() // V7
{
   TrackChStrNew st;
   BunchParam bunch;

    Twiss4D v;

    if(mainw_->Get4DBetasFromFile(false, false, v)) return;
   
    if(mainw_->GetSpaceChargeParam(false, bunch)) return;

     sigmaS_ = bunch.L;
     sigmaP_ = bunch.Dp;
     alphaL_ = bunch.alpha;
     DltS_  =  bunch.sOffest;

     if (alphaL_ >  1.0)  alphaL_ =  0.9999;
     if (alphaL_ < -1.0)  alphaL_ = -0.9999;


   static TrackerDistributionDialogNew* dialog = 0;
   
   if (!dialog) {
     dialog = new TrackerDistributionDialogNew(0);
   }

   st.Npart = N_;
   st.Rmin  = rmin_;
   st.Rmax  = rmax_;

   st.bx1   = v.btx1; 
   st.ax1   = v.alfx1; 
   st.bx2   = v.btx2; 
   st.ax2   = v.alfx2; 

   st.by1   = v.bty1; 
   st.ay1   = v.alfy1; 
   st.by2   = v.bty2; 
   st.ay2   = v.alfy2; 

   st.u      = v.u; 
   st.eps1   = v.e1; 
   st.eps2   = v.e2; 

   st.eta1   = v.dx; 
   st.eta1p  = v.dxp; 
   st.eta2   = v.dy; 
   st.eta2p  = v.dyp;

   st.sigma_s  = sigmaS_;
   st.sigma_p  = sigmaP_;
   st.alpha_l  = alphaL_;
   st.current  = bunch.I;
   st.deltafunction = false;
   
   dialog->data_ = st;
   dialog->set();
 
   bool done = true;
   do{
       done = true;
       if (dialog->exec() != QDialog::Accepted) return;
       st = dialog->data_;    
       sigmaS_ = st.sigma_s;
       sigmaP_ = st.sigma_p;
       alphaL_ = st.alpha_l;
       // DltS_  = st . ???
 
       N_     = st.Npart;
       rmin_  = st.Rmin;
       rmax_  = st.Rmax;

       if (rmax_>6.0)         {rmax_ = 6.;         done = false;}
       if (rmax_<0.5)         {rmax_ = 0.5;        done = false;}
       if (rmin_<0.0)         {rmin_ = 0.;         done = false;}
       if ((rmax_-rmin_)<0.5) {rmin_= rmax_-0.5;   done = false;}
       
       if(!done) OptimMessageBox::warning(this, "Error in parameters", "Rmax and Rmin are out of range", QMessageBox::Ok);

   } while(!done);

   if(mainw_->interrupted_){ mainw_->interrupted_= false;  return; }
   if(mainw_->analyze( !mainw_->analyzed_, 1) ) return; // first arg true : parse parameters 
 
   vin_.resize(N_);
   vfin_.resize(N_);

   TotalTurnsTracked_ = 0;
   
   if (!st.deltafunction ) {
    if( setInitCoordinates(v) ) { // initialization failed. Cleanup and return;
       vin_.resize(0);
       vfin_.resize(0);
       return;
    }
   }
   else {
      setInitCoordinatesDelta();
   }


   for(int i=0; i<N_; ++i) { // initialize pid with a default value. 
     auto& particle = vin_[i]; 
      particle.lost           = 0;  
      particle.pid            = i;  
      particle.weight         = 1.0; 
   }

    distributionAct_->setEnabled(true);
            readAct_->setEnabled(true);
         saveOutAct_->setEnabled(true);
          saveInAct_->setEnabled(true);
     saveMomentsAct_->setEnabled(true);
	   inputAct_->setEnabled(true);
        trackNewAct_->setEnabled(true);
        poincareAct_->setEnabled(true);
 eigenemittancesAct_->setEnabled(false);
         momentsAct_->setEnabled(false);
    correlationsAct_->setEnabled(false);
      latticeFunAct_->setEnabled(false);
      dispersionAct_->setEnabled(false);
       intensityAct_->setEnabled(false);

   view_elem_=0;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTrackerNew::cmdRead()
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
     auto& particle = vin_[i];
     if(buf[0]=='#') continue;
     if ( int k = decodeExtLine(buf, dat, ncols)  ){
       for(int j=0; j<6; ++j){ 
         particle[j] = dat[j]+dx[j] + dsx[j]*gauss(); 
       }
       particle.pid = (k>=7) ? dat[6]                   : i;
       particle.weight       = (k>=8) ? dat[7]          : 1.0;
       particle.lost         = (k>=ncols) ? int(dat[8]) : 0;
       ++i;
     }
   }

   view_elem_ = 0;

   auto scatterdata = std::make_shared<TrackingScatterData>(&vin_[0], N_, ViewType::input);
   inputscatter_ = new ScatterPlotItem(scatterdata);
   plot6_->setData(*scatterdata);

   cmdInput(); // switch to input view

    distributionAct_->setEnabled(true);
            readAct_->setEnabled(true);
         saveOutAct_->setEnabled(true);
          saveInAct_->setEnabled(true);
     saveMomentsAct_->setEnabled(true);
	   inputAct_->setEnabled(true);
	  outputAct_->setEnabled(false);
 eigenemittancesAct_->setEnabled(false);
         momentsAct_->setEnabled(false);
    correlationsAct_->setEnabled(false);
      latticeFunAct_->setEnabled(false);
       intensityAct_->setEnabled(false);
        trackNewAct_->setEnabled(true);
        poincareAct_->setEnabled(true);

   return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTrackerNew::cmdSave(int inout)
{

  // inout = 0 : save input dist 
  // inout ! 0 : save output dist 

  TrackerSaveDistributionDialog* dialog = 0;
  if (!dialog) {
    dialog = new TrackerSaveDistributionDialog(this);
  }  

  if ( dialog->exec() == QDialog::Rejected ) return;

  std::fstream fs;

  QString FileName = QFileDialog::getSaveFileName ( 0, "Save Tracking Data", ".");
  if (FileName == "" ) return; 
  
  fs.open( FileName.toUtf8().data(),  std::ios_base::out | std::ios_base::trunc );
  if (fs.fail()) {
     OptimMessageBox::warning(this, "Error opening file - ", FileName.toUtf8().data(), QMessageBox::Ok);  
    return;
  }    

  fmt::print(fs,"OptiM Track Data\n");
    
  static std::string const info_all = fmt::format("#{:>15s} {:>16s} {:>16s} {:>16s} {:>16s} {:>16s} {:>8s} {:>8s} {:>8s} {:>8s} {:>8s}\n",
					    "x[cm]",  "x'",  "y[cm]",  "y'",  "s[cm]",  "dp/p",  "label",  "weight",  "code",  "elm",  "pass");  
  static std::string const info     =  fmt::format("# {:>15s} {:>16s} {:>16s} {:>16s} {:>16s} {:>16s} {:>8s} {:>8s}\n",
					    "x[cm]",  "x'",  "y[cm]",  "y'",  "s[cm]",  "dp/p",  "label",  "weight");  


  fmt::print(fs, "{:s}", (dialog->data_.saveall ? info_all: info));
  
  Bunch& v = inout ? vfin_ : vin_;

  for( int i=0; i<N_; ++i) {
    if ((!dialog->data_.saveall) && (v[i].lost != 0) ) continue;
    for(int j=0; j<6; ++j) {
      fmt::print(fs,"{:16.5g} ",v[i][j]);
     }
     fmt::print(fs,"{:8d} {:8g}", v[i].pid, v[i].weight);

     if (dialog->data_.saveall) {
       fmt::print(fs,"{:8d} {:8d} {:8d}", v[i].lost, v[i].nelem, v[i].npass);
     }

     fmt::print(fs,"\n");
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimTrackerNew::trackBunchExact(Element const* ep, double Enr0,  RMatrix_t<3>& frame,
				Bunch& v, int N, int n_turn, int n_elem) // V7
{

  //std::cout << " OptimTrackerNew::trackBunchExact element: " << ep->name() << std::endl;
  TrackParam prm;
  double enr;
  int    rt = 0;
  static char const *msg[]={
    "Bunch contains only one particle !",
    "All particles have the same longitudinal coordinate.",
    "Spline error for original wake computation (err 3).",
    "Spline error for fitted wake computation (err 4).",
    "Spline error for fitted wake computation (err 5).",
    "Particle is outside the range of the wake data provided in the file."
  };
  
  if( toupper(ep->name()[0])=='Y') {  
    // wake field
    rt = trackWake(ep, v, N, Enr0, mainw_->ms, &(mainw_->ext_dat[ep->N]));
    if(rt) {
      OptimMessageBox::warning(this, "Tracking err. in wake elm.", msg[rt-1], QMessageBox::Ok);
      	return 1;
    }
    return 0;
  }

  RMatrix m1;
  m1.toUnity();
  ep->preTrack(frame, mainw_->ms, Enr0, n_elem, prm, m1);

 
int kloop = 0;
int kcond = 0;

  #pragma omp parallel for if( parallel_tracking_) 
  for(int j=0; j<N; ++j) {

    enr = Enr0;
    if (v[j].lost != 0 ) continue; // do not track lost particles. 

    ep->trackOnce(mainw_->ms, enr, n_elem, n_turn,  prm,  m1, v[j] );
  }

  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTrackerNew::cmdTrackingNew( bool poincare) // V7
{

  parallel_tracking_  = Globals::preferences().parallel_tracking;

  if (mainw_->interrupted_ ) { mainw_->interrupted_ = false; return;}
  if (mainw_->analyze(!mainw_->analyzed_,1)) return;

  double phi    = mainw_->tetaYo0_ * (PI/180.0);
  double teta   = mainw_->tetaXo0_ * (PI/180.0);

  double sp = sin(phi);
  double cp = cos(phi);

  double st = sin(teta);
  double ct = cos(teta);
  
  RMatrix_t<3> frame = {{ ct, 0.0, st}, { 0.0, 1.0, 0.0}, { -st, 0.0, ct}};
  
  poincareAct_->setEnabled(poincare);

  // clear all existing scatter plot data 
  plot6_->clear();

  // input data
  auto scatterdata = std::make_shared<TrackingScatterData>(&vin_[0], N_, ViewType::input);
  inputscatter_ = new ScatterPlotItem(scatterdata);
  plot6_->setData(*scatterdata);

  outputscatter_ = 0;

   
  RMatrix me;
  RMatrix tm;
  Twiss twiss;

  double EnrNew, dtx, dty, capa, dPdS, dSdP, dalfa;
  char nm, buf[LSTR+1];
  int i, ie;

  std::complex<double> ev[4][4];
  int nline;

  // Print tracking status update to the text window
  // Note: the output window is created by the dialog only if requested 

  auto TrackOutCh = mainw_->getAttachedSubWin(WindowId::TrackOutCh);

  OptimTextEditor* editor = 0;
  if (PrintResults_) { 
    if (!TrackOutCh) {
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


  //int Nelm_f = (view_at_elem_ >= 0 ) ?  view_at_elem_ : mainw_->nelm_;
  //int Nelm_f =   elm_selection_.(view_at_elem_ >= 0 ) ?  elm_selection_->rbegin()->first : mainw_->nelm_;

  double ms     = mainw_->ms;
  double tetaY  = mainw_->tetaYo0_;
  double Enr    = mainw_->Ein;
  double gamma  = 1.0+Enr/ms;
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
    for( int i=0; i<N_; ++i) { vfin_[i] = vin_[i];}
  }  

   Bunch& v = vfin_;

   twiss.BtX  = mainw_->BetaXin;        twiss.BtY  = mainw_->BetaYin;
   twiss.AlX  = mainw_->AlfaXin;        twiss.AlY  = mainw_->AlfaYin;
   twiss.DsX  = mainw_->DispXin;        twiss.DsY  = mainw_->DispYin;
   twiss.DsXp = mainw_->DispPrimeXin;   twiss.DsYp = mainw_->DispPrimeYin;

   if( mainw_->CtSt_.IsRingCh ){
     mainw_->findRMatrix(tm);
     i = find_tunes(tm, 100., twiss, &dalfa);
     if(i) {
       OptimMessageBox::warning(this, "Close Error", "Cannot close for X or Y", QMessageBox::Ok);
       return;
     }
   }
  
   twiss.nuX = 0.0;			 
   twiss.nuY = 0.0;
   
   twiss.eigenvectors(ev);
   
   sqlite::execute(*con_, "DELETE FROM Moments WHERE rowid IN (SELECT max(rowid) FROM moments);", true); 

  progress_bar_ = new QProgressDialog(0);
  progress_bar_->setWindowModality(Qt::ApplicationModal);
  connect ( progress_bar_, SIGNAL( canceled()), this, SLOT( cancel()) ) ;  
  progress_bar_->resize(400, 150);
  progress_bar_->setWindowTitle("Tracking Progress");
  progress_bar_->setLabelText(QString("Tracking %1 particles %2 time(s) through the lattice.\n Using %3 thread(s).").arg(N_).arg(nturn_).arg( omp_get_max_threads() ));
  progress_bar_->setMinimumDuration(4000); 

  omp_set_dynamic(0);       // Explicitly disable dynamic teams
  // omp_set_num_threads(4); 

  int kin = 0;
  
  for(int k=kin; k<nturn_; ++k) {

     ie = 0;
     double spos = 0.0; // position around the ring

     if(dataspec_ == TrackerParameters::all) {  // INITIAL CONDITION FOR OUTPUT AT ALL ELEMENTS 
           BeamMoments mom(gamma,v, N_, parallel_tracking_);
	   mom.s = spos;
           mom.dbWrite(*con_, TotalTurnsTracked_, 0, N_); // NOTE: i is the element index
     }
	   
     for(int i=0; i <mainw_->nelm_; ++i) { 

       auto ep     = mainw_->beamline_[i];
       nm     = toupper(ep->name()[0]);
       dSdP   = ep->length()/(gamma*gamma);
       EnrNew = Enr;
       
       // compute transfer matrix (for fast tracking) 
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
            if( TrackFast_ ) { me = ep->rmatrix(Enr, ms, tetaY, 0.0, 3); };  // matrix is needed only for fast tracking 
            break;
       }
       
       //*** ep->propagateLatticeFunctions(me, twiss, ev); // Why is twiss required ??? is it used ???
                                                           // is this an incomplete attempt at modeling space charge ??  

       if( TrackFast_ ) { // TrackFast_ == Track using transfer matrices. Also include correctors
	                  // and update energy when going through accelerating elements.    
	     trackBunch(nm, Hrt, ep.get(), v, me, N_, k+1, i, dPdS, dPdE, capa, dtx, dty, dSdP);
       }
       else {
	   // NOTE: TrackFast_==false
	   if(trackBunchExact(ep.get(), Enr, frame, v, N_, k+1, i)) { mainw_->interrupted_ = true; return; }
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

  	spos += ep->length()*0.01;

	 //     if(nturn_==1)
        if(dataspec_ == TrackerParameters::all) {  //OUTPUT AT ALL ELEMENTS
           BeamMoments mom(gamma,v, N_, parallel_tracking_);
	   mom.s = spos;
           mom.dbWrite(*con_, TotalTurnsTracked_, i+1, N_); // NOTE: i is the element index
        }
        else {
	  if( elm_selection_.find(i) != elm_selection_.end()) { 
	    // (view_at_elem_>0)&&(view_at_elem_== i)) { // OUTPUT AT SPECIFIC ELEMENT(S)
             BeamMoments mom(gamma,v, N_, parallel_tracking_);
	     mom.s = spos;
	     mom.dbWrite(*con_, TotalTurnsTracked_, i, N_);
	     v.lossProfile();
	  }   
        }
       
	if((k==nturn_-1) && (i== mainw_->nelm_-1)) break;
       
        if (mainw_->interrupted_) {
	   mainw_->interrupted_ = OptimQuestionMessage(this,  "Tracking", "Do you want to interrupt tracking  ?",
						       QMessageBox::Yes| QMessageBox::No) == QMessageBox::Yes;
	   break;
        }
  
        progress_bar_->setValue(int( double( (i+1) + (k *mainw_->nelm_) )/double(mainw_->nelm_*nturn_)  * 100.0 ));  
        QCoreApplication::processEvents();


     } // elements loop

     if (mainw_->interrupted_) break; //  interrupted in the inner loop, we need to break out of the outer loop.
 
     TotalTurnsTracked_++;

     //     if( !averaged_ && (view_at_elem_ == -1 ) ){
     //     BeamMoments mom(gamma,v, N_, parallel_tracking_);
     //     mom.dbWrite(*con_, TotalTurnsTracked_, 0, N_);
     //}

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


     
     if( IncrementTurns_){ 
        if(mainw_->analyze( false,k+2) ) return;
     }


     //........................................................
     // In Poincare mode, store turn-by-turn state coordinates

     if (poincare) { 
        for (int i=0; i< N_; ++i ) { 
	   fmt::print(fpu_.get(),"{:12.6g} {:12.6g} {:12.6g} {:12.6g} {:12.6g} {:12.6g} {:12d} \n",
	  	                      v[i].c[0],v[i].c[1],
		                      v[i].c[2],v[i].c[3],
		                      v[i].c[4],v[i].c[5], i);
	};
      } // if poincare
  }; // for int k=nturns ... 


  //....................................................

   progress_bar_->setValue(100);

   view_elem_=1;

   if (poincare) {
     auto scatterdata = std::shared_ptr<PoincareScatterData>( new PoincareScatterData(fpu_, ViewType::output));
       outputscatter_ = new ScatterPlotItem(scatterdata);
       plot6_->setData(*scatterdata);
   }
   else {
     auto scatterdata = std::shared_ptr<TrackingScatterData>( new TrackingScatterData(&vfin_[0], N_, ViewType::output));
       outputscatter_ = new ScatterPlotItem(scatterdata);
       plot6_->setData(*scatterdata);
   }

   std::string sbuf;
   if(PrintResults_){
     QTextCursor cursor = editor->textCursor();
     cursor.select(QTextCursor::LineUnderCursor);
     cursor.removeSelectedText();
     //i = (nturn_==1) ? Nelm_f : (TotalTurnsTracked_-1);
     editor->insertPlainText(fmt::format("{:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s}\n", "EmitX_N[cm]", "EmitY_N[cm]", "Xmax[cm]", "Ymax[cm]", "dp/p", "Intensity", "Lost").c_str());
     BeamMoments mom(gamma,v, N_, parallel_tracking_);
     sbuf += fmt::format("{:12.4g} {:12.4g} {:12.4f} {:12.4f} {:12.4g} {:12.4f} {:>12d}\n", mom.eps[0]*mom.bta*mom.gma, mom.eps[1]*mom.bta*mom.gma,
			 mom.umax[0], mom.umax[1],  sqrt(mom.cov[5][5]), mom.intensity, mom.nlost);
     editor->insertPlainText(sbuf.c_str());
     sprintf(buf, "Total Turns Tracked %d\n", TotalTurnsTracked_);
     editor->insertPlainText(buf);

     editor->document()->setModified(false);
    }

    cmdOutput(); // switch to output view
    
    distributionAct_->setEnabled(true);
            readAct_->setEnabled(true);
         saveOutAct_->setEnabled(true);
          saveInAct_->setEnabled(true);
     saveMomentsAct_->setEnabled(true);
	   inputAct_->setEnabled(true);
	  outputAct_->setEnabled(true);
         momentsAct_->setEnabled(true);
    correlationsAct_->setEnabled(true);
 eigenemittancesAct_->setEnabled(true);
      latticeFunAct_->setEnabled(true);
      dispersionAct_->setEnabled(true);
       intensityAct_->setEnabled(true);

    mainw_->mdiArea->setActiveSubWindow(mainw_->Tracker_.data());;    

    mainw_->interrupted_  =  false;
    Q_EMIT( tracking_done((QPrivateSignal())) );
    
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTrackerNew::cmdSwitchPlane( PlaneType plane)
{
  /*
  plane_ = plane;

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
  plot6_->replot();
  
  setWindowFlags(windowFlags()| Qt::WindowStaysOnTopHint);
  show();
  */
  return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTrackerNew::cmdInput()
{

  setWindowTitle( "Optim Tracker: Input Distribution"); 
  if (outputscatter_) outputscatter_->hide();
  inputscatter_->show();
  plot6_->setView(ViewType::input);
  inputAct_->setChecked(true);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTrackerNew::cmdOutput()
{
  setWindowTitle( "Optim Tracker: Output Distribution"); 
  if (outputscatter_) outputscatter_->show();
  plot6_->setView(ViewType::output);
  outputAct_->setChecked(true);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTrackerNew::cmdSwitchPlane( int view)
{
  cmdSwitchPlane( static_cast<PlaneType>(view) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTrackerNew::trackBunch(char nm, double Hrt, Element const *ep,
	Bunch& v, RMatrix const& me, int N, int k, int i,
	double dPdS, double dPdE, double capa, double dtx, double dty, double dSdP) // V7
{
  double x, y, s, c;
  
  for(int j=0; j<N_; ++j) {

    auto& particle = v[j];
    
    int ret = 0;

    if (! j%1000 == 0) { QCoreApplication::processEvents(); }

    //if(loss_[j].lost !=0 ) continue;
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
          particle[0] += ep->length() * (particle[1] + 0.5 *dtx);
          particle[1] += dtx;
          particle[2] += ep->length() * (particle[3] + 0.5 *dty);
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
              if( (fabs(2.*x*y)>(ep->S*ep->S)) || (x*x+y*y>ep->A*ep->A)) ret=1;
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

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimTrackerNew::trackWake(Element const* ep, Bunch& v, int N, double Enr0, double ms, ExtData* p) // V7
{
  if ( N_<2 ) return 1; // only one particle in the beam
  
  double P0 = sqrt(2.*ms*Enr0+Enr0*Enr0)*1e6;

  // Find min and max s
  double smin = v[0][4];
  double smax = smin;

  for(int i=0; i<N_; ++i){
     auto& particle = v[i]; 
     if(smin > particle[4]) smin = particle[4];
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
     auto& particle = v[i];  
     for( int n=0; n<nb; ++n) {
        x = particle[4] - sb[n];
        if(x<0.0) continue;
        if(splint( &p->x[0], &p->y[0], &p->v[0], p->n, x, &y)){rt=3; goto rttw;} //spline err
        switch(ep->plane()){
   	  case 0:  // T - both transverse
      	    wx[n] += particle[0]*ep->B*y/(N*P0);
      	    wy[n] += particle[2]*ep->B*y/(N*P0);
      	    break;
   	  case 1:  // X - plane
      	    wx[n] += particle[0]*ep->B*y/(N*P0);
      	    break;
   	  case 2:  // Y - plane
      	    wx[n] += particle[2]*ep->B*y/(N*P0);
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
       auto& particle = v[i];
       if(splint(&sb[0], &wx[0], &vx[0], nb, particle[4], &x)){rt=4; goto rttw;}//spline err
       switch(ep->plane()){
   	 case 0:  // T - both transverse
   	   if(splint(&sb[0], &wy[0], &vy[0], nb, particle[4], &y)){rt=5; goto rttw;}//spline err
      	   particle[1] +=x;
      	   particle[3] +=y;
      	   break;
   	 case 1:  // X - plane
      	   particle[1] +=x;
      	   break;
   	 case 2:  // Y - plane
      	   particle[3] +=x;
      	   break;
   	 case 3:  // Longitudinal
      	   particle[5] +=x;
      	   break;
      }
   }

rttw:
   return rt;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimTrackerNew::cancel()
 {
   mainw_->interrupted_ = true;
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimTrackerNew::cmdPoincare()
{
  // enable poincare plot and track;
  // replace tracker plot with a poincare plot ?? 

  cmdTrackingNew( true );
  trackNewAct_->setEnabled(false);
}
