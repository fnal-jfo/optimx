//  =================================================================
//
//  OptimMainWindow.h
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

#ifndef OPTIMMAINWINDOW_H
#define OPTIMMAINWINDOW_H

#include <cstring>
#include <cstdio>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <memory>


#include <Beamline.h>        
#include <ControlDialog.h>        
#include <SpaceChargeControlDialog.h>        
#include <ExternalPlotDialog.h>
#include <FunctionDialog.h>      // old 
#include <LatticeDialog.h>       // old 
#include <ToolsControlDialog.h>
#include <Control4DDialog.h>
#include <EditReadDialog.h>
#include <CompressDialog.h>  
#include <RMatrixFwd.h>  
#include <OptimMdiSubWindow.h>

#include <QtGlobal>
#include <QMainWindow>
#include <QPointer>
#include <QPrinter>
#include <QColor>
#include <QState>

#include <Coordinates.h>
#include <SCalculator.h>
#include <Cavity.h>
#include <OptimPlot.h>

#define MAXFILES 10

 class QPlainTextEdit;
 class OptimEditor;
 class OptimTextEditor;
 class QWidget;
 class QAction;
 class QMenu;
 class QState;
 class QUrl;
 class QMdiArea;
 class OptimMdiSubWindow;
 class QSignalMapper;
 class OptimTracker; 
 class OptimTrackerNew; 
 class OptimStateMachine;
 class Twiss;
 class Twiss4D;


struct Element; 
struct PTracStruct;
struct TrackParam;
struct OrbitNStruct;
struct PlotSpec;
struct LegoData;
struct FitElem;
struct FitStep;

#if QT_VERSION < 0x050000
class BoolMapper: public QObject {
 
  Q_OBJECT
  
  public:

  BoolMapper( QObject* parent) : QObject(parent), condition_(false), object_(0)  {}
  void setMapping( QObject* object, bool condition) { object_ = object; condition_ = condition; }

  public slots:
    
  void map() { emit mapped( condition_); }
 
  signals:
  
  void mapped(bool); 

 private:

  bool     condition_;
  QObject* object_;

};
#endif
  
enum WindowId {
  LatticeCh, DigCh, Dig4Ch, TrackOutCh,
  BetaCh, SizeCh, SizeTotCh, PhaseCh, TracCh, Beta4Ch,
  Size4Ch, Proj4Ch, Proj4TotCh, Phase4Ch, Disp4Ch, SizeSpCh, ProjSpTotCh, ProjSpCh, PhaseSpCh,
  BetaSpCh, SizeSpBCh, PhaseSpBCh, BetaSpBCh,
  TrackerIntensityPlt, TrackerEmittancePlt, TrackerCentroidPlt, TrackerMomentsPlt, TrackerCorrelationsPlt,
  TrackerDistPlt, Tracker4DBetasPlt, TrackerDispPlt, Tracker4DEpsPlt, BetaLongCh, PoincarePlt, Chroma
}; 


class OptimMainWindow : public QMainWindow {

 friend class OptimStateMachine; 
 friend class OptimTracker; 
 friend class OptimTrackerNew; 
 friend class OptimPlot; 
 friend class OptimEditor; 
 friend class SubwindowEventFilter;
 
    Q_OBJECT

 public:
     OptimMainWindow(QWidget * parent = 0, Qt::WindowFlags flags = Qt::WindowFlags() );
     virtual ~OptimMainWindow();
     
     bool eventFilter(QObject *watched, QEvent *event);
     bool commandLine(int& argc, char** argv);


 protected:
     void initStateMachine();
     void closeEvent(QCloseEvent *event);


 signals:

   void latticeFileLoaded(); 
   void fitUndoAllowed(bool yes); 
   void fitInProgress(); 
   void fitDone(); 

   void trackerPlotPositions();
   void trackerPlotDistribution();
   void trackerSavePositions();
   
 public slots:

   void detach();
   void attach();
   void detachAll();
   void cmdAttachDetach(); 
   void resizeLatticeWindow();

 private slots:

