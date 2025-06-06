//  =================================================================
//
//  OptimTrackerNew.h
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

#ifndef OPTIMTRACKERNEW_H
#define OPTIMTRACKERNEW_H

#include <QMainWindow>
#include <QPointer>
#include <RMatrixFwd.h>
#include <Bunch.h>
#include <vector>
#include <memory>
#include <map>
#include <cstdio>
#include <ExtraScatterDialog.h>
#include <ElmSelection.h>
#include <TrackSavePositionsDialog.h>
#include <TrackerDistributionDialog.h>
#include <TrackPositionsDialog.h>
#include <TrackPlotDistributionDialog.h>
#include <sqlite/connection.hpp>

class  QWidget;
class  QAction;
class  QMenu;
class  Element;
class  OptimMainWindow;
class  TrackerPlot;
class  TrackerPlot6;
class  QwtPlotZoomer;
class  QMdiSubWindow;
class  QProgressDialog;;

struct Twiss;
struct BeamMoments;
struct Coordinates;
struct ScatterPlotItem;
struct ExtData;
class  Bunch;

enum class ViewType: int;
enum class PlaneType: int;


class OptimTrackerNew : public QMainWindow {

 friend class OptimMainWindow;

     Q_OBJECT
 
 public:
 
     OptimTrackerNew(QWidget* parent = 0, Qt::WindowFlags flags =  Qt::WindowFlags() );
     virtual ~OptimTrackerNew();

 private slots:

     void cancel();
     void cmdParameters();
     void cmdInitialize();
     void cmdRead();
     void cmdSave(int inout);
     void cmdTrackingNew(bool poincare=false);
     void cmdSwitchPlane(int view);
     void cmdSwitchPlane(PlaneType view);
     void cmdInput();
     void cmdOutput();
     void cmdPoincare();

     void cmdTrackerSaveMoments();
     void cmdTrackerSavePositions();
     void cmdTrackerPlotDistribution();
     void cmdTrackerPlotPositions();
     void cmdTrackerPlotCentroids();
     void cmdTrackerPlotEigenEmittances();
     void cmdTrackerPlotLatticeFunctions();
     void cmdTrackerPlotDispersion();
     void cmdTrackerPlotIntensity();
     void cmdTrackerPlotMoments();
     void cmdTrackerPlotCorrelations();

 signals:

  void tracking_done(QPrivateSignal);
     
 private:

     void showData();

     int  setInitCoordinates(); // used without GUI

     int  setInitCoordinates(Twiss4D const& v);
     int  setInitCoordinatesDelta(); // delta function

     int  trackBunchExact(Element const *ep, double Enr0,  RMatrix_t<3>& frame,
			    Bunch& v, int N, int n_turn, int n_elem); // V7

     void trackBunch(char nm, double Hrt, Element const* ep,
                                  Bunch& v, RMatrix const& me, int N, int k, int i,
				  double dPdS, double dPdE, double capa, double dtx, double dty, double dSdP); // V7
 
     int trackWake(Element const* ep, Bunch& v, int N, double Enr0, double ms, ExtData* p); // V7



     QMenu*                 fileMenu;
     QMenu*           parametersMenu;
     QMenu*       phaseSpaceViewMenu;
     QMenu*                plotsMenu;
  
     QAction*          parametersAct_;
     QAction*        distributionAct_;
     QAction*                readAct_;
     QAction*             saveOutAct_;
     QAction*              saveInAct_;
     QAction*         saveMomentsAct_;
     QAction*                 xxpAct_;
     QAction*                 yypAct_;
     QAction*                  xyAct_;
     QAction*                sdppAct_;
     QAction*               inputAct_;
     QAction*              outputAct_;
     QAction*             momentsAct_;
     QAction*        correlationsAct_;
     QAction*     eigenemittancesAct_;
     QAction*          latticeFunAct_;
     QAction*           centroidsAct_;
     QAction*          dispersionAct_;
     QAction*           intensityAct_;
     QAction*            trackNewAct_;
     QAction*            poincareAct_;


     sqlite::connection* con_;

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

     int          view_at_elem_;
     int          view_elem_;
     ElmSelection elm_selection_;

     bool     TrackFast_;
     bool     PrintResults_;
     bool     IncrementTurns_;
     char     TrackFilter_[1024];
     bool     MatchCase_;
     int      TotalTurnsTracked_;

     int      dataspec_;   // type of data to be written to db

     double DltS_;

     Bunch vin_; 
     Bunch vfin_; 

     std::vector<int> p_elm_view_;     // numbers of Elements for storage of tracking data

     OptimMainWindow*        mainw_;
     QProgressDialog*        progress_bar_;

     TrackerPlot6*           plot6_; 


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

