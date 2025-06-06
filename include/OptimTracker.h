//  =================================================================
//
//  OptimTracker.h
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

#ifndef OPTIMTRACKER_H
#define OPTIMTRACKER_H

#include <QMainWindow>
#include <QPointer>
#include <RMatrixFwd.h>
#include <Bunch.h>
#include <vector>
#include <memory>
#include <cstdio>
#include <ExtraScatterDialog.h>
#include <TrackSavePositionsDialog.h>
#include <TrackerDistributionDialog.h>
#include <TrackPositionsDialog.h>
#include <TrackPlotDistributionDialog.h>

class  QWidget;
class  QAction;
class  QMenu;
class  Element;
class  OptimMainWindow;
class  TrackerPlot;
class  QwtPlotZoomer;
class  QMdiSubWindow;
class  QProgressDialog;;

struct Twiss;
struct BeamMoments;
struct Coordinates;
struct ScatterPlotItem;
struct ExtData;
class Bunch;

enum class PlaneType: int;


class OptimTracker : public QMainWindow {

 friend class OptimMainWindow;

     Q_OBJECT
 
 public:
 
  OptimTracker(QWidget* parent = 0, Qt::WindowFlags flags =  Qt::WindowFlags()   );
     virtual ~OptimTracker();

 private slots:

     void cancel();
     void cmdTrackerPlotDistribution();
     void cmdTrackerPlotPositions();
     void cmdParameters();
     void cmdInitialize();
     void cmdRead();
     void cmdSave();
     void cmdPlotEmittance();
     void cmdPlotIntensity();
     void cmdTrackSavePos();
     void cmdTracking(bool poincare=false);
     void cmdSwitchView(int view);
     void cmdSwitchView(PlaneType view);
     void cmdInput();
     void cmdOutput();
     void cmdPoincare();

 private:

     void showData();

     int  setInitCoordinates();

     int  trackParticleExact(Element const *ep, double Enr0,  double tetaY,
			    Bunch& v, int N, int n_turn, int n_elem); // V7

     int  trackParticleExactNew(Element const *ep, double Enr0,  RMatrix_t<3>& frame,
			    Bunch& v, int N, int n_turn, int n_elem); // V7

     void trackParticleThroughElem(char nm, double Hrt, Element const* ep,
                                  Bunch& v, RMatrix const& me, int N, int k, int i,
				  double dPdS, double dPdE, double capa, double dtx, double dty, double dSdP); // V7
 
     void getHistory(BeamMoments* p_hist, double gamma, Twiss const& twiss, Bunch const& v); // V7

     int trackWake(Element const* ep, Bunch& v, int N, double Enr0, double ms, ExtData* p); // V7



     QMenu*                 fileMenu;
     QMenu*           parametersMenu;
     QMenu*       phaseSpaceViewMenu;
     QMenu*                plotsMenu;
  
     QAction*          parametersAct_;
     QAction*        distributionAct_;
     QAction*                readAct_;
     QAction*                saveAct_;
     QAction*                 xxpAct_;
     QAction*                 yypAct_;
     QAction*                  xyAct_;
     QAction*                sdppAct_;
     QAction*               inputAct_;
     QAction*              outputAct_;
     QAction*          emittancesAct_;
     QAction*           intensityAct_;
     QAction*               trackAct_;
     QAction*            poincareAct_;

     double   N_;      			
     int      nturn_;  
     double   xmin_;   		
     double   xmax_;   
     double   ymin_;   		
     double   ymax_;   
     double   xpmin_;  		
     double   xpmax_;  
     double   ypmin_;  		
     double   ypmax_;  
     double   pmin_;   		
     double   pmax_;   
     double   smin_;   		
     double   smax_;   
     double   rmin_;   			
     double   rmax_;   
     double   sigmaP_; 
     double   sigmaS_; 
     double   alphaL_; 

     PlaneType plane_;   

     int      view_at_elem_;
     int      view_elem_;
     int      total_elm_view_;
     bool     TrackFast_;
     bool     PrintResults_;
     bool     IncrementTurns_;
     char     TrackFilter_[1024];
     bool     MatchCase_;
     int      TotalTurnsTracked_;

     double DltS_;

     std::vector<std::vector<BeamMoments>>      history_; 
     Bunch  vin_; 
     Bunch  vfin_; 

     std::vector<int> p_elm_view_;     // numbers of Elements for storage of tracking data

     OptimMainWindow*        mainw_;
     QProgressDialog*        progress_bar_;

     TrackerPlot*            plot_; 
     ScatterPlotItem*        outputscatter_;
     ScatterPlotItem*        inputscatter_;
   


     
     ExtraScatterDlgStruct   ExScStr_;  // should be moved out of this header file 
     TracSaveStruct          TrSv_;     // should be moved out of this header file 
     TrGistDistrDlgStruct    TrGs_;     // should be moved out of this header file 
     PltAverPosDlgStruct     TrPlot_;    // should be moved out of this header file 

     QPointer<QMdiSubWindow> outputsw_; 

     std::shared_ptr<FILE>   fpu_;

     bool                    parallel_tracking_; // true = multithreaded tracking 
     
};

#endif