   void stateNoSubWindow();
   void stateSubWindowPresent();
   void stateNoEditor();
   void stateEditorPresent();
   void stateNoLattice();
   void stateLatticeLoaded();
   void stateNoTracker();
   void stateTrackerLoaded();
   void stateNoTextEditor();
   void stateFitDone();
   void stateFitInProgress();
   
   void stateTextEditorLoaded();
   void updateEditMenuState(bool yes=true);
   void updateEditMenuState(QMdiSubWindow*);
   void updateFitMenuState(bool yes);
   
   void openRecentFile();
   void setSubWindowFocus();
   void updateSubWinMenu();
   void setActiveSubWindow( bool state );

   void print(QPrinter* );
   void showPositionInDocument(QPoint pos);
   
   // -----  File  ---------

   void cmdNewFile();
   void cmdNewTxt();
   void open(QString argfilename="");
   void openTxt(QString argfilename="");
   void save();
   void saveAs();
   int  cmdExit();
   void cmdSetupPrinter();
   void cmdPrintPreview();
   void cmdPrint();
   // void cmdFileExecute();
   void cmdRunCalculator();
   void cmdFileGetFileName();
   void cmdToggle_mode();
   void cmdToggle_line();
    //  ** Export 

   void cmdExportResolve();
   void cmdExportDimad();     
   void cmdExportCebafDimad();
   void cmdExportMad8();
   void cmdExportMadX();

   //   *** Import 

   void cmdImportResolve ();
   void cmdCompress();
   void cmdCompressOld();
   void cmdImportDimad ();
   void cmdImportMad8();
   void cmdImportMadX();

   // -----  Edit   ---------                                                                                                                                                                    
 
   void cmdEditDelete();
   void cmdEditCut();
   void cmdEditCopy();
   void cmdEditPaste();
   void cmdEditUndo();
   void cmdEditRedo();
   void cmdEditClearAll();

   int  cmdEditRead( char const* fname=0);
   void cmdAnalyze();
   void cmdAnalyzeHeader();
   void cmdAnalyzeLine();
   void cmdSetWindowStyle();
   void cmdConsistencyCheck();
   void cmdScaleEnergy();
   void cmdEditInvert();
   void cmdDecrement();
   void cmdIncrement();
   void cmdSteps();

   // -----  Search  ---------   

   void cmdEditorFind(bool repeat=false);
   void cmdEditorReplace();
   void cmdEditorNext();
 
   // -----  Fitting  --------- 

   void cmdFitBetas ();
   void cmdFittingStop ();
   void cmdFittingUndo ();
   void cmdFittingControl();
  
   // -----  Tools   ---------   
    
   void cmdCloseSym();
   int  cmdCloseLattice();
   void cmdTuneDiagram();  
   int  cmdCloseTraject();
   void cmdViewOrbit();
   void cmdViewOrbitNew();
   void cmdToolsShowExtern();
   void cmdToolsControl();
   void cmdViewElement();
   void cmdTrajectory();
   void cmdTrajectoryNew();
   void cmdTypeTrajectory();
   void cmdTracker(); 
   void cmdTrackerPlotPositions();
   void cmdTrackerPlotDistribution();
   void cmdTrackerSavePositions();

 
   // -----  View ---------

   void cmdBetas();
   void cmdBetasNew();
   void cmdLBetas();
   void cmdIntegrals();
   void cmdPhases();
   void cmdSizes();  
   void cmdViewMatrix();  
   void cmdViewControl();
   void cmdViewLattice();
   void cmdViewLatticeTable(); // experimental
   void cmdSaveFunctions();
   void cmdViewFunctions();
   void cmdIntegrStep();
   
   // -----  View 4D ---------

   void cmdView4DControl();
   void cmdViewUpdate4Dblock ();
   void cmdView4DClose ();
   void cmdView4DBeta ();
   void cmdView4DBetaNew();
   void cmdView4DSize ();
   void cmdView4DProjection();
   void cmdView4DPhases();
   void cmdView4dDispersion();
   void cmdView4dDispersionNew();
   void cmdView4dFunctionsAtElem ();
   void cmdView4Dfunctions();
   void cmdIntegrals4D();
   void cmdChroma4D();
   void cmdCompaction4D();

   // -----  Space Charge ---------

    void cmdSpcharge_betas(); 
    void cmdSpcharge_sizes();
    void cmdSpcharge_phases();
    void cmdSpacecharge_functions();
    void cmdSpChargeControl();
    void cmdSpChargeCloseLattice();
    void cmdSpacecharge_proj();

   // -----  Window ---------


   // -----  Preferences ---------

    void cmdEditorPreferences();
    void cmdPlotPreferences();
    void cmdCompPreferences();
    void cmdSavePreferences();

  // -----  Help  ---------

    void about();
    void help();
    void localBrowserOverride( QUrl const& url);
 
    void setActiveSubWindow(QObject *window);

 protected:

     void keyPressEvent ( QKeyEvent * event );
 
 private:


     QMdiSubWindow* getLatestDetached( WindowId id);  
     OptimPlot*     addPlot(WindowId id, PlotSpec const& plotspecs, std::vector<LegoData> const&,
			                 DBSpecs const& dbspecs=DBSpecs());
     
     void customizeSystemMenu(QMdiSubWindow* sw); 
     void updateSubWindowsListMenu();

     QMdiSubWindow* getAttachedSubWin( WindowId id );  
     OptimMdiSubWindow* createAttachedSubWin( QWidget* w, WindowId id ); 

     void createActions();
     void createMenus();
     void createToolBars();
     void createStatusBar();
     void readSettings();
     void writeSettings();
     void updateRecentFileActions();

     QMdiSubWindow* activeMdiChild();
     QMdiSubWindow* findOptimEditor(const QString &fileName);
     QMdiSubWindow* findOptimEditor();
 
     void   ctor();
     void   chroma4D(double& chroma1, double& chroma2);;
     void   resetEditorFont();

     void   ringMode();
     void   lineMode();
     void   designOrbitMode();
     void   excitedOrbitMode();
     
     int    InverseOrbit(double* tilt_out, int* ref);
     void   ReplaceLineForInv(OptimEditor* editor, int nline,
			      int n1, double dat1, int n2, double dat2);
     void   DecInc(int ddt);
     int            getLineCalc(QPlainTextEdit*   editor,  char* buf,   int   L,  int nline );
     void           replaceLine(QPlainTextEdit*   editor,  int   nline, char* buf, QColor const& color = QColor(Qt::black) );
     void   replaceExistingLine(QPlainTextEdit*   editor,  int   nline);
     int            getLineCmt(OptimEditor* editor,  char* buf,   int   L,  int nline);
     int    decodeXLine(char* str, resolve* v, int* ntp);
     int    analyze(bool reprint, int i=1); 
     int    analyze2(Coordinates& v); 
     int    analyzeElement(OptimEditor* editor, int nline, char *buf, int nmtr, std::shared_ptr<Element>& Elmp);
  void   print_elm(Element const* el, char* buf);
     int    getTrajParamFromFile(bool Reprint, bool Update, Coordinates& v);
     double findRMatrix(RMatrix& tm);
     int    transferTraject (Coordinates const& vin, Coordinates& vout);
     int    getDataFromFile(char* bufinp);


     void   substituteBetas ( Twiss& v);
     Twiss             betas( Twiss const& vstart);
     Twiss          betasNew( Twiss const& vstart);
     void    setInitialBetas( Twiss& v);

     void   integrStep(double dL, double gamma, double Hrt, double dEn2,
	             Twiss const& v, Element& e, double& dgx,  double& dgy, double& dB2, double& nuxpr,
	             double& nuypr, double& emxn, double& emyn);

     void   integrStep(double dL, double gamma, double Hrt, double dEn2,
	             Twiss4D const& v, Element& e, double& dg1,  double& dg2, double& dB2, double& nu1pr,
		     double& nu2pr, double& em1n, double& em2n);

     void   typeBetas(OptimTextEditor* editor, Twiss& v, double alfa, double enr, int elem); 
     void   Phases (Twiss& v);
     void   Sizes  (Twiss& v);
     int    PrintBdL(int n, std::string names[], int nln, char* buf);
     int    TransferTraject (Coordinates& vin, Coordinates& vout);
     int    ViewFunctions (char const* filenm=0, FunctionDlgStruct* NStf=0, char const* comment=0, bool ClsLat=false); //  ClsLat = close lattice (periodic solution)

     // ---------- Tools -------------

     void      Trajectory (Coordinates& v, Twiss& vt);
     void   TrajectoryNew (Coordinates& v, Twiss& vt);
     int    TypeTrajectory(char const* filenm, PTracStruct *NSt, bool ClsLat, char OutputType);

     int    closeLat2 (bool sym);
     int    closeTrajectFull (Coordinates& vf);
     int    closeTraject (Coordinates& vf);
     int       viewOrbit (char* filenm, OrbitNStruct* NSt, char const* comment);
     int    viewOrbitNew (char* filenm, OrbitNStruct* NSt, char const* comment);

     // ----- View 4D ---------

     void              Betas4D (Twiss4D& v);
     void           Betas4DNew (Twiss4D& v);
     void             Phases4D (Twiss4D& v);
     void               Sizes4D(Twiss4D& v);
     void                Proj4D(Twiss4D& v);
     void         Dispersion4D (Twiss4D& v); 
     void      Dispersion4DNew (Twiss4D& v); 
     void    Print4DBetasToMain(Twiss4D& v);
     void    TypeEigenVectorsToOutW(OptimTextEditor* editor,
	                         RMatrix& tm, std::complex<double> lambda[4], std::complex<double> v[4][4], 
                                 Twiss4D& vt, int err);
     int          ViewFunctions4D (char* filenm=0, FunctionDlgStruct* NStf=0);
     void    View4dFunctionsAtElem(RMatrix& tm, Twiss4D& v);
     int     Get4DBetasFromFile(bool Reprint, bool cmdUpdate, Twiss4D& v);

     // --------- Import ------ 

     void    PrintToWindow(char const* buf, char const* buf2, QPlainTextEdit* editor);
     int     ReadElemDimad(FILE *fp, char* buf, dimad* el, QPlainTextEdit* editor);
     char*   read_one_line(char* buf, int len, FILE* fp, QPlainTextEdit* editor);
     char*   FindReadLine(char* buf, int len, FILE* fp, QPlainTextEdit* editor, char const* src);
     void    DecodeDimadLine(dimad* v, double Hrt, FILE* fp);

    // --------- Space Charge  ------ 

    double spFindRMatrix(RMatrix& tm, Twiss4D& v, BunchParam& bunch, double iscale=1.0); // *** legacy
    double spChargeRMatrix(RMatrix& tm, Twiss4D& v, BunchParam& bunch, double iscale=1.0); 
    
    int         spaceChargeInitTwiss4D(Twiss4D& v,BunchParam& bunch);  
    int         SpchargePeriodicBetas(Twiss4D& v, BunchParam& bunch, bool display_on = true);

    //void               Spcharge_betas(Twiss4D& v, BunchParam& bunch, double iscale=1.0, bool display_on = true);  // *** legacy 
    void                spChargeBetas(Twiss4D& v, BunchParam& bunch, double iscale=1.0, bool display_on = true);  

    int       GetSpaceChargeParam(bool Reprint,   BunchParam& bunch);

    //void             SpaceChargeSizes(Twiss4D& v, BunchParam& bunch); // *** legacy
    void                spChargeSizes(Twiss4D& v, BunchParam& bunch);
    
    //void              Spcharge_phases(Twiss4D& v, BunchParam& bunch); // *** legacy
    void               spChargePhases(Twiss4D& v, BunchParam& bunch); 
    
    //void              SpaceChargeProj(Twiss4D& v, BunchParam& bunch); // *** legacy
    void                 spChargeProj(Twiss4D& v, BunchParam& bunch);

    //void      ViewFunctionsSpCh(char*);     //*** legacy
    void      tabulateFunctionsSpCh(char*);
    
    //int       ViewFunctionsSpCh (char* filenm, FunctionDlgStruct* NStf); //  *** legacy
    int       tabulateFunctionsSpCh(char* filenm, FunctionDlgStruct* NStf);

    double    GetCWSpaceChargeMatr(double& Enr, double ms, double current,
	     			   BeamSize& bs, Element* el, double& tetaY, double dalfa, RMatrix& mi, int st);
    
    // ---------- Fitting -------------

    int      GetFitBeta(int& nline, Twiss& vfin, Twiss& dv, OptimEditor* editor);
    int      GetFitParam (FitStep*    fstep, Twiss vfin[], Twiss dv[], int npoint[], FitElem group[], int& ngr, bool); // V7
    void     PrintFitParam (OptimTextEditor* editor, FitStep* fstep, Twiss vfin[], Twiss dv[], int npoint[], FitElem group[], int& ngr);
    void     PrintBetaParam (OptimTextEditor* editor, Twiss& v, Twiss& dv);
    double   FindError(Twiss   vfin[], Twiss dv[], int npoint[], FitStep* fstep);
    void     addErr(double& err, Twiss const& v, Twiss const& vfin, Twiss const& dv);
    bool     GetGradient(Twiss vfin[], Twiss dv[], int npoint[], FitElem group[], int ngr, FitStep* fstep, double* G);
    int      DoStep(FitElem group[], int ngr, double* dG, double a); // V7
    void     PrintGroupElement(OptimTextEditor* editor, FitElem *group, int ngr);
    void     RewriteElementList( std::vector<std::shared_ptr<Element> >& ElmL);
    void     RewriteElementListFit( std::vector<std::shared_ptr<Element> >& ElmL, FitElem group[], int ngr);
    void     saveElements();
    int      analyzeCompress (FitElem group[], int ngr, int npoint[]);
    int      analyzeWithoutCompress(FitElem group[], int ngr, int npoint[]);

    
    int      TrackOffLine(char *InputPartPosFile, char *TrackResFile, bool MatchCase, char *filter, int nturn, char ring);
    int      ViewMachine(char* filenm, FunctionDlgStruct* NStf, char* comment, bool ClsLat);
    double   ChangeGroupSetting(FitElem group[], double delta_d);
    bool     SetGradientStep(Twiss vfin[], Twiss dv[], int npoint[], FitElem group[], int ngr, FitStep* fstep); // V7
 
     QMdiArea*           mdiArea;
     QSignalMapper *windowMapper;

     QMenu*             fileMenu;
     QMenu*             editMenu;
     QMenu*           windowMenu;
     QMenu*             helpMenu;
  
     QToolBar*       fileToolBar;
     

     QMenu*            fileMenu_;
     QMenu*            editMenu_;
     QMenu*          searchMenu_;
     QMenu*         fittingMenu_;
     QMenu*           toolsMenu_;
     QMenu*            viewMenu_;
     QMenu*          view4DMenu_;
     QMenu*     spaceChargeMenu_;
     QMenu*          windowMenu_;
     QMenu*            helpMenu_;
     
     enum { MaxRecentFiles_ = 5 };
     QAction* recentFileActs_[MaxRecentFiles_];

     QAction* designReferenceOrbitToggleAct_;
     QAction* ringLineToggleAct_;       
     //QAction*           lineAct_;       
     QAction*            newAct_;       
     QAction*           openAct_;
     QAction*        newTextAct_;
     QAction*       openTextAct_;
     QAction*       compressAct_;
     QAction*    compressOldAct_;
     QAction*     calculatorAct_;
     QAction*     importMad8Act_;
     QAction*     importMadXAct_;
     QAction*    importDimadAct_;
     QAction*  importResolveAct_;
     QAction*     exportMad8Act_;
     QAction*     exportMadXAct_;
     QAction*    exportDimadAct_;
     QAction*  exportResolveAct_;
     QAction*           saveAct_;
     QAction*         saveAsAct_;
     QAction*        executeAct_;
     QAction*    getFileNameAct_;
     QAction*   printPreviewAct_;
     QAction*          printAct_;
     QAction*     printSetupAct_;
     QAction*           exitAct_;



     QAction*              undoAct_;       
     QAction*              redoAct_;       
     QAction*               cutAct_;
     QAction*              copyAct_;
     QAction*             pasteAct_;
     QAction*          clearAllAct_;
     QAction*            deleteAct_;
     QAction*    analyzeLatticeAct_;
     QAction*     analyzeHeaderAct_;
     QAction*       analyzeLineAct_;
     QAction*  consistencyCheckAct_;
     QAction*     invertLatticeAct_;
     QAction*       scaleEnergyAct_;
     QAction* readFieldIntegralAct_;
     QAction*         incrementAct_;
     QAction*         decrementAct_;
     QAction*             stepsAct_;

     QAction*      findAct_;
     QAction*   replaceAct_;
     QAction*      nextAct_;

     QAction*     fitBetasAct_;
     QAction*      fitUndoAct_;
     QAction*         stopAct_;
     QAction*   fitControlAct_;

     QAction*            closeLatticeAct_;
     QAction*        closeSymmetricalAct_;
     QAction*                 trackerAct_;
     QAction*    trackerPlotPositionsAct_;
     QAction*    trackerSavePositionsAct_;
     QAction* trackerPlotDistributionAct_;
     QAction*    trackerPlotEmittanceAct_;
     QAction*    trackerPlotIntensityAct_;
 
     QAction*            trajectoryAct_;
     QAction*         trajectoryNewAct_;
     QAction*        typeTrajectoryAct_;
     QAction*       closeTrajectoryAct_;
     QAction*           tuneDiagramAct_;
     QAction*      showExternalFileAct_;
     QAction*          toolsControlAct_;

     QAction*                betasAct_;
     QAction*             betasNewAct_;
     QAction*               betasLAct_;
     QAction*                sizesAct_;
     QAction*               phasesAct_;
     QAction*  functionsAtElementsAct_;
     QAction*            integralsAct_;
     QAction*               matrixAct_;
     QAction*      integrationStepAct_;
     QAction*            functionsAct_;
     QAction*        saveFunctionsAct_;
     QAction*              latticeAct_;
     QAction*         latticeTableAct_;
     QAction*                orbitAct_;
     QAction*             orbitNewAct_;
     QAction*              controlAct_;
     QAction*               detachAct_;
     QAction*               attachAct_;
     QAction*         attachDetachAct_;


     QAction*        update4DBetasAct_;
     QAction*              close4DAct_;
     QAction*              betas4DAct_;
     QAction*           betas4DNewAct_;
     QAction*         dispersion4DAct_;
     QAction*      dispersion4DNewAct_;
     QAction*              sizes4DAct_;
     QAction*            project4DAct_;
     QAction*             phases4DAct_;
     QAction*          functions4DAct_;
     QAction*    functions4DAtElemAct_;
     QAction*            control4DAct_;
     QAction*          integrals4DAct_;
     QAction*             chroma4DAct_; // general chromaticity
     QAction*         compaction4DAct_; // general compacticon   

     QAction*              cwBetasAct_;
     QAction*              cwSizesAct_;
     QAction*            cwProjectAct_;
     QAction*             cwPhasesAct_;
     QAction*          cwFunctionsAct_;
     QAction*       cwCloseLatticeAct_;
     QAction*            controlSCAct_;
  
     QAction*              cascadeAct_;
     QAction*                 tileAct_;
     QAction*         arrangeIconsAct_;
     QAction*             closeAllAct_;
     QAction*          windowStyleAct_;
     QAction*        fileSeparatorAct_;
     QAction*        openedSubWinsAct_;
     QAction*     winMenuSeparatorAct_;


     QAction*    editorPreferencesAct_;
     QAction*      plotPreferencesAct_;
     QAction*      compPreferencesAct_;
     QAction*      savePreferencesAct_;

     QAction*           optimHelpAct_;
     QAction*           aboutAct;
     QAction*           aboutQtAct;


     // -----------------------------------

     SCalc                        calc_;

     ControlStruct                CtSt_;
     ToolDlgStruct              NstTool;
     LatticeDlgStruct           NStLat_;
     ComprsDlgStruct           NStCompr;              
     Dlg4DCtrlStr                 NSt4D;
     EditReadDlgStruct          edtrdSt;
     FunctionDlgStruct            NStf_;
     SpaceChargeControlStruct     SCSt_;  // space charge dialog struct;    

     std::vector<ExtData> ext_dat; 
     char    MainFileDir[256];   

     bool interrupted_;    //  true if user requested interruption of a fit  
     bool analyzed_;   //  true if lattice file has already been analyzed

     int nelm_;     // [ was: NElm ]
     int nelmlist_; // [ was: NElmList ]      
     int NmbPer;

     std::vector<std::shared_ptr<Element>>  elmdict_; // Elements       // [ was: ElmList] 
     // std::vector<std::shared_ptr<Element>> beamline_;                   // beamline array // [ was: Elm]
     Beamline beamline_;                   // beamline array // [ was: Elm]

     bool   use_fractional_tune_;
     bool   phase_advance_constraint_;
     
     double Ein; 
     double ms;
     double Hr;
     double BetaXin; 
     double AlfaXin; 
     double BetaYin; 
     double AlfaYin;
     double QXin;
     double QYin;
     double ex_; 
     double ey_; 
     double dpp_; 
     double DispXin;
     double DispYin;
     double DispPrimeXin; 
     double DispPrimeYin;      
     double Length_;  // reference orbit length 
     double DeltaL_;  // excited orbit lengthening

     double stepint; 
                 
     bool   CtOr;           // Start from file in orbit dialog
     bool   OrbFirstLetter;
     bool   OrbMatchCase;
     char   OrbFilter[1024];

     double xo_;           // current initial conditions for orbit calculations
     double yo_;          
     double zo_;
     double so_;
     double tetaXo_;
     double tetaYo_; 
     double stepo_;     

     double xo0_;           // default initial conditions for orbit calculations
     double yo0_;           // defined in lattice file.  
     double zo0_;
     double so0_;
     double tetaXo0_; 
     double tetaYo0_;
     double stepo0_;     

     double dL_;           // step sizes for interactive parameter variation   
     double dB_;           
     double dG_;
     double dS_;
     double dV_;
     double dA_;
     double dPhase_;
     double dofsX_;
     double dofsY_;
  
     std::vector<std::shared_ptr<Element>>  ElmListFit_; // copy of elements involved in a fit (for undo) 

     int    NElmListFit;         // Number of different Elements in the Element list for fit save

     char   ElemNameFCh[27];     // First letters valid as Element names.
     int    LineLFin;            // Line no of Element list last line in editor
     int    LineLIn;             // Line no of first line of Element list
     int    LineIn;            
     int    Ncvg;                // maximum number of iterations allowed for trajectory closure
     double cnvg, CvgErr;        // convergence coefficient and relative error for trajectory closure
     int    lineOptiM;           // line no for OptiM marker in lattice file 
     char   funcFilter[1024];      // filter for cmdViewFunctions()
     double stepv;               // Step for  cmdViewFunctions()
     int    startFrom;
     int    stopAt;              // Start/stop from line numbers - for  cmdViewFunctions()
     bool   printnames;
     bool   MatchCase;
     bool   PrintEnerg;
     bool   NoFirstLetter;       // true: omit first charater in Element name (first char = type) 
     bool   StartSfrom0;
     double scale_read;          // scale for read field integrals
     int    m1, m2;		 // initial and final Element numbers for matrix calculations

     Coordinates   v_anlz;

     int view_elem;              // number of Element to do "View Functions at Element

     ODEparam p;

     double   cav_accuracy;       // accuracy for ODE integration in cavity
     double   cav_accuracy_L;     // accuracy for ODE integration in cavity
     int          arraylen;       // length of array for plots
     int         n_cav_int;  

     double space_charge_step_;

     //bool IncludeMode;
     //char IncludeFileName[1024];
     //int  InclLineNum;

     //  .... beam-beam  
     int     Nbb;                     // Full number of beam-beam IPs
     std::vector<int>    nslicep;     // no umber of slices for the beam-beam Element
     std::vector<double> Npp; 
     std::vector<double> emxp; 
     std::vector<double> emyp; 
     std::vector<double> sigmasp; 
     std::vector<double> Scolisp; 

     QPointer<QMdiSubWindow>  LatticeCh_;
     QPointer<QMdiSubWindow>    Tracker_;

     QPrinter printer_;

private:

    OptimStateMachine* machine_; 
    QState*            fitinprogress_;
    
 };

 #endif

