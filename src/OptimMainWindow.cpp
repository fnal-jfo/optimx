//  =================================================================
//
//  OptimMainWindow.cpp
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

#include <platform.h>
#include <Constants.h>
#include <ControlDialog.h>
#include <EditReadDialog.h>
#include <EnergyScaleDialog.h>
#include <FindDialog.h>
#include <GeneralPreferencesDialog.h>
#include <MatrixDialog.h>
#include <Element.h>
#include <FitControlDialog.h>
#include <PlotPreferencesDialog.h>
#include <Globals.h>
#include <RMatrix.h>
#include <Twiss.h>
#include <TrackParam.h>
#include <OptimApp.h>
#include <OptimCalc.h>
#include <OptimEditor.h>
#include <OptimHelpAssistant.h>
#include <OptimMainWindow.h>
#include <OptimMessages.h>
#include <OptimPlot.h>
#include <OptimStates.h>
#include <OptimStateMachine.h>
#include <OptimTextEditor.h>
#include <OptimTracker.h>
#include <OptimTrackerNew.h>
#include <OptimTuneDiagram.h>
#include <EditorPreferencesDialog.h>
#include <ReplaceDialog.h>
#include <RootFinder.h>
#include <StepsDialog.h>
#include <TuneDiagramDialog.h>
#include <Utility.h>
#include <QRegularExpression>
#include <QAction>
#include <QKeySequence>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QHeaderView>
#include <QHelpEngine>
#include <QMap>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QStatusBar>
#include <QStyleFactory>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTextStream>
#include <QToolBar>
#include <QUrl>
#include <QtGui>
#include <QHistoryState>
#include <QFinalState>

#include <QwtLegend>
#include <QwtPlotRenderer>
#include <QwtScaleWidget>

#include <cstdlib> // for gentenv
#include <iostream>
#include <algorithm>
#include <cfloat>
#include <cstring>
#include <cmath>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <fmt/format.h>

// std::regex not supported if g++ < 4.9.X
// or MVC C++ < v15.0
#if GCC_VERSION >= 40900 || _MSC_VER >= 1910
#include <regex>
namespace compat = std;
#else
#include <boost/regex.hpp>
namespace compat = boost;
#endif

static const unsigned int LSTR = 1024;

using std::acosh;

using Constants::C_DERV1; 
using Constants::C_DERV3;
using Constants::C_CGS;

using Constants::PI; 


using Utility::gauss; 
using Utility::decodeLine;
using Utility::getFileNameOpt;
using Utility::decodeNumber;
using Utility::GetVariableNameSp;
using Utility::copyname;
using Utility::filterName;
using Utility::getElmName;
using Utility::getElmNameX;
using Utility::strcmpr;
using Utility::strcmpre;
using Utility::correctNames;
using Utility::getVariableName;
using Utility::strippedName;
using Utility::isValidType;

int ierr = 0; // error code 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Application level shortcut overrides.

class SubwindowEventFilter: public QObject { 

public:

  SubwindowEventFilter( OptimMainWindow* parent) 
     : QObject(parent), mainw_(parent) {}

  bool eventFilter(QObject *watched, QEvent* event)
  {
  
  OptimEditor*      editor         = qobject_cast<OptimEditor*>(watched);

  QKeyEvent*   kevent = 0;
  QKeySequence keyseq;

  bool ignore_autorepeat = Globals::preferences().ignore_autorepeat;
    
  if ( editor ) {

    switch ( event->type() ) {

    case QEvent::KeyPress:
         kevent = dynamic_cast<QKeyEvent*>(event);
         keyseq = QKeySequence(kevent->modifiers() | kevent->key());
	 
         if ( (keyseq == QKeySequence(Qt::ControlModifier | Qt::Key_Right                         )) ||
	      (keyseq == QKeySequence(Qt::ControlModifier | Qt::Key_Right+Qt::KeypadModifier      )) ||
	      (keyseq == QKeySequence(Qt::ControlModifier | Qt::Key_Direction_R                   )) || 
	      (keyseq == QKeySequence(Qt::ControlModifier | Qt::Key_Direction_R+Qt::KeypadModifier)) ) {
	   if (ignore_autorepeat) {
	     if (kevent->isAutoRepeat()) return true;  // accept, but ignore autorepeat events
	   }
	   mainw_->DecInc(1);
	   return true;
	 }
         if ( (keyseq == QKeySequence(Qt::ControlModifier | Qt::Key_Left                          )) ||
	      (keyseq == QKeySequence(Qt::ControlModifier | Qt::Key_Left+Qt::KeypadModifier       )) ||
	      (keyseq == QKeySequence(Qt::ControlModifier | Qt::Key_Direction_L                   )) || 
	      (keyseq == QKeySequence(Qt::ControlModifier | Qt::Key_Direction_L+Qt::KeypadModifier)) ) {
	   if (ignore_autorepeat) {
             if (kevent->isAutoRepeat()) return true;        // accept but ignore autorepeat events
	   }
	   mainw_->DecInc(-1);
	   return true;
	 }
	 break;
       default:
         break;
    } // switch
    
  } // editor 
  
  // mouse button press events inside the OptimTextEditor widget are associated with the viewport (child) widget 
  // so we test if the parent is an editor.
  // the code below displays line and col num in the status bar (bottom) 

  QPlainTextEdit* generic_editor = 0;

  if (event->type() ==  QEvent::FocusIn ){
    if  ( (generic_editor = qobject_cast<QPlainTextEdit*>(watched)) ) {
          mainw_->updateEditMenuState(true);
     }
  }
  if ( event->type() ==  QEvent::MouseButtonPress ) { 
     if ( ( generic_editor = qobject_cast<QPlainTextEdit*>(watched->parent() )) ) {
	 QMouseEvent* mevent = dynamic_cast<QMouseEvent*>(event);
         QTextCursor  cursor = generic_editor->cursorForPosition( mevent->pos());
         mainw_->showPositionInDocument( QPoint( cursor.columnNumber()+1, cursor.blockNumber()+1) );
      }
   }
  
  return QObject::eventFilter(watched, event);
 }

  OptimMainWindow* mainw_;
};

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimMainWindow::OptimMainWindow(QWidget* parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags), printer_(QPrinter::HighResolution), ext_dat(MAXFILES)
{
     mdiArea = new QMdiArea;
     mdiArea->setObjectName("OptimMainWindowMdiArea");
     mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
     mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
     setCentralWidget(mdiArea);

     windowMapper = new QSignalMapper(this);
#if QT_VERSION <  0x051500 
     connect(windowMapper, SIGNAL(mapped(QObject*)),
             this, SLOT(setActiveSubWindow(QObject*)));
#else
     connect(windowMapper, SIGNAL(mappedObject(QObject*)),
             this, SLOT(setActiveSubWindow(QObject*)));
#endif
     connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*) ),
             this,     SLOT(updateEditMenuState(QMdiSubWindow*)) );

     connect( this, SIGNAL(fitUndoAllowed(bool)), this, SLOT(updateFitMenuState(bool)) );

     createActions();
     createMenus();
     createToolBars();
     createStatusBar();
     
     readSettings();

     setWindowTitle( qApp->applicationName() + " (LINE)" );
     setUnifiedTitleAndToolBarOnMac(true);

     setObjectName("OptimMainWindow");

     installEventFilter(this);
     qApp->installEventFilter(new SubwindowEventFilter(this) );
     initStateMachine();
     
#ifdef USE_MSWINDOWS
     setWindowIcon( QIcon(":/bitmaps/OptimApp.png") );
#else     
     setWindowIcon( QIcon(":/bitmaps/OptimApp.ico") );
#endif

     ctor(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool OptimMainWindow::eventFilter(QObject *watched, QEvent *event)
{

    // ------------------------------------------------------------------------  
    // Certain Optim shortcuts clash with the predefined Qt shortcuts 
    // These shortcuts are defined here.  
    // ------------------------------------------------------------------------
  
    if (event->type() == QEvent::ShortcutOverride) {
  
       QKeyEvent*            keyEvent  = static_cast<QKeyEvent*>(event);
       Qt::Key               key       = Qt::Key ( keyEvent->key() ); 
       Qt::KeyboardModifiers modifiers = keyEvent->modifiers(); 

       if ( QKeySequence(key+modifiers) == QKeySequence(Qt::Key_F1) ) {
	 keyEvent->setAccepted(true);
         optimHelpAct_->trigger();
	 return true;
       }
       if ( QKeySequence(key+modifiers) ==  QKeySequence(Qt::Key_F2) )  {
	 keyEvent->setAccepted(true);
         analyzeLatticeAct_->trigger();
	 return true;
       }

       if ( QKeySequence(key+modifiers) ==  QKeySequence(Qt::Key_F3) ) {
	 keyEvent->setAccepted(true);
         nextAct_->trigger();
	 return true;
       }
       if ( QKeySequence(key+modifiers) ==  QKeySequence(Qt::Key_F4) ) {
	 keyEvent->setAccepted(true);
         findAct_->trigger();
	 return true;
       }

       if ( QKeySequence(key+modifiers) ==  QKeySequence(Qt::Key_F5) )  {
	 keyEvent->setAccepted(true);
         analyzeLineAct_->trigger();
	 return true;
       }

       if ( QKeySequence(key+modifiers) ==  QKeySequence(Qt::Key_F6) ) {
	 keyEvent->setAccepted(true);
         controlAct_->trigger();
	 return true;
       }

       if (  QKeySequence(key+modifiers) == QKeySequence(Qt::Key_F7) ) {
	 keyEvent->setAccepted(true);
         detachAct_->trigger();
	 return true;
       }

       if (  QKeySequence(key+modifiers) == QKeySequence(Qt::Key_F8) )  {
	 keyEvent->setAccepted(true);
         functionsAtElementsAct_->trigger();
	 return true;
       }

       if (  QKeySequence(key+modifiers) == QKeySequence(Qt::Key_F10) ) {
	 keyEvent->setAccepted(true);
         stepsAct_->trigger();
	 return true;
       }


       if (  QKeySequence(key+modifiers) == QKeySequence(Qt::Key_F4) ) {
	 keyEvent->setAccepted(true);
         exitAct_->trigger();
	 return true;
       }

       if (  QKeySequence(key+modifiers) == QKeySequence(Qt::Key_F2 | Qt::ControlModifier) )  { // will not work with standard KDE/kwin bindings.
	                                                                                        // unless Ctrl-F2 is disabled in KDE
       	 keyEvent->setAccepted(true);
         analyzeHeaderAct_->trigger();
       	 return true;
       }

       if (  QKeySequence(key+modifiers) == QKeySequence(Qt::Key_F2 | Qt::ControlModifier | Qt::ShiftModifier ))  {  // alternative with standard KDE/kwin bindings
       	 keyEvent->setAccepted(true);
         analyzeHeaderAct_->trigger();
       	 return true;
       }

       if (  QKeySequence(key+modifiers) == QKeySequence(Qt::Key_F5 | Qt::ControlModifier) )   {
	 keyEvent->setAccepted(true);
         calculatorAct_->trigger();
	 return true;
       }
       
       
       if (  QKeySequence(key+modifiers) == QKeySequence( Qt::Key_B | Qt::AltModifier) ) {
	 keyEvent->setAccepted(true);
         betasAct_->trigger();
         event->setAccepted(true);
	 return true;
       }

       if ( QKeySequence(key+modifiers) ==  QKeySequence(Qt::Key_I | Qt::AltModifier) ) {
	 if (LatticeCh_) {
	   keyEvent->setAccepted(true);
           integralsAct_->trigger();
	   return true;
	 }
	 else {
	 keyEvent->setAccepted(false); 
	   return false;
	 }  
       }

       if (  QKeySequence(key+modifiers) ==  QKeySequence(Qt::Key_F | Qt::AltModifier) ) {
	 if (LatticeCh_) {
	   keyEvent->setAccepted(true);
           functionsAct_->trigger();
	   return true;
	 }
	 else {
	   keyEvent->setAccepted(false); 
	   return false;
	 }
       }

       if (  QKeySequence(key+modifiers) ==  QKeySequence(Qt::Key_L | Qt::AltModifier) ) {
	 keyEvent->setAccepted(true);
         latticeAct_->trigger();
	 return true;
       }

       if (  QKeySequence(key+modifiers) ==  QKeySequence(Qt::Key_M | Qt::AltModifier) ) {
	 keyEvent->setAccepted(true);
         matrixAct_->trigger();
	 return true;
       }

       if (  QKeySequence(key+modifiers) ==  QKeySequence(Qt::Key_O | Qt::AltModifier) ) {
	 keyEvent->setAccepted(true);
         orbitAct_->trigger();
	 return true;
       }
       if (  QKeySequence(key+modifiers) ==  QKeySequence(Qt::Key_P+Qt::AltModifier) ) {
         if (LatticeCh_) {
	   keyEvent->setAccepted(true);
           phasesAct_->trigger();
	   return true;
	 }
	 else {
	   keyEvent->setAccepted(false); 
	   return false;
	 }
       }

       if (  QKeySequence(key+modifiers) == QKeySequence(Qt::Key_S | Qt::AltModifier) ) {
	 if (LatticeCh_) {
	   keyEvent->setAccepted(true);
           sizesAct_->trigger();
	   return true;
	 }
         else {
	   keyEvent->setAccepted(false); 
	   return false;
	 } 
       }

#if 0
       if (  QKeySequence(key+modifiers) == QKeySequence( Qt::Key_T+Qt::AltModifier+Qt::ShiftModifier) ) {

	 std::cout << "key "       << std::bitset<32>(key)       << std::endl;
	 std::cout << "modifiers " << std::bitset<32>(modifiers) << std::endl;

	 if (LatticeCh_) {
	    keyEvent->accept();
            typeTrajectoryAct_->trigger();
            return true;
          }
          else {
	    keyEvent->setAccepted(false); 
	    return false;
	  }
       } 

      if (  QKeySequence(key+modifiers) == QKeySequence(Qt::Key_T+Qt::AltModifier) ) {
       	 if (LatticeCh_) {
	    keyEvent->setAccepted(true);
            integrationStepAct_->trigger();
            return true;
	 }
	 else {
	   keyEvent->setAccepted(false); 
	   return false;
	 }  
       }
#endif       
      if (  QKeySequence(key+modifiers) == QKeySequence(Qt::Key_Backspace | Qt::AltModifier) ) {
	 keyEvent->setAccepted(true);
         undoAct_->trigger();
	 return true;
       }

       if (  QKeySequence(key+modifiers) == QKeySequence(Qt::Key_E |  Qt::ControlModifier) )   {
	 keyEvent->setAccepted(true);
         executeAct_->trigger();
	 return true;
       }

       //----------------------------------------------------------------
       // It seems that because Ctrl+O is pre-defined for QMainWindow
       // adding it explicitly as a shortcut in a menu defines it twice.
       // This section is commented out for now.
       //----------------------------------------------------------------

#if 0
#ifndef USE_WINDOWS
       if ( QKeySequence(key+modifiers) == QKeySequence(Qt::Key_O | Qt::ControlModifier) )   {
	 std::cout << "Ctrl-O" << std::endl;
	 keyEvent->setAccepted(true);
         openAct_->trigger();
	 return false;
       }
#endif
#endif        
       if (  QKeySequence(key+modifiers)  ==  QKeySequence(Qt::Key_W | Qt::ControlModifier) ||
	     QKeySequence(key+modifiers)  ==  QKeySequence(Qt::Key_S | Qt::ControlModifier) )   {
	 keyEvent->setAccepted(true);
         saveAct_->trigger();
	 return true;
       }
 
    }

   return QObject::eventFilter(watched, event);
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::ctor() 
{

  printer_.setPageSize(QPageSize(QPageSize::Letter));

  mdiArea->setBackground( QBrush( QColor(0x80, 0xff, 0xff) ) );

  // MDI windows 

  // ... Text windows 
  LatticeCh_  = 0; // Main Lattice Editor 	 

  analyzed_   = false;
  beamline_.clear();
  nelm_       = 0; 
  elmdict_.clear();  
  nelmlist_   = 0;
  NElmListFit = 0;

  m1            = 1;
  m2            = 1;
  stepv         = 0;
  stepint       = 0.5;
  strcpy(funcFilter,"*");
  startFrom     = 0;
  stopAt        = 0;
  printnames    = true;
  NoFirstLetter = false;
  MatchCase     = false;
  PrintEnerg    = false;
  StartSfrom0   = true;
     
    
  CtSt_.IsRingCh         = false;
  CtSt_.CompAtExcitedOrb = false;
  CtSt_.AutoBeta         = true;
  CtSt_.AutoLattice      = true;
  CtSt_.ClearPlot        = true;
  CtSt_.ClearText        = true;
  CtSt_.PlotBoxes        = true;
  CtSt_.RewriteBuf       = false;
  CtSt_.PlotTotalSize    = false;
  CtSt_.PlotApertures    = true;
  CtSt_.ArrayLen         = 5000;
  CtSt_.CouplThreshold   = 0.05;
  CtSt_.NStep            = 100; 
  CtSt_.Accuracy         = 1.0e-6;
  CtSt_.AccuracyL        = 1.e-2;

  CtOr    = true;
  xo_      = 0.0;
  yo_      = 0.0;
  zo_      = 0.0;
  so_      = 0.0;
  tetaXo_  = 0.0;
  tetaYo_  = 0.0;
  stepo_   =0.0;
  
  xo0_     = 0.0;
  yo0_     = 0.0;
  zo0_     = 0.0;
  so0_     = 0.0;
  tetaXo0_ = 0.0;
  tetaYo0_ = 0.0;
  stepo0_   =0.0;

  OrbFirstLetter = false;
  OrbMatchCase  =  false; 
  strcpy(OrbFilter,"*");
 
  strcpy(ElemNameFCh,"ABCDEFGHIJKLMOQRSTUWXZ");
 
  // ... default step sizes used for interactive fitting 
 
  dL_     = 1.0;	
  dB_     = 0.01;	
  dG_     = 0.005;	
  dS_     = 0.0001;   
  dV_     = 1; 
  dA_     = 0.1; 
  dofsX_  = 0.01; 
  dofsY_  = 0.01; 
  dPhase_ = 1.0;
     

  interrupted_  = false;
  view_elem = false;

  //HelpState = false;
  //ContextHelp = false;
  //HelpCursor = 0;

  strcpy(NStLat_.Filter,     "*" );
  NStLat_.MltpOrder        =    10;
  NStLat_.printBdL         = false;  
  NStLat_.printFirstLetter = false;
  NStLat_.MatchCase        = false;
  NStLat_.StartSFromFile   = false;

  strcpy(edtrdSt.Filter,     "*" ); 	
  edtrdSt.MatchCase        = false;
  edtrdSt.FirstLetter[0]   = 0;    
  edtrdSt.CompareOnly      = false;

  NStCompr.offset          = 0;
  NStCompr.cor             = false;
  NStCompr.sext            = false;
  NStCompr.sol             = false;

  scale_read = 1.0;

  NSt4D.UpdateTwiss           = false;
  NSt4D.ComputeAtExcitedOrbit = false;

  space_charge_step_ = 1.0;
  Ncvg         = 2000;    // was 1000.;
  cnvg         = 0.1;
  CvgErr       = 1.0e-8;  // was 1.0e-9;

  SCSt_.step       = space_charge_step_;
  SCSt_.relaxation = 0.1;
  SCSt_.maxiters   = 20;
  SCSt_.isteps     = 1;
  SCSt_.eps        = 1.0e-3;
  SCSt_.algo       = RootFinder::powell_hybrid;
  SCSt_.use4dblock = false; 
  
  NstTool.LinClosure        = false; // closure using linear map only
  NstTool.FullClosure       = true;  // closure includes nonlinearities and correctors
  NstTool.Closure4D         = true;  // restrict closure to 4D
  NstTool.NormalPlot        = true;
  NstTool.NormBySigma       = false;
  NstTool.TogetherWithEnvel = false;
  NstTool.longposcm         = true;
  NstTool.longposns         = false;
  NstTool.longposdeg        = false;
  NstTool.longposrd         = false;
  NstTool.rffrequency       = 1.0; 
  NstTool.ConvParam         = 0.1;
  NstTool.ConvErr           = 1.0e-9;
  NstTool.Nmax              = 1000;

  appstate.IncludeFileName[0] = 0;
  appstate.InclLineNum = 0;
  appstate.IncludeMode = false;
  appstate.calc = &calc_; // FIXME
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 OptimMainWindow::~OptimMainWindow()
 {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QMdiSubWindow*  OptimMainWindow::getAttachedSubWin( WindowId id )
{
  // get an "attached" version of the output window if it exists. Otherwise, return a null ptr.  
  
  auto wlist = mdiArea->subWindowList(); 

  for ( auto it=wlist.begin(); it != wlist.end(); ++it) {

    if ( !( (*it)->property("winid").isValid()) ) continue;     
    if ((*it)->property("winid").toInt() !=  id ) continue;     // skip window for other types. 

    if ( !( (*it)->property("attached").isValid()) ) continue;     
    if ((*it)->property("attached").toBool() )       return (*it); // return the attached one (there should be only one !). 
  }        

  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimMdiSubWindow*  OptimMainWindow::createAttachedSubWin( QWidget* w, WindowId id ) 
{ 
  // Create a new "attached" output window. NOTE: this *assumes*
  // that such a window either does not exist or exists in detached state.
  // This function should be called only if getAttachedSubWin returns a null ptr.

  
  // check if a detached version already exist. If so, use the properties
  // of the detached window to set some of the new window properties.

  auto wlist = mdiArea->subWindowList(QMdiArea::CreationOrder); 

  QMdiSubWindow* dw = 0; // detached windowof type id

  auto it=wlist.end();
  while ( it-- != wlist.begin() ) { // inverse creation order
    if ((*it)->property("attached").toBool())  continue;
    if ((*it)->property("winid").toInt() == id ) {
      dw = (*it);
      break;
    }
  };

  OptimPlot*   pw = qobject_cast<OptimPlot*>(w);
  OptimPlot*  dpw = dw ? qobject_cast<OptimPlot*>(dw->widget()) : 0;

  // Set the new window geometry. Use geometry of the last created
  // (detached) window of the same time, if it exists. 
  
  int x  =   dw ? dw->geometry().x() + 10 : mdiArea->geometry().width()/2;
  int y  =   dw ? dw->geometry().y() + 10 : 0;
  int width   =   dw ? dw->geometry().width()  : mdiArea->geometry().width()/2;
  int height  =   dw ? dw->geometry().height() : mdiArea->geometry().height();
  
  auto sw = new OptimMdiSubWindow();
  sw->setWidget(w);
  sw->setAttribute(Qt::WA_DeleteOnClose);

  mdiArea->addSubWindow(sw); 

  sw->setMinimumSize(100, 100); 
  sw->setGeometry( x, y, width, height );
  sw->setProperty("attached", true);
  sw->setProperty("winid", id) ;
  customizeSystemMenu(sw);


  return sw;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::resetEditorFont()
 {

   QList<QMdiSubWindow*> swlist = mdiArea->subWindowList();
   QPlainTextEdit* editor = 0;
   QFont font;

   font.fromString(Globals::preferences().editor_default_font.specs);

   for ( auto it=swlist.begin(); it != swlist.end(); ++it) {
     if ( !(editor = qobject_cast<QPlainTextEdit*>( (*it)->widget()) ) ) return;
	  editor->setFont(font);
   }

   OptimTextEditor::setDefaultFont(font);   
   OptimEditor::setDefaultFont(font);   

 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdNewTxt()
{
   OptimTextEditor* editor = 0;

   QMdiSubWindow*  sw = mdiArea->addSubWindow( (editor = new OptimTextEditor()) ); 
   connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
   connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
   connect(editor, SIGNAL(redoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
   connect(editor->document(), SIGNAL(modificationChanged(bool)), saveAct_, SLOT(setEnabled(bool)) );
   editor->newFile();
   sw->setGeometry(0, 0, mdiArea->geometry().width()/2,  mdiArea->geometry().height());
   editor->show();
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimMainWindow::cmdNewFile()
 {
   if(LatticeCh_) { 
    OptimMessageBox::warning(this, "Open Error", "You must first close the current lattice file", QMessageBox::Ok);
    return;
  }
  
  // ... New Lattice   

  char const* buf[]={"$E=200.",
  	  "OptiM",
  	  "# Insert comment after #",
  	  "# Remove one of two # in the next two lines and put file name to include external file",
  	  "##INCLUDE FileName ",
  	  "##END_INCLUDE",
  	  "Energy[MeV]=$E  Mass[MeV]=0.511006",
  	  "Emittance: ex[cm]=1.e-5.  ey[cm]=1.e-7  DP/P=1e-4.",
  	  "Initial: BetaX[cm]=109.141 	BetaY[cm]=843.674",
  	  "         AlphaX=-0.255721 	AlphaY=1.25601",
  	  "         DispersX[cm]=110.518 	DispersY[cm]=0",
  	  "         Dsp_PrimeX=0.     DspPrimeY=0.",
  	  "         X=0.	Y=0.	Z=0.",
  	  "         tetaX=0.     tetaY=0.",
  	  "#  Elements: O,I-drift, D,B-sector dipole, G-magnet edge ",
  	  "#       Q-quad, S-sextupole, C,CC-solenoid and focusing only pseudo-solenoid",
  	  "#       M-multipole(length=0), K,Z-transverse and longitudinal correctors",
  	  "#       A,W-RF cavity(standing wave), E-electroctatic acc.section",
  	  "#       X-transfer matrix (next 6 lines have to hold the matrix itself)",
  	  "#       R-combined magneto-electrostatic bend, L - electrostatic quad",
  	  "#       F-Litium lens, H-Scraper or aperture",
  	  "#       T & U - transverse and longitudinal scattering",
  	  "#",
  	  "#Uncoment 3 lines below to make a reference to an external file",
  	  "#file reference start",
  	  "#file_reference_number file_name",
  	  "#file reference end",
  	  "#Above file_reference_number can be any number within 0-9;",
  	  "#You can make references up to 10 files between",
  	  "#lines \"file reference start\" and \"file reference end\" ",
  	  "#",
  	  "begin lattice. Number of periods=1",
  	  "o D o qF o D o qD",
  	  "end lattice",
  	  "begin list: dL=1.  dB=.1  dG=0.01  dS=0.01",
  	  "o L[cm]=10.",
  	  "D L[cm]=100. B[kG]=5.25298",
  	  "qF L[cm]=20 G[kG/cm]=0.08 Tilt[deg]=0",
  	  "qD L[cm]=20 G[kG/cm]=-0.28 Tilt[deg]=0",
  	  "end list of Elements"};

   int nlines = sizeof(buf)/sizeof(char const*);

   OptimEditor*   editor = 0;
   LatticeCh_ = mdiArea->addSubWindow( (editor = new OptimEditor() ) );

#if QT_VERSION < 0x050000 
   BoolMapper* mapper1 = new BoolMapper(this);
   mapper1->setMapping( saveAct_, 1);
   connect(editor,  SIGNAL(textChanged()),   mapper1,  SLOT(map()));
   connect(mapper1, SIGNAL(mapped(bool)),    saveAct_, SLOT(setEnabled(bool)));
#else
   connect(editor, &QPlainTextEdit::textChanged,  [this](){ saveAct_->setEnabled(true);} );
#endif
   connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
   connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
   connect(editor, SIGNAL(redoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
   resizeLatticeWindow();
   
   if (editor) { 
      for(int i=0; i<nlines; ++i) {
        editor->insertPlainText(buf[i]);
        editor->insertPlainText("\n");
      }
      editor->document()->setModified(false);
      editor->newFile();
      editor->show();
   }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimMainWindow::openTxt( QString fn )
 {

     
     OptimTextEditor* editor = 0;
     QMdiSubWindow*       sw = 0;
     QSettings settings;
      
     QString      fileName   = (fn != "") ? fn : QFileDialog::getOpenFileName(this, "Open Text File",  settings.value("optworkdir").toString() );
      
     if (fileName=="") return;  

     sw = mdiArea->addSubWindow( (editor = new OptimTextEditor()) ); 

#if QT_VERSION < 0x050000
     BoolMapper* mapper1 = new BoolMapper(this);
     mapper1->setMapping( saveAct_, 1);
     connect(editor,  SIGNAL(textChanged()),  mapper1,  SLOT(map()));
     connect(mapper1, SIGNAL(mapped(bool)),    saveAct_, SLOT(setEnabled(bool)));
#else
     //connect(editor, &QTextEdit::textChanged,  [this](){ saveAct_->setEnabled(true);} );
     connect(editor, &QPlainTextEdit::textChanged,  [this](){ saveAct_->setEnabled(true);} );
#endif

     connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
     connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
     connect(editor, SIGNAL(redoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );

     if (editor->loadFile(fileName)) {
            QString curFile = fileName;
            setWindowFilePath(curFile);
            QStringList files = settings.value("recentFileList").toStringList();
            files.removeAll(fileName);
            files.prepend(fileName);
            while (files.size() > MaxRecentFiles_)
            files.removeLast();
            settings.setValue("recentFileList", files);
            updateRecentFileActions();
            statusBar()->showMessage(tr("File loaded"), 2000);
            sw->setGeometry(0, 0, mdiArea->geometry().width()/2,  mdiArea->geometry().height());
            settings.setValue("optworkdir", QFileInfo(fileName).dir().path());
	    editor->show();
     }
     
     else {
       // editor->close(); // it should be deleted
       sw->close();
     }
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimMainWindow::open( QString argfilename)
 {
   QString filter("OptiM (*.opt *.OPT *.opc *.OPC);;"); 
   QSettings settings;
   
   QString fileName = argfilename.isEmpty() ? QFileDialog::getOpenFileName(this, "Open OptiM Lattice File", settings.value("optworkdir").toString(), filter): argfilename;

   if (!fileName.isEmpty()) {
 
         QMdiSubWindow* existing = findOptimEditor(fileName);     
         if (existing) {
             mdiArea->setActiveSubWindow(existing);
             return;
         }

	 OptimEditor* editor   = 0;
         if (!LatticeCh_) { 
	   LatticeCh_= mdiArea->addSubWindow( (editor = new OptimEditor()) );
#if QT_VERSION < 0x050000
           BoolMapper* mapper1 = new BoolMapper(this);
           mapper1->setMapping( saveAct_, 1);
           connect(editor,  SIGNAL(textChanged()),  mapper1,  SLOT(map()));
           connect(mapper1, SIGNAL(mapped(bool)),    saveAct_, SLOT(setEnabled(bool)));
#else
	   connect(editor, &QPlainTextEdit::textChanged,  [this](){ saveAct_->setEnabled(true);} );
#endif
	   connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
           connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
           connect(editor, SIGNAL(redoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
  	   resizeLatticeWindow();
         }  
         else {
           if( OptimInformationMessage( this, "Open File", "Only one lattice file can be active. Open as a regular text file ? ",
					QMessageBox::Yes | QMessageBox::No) ==  QMessageBox::Yes )  {
	     openTxt(fileName);
	     return;
	   } 
           return; 
	 } 

         if (editor->loadFile(fileName)) {
            QString curFile = fileName;
            setWindowFilePath(curFile);
            QStringList files = settings.value("recentFileList").toStringList();
            files.removeAll(fileName);
            files.prepend(fileName);
            while (files.size() > MaxRecentFiles_)
            files.removeLast();
            settings.setValue("recentFileList", files);
            updateRecentFileActions();
            statusBar()->showMessage(tr("File loaded"), 2000);
            settings.setValue("optworkdir", QFileInfo(fileName).dir().path());
	    LatticeCh_->show();
            emit latticeFileLoaded(); 
         } else {
	    LatticeCh_->close();
         }
     }
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::save()
{  
  
  QMdiSubWindow*           sw  = activeMdiChild();
  if (!sw) return; 

  OptimEditor*         editor  = qobject_cast<OptimEditor *>(sw->widget());
  OptimTextEditor* texteditor  = qobject_cast<OptimTextEditor *>(sw->widget());
   
   if ( editor ) {
       editor->save();
       statusBar()->showMessage(tr("File saved"), 2000);
       return;
   }
   if (texteditor ) {
       texteditor->save();
       statusBar()->showMessage(tr("File saved"), 2000);
       return;
   }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::saveAs()
 {

   OptimEditor*        editor      = 0;
   OptimTextEditor*    txteditor   = 0;
   OptimPlot*          plot        = 0;
   OptimTuneDiagram*   tunedg      = 0;

   if (  mdiArea->currentSubWindow() ) {
     QWidget* w =  mdiArea->currentSubWindow()->widget();  
     editor    = qobject_cast<OptimEditor *>(w); 
     txteditor = qobject_cast<OptimTextEditor *>(w); 

     if( editor ) {
         QString filter("OptiM (*.opt *.OPT *.opc *.OPC);;"); 
	 QString filename =  QFileDialog::getSaveFileName(0, "Save File As", editor->fileInfo().absoluteFilePath(), filter);
	 if ( filename == "" ) return; 
	 editor->saveFile(filename);
	 editor->clear();
         editor->loadFile(filename); 
         statusBar()->showMessage(tr("File saved."), 2000);
     }

     if (txteditor) { 
          
         QString filename =  QFileDialog::getSaveFileName(0, "Save File As", txteditor->fileInfo().absoluteFilePath());
	 if ( filename == "" ) return; 
	 txteditor->saveFile(filename);
	 txteditor->clear();
         txteditor->loadFile(filename); 
         statusBar()->showMessage(tr("File saved."), 2000);
     }

     if ( (plot = qobject_cast<OptimPlot *>(w)) )  { 
       plot->saveAs(); 
       statusBar()->showMessage(tr("Plot saved."), 2000);
     }

     if ( (tunedg = qobject_cast<OptimTuneDiagram *>(w)) )  { 
       tunedg->saveAs(); 
       statusBar()->showMessage(tr("TuneDiagram saved."), 2000);
     }
   }
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdSetWindowStyle() 
{
  QAction* caller = 0;
  if ( (caller = qobject_cast<QAction*>(sender())) ) {
    auto style = caller->text().remove(QChar('&')); 
    qApp->setStyle(style);                // "&" may be present (defines following char as accelerator)
    qApp->setProperty("GUIstyle", style); // store the style name as a property 
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdEditCut()
{

  mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
  QMdiSubWindow* sw = mdiArea->activeSubWindow();
  if (!sw) { return;}   // no subwindow had focus. 

  QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(sw->widget()); 
  if (!editor) return; // the window that had focus is not an editor  

  editor->cut();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void OptimMainWindow::cmdEditCopy()
{
  mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
  QMdiSubWindow* sw = mdiArea->activeSubWindow();
  if (!sw) { return;}   // no subwindow had focus. 

  QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(sw->widget()); 
  if (editor) {
    editor->copy();
    return; // the window that had focus is not an editor  
  }

  OptimPlot* plot = qobject_cast<OptimPlot*>(sw->widget()); 
  if (plot) {
    plot->copyToClipboard();
    return;
  }

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdEditPaste()
{
  
  mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
  QMdiSubWindow* sw = mdiArea->activeSubWindow();
  if (!sw) { return;}   // no subwindow had focus. 

  QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(sw->widget()); 
  if (!editor) return; // the window that had focus is not an editor  

  editor->paste();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdEditClearAll()
{
  mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
  QMdiSubWindow* sw = mdiArea->activeSubWindow();
  if (!sw) { return;}   // no subwindow had focus. 

  QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(sw->widget()); 
  if (!editor) return; // the window that had focus is not an editor  

  editor->clear();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdEditDelete()
{
  mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
  QMdiSubWindow* sw = mdiArea->activeSubWindow();
  if (!sw) { return;}   // no subwindow had focus. 

  QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(sw->widget()); 
  if (!editor) return; // the window that had focus is not an editor  

  QTextCursor cursor = editor->textCursor();
  cursor.removeSelectedText();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdEditUndo()
{
  mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
  QMdiSubWindow* sw = mdiArea->activeSubWindow();
  if (!sw) { return;}   // no subwindow had focus. 

  QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(sw->widget()); 
  if (!editor) return; // the window that had focus is not an editor  

  editor->undo();
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdEditRedo()
{
  mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
  QMdiSubWindow* sw = mdiArea->activeSubWindow();
  if (!sw) { return;}   // no subwindow had focus. 

  QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(sw->widget()); 
  if (!editor) return; // the window that had focus is not an editor  

  editor->redo();
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimMainWindow::about()
 {
   // ref: https://sourceforge.net/p/predef/wiki/Architectures/

   QString architecture;

#if defined(__x86_64)
   architecture = QString( "Hardware Architecture: intel x86 64-bit <br><br>" );
#elif defined(_X86_)
   architecture = QString( "Hardware Architecture: intel x86 32-bit (MinGW32)<br><br>");
#endif
   
   QMessageBox::about(this, tr("About OptiM"),
	        tr("OptimX <br>"
	           "Build date: " __DATE__ "<br>")+
                   architecture +    
                tr ("(c) 2014-2025 FermiForward Discovery Group/Fermilab <br>"
                   "Original author: val@fnal.gov <br>"
                   "Qt port and further enhancements: ostiguy@fnal.gov <br>" ));
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimMainWindow::createActions()
 {
 
  for (int i=0; i<MaxRecentFiles_; ++i) {
    recentFileActs_[i] = new QAction(this);
    recentFileActs_[i]->setVisible(false);
    connect(recentFileActs_[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
  }

  ringLineToggleAct_ = new QAction( QIcon(":/bitmaps/toggle_line.bmp"),     tr("Ring/Line"),    this);
  designReferenceOrbitToggleAct_ = new QAction( QIcon(":/bitmaps/toggle.bmp"),          tr("Design/Reference Orbit"),    this);
  connect( ringLineToggleAct_,             SIGNAL(triggered()), this, SLOT(cmdToggle_line()) ); 
  connect( designReferenceOrbitToggleAct_, SIGNAL(triggered()), this, SLOT(cmdToggle_mode()) ); 

     newAct_ = new QAction( tr("New"),    this);

   openAct_ = new QAction(  QIcon(":/bitmaps/OPEN.BMP"),   tr("&Open ..."),  this);
   openAct_->setStatusTip(tr("Open an existing file"));

   openAct_->setShortcut( QKeySequence(tr("Ctrl+O") ));
   // QKeySequence::Open

   connect(openAct_, SIGNAL(triggered()), this, SLOT(open()));
   connect( newAct_, SIGNAL(triggered()), this, SLOT(cmdNewFile()));

  newTextAct_ = new QAction( QIcon(":/bitmaps/NEW.BMP"), tr("&New text"),  this);
      openTextAct_ = new QAction( tr("Open &Text ..."),     this);
      compressAct_ = new QAction( tr("&Compress ..."),      this);
   compressOldAct_ = new QAction( tr("&Compress [Old]..."), this);
    importMad8Act_ = new QAction( tr("from MAD8"),          this);
    importMadXAct_ = new QAction( tr("from MADX"),          this);
   importDimadAct_ = new QAction( tr("from DIMAD"),         this);
 importResolveAct_ = new QAction( tr("from Resolve"),       this);
    exportMad8Act_ = new QAction( tr("to &MAD8"),           this);
    exportMadXAct_ = new QAction( tr("to MAD&X"),           this);
   exportDimadAct_ = new QAction( tr("to &CEBAF DIMAD"),    this);
 exportResolveAct_ = new QAction( tr("to &Resolve"),        this);
          saveAct_ = new QAction( QIcon(":/bitmaps/SAVE.BMP"), tr("Save "),             this);
          saveAct_->setStatusTip("Save File"); 
          saveAct_->setShortcut( QKeySequence(tr("Ctrl+W") )); //QKeySequence::Save; 
          //saveAct_->setShortcutContext( Qt::ApplicationShortcut);
        saveAsAct_ = new QAction( tr("Save &As ..."),       this);
    calculatorAct_ = new QAction( tr("&Run Calculator "),   this);
    calculatorAct_->setShortcut( QKeySequence(tr("Ctrl+F5") ));
       executeAct_ = new QAction( tr("Execute "),          this);
       executeAct_->setShortcut( QKeySequence(tr("Ctrl+E") ));
   getFileNameAct_ = new QAction( tr("&Get File Name ..."), this);
  printPreviewAct_ = new QAction(  QIcon(":/bitmaps/PREVIEW2.BMP"), tr("Print Preview .."),  this);
         printAct_ = new QAction(  QIcon(":/bitmaps/PRINT.BMP"),    tr("Print ..."),         this);
    printSetupAct_ = new QAction( tr("P&rint Setup .."),    this);
          exitAct_ = new QAction( tr("E&xit"),              this);
          exitAct_->setShortcut( QKeySequence(tr("Alt+F4") ));

  connect(newTextAct_,  SIGNAL(triggered()), this, SLOT(cmdNewTxt()));
  connect(openTextAct_, SIGNAL(triggered()), this, SLOT(openTxt()));
  connect(saveAct_,     SIGNAL(triggered()), this, SLOT(save()) );
  connect(saveAsAct_,   SIGNAL(triggered()), this, SLOT(saveAs()) );
  connect(exitAct_,     SIGNAL(triggered()), this, SLOT(cmdExit()) );

  connect(  calculatorAct_,   SIGNAL(triggered()),  this,  SLOT(cmdRunCalculator()) );
  connect( getFileNameAct_,   SIGNAL(triggered()),  this,  SLOT(cmdFileGetFileName()) );
  connect( exportResolveAct_, SIGNAL(triggered()),  this,  SLOT(cmdExportResolve()) ); 
  connect( exportDimadAct_,   SIGNAL(triggered()),  this,  SLOT(cmdExportCebafDimad()) );
  connect( exportMad8Act_,    SIGNAL(triggered()),  this,  SLOT(cmdExportMad8()));     
  connect( exportMadXAct_,    SIGNAL(triggered()),  this,  SLOT(cmdExportMadX()));     

  connect(importResolveAct_,  SIGNAL(triggered()), this,  SLOT( cmdImportResolve() ));
  connect(  importDimadAct_,  SIGNAL(triggered()), this,  SLOT( cmdImportDimad() ));
  connect(   importMad8Act_,  SIGNAL(triggered()), this,  SLOT( cmdImportMad8() ));
  connect(   importMadXAct_,  SIGNAL(triggered()), this,  SLOT( cmdImportMadX() ));
  connect(     compressAct_,  SIGNAL(triggered()), this,  SLOT( cmdCompress() ));
  connect(  compressOldAct_,  SIGNAL(triggered()), this,  SLOT( cmdCompressOld() )); // DEBUG FIXME
  connect(        printAct_,  SIGNAL(triggered()), this,  SLOT( cmdPrint() ));
  connect(   printSetupAct_,  SIGNAL(triggered()), this,  SLOT( cmdSetupPrinter() ));
  connect( printPreviewAct_,  SIGNAL(triggered()), this,  SLOT( cmdPrintPreview() ));

                undoAct_ = new QAction( QIcon(":/bitmaps/UNDO.BMP"),  tr("&Undo"), this);
                undoAct_->setShortcut( QKeySequence(tr("Alt+Backspace") ));
                redoAct_ = new QAction( QIcon(":/bitmaps/UNDO.BMP"),  tr("&Redo"), this);
                 cutAct_ = new QAction(  QIcon(":/bitmaps/CUT.BMP"),   tr("Cut"),    this);
                 cutAct_->setShortcut( QKeySequence(tr("Shift+Del") ));
                copyAct_ = new QAction(  QIcon(":/bitmaps/COPY.BMP"),  tr("Copy"),   this);
                copyAct_->setShortcut( QKeySequence(tr("Ctrl+Ins") ));
               pasteAct_ = new QAction(  QIcon(":/bitmaps/PASTE.BMP"), tr("Paste"),  this);
               pasteAct_->setShortcut( QKeySequence(tr("Shift+Ins") ));
            clearAllAct_ = new QAction( tr("Clear All"),                this); 
            clearAllAct_->setShortcut( QKeySequence(tr("Ctrl+Del") ));
              deleteAct_ = new QAction( tr("&Delete"),                  this);

      analyzeLatticeAct_ = new QAction( tr("&Analyze Lattice"),         this);
      analyzeLatticeAct_->setShortcut( QKeySequence(tr("F2") ));
       analyzeHeaderAct_ = new QAction( tr("Analyze &Header"),          this);
       analyzeHeaderAct_->setShortcut( QKeySequence(tr("Ctrl+F2") ));
       analyzeLineAct_   = new QAction( tr("Analyze &Line"),             this);
         analyzeLineAct_->setShortcut( QKeySequence(tr("F5") ));
    consistencyCheckAct_ = new QAction( tr("Consis&tency Check"),       this);
       invertLatticeAct_ = new QAction( tr("I&nvert Lattice"),           this);
         scaleEnergyAct_ = new QAction( tr("Scale &Energy ..."),         this);
   readFieldIntegralAct_ = new QAction( tr("&Read Field Integral ..."),  this);
           incrementAct_ = new QAction( tr("I&ncrement"),                this);
           incrementAct_->setShortcut( QKeySequence(tr("Ctrl+Right") ));
           decrementAct_ = new QAction( tr("D&ecrement"),               this);
           decrementAct_->setShortcut( QKeySequence(tr("Ctrl+Left") ));
	       stepsAct_ = new QAction( QIcon(":/bitmaps/step.bmp"), tr("S&teps"),    this);
               stepsAct_->setShortcut( QKeySequence(tr("F10") ));

	   connect(   deleteAct_,   SIGNAL(triggered()), this, SLOT(cmdEditDelete()));
	   connect( clearAllAct_,   SIGNAL(triggered()), this, SLOT(cmdEditClearAll()));
	       connect( undoAct_,   SIGNAL(triggered()), this, SLOT(cmdEditUndo()));
	       connect( redoAct_,   SIGNAL(triggered()), this, SLOT(cmdEditRedo()));
	       connect(  cutAct_,   SIGNAL(triggered()), this, SLOT(cmdEditCut()));
	       connect( copyAct_,   SIGNAL(triggered()), this, SLOT(cmdEditCopy()));
	       connect(pasteAct_,   SIGNAL(triggered()), this, SLOT(cmdEditPaste()));

    connect(readFieldIntegralAct_, SIGNAL(triggered()), this, SLOT( cmdEditRead() ));
        connect(analyzeHeaderAct_, SIGNAL(triggered()), this, SLOT(cmdAnalyzeHeader()));
       connect(analyzeLatticeAct_, SIGNAL(triggered()), this, SLOT(cmdAnalyze()));
          connect(analyzeLineAct_, SIGNAL(triggered()), this, SLOT(cmdAnalyzeLine()));
     connect(consistencyCheckAct_, SIGNAL(triggered()), this, SLOT(cmdConsistencyCheck()));
        connect(invertLatticeAct_, SIGNAL(triggered()), this, SLOT(cmdEditInvert()));
     connect(incrementAct_,        SIGNAL(triggered()), this, SLOT(cmdIncrement()));
     connect(decrementAct_,        SIGNAL(triggered()), this, SLOT(cmdDecrement()));
     connect(stepsAct_,            SIGNAL(triggered()), this, SLOT(cmdSteps()));
     connect(scaleEnergyAct_,      SIGNAL(triggered()), this, SLOT(cmdScaleEnergy()));
		  
        findAct_ =  new QAction( QIcon(":/bitmaps/FIND.BMP"),tr( "Find ..."),  this); 
        findAct_->setShortcut( QKeySequence(tr("F4") )); 
     replaceAct_ =  new QAction( tr("Replace ..."), this); 
        nextAct_ =  new QAction( QIcon(":/bitmaps/FINDNEXT.BMP"),tr("Next"),   this); 
        nextAct_->setShortcut( QKeySequence(tr("F3") ));

	connect(findAct_,      SIGNAL(triggered()), this, SLOT(cmdEditorFind())); // default arg repeat = false
     connect(replaceAct_,   SIGNAL(triggered()), this, SLOT(cmdEditorReplace()));
     connect(nextAct_,      SIGNAL(triggered()), this, SLOT(cmdEditorNext()));


        fitBetasAct_ =   new QAction( QIcon(":/bitmaps/FIT.BMP"), tr("Fit Twiss"), this);
	 fitUndoAct_ =   new QAction( QIcon(":/bitmaps/UNDO.BMP"), tr("Undo"), this);
	    stopAct_ =   new QAction( QIcon(":/bitmaps/STOP.BMP"), tr("Stop"), this);
      fitControlAct_ =   new QAction( QIcon(":/bitmaps/GEAR.ICO"), tr("Control"), this);

       connect(fitBetasAct_,    SIGNAL(triggered()), this, SLOT(cmdFitBetas()) );
       connect(stopAct_,        SIGNAL(triggered()), this, SLOT(cmdFittingStop()) );
       connect(fitUndoAct_,     SIGNAL(triggered()), this, SLOT(cmdFittingUndo()) );
       connect(fitControlAct_,  SIGNAL(triggered()), this, SLOT(cmdFittingControl()) );

	      closeLatticeAct_ =  new QAction(  QIcon(":/bitmaps/CLOSELAT.BMP"), tr("Close Lattice"), this);
	      closeLatticeAct_->setShortcut( QKeySequence(tr("Alt+C") ));
          closeSymmetricalAct_ =  new QAction(  QIcon(":/bitmaps/CLOSESYM.BMP"), tr("Close Symmetrical"), this);
                   trackerAct_ =  new QAction( tr("Tracker ..."), this);
      trackerPlotPositionsAct_ =  new QAction( tr("Plot Positions"), this);
      trackerSavePositionsAct_ =  new QAction( tr("Save Positions" ), this);
   trackerPlotDistributionAct_ =  new QAction( tr("Plot Distribution"), this);
                trajectoryAct_ =  new QAction( tr("Trajectory [Legacy]..."), this);
                trajectoryAct_->setShortcut( QKeySequence(tr("Ctrl+T") ));
             trajectoryNewAct_ =  new QAction( tr("Trajectory ..."), this);
            typeTrajectoryAct_ =  new QAction( tr("Type Trajectory"), this);
            typeTrajectoryAct_->setShortcut( QKeySequence(tr("Alt+Shift+T") ));
           closeTrajectoryAct_ =  new QAction( tr("Close Trajectory"), this);
               tuneDiagramAct_ =  new QAction( tr("Tune Diagram ..."), this);
               tuneDiagramAct_->setShortcut( QKeySequence(tr("Alt+Shift+D") ));
              toolsControlAct_ =  new QAction( tr("Control"), this);
          showExternalFileAct_ =  new QAction( tr("Show External File ..."), this);


       connect(closeLatticeAct_,            SIGNAL(triggered()), this, SLOT(cmdCloseLattice()) );
       connect(closeSymmetricalAct_,        SIGNAL(triggered()), this, SLOT(cmdCloseSym()) );
       connect(tuneDiagramAct_,             SIGNAL(triggered()), this, SLOT(cmdTuneDiagram()) );
       connect(typeTrajectoryAct_,          SIGNAL(triggered()), this, SLOT(cmdTypeTrajectory()) );
       connect(closeTrajectoryAct_,         SIGNAL(triggered()), this, SLOT(cmdCloseTraject()) );
       connect(trajectoryAct_,              SIGNAL(triggered()), this, SLOT(cmdTrajectory()) );
       connect(trajectoryNewAct_,           SIGNAL(triggered()), this, SLOT(cmdTrajectoryNew()) );
       connect(trackerAct_,                 SIGNAL(triggered()), this, SLOT(cmdTracker()) );
       connect(trackerPlotPositionsAct_,    SIGNAL(triggered()), this, SLOT(cmdTrackerPlotPositions()) );
       connect(trackerSavePositionsAct_,    SIGNAL(triggered()), this, SLOT(cmdTrackerSavePositions()) );
       connect(trackerPlotDistributionAct_, SIGNAL(triggered()), this, SLOT(cmdTrackerPlotDistribution()) );
       connect(toolsControlAct_,            SIGNAL(triggered()), this, SLOT(cmdToolsControl()) );
       connect(showExternalFileAct_,        SIGNAL(triggered()), this, SLOT(cmdToolsShowExtern()));

        closeLatticeAct_->setIcon(QIcon(":/bitmaps/CLOSELAT.BMP"));
    closeSymmetricalAct_->setIcon(QIcon(":/bitmaps/CLOSESYM.BMP"));


                    betasAct_ =  new QAction( tr("Betas [Legacy]"),          this);
                 betasNewAct_ =  new QAction( tr("Betas"),       this);
                    betasAct_->setShortcut( QKeySequence(tr("Alt+B") ));
		    //betasLAct_ =  new QAction( tr("Long Beta"),     this);
                    sizesAct_ =  new QAction( tr("Sizes"),          this);
                    sizesAct_->setShortcut( QKeySequence(tr("Alt+S") )); 
                   phasesAct_ =  new QAction( tr("Phases"),         this);
                   phasesAct_->setShortcut( QKeySequence(tr("Alt+P") ));
      functionsAtElementsAct_ =  new QAction( tr("Functions at Elements ..."), this);
      functionsAtElementsAct_->setShortcut( QKeySequence(tr("F8") ));
                integralsAct_ =  new QAction( QIcon(":/bitmaps/INTEGR.BMP"),tr("Integrals"),      this);
                integralsAct_->setShortcut( QKeySequence(tr("Alt+I") ));
                   matrixAct_ =  new QAction( tr("Matrix ..."),         this);
                   matrixAct_->setShortcut( QKeySequence(tr("Alt+M") )); 
          integrationStepAct_ =  new QAction( tr("Integration Step ..."),    this);
          integrationStepAct_->setShortcut( QKeySequence(tr("Alt+T") )); 
                functionsAct_ =  new QAction( QIcon(":/bitmaps/function.bmp"), tr("Functions ..."),      this);
                functionsAct_->setShortcut( QKeySequence(tr("Alt+F") ));
	    saveFunctionsAct_ =  new QAction( tr("Save Functions"),    this);
                  latticeAct_ =  new QAction( tr("Lattice ..."),        this);
                  latticeAct_->setShortcut( QKeySequence(tr("Alt+L") ));
             latticeTableAct_ =  new QAction( tr("Lattice Table..."),        this);
                    orbitAct_ =  new QAction( tr("Orbit ..."),          this);
                    orbitAct_->setShortcut( QKeySequence(tr("Alt+O") ));
                 orbitNewAct_ =  new QAction( tr("Orbit [New] ..."),      this);
                  controlAct_ =  new QAction( tr("Control ..."),        this);
                  controlAct_->setShortcut( QKeySequence(tr("F6") ));

        betasAct_->setIcon(QIcon(":/bitmaps/PLOTBETA.BMP"));
        betasNewAct_->setIcon(QIcon(":/bitmaps/PLOTBETA.BMP"));
        matrixAct_->setIcon(QIcon(":/bitmaps/matrix.bmp"));


	connect(betasAct_,    SIGNAL(triggered()), this, SLOT(cmdBetas()));
	connect(betasNewAct_, SIGNAL(triggered()), this, SLOT(cmdBetasNew()));
	// connect(betasLAct_,   SIGNAL(triggered()), this, SLOT(cmdLBetas())); // long betas
	connect(integralsAct_, SIGNAL(triggered()), this, SLOT(cmdIntegrals()));
	connect(phasesAct_,    SIGNAL(triggered()), this, SLOT(cmdPhases()));	
        connect(sizesAct_,     SIGNAL(triggered()), this, SLOT(cmdSizes()));
        connect(matrixAct_,    SIGNAL(triggered()), this, SLOT(cmdViewMatrix()));
        connect(controlAct_,   SIGNAL(triggered()), this, SLOT(cmdViewControl()));
        connect(latticeAct_,   SIGNAL(triggered()), this, SLOT(cmdViewLattice()));
        connect(latticeTableAct_,    SIGNAL(triggered()), this, SLOT(cmdViewLatticeTable()));
        connect(orbitAct_,           SIGNAL(triggered()), this, SLOT(cmdViewOrbit()));
        connect(orbitNewAct_,        SIGNAL(triggered()), this, SLOT(cmdViewOrbitNew()));
        connect(functionsAct_,       SIGNAL(triggered()), this, SLOT(cmdViewFunctions()));
        connect(saveFunctionsAct_,   SIGNAL(triggered()), this, SLOT(cmdSaveFunctions()));
        connect(integrationStepAct_, SIGNAL(triggered()), this, SLOT(cmdIntegrStep()) );
        connect(functionsAtElementsAct_, SIGNAL(triggered()), this, SLOT(cmdViewElement() ));
 
	    update4DBetasAct_  =  new QAction( tr("&Update 4D Betas"),         this);
	    update4DBetasAct_->setShortcut( QKeySequence(tr("Ctrl+Shift+U") ));
                  close4DAct_  =  new QAction( tr("&Close 4D"),                this);
 	          close4DAct_->setShortcut( QKeySequence(tr("Ctrl+Shift+C") ));
                  betas4DAct_  =  new QAction( tr("&Betas 4D [Legacy]"),               this);
                  betas4DAct_->setShortcut( QKeySequence(tr("Ctrl+Shift+B") ));
               betas4DNewAct_  =  new QAction( tr("&Betas 4D"),            this);
             dispersion4DAct_  =  new QAction( tr("&Dispersion 4D [Legacy]"),           this);
             dispersion4DAct_->setShortcut( QKeySequence(tr("Ctrl+Shift+D") ));
          dispersion4DNewAct_  =  new QAction( tr("&Dispersion 4D"),       this);
                  sizes4DAct_  =  new QAction( tr("&Sizes 4D "),               this);
                  sizes4DAct_->setShortcut( QKeySequence(tr("Ctrl+Shift+S") ));
                project4DAct_  =  new QAction( tr("Size P&rojections 4D "),         this);
                 phases4DAct_  =  new QAction( tr("&Phases 4D ... "),          this);
                 phases4DAct_->setShortcut( QKeySequence(tr("Ctrl+Shift+P") ));
              functions4DAct_  =  new QAction( tr("&Functions 4D"),            this);
              functions4DAct_->setShortcut( QKeySequence(tr("Ctrl+Shift+F") ));
        functions4DAtElemAct_  =  new QAction( tr("Functions at &Elem. ..."),  this);
        functions4DAtElemAct_->setShortcut( QKeySequence(tr("Ctrl+Shift+F8") ));
              integrals4DAct_ =  new QAction( QIcon(":/bitmaps/INTEGR.BMP"),tr("Integrals"),      this);
	       //integrals4DAct_->setShortcut( QKeySequence(tr("Alt+I") ));
               chroma4DAct_   =  new QAction( tr("Chromaticity"),             this);// general chromaticity
            compaction4DAct_  =  new QAction( tr("Momentum Compaction"),      this);// general compacticon   
	      control4DAct_   =  new QAction( tr("Con&trol ..."),             this);


        connect(update4DBetasAct_,     SIGNAL(triggered()), this, SLOT(cmdViewUpdate4Dblock()) );
	connect(close4DAct_,           SIGNAL(triggered()), this, SLOT(cmdView4DClose()) );
	connect(betas4DAct_,           SIGNAL(triggered()), this, SLOT(cmdView4DBeta()) );
	connect(betas4DNewAct_,        SIGNAL(triggered()), this, SLOT(cmdView4DBetaNew()) );
        connect(dispersion4DAct_,      SIGNAL(triggered()), this, SLOT(cmdView4dDispersion()));
        connect(dispersion4DNewAct_,   SIGNAL(triggered()), this, SLOT(cmdView4dDispersionNew()));
	connect(sizes4DAct_,           SIGNAL(triggered()), this, SLOT(cmdView4DSize()));
	connect(project4DAct_,         SIGNAL(triggered()), this, SLOT(cmdView4DProjection()));
	connect(phases4DAct_,          SIGNAL(triggered()), this, SLOT(cmdView4DPhases()));
        connect(functions4DAct_,       SIGNAL(triggered()), this, SLOT(cmdView4Dfunctions()));
        connect(functions4DAtElemAct_, SIGNAL(triggered()), this, SLOT(cmdView4dFunctionsAtElem ()));
	connect(control4DAct_,         SIGNAL(triggered()), this, SLOT(cmdView4DControl()));
	connect(integrals4DAct_,       SIGNAL(triggered()), this, SLOT(cmdIntegrals4D()));
	connect(chroma4DAct_,          SIGNAL(triggered()), this, SLOT(cmdChroma4D()));
	connect(compaction4DAct_,      SIGNAL(triggered()), this, SLOT(cmdCompaction4D()));


                  cwBetasAct_  =  new QAction( tr("CW &Betas"),        this);
                  cwSizesAct_  =  new QAction( tr("CW &Sizes"),        this);
                cwProjectAct_  =  new QAction( tr("CW P&rojections"),  this);
                 cwPhasesAct_  =  new QAction( tr("CW &Phases"),       this);
              cwFunctionsAct_  =  new QAction( tr("CW &Functions"),    this);
           cwCloseLatticeAct_  =  new QAction( tr("CW Close Lattice"), this);
                controlSCAct_  =  new QAction( tr("Con&trol"),         this);
  
	connect( cwBetasAct_,        SIGNAL(triggered()), this, SLOT( cmdSpcharge_betas()  ));
	connect( cwSizesAct_,        SIGNAL(triggered()), this, SLOT( cmdSpcharge_sizes()  ));
	connect( cwProjectAct_,      SIGNAL(triggered()), this, SLOT( cmdSpacecharge_proj()  ));
	connect( cwPhasesAct_,       SIGNAL(triggered()), this, SLOT( cmdSpcharge_phases() ));
	connect( cwFunctionsAct_,    SIGNAL(triggered()), this, SLOT( cmdSpacecharge_functions() ));
	connect( cwCloseLatticeAct_, SIGNAL(triggered()), this, SLOT( cmdSpChargeCloseLattice() ));
	connect( controlSCAct_,      SIGNAL(triggered()), this, SLOT( cmdSpChargeControl()  ));
     

  
                  cascadeAct_  =  new QAction( tr("&Cascade"),         this);
                     tileAct_  =  new QAction( tr("&Tile"),            this);
             arrangeIconsAct_  =  new QAction( tr("Arrange &Icons"),   this);
                 closeAllAct_  =  new QAction( tr("Close All"),       this);
		  detachAct_ = new QAction(  QIcon(":/bitmaps/detach.bmp"),   tr("Detach"),         this);
		  attachAct_ = new QAction(  QIcon(":/bitmaps/detach.bmp"),   tr("Attach"),         this);
	    attachDetachAct_ = new QAction(  QIcon(":/bitmaps/detach.bmp"),   tr("Attach/Detach"),  this);
	    attachDetachAct_->setShortcut( QKeySequence(tr("F7") ));

	connect(       cascadeAct_, SIGNAL(triggered()), mdiArea, SLOT( cascadeSubWindows()) );
	connect(          tileAct_, SIGNAL(triggered()), mdiArea, SLOT( tileSubWindows()) );
	connect(      closeAllAct_, SIGNAL(triggered()), mdiArea, SLOT( closeAllSubWindows()) );
        connect(detachAct_, SIGNAL(triggered()), this, SLOT(detach() ));
        connect(attachAct_, SIGNAL(triggered()), this, SLOT(attach() ));
        connect(attachDetachAct_, SIGNAL(triggered()), this, SLOT(cmdAttachDetach() ));

    // Preferences Menu
		 

	     editorPreferencesAct_  =  new QAction( tr("Editor"),           this);
	       plotPreferencesAct_  =  new QAction( tr("Plots"),            this);
	       compPreferencesAct_  =  new QAction( tr("Computations"),     this);
	       savePreferencesAct_  =  new QAction( tr("Save Preferences"), this);

     // Note: window | style actions are defined and connected when the menu entries are defined. 

		  connect( editorPreferencesAct_, SIGNAL(triggered()), this,    SLOT( cmdEditorPreferences()) );
		  connect(   plotPreferencesAct_, SIGNAL(triggered()), this,    SLOT( cmdPlotPreferences()) );
		  connect(   compPreferencesAct_, SIGNAL(triggered()), this,    SLOT( cmdCompPreferences()) );
		  connect(   savePreferencesAct_, SIGNAL(triggered()), this,    SLOT( cmdSavePreferences()) );


     //     separatorAct->setSeparator(true);

     aboutAct = new QAction(tr("&About OptiMX"), this);
     aboutAct->setStatusTip(tr("Show the application's About Box"));
     connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

     aboutQtAct = new QAction(tr("About &Qt"), this);
     aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
     connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

     optimHelpAct_ = new QAction(tr("OptiMX Help"), this);
     optimHelpAct_->setShortcut( QKeySequence(tr("F1") ));
     optimHelpAct_->setStatusTip(tr("OptiMX documentation."));
     connect(optimHelpAct_, SIGNAL(triggered()), this, SLOT(help()) );
   
     // connections for custom key accelerators

     QObject::connect(qobject_cast<OptimApp*>(qApp), SIGNAL(sigHelp()),  optimHelpAct_, SLOT(trigger()) );

 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimMainWindow::createMenus()
 {
   char* envdbg = std::getenv("OPTIMX_LEGACY");
     
    fileMenu_ = menuBar()->addMenu(tr("&File"));
    fileMenu_->addAction(newAct_); 
    fileMenu_->addAction(openAct_); 
    fileMenu_->addAction(newTextAct_); 
    fileMenu_->addAction(openTextAct_); 
    fileMenu_->addSeparator();
    fileMenu_->addAction( compressAct_); 
    // fileMenu_->addAction( compressOldAct_); // FOR DEBUGGING
    QMenu* importMenu = fileMenu_->addMenu("Import");
      importMenu->addAction(importMad8Act_); 
      importMenu->addAction(importMadXAct_); 
      importMenu->addAction(importDimadAct_); 
      importMenu->addAction(importResolveAct_); 
    QMenu* exportMenu = fileMenu_->addMenu("Export");
      exportMenu->addAction(exportMad8Act_); 
      exportMenu->addAction(exportMadXAct_); 
      exportMenu->addAction(exportDimadAct_); 
      exportMenu->addAction(exportResolveAct_); 
    fileMenu_->addSeparator();
    fileMenu_->addAction(saveAct_); 
    fileMenu_->addAction(saveAsAct_); 
    fileMenu_->addSeparator();
    fileMenu_->addAction(calculatorAct_); 
    fileMenu_->addAction(executeAct_); 
    fileMenu_->addAction(getFileNameAct_); 
    fileMenu_->addSeparator();
    fileMenu_->addAction(printPreviewAct_); 
    fileMenu_->addAction(printAct_); 
    fileMenu_->addAction(printSetupAct_); 
    fileSeparatorAct_ = fileMenu_->addSeparator();
    for (int i = 0; i < MaxRecentFiles_; ++i) {
       fileMenu_->addAction(recentFileActs_[i]);
    }
    fileMenu_->addSeparator();
    fileMenu_->addAction(exitAct_);
    updateRecentFileActions();

    editMenu_ = menuBar()->addMenu(tr("&Edit")); 
    editMenu_->addAction(undoAct_); 
    editMenu_->addAction(redoAct_); 
    editMenu_->addSeparator();
    editMenu_->addAction(cutAct_); 
    editMenu_->addAction(copyAct_); 
    editMenu_->addAction(pasteAct_); 
    editMenu_->addSeparator();
    editMenu_->addAction(clearAllAct_); 
    editMenu_->addAction(deleteAct_); 
    editMenu_->addSeparator();
    editMenu_->addAction(analyzeLatticeAct_); 
    editMenu_->addAction(analyzeHeaderAct_); 
    editMenu_->addAction(analyzeLineAct_); 
    editMenu_->addAction(consistencyCheckAct_);
    editMenu_->addSeparator(); 
    editMenu_->addAction(invertLatticeAct_); 
    editMenu_->addAction(scaleEnergyAct_); 
    editMenu_->addAction(readFieldIntegralAct_);
    editMenu_->addSeparator(); 
    editMenu_->addAction(incrementAct_); 
    editMenu_->addAction(decrementAct_); 
    editMenu_->addAction(stepsAct_); 
    
    connect(  editMenu_, SIGNAL(aboutToShow()),  this, SLOT( updateEditMenuState())  ); 
    
    searchMenu_ = menuBar()->addMenu(tr("&Search"));
    searchMenu_->addAction( findAct_);
    searchMenu_->addAction( replaceAct_);
    searchMenu_->addAction( nextAct_);

    fittingMenu_ = menuBar()->addMenu(tr("F&itting"));
    fittingMenu_->addAction(fitBetasAct_);  
    fittingMenu_->addSeparator(); 
    fittingMenu_->addAction(fitUndoAct_ );
    fittingMenu_->addAction(stopAct_);  
    fittingMenu_->addSeparator(); 
    fittingMenu_->addAction(fitControlAct_);  

    toolsMenu_ = menuBar()->addMenu(tr("T&ools"));
    toolsMenu_->addAction(closeLatticeAct_);
    toolsMenu_->addAction(closeSymmetricalAct_);
    toolsMenu_->addSeparator();
    toolsMenu_->addAction(trackerAct_);
    QMenu* trackingMenu = toolsMenu_->addMenu("View Tracking Results");
    trackingMenu->addAction(trackerPlotPositionsAct_);
    trackingMenu->addAction(trackerSavePositionsAct_ );
    trackingMenu->addAction(trackerPlotDistributionAct_);
    toolsMenu_->addSeparator();
    if (envdbg) { 
      toolsMenu_->addAction(trajectoryAct_);
    }
    toolsMenu_->addAction(trajectoryNewAct_);
    toolsMenu_->addAction(typeTrajectoryAct_);
    toolsMenu_->addAction(closeTrajectoryAct_);
    toolsMenu_->addSeparator();
    toolsMenu_->addAction(tuneDiagramAct_);
    toolsMenu_->addSeparator();
    toolsMenu_->addAction(showExternalFileAct_);
    toolsMenu_->addSeparator();
    toolsMenu_->addAction(toolsControlAct_);

    viewMenu_ = menuBar()->addMenu(tr("&View"));
    if (envdbg) { 
      viewMenu_->addAction(betasAct_);  
    }
    viewMenu_->addAction(betasNewAct_);  
    //viewMenu_->addAction(betasLAct_);  // Longitudinal betas 
    viewMenu_->addAction(sizesAct_);   
    viewMenu_->addAction(phasesAct_);  
    viewMenu_->addSeparator();
    viewMenu_->addAction(functionsAtElementsAct_); 
    viewMenu_->addAction(integralsAct_);  
    viewMenu_->addAction(matrixAct_);  
    viewMenu_->addAction(integrationStepAct_);  
    viewMenu_->addSeparator();
    viewMenu_->addAction(functionsAct_);  
    // viewMenu_->addAction(saveFunctionsAct_);  OBSOLETE ???  
    viewMenu_->addAction(latticeAct_);  
    viewMenu_->addAction(latticeTableAct_); // DEBUGGING 

    if (envdbg) { 
       viewMenu_->addAction(orbitNewAct_);  
    }
    viewMenu_->addAction(orbitAct_);  
    viewMenu_->addSeparator();
    viewMenu_->addAction(controlAct_);  

    view4DMenu_ = menuBar()->addMenu(tr("View 4&D")); 
    view4DMenu_->addAction(update4DBetasAct_);
    view4DMenu_->addAction(close4DAct_);
    view4DMenu_->addSeparator();
    if (envdbg) { 
       view4DMenu_->addAction(betas4DAct_);
    }
    view4DMenu_->addAction(betas4DNewAct_);

    if (envdbg) { 
    view4DMenu_->addAction(dispersion4DAct_);
    }
    view4DMenu_->addAction(dispersion4DNewAct_);
    view4DMenu_->addAction(sizes4DAct_);
    view4DMenu_->addAction(project4DAct_);
    view4DMenu_->addAction(phases4DAct_);
    view4DMenu_->addSeparator();
    view4DMenu_->addAction(functions4DAct_);
    view4DMenu_->addAction(functions4DAtElemAct_);
    view4DMenu_->addSeparator();
    view4DMenu_->addAction(integrals4DAct_);
    view4DMenu_->addAction(chroma4DAct_);
    // EXPERIMENTAL view4DMenu_->addAction(compaction4DAct_);
    view4DMenu_->addSeparator();
    view4DMenu_->addAction(control4DAct_);

    spaceChargeMenu_ = menuBar()->addMenu(tr("S&paceCharge"));

    spaceChargeMenu_->addAction(cwCloseLatticeAct_);
    spaceChargeMenu_->addSeparator();
    spaceChargeMenu_->addAction(cwBetasAct_);
    spaceChargeMenu_->addAction(cwSizesAct_);
    spaceChargeMenu_->addAction(cwProjectAct_);
    spaceChargeMenu_->addAction(cwPhasesAct_);
    spaceChargeMenu_->addAction(cwFunctionsAct_);
    spaceChargeMenu_->addSeparator();
    spaceChargeMenu_->addAction(controlSCAct_);
  
    windowMenu_ = menuBar()->addMenu(tr("&Window"));

    windowMenu_->addAction(cascadeAct_);
    windowMenu_->addAction(tileAct_);
    windowMenu_->addAction(arrangeIconsAct_);
    windowMenu_->addAction(closeAllAct_);
   
    windowMenu_->addSeparator();
    windowMenu_->addAction(detachAct_);  
    windowMenu_->addAction(attachAct_);  
    winMenuSeparatorAct_ = windowMenu_->addSeparator();
   
    connect( windowMenu_, SIGNAL(aboutToShow()), this,  SLOT( updateSubWinMenu()) );  

    QMenu* preferencesMenu = menuBar()->addMenu(tr("&Preferences"));
    preferencesMenu->addAction(editorPreferencesAct_);
    preferencesMenu->addAction(plotPreferencesAct_);
    preferencesMenu->addAction(compPreferencesAct_); // ***** DISABLED (General Preferences) 
    QMenu* styleMenu = preferencesMenu->addMenu("GUI Style"); 

    QStringList validkeys = QStyleFactory::keys();
    for( auto it = validkeys.begin(); it != validkeys.end(); ++it) {
      QAction* action;
      styleMenu->addAction( (action = new QAction(*it, this)) );
      connect(action, SIGNAL(triggered()), this, SLOT(cmdSetWindowStyle()));
    }     
    preferencesMenu->addSeparator();
    preferencesMenu->addAction(savePreferencesAct_);
 
    helpMenu_ = menuBar()->addMenu(tr("&Help"));
    helpMenu_->addAction(optimHelpAct_);
    helpMenu_->addAction(aboutAct);
    helpMenu_->addAction(aboutQtAct);
    
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimMainWindow::createToolBars()
 {
   
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openAct_);
    fileToolBar->addAction(newTextAct_);
    fileToolBar->addAction(saveAct_);
    fileToolBar->addAction(cutAct_);
    fileToolBar->addAction(copyAct_);
    fileToolBar->addAction(pasteAct_);


    fileToolBar->addAction(stepsAct_);
    fileToolBar->addAction(attachDetachAct_);
    fileToolBar->addAction(undoAct_);
    fileToolBar->addAction(findAct_);
    fileToolBar->addAction(nextAct_);
    fileToolBar->addAction(printAct_);
    fileToolBar->addAction(printPreviewAct_);
    fileToolBar->addAction(closeLatticeAct_);
    fileToolBar->addAction(betasNewAct_);
    fileToolBar->addAction(functionsAct_);
    fileToolBar->addAction(matrixAct_);
    fileToolBar->addAction(integralsAct_);

    fileToolBar->addAction(closeSymmetricalAct_);
    fileToolBar->addAction(fitBetasAct_);
    fileToolBar->addAction(stopAct_);
    fileToolBar->addAction(ringLineToggleAct_);
    fileToolBar->addAction(designReferenceOrbitToggleAct_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimMainWindow::createStatusBar()
 {
     statusBar()->showMessage(tr("Ready"));
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimMainWindow::readSettings()
 {
     QSettings settings;
     QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
     QSize size = settings.value("size", QSize(400, 400)).toSize();
     move(pos);
     resize(size);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimMainWindow::writeSettings()
 {
   QSettings settings;
   settings.setValue("pos", pos());
   settings.setValue("size", size());
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QMdiSubWindow* OptimMainWindow::activeMdiChild()
 {
     if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return activeSubWindow; 
     return 0;
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QMdiSubWindow* OptimMainWindow::findOptimEditor(const QString &fileName)
 {
     QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

     foreach (QMdiSubWindow* window, mdiArea->subWindowList()) {
         OptimEditor* editor = qobject_cast<OptimEditor*>(window->widget());
         if (!editor) continue;

	 if ( (editor->currentFile() == canonicalFilePath) || (fileName == "") )
	   return  window;
     }
     return 0;
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


QMdiSubWindow* OptimMainWindow::findOptimEditor()
{
     foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
         OptimEditor* editor = qobject_cast<OptimEditor*>(window->widget());
         if   (!editor) continue;
         else return window;
     }
     return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void OptimMainWindow::setActiveSubWindow(QObject *window)
 {
     if (!window)
         return;
     mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
 }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::cmdExit()
{

  int st = 0;
  
  int status = QMessageBox::question(this, "OptiMX", "Do you really want to exit ? ", QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel);

  switch (status) {

    case QMessageBox::Ok:
      
      interrupted_ = true;
  
      writeSettings();
      mdiArea->closeAllSubWindows();

      qApp->exit(0);
      break;
      
    default:
      break;

  }
  return status;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::print_elm(Element const* el, char*  buf)
{
  using fmt::format;
  std::string str;
  
  switch ( el->etype() ) {
		
     case 'O': 
       str = format("{:s} L[cm]={:<8.8g}",el->name(), el->length());
       break;
     case 'I':
       str = format("{:s} L[cm]={:<8.8g} Tilt[deg]={:<6.3g}",el->name(), el->length(), el->tilt());
       break;
     case 'H': 
       str = format("{:s} Ax[cm]={:<g} Ay[cm]={:<g} Shape={:<d} OffsetX[cm]={:<g} OffsetY[cm]={:<5.3g} Tilt[deg]={:<5.3g}",
                   el->name(), el->S, el->A, el->N, el->B, el->G, el->tilt());
       break;
     case 'Q':
       str = format("{:s}  L[cm]={:<8.8g} G[kG/cm]={:<7.7g} Tilt[deg]={:<5.2g} offsX[cm]={:<.3g} offsY[cm]={:<.3g}",
	       el->name(), el->length(), el->G, el->tilt(), el->offsX(), el->offsY());
       break;
     case 'L':
       str = format("{:s} L[cm]={:<8.8g} Ge[kV/cm^2]={:<g} ", el->name(), el->length(), el->G);   // electrostatic quad
       break;
     case 'F':
       str = format("{:s} L[cm]={:<8.8g} j[kA/cm^2]={:<g}",  el->name(), el->length(), el->G);  // lithium lens
       break;
     case 'G': 
       str = format("{:s} B[kG]={:<8.8g} Angle[deg]={:<g}  EffLen[cm]={:<g}  Tilt[deg]={:<g}", el->name(), el->B, el->G, el->S, el->tilt());
       break;
     case 'C':
       str = format("{:s} L[cm]={:<8.8g} B[kG]={:<8.8g} Aperture[cm]={:<8.8g}", el->name(), el->length(), el->B, el->A);
       break;
     case 'D': 
     case 'B':
       str = format("{:s} L[cm]={:<8.8g} B[kG]={:<8.8g} G[kG/cm]={:<7.7g} Tilt[deg]={:<g}", el->name(), el->length(), el->B, el->G, el->tilt());
       break;
     case 'R':
       str = format("{:s} L[cm]={:<8.8g} B[kG]={:<8.8g} Gb[kG/cm]={:<g} E[kV/cm]={:<8.8g} Ge[kV/cm^2]={:<g}",
		   el->name(), el->length(), el->B, el->G, el->S, el->tilt());
       break;
     case 'S': 
       str = format("{:s} L[cm]={:<8.8g} S[kG/cm/cm)]={:<g} Tilt[deg]={:<g}", el->name(), el->length(), el->S, el->tilt());
       break;
     case 'M':
       str = format("{:s} Order:m={:<d} Bm*L[kG/cm^(m-1)]={:<g} Tilt[deg]={:<g}", el->name(), el->N, el->S, el->tilt());
       break;
     case 'K':
       str = format("{:s} L[cm]={:<8.8g} B[kG]={:<g} Tilt[deg]={:<g}", el->name(), el->length(), el->B, el->tilt());
       break;
     case 'E':
       str = format("{:s} L[cm]={:<8.8g} Delta_E[MeV]={:<g}", el->name(), el->length(), el->B);
       break;
     case 'A':
       str = format("{:s} L[cm]={:<8.8g} Ncell={:<d} Eff_L[cm]={:<g}  A[MeV]={:<8.8g} Phase[deg]={:<g}  WaveL[cm]={:<g}",
		    el->name(),   el->length(),  el->N,  el->B,        el->G,      el->S,         el->tilt());
       break;
     case 'W':
       str = format("{:s} L[cm]={:<8.8g}  A[MeV]={:<8.8g}  Phase[deg]={:<g}  WaveL[cm]={:<g} FileRef={:<d}",
		    el->name(),   el->length(),  		el->G,      	el->S,         el->tilt(),		el->N);
       break;
     case 'Z':
       str = format("{:s} L[cm]={:<8.8g} DE[MeV]={:<8.8g}",
		    el->name(),   el->length(),  	el->G);
       break;
     case 'X':
       str = format("{:s} Energy increase[MeV]={:<8.8g} L[cm]={:<8.8g}", el->name(),   el->G,   el->length());
       break;
     case 'T':
       str = format("{:s} Rms angle[mrad]={:<g}  1/L*dL/dx[1/cm]={:g} Tilt[deg]={:<g}", el->name(),   el->B,   el->G, el->tilt());
       break;
     case 'U':
       str = format("{:s} Rms loss[MeV]={:<g} 1/L*dL/dx[1/cm]={:<g} Tilt[deg]={:<g}", el->name(),   el->B,   el->G, el->tilt());
       break;
  default:
    str = "";
  } // switch

  strcpy(buf, str.c_str());  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::closeTraject (Coordinates& vf)
{

  RMatrix tm;
  Coordinates v;
  double m[4][4], minv[4][4];
  char err[256];
  
  findRMatrix(tm);
  v[0] = v[1] = v[2] = v[3] = v[4] = 0.0;
  v[5] = vf[5];
  transferTraject (v, v); // compute trajectory offset from transfer matrix 

  for(int i=0; i<4; ++i) { for(int j=0; j<4; ++j) m[i][j] =-tm[i][j];}
  for(int i=0; i<4; ++i) { m[i][i] += 1.0;}
  
  err[0] = 0;

  Utility::matr_invert(4, m, minv, err);
  
  if(err[0]){

    OptimMessageBox::warning(this, "Close Error", 
      	"Cannot close trajectory. Make sure that Twiss parameters can be closed.",
      	QMessageBox::Ok);
    return 1;
  }
   
  vf[0] = minv[0][0]*v[0] + minv[0][1]*v[1] + minv[0][2]*v[2] + minv[0][3]*v[3];
  vf[1] = minv[1][0]*v[0] + minv[1][1]*v[1] + minv[1][2]*v[2] + minv[1][3]*v[3];
  vf[2] = minv[2][0]*v[0] + minv[2][1]*v[1] + minv[2][2]*v[2] + minv[2][3]*v[3];
  vf[3] = minv[3][0]*v[0] + minv[3][1]*v[1] + minv[3][2]*v[2] + minv[3][3]*v[3];

  
  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::closeTrajectFull (Coordinates& vf)
{
  QCursor saved_cursor = cursor();
  setCursor(Qt::BusyCursor);
  
  
  Coordinates v = vf;
  
  int ncl = (NstTool.Closure4D) ?  4 : 6;
 
  int i=0; 
  for(i=0; i < Ncvg; ++i) {

    if (transferTraject(v,vf)) break;
    if (NstTool.Closure4D){ vf[4] = v[4]; vf[5]=v[5];}

      double err = 0; 
      double sum = 0;

      for(int j=0; j<ncl; ++j){

  	vf[j] = (1.0 - cnvg)*v[j] + cnvg*vf[j];
        err   += fabs(vf[j] - v[j]);
        sum   += fabs(vf[j]);
        v[j] = vf[j];
      }

      if ( err <= CvgErr*sum)              break; // relative error 
      if ((sum < 1.0e-6) && (err<1.0e-6))  break; // absolute error
    }

  setCursor(saved_cursor);

  return  (i< (Ncvg-1) ) ? 0 : 1;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::cmdCloseTraject()
{
  Coordinates v;
   
  if(interrupted_) {interrupted_ = false; return 1; }

  if(analyzed_) { if(analyze(false)) return 1; } else { if( analyze(true) ) return 1;}
   
  getTrajParamFromFile(false, false, v);

  if(closeTraject(v))  return 0; // Always perform linear closure 

  if( NstTool.FullClosure ) { 
    if ( closeTrajectFull(v)) {
      OptimMessageBox::warning( this, 
      "Close Error", "Cannot close trajectory. Make sure that the Twiss parameters can be closed. If OK reduce the convergence coefficient.",
      QMessageBox::Ok);
      return 1;
    }
  }

  getTrajParamFromFile(false, true, v); // update the parameter block

  return 0;
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::getTrajParamFromFile(bool Reprint, bool Update, Coordinates& v)

{
  
  char const* bufa[]={"TrajParamBlock",
		 "\tX[cm]=0.0 \tTeta_X[rad]=0.0",
		 "\tY[cm]=0.0 \tTeta_Y[rad]=0.0",
		 "\ts[cm]=0.0 \tDeltaP/P=0.0",
		 "EndTrajParamBlock\n"};
  
   int  buflen = sizeof(bufa)/sizeof(char*);

 double dat[2];
 char buf[257]; 
 
 if (!LatticeCh_ ) return 1;
 OptimEditor* editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());     
 if (!editor) return 1;
 
 int nmblines = editor->document()->blockCount ();

 // look for "EndTrajParamBlock"
 // when found, nline points to this marker 
 int nline = 0;
 for( nline = LineLFin; nline<nmblines; ++nline) {
   strcpy(buf, editor->getLine(nline).toUtf8().data());
   if(strcmpr(bufa[0], buf)) break; 
 }
   
 // Create TrajParamBlock if none was found 
   
 if(nline == nmblines) {
   
   if( OptimInformationMessage( this, "Trajectory", "There is no TrajParamBlock. Do you want to create it?",
			    QMessageBox::Yes | QMessageBox::No) ==  QMessageBox::No ) {
       return 1;
   }
 
     replaceLine(editor, nline-1, buf);
     QTextCursor cursor = editor->textCursor();
     cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor); 
     
     for( int i=0; i<buflen; i++){ 
         cursor.insertBlock();
         cursor.insertText(bufa[i]);
     }
     for( int i=0; i<6; ++i) v[i]=0.;
     return 1;
 }
   
 // decode trajectory parameters
   
 nline++;

 nline = getLineCmt(editor, buf, LSTR, nline);
 if (nline < 0) return 1;

 if(!Update){ decodeLine(buf,dat,2);  v[0]=dat[0];  v[1]=dat[1];}
 if( Reprint||Update) {
   sprintf(buf,"\tX[cm]=%lg \tTeta_X[rad]=%lg",v[0], v[1]);
   replaceLine(editor, nline-1, buf);
 }

 nline = getLineCmt(editor, buf, LSTR, nline);
 if(!Update){ decodeLine(buf,dat,2);  v[2]=dat[0];  v[3]=dat[1];}
 if(Reprint||Update){
   sprintf(buf,"\tY[cm]=%lg \tTeta_Y[rad]=%lg",v[2], v[3]);
   replaceLine(editor, nline-1, buf);
 }

 nline = getLineCmt(editor, buf, LSTR, nline);
 if(!Update ){ decodeLine(buf,dat,2);  v[4]=dat[0];  v[5]=dat[1];}
 if((Reprint||Update) && (!NstTool.Closure4D)) {
   sprintf(buf,"\ts[cm]=%lg \tDeltaP/P=%lg",v[4], v[5]);
   replaceLine(editor, nline-1, buf);
 }

 return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double OptimMainWindow::findRMatrix(RMatrix& tm)
{

  // compute transfer matrix for the whole beamline
  // returns: tm and enr, the total (kinetic) energy 

  double tetaY = tetaYo0_;
  double Enr   = Ein;
  
  tm.toUnity();
  for (int i=0; i<nelm_; ++i){
     auto ep    = beamline_[i];
     RMatrix me = ep->rmatrix(Enr, ms, tetaY, 0.0, 3.0); // 3 ==> full element ? 
     tm = me*tm;
  }
  return Enr;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::setInitialBetas(Twiss& v)
{
   v.BtX  = BetaXin;
   v.BtY  = BetaYin;

   v.AlX  = AlfaXin;
   v.AlY  = AlfaYin;

   v.DsX  = DispXin;
   v.DsY  = DispYin;

   v.DsXp = DispPrimeXin;
   v.DsYp = DispPrimeYin;

   v.nuY = v.nuX =0.0; // why not get nuX and nuY from the input file ? 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::transferTraject (Coordinates const& vin, Coordinates& vout)
{

  // trajectory computed using the (linear) transfer matrices 
  
  RMatrix me;

  TrackParam prm;
  char buf[128];
  
  vout = vin;
  
  double tetaY= tetaYo0_;
  double Enr  = Ein;
  double Hrt  = sqrt(Enr*(2.0*ms+Enr))/Constants::C_DERV1; // momentum
  
  if(NstTool.LinClosure){

    for(int i=0; i<nelm_; ++i) {

      auto ep = beamline_[i];
      switch( ep->etype() ) {
      
        case 'E': 
        case 'X':  
        case 'W': 
        case 'A':
	  {
           double EnrNew = Enr;
           me   = ep->rmatrix( EnrNew, ms, tetaY, 0.0, 3);
	   vout.c = me*vout.c;
           double capa = sqrt(sqrt((2.*Enr*ms+Enr*Enr)/(2.*EnrNew*ms+EnrNew*EnrNew)));
           vout[0] *= capa;
	   vout[1] *= capa;
	   vout[2] *= capa;
	   vout[3] *= capa;
           vout[5] *= capa*capa*(ms+EnrNew)/(ms+Enr);
           Enr = EnrNew;
           Hrt = sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;
	  }
          break;
        case 'K':
	  {
           double dtx = ep->length()*ep->B/Hrt*cos(PI*ep->tilt()/180.);  //***FIXME*** Why do this here ? 
           double dty = ep->length()*ep->B/Hrt*sin(PI*ep->tilt()/180.);
           vout[0] += ep->length() * ( vout[1] + 0.5 *dtx);
           vout[1] += dtx;
           vout[2] += ep->length() * ( vout[3] + 0.5 *dty);
           vout[3] += dty;
           me   = ep->rmatrix( Enr, ms, tetaY, 0.0, 3);  //***FIXME*** Why does this need to be called ?
	  }
	  break;
        case 'Z':
          vout[0] += ep->length() * vout[1];
          vout[2] += ep->length() * vout[3];
          vout[5] += ep->G*(Enr+ms)/(Enr*Enr+2.*Enr*ms);
          me   = ep->rmatrix( Enr, ms, tetaY, 0.0, 3);  //***FIXME*** Why does this need to be called ?
          break;
        default:
          me   = ep->rmatrix( Enr, ms, tetaY, 0.0, 3);
          vout.c = me*vout.c;
      } // switch
    } // for
  }
  else { // LinClosure == false   we track along the excited trajectory 
    
     for( int i=0; i<nelm_; ++i) {
       auto ep = beamline_[i];
       ep->preTrack (ms, Enr,  tetaY, i, prm, me);
       if(ep->trackOnce( ms, Enr, i, 0, prm, me, vout)) {
      	 sprintf(buf,"Particle is lost at Element %d (%s)", i+1, ep->name());
         return 1;
       }
       me   = ep->rmatrix( Enr, ms, tetaY, 0.0, 3);  //***FIXME*** Why does this need to be called ?
     }
  }
  
  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::getDataFromFile(char* bufinp)
{

  char buf[LSTR],filenm[80], *bin, *bfin;
  int i,n;
  //getting file reference
  strcpy(buf,bufinp);
  bin=buf;
  
  while((*bin==' ' || *bin=='\t') && *bin !='\000' ) bin++;

gdferr:

  if( *bin == 0) {
    sprintf(buf,"Syntax error at line %s", bufinp);
    OptimMessageBox::warning(this,"Parsing error", buf, QMessageBox::Ok);
    return 1;
   }

   bfin = bin;
   
   while(*bfin!=' ' && *bfin!='\t' && *bfin!='\000') bfin++;
   if (*bfin==0) goto gdferr;
   *bfin++ = 0;
   n = atoi(bin);

   //getting file name

   if( !getFileNameOpt(bfin, MainFileDir, filenm) ) goto gdferr;
   
   // Memory allocation
   
   if((n>=MAXFILES)||(n<0)){
     sprintf(buf,"File reference number of %d is outside allowed range of [0,%d]",
		  n,MAXFILES-1);
     OptimMessageBox::warning(this,"Parsing error", buf, QMessageBox::Ok);
     return 1;
   }
   
   if(ext_dat[n].n != 0){
     OptimMessageBox::warning(this, "Parsing error", "Duplicate reference to an external file", QMessageBox::Ok);
     return 1;
   }

   auto fhdel = [](FILE* p) { (p ? std::fclose(p) : 0);};  
   FILE* fp = fopen(filenm,"r"); 
   std::unique_ptr<FILE, decltype(fhdel)> sfp(fp, fhdel);
   if((fp=fopen(filenm,"r")) == 0) {
     OptimMessageBox::warning(this, "I/O Error", (QString("Could not open file") + filenm).toUtf8().data(), QMessageBox::Ok);
    return 1;
   }
   i=0;
   
   while(fgets(buf, LSTR-1, fp) ) if(buf[0]!='#')i++;
   //fclose(fp);

   if(i<5) {

gdffer:	
     OptimMessageBox::warning(this, "Parse error: external file is too short-", filenm, QMessageBox::Ok );
     return 1;
   }
   

   try {
    ext_dat[n].x.resize(i);
    ext_dat[n].y.resize(i);
    ext_dat[n].v.resize(i);
   }
   catch (...) {
      OptimMessageBox::warning(this, "Error", "Cannot allocate memory", QMessageBox::Ok );
      return 1;
   }
   
   ext_dat[n].n = 1;

   // Read external file ...
    auto fhdel2 = [](FILE* p) { (p ? std::fclose(p) : 0);};  
    fp = fopen(filenm,"r");
    std::unique_ptr<FILE,decltype(fhdel2)>sfp1(fp,fhdel2);
    i = 0;
   
   while( fgets(buf, LSTR-1, fp) ) {

     if(buf[0]=='#') continue;

     if(sscanf(buf,"%le %le ", &(ext_dat[n].x[i]), &(ext_dat[n].y[i]))==2) ++i;

     if(i>2) if(ext_dat[n].x[i-1] <= ext_dat[n].x[i-2]) {
       OptimMessageBox::warning(this, "Error: X coordinate must be monotone increasing.", filenm, QMessageBox::Ok );
       return 1;
     }
   }
   //fclose(fp);
   if(i<5) goto gdffer;
   
   ext_dat[n].n=i;

   // Vector for spline interpolation

   spline(&ext_dat[n].x[0], &ext_dat[n].y[0], ext_dat[n].n, &ext_dat[n].v[0]);
   
   return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::closeLat2 (bool sym)
{

  char buf[256]; 
  char const *cher[3]={"X","Y","X&Y"};
  Twiss v;
  
  if (interrupted_) { interrupted_ = false; return 0;}
  
  // calculate transfer matrix and tunes
  
  if(analyzed_) { 
    ierr =  analyze(false); 
  } 
  else { 
    ierr = analyze(true);
  }
  
  if (ierr) return 1;
   
  RMatrix tm;

  findRMatrix(tm); // tm matrix for the whole ring 
  
  double alfa = 0; // momentum compaction factor

  int ierr = 0;

  static int diag[] = {1, -1,  1, -1,  1,  1 }; // momentum inversion symmetry transformation (x = x, px = - px , y = y,  py = -py , z = z,  pz = pz) 
                                                // note pz -> -pz needed for alpha_z = 0. In this case, the dispersion derivative will not be zero.   

  static RMatrix rfx = RMatrix::reflection(diag); 

  if(sym) {

    //-----------------------------------------------------------------------------------------------
    // To create a symmetric period, we need to go through the elements of the line in reverse order.
    // assume the lattice is made of n elements with matrices A_1 ... A_n so R = A_1*A_2* .. A_n 
    // We need to compute A_n*A_n-1* ... A_1.
    // To avoid doing doing this explicitly we note that if we compute the inverse of R,
    // R^(-1) =  A_n^(-1) *A_n-1^(-1) * ... A_1(-1)
    // the order is reversed since we are effectively moving backwards.
    // the symmetry transformation effectively reverse the focusing in
    // the inverse so as to make it the same as in the original line. 
    //-----------------------------------------------------------------------------------------------
    
    RMatrix inverse   = -RMatrix('J')*tm.transpose()*RMatrix('J');  // matrix inverse  
    RMatrix mf        = rfx*inverse*rfx.transpose()*tm;             // symmmetry transform 

    ierr   = find_tunes(mf, 2*Length_, v, &alfa);
  }
  else  { 

    ierr = find_tunes(tm, Length_, v, &alfa);
  }
  
  double gamma = 1.0 + Ein/ms; 
  double gm2   = gamma*gamma;  


  // Note: tm[4][0] = dz/dx etc ... ;   (z < 0)  == > orbit is longer
  //       by definition, mcf alpha = (dL/L) / (dp/p) 

  alfa =   - ( v.DsX * tm[4][0] + v.DsXp * tm[4][1] +
	       v.DsY * tm[4][2] + v.DsYp * tm[4][3] + ( tm[4][5] - Length_ / gm2 ) ) / Length_;

  if(ierr){
   strcpy(buf,"Cannot close for "); strcat(buf, cher[ierr-1]);
   OptimMessageBox::warning(this, "Close Error", buf, QMessageBox::Ok);
   return 1;
  }
 
  OptimTextEditor* editor = 0 ;
  auto DigCh = getAttachedSubWin(WindowId::DigCh);

  if (!DigCh ) { 
    DigCh = createAttachedSubWin( (editor = new OptimTextEditor()),  WindowId::DigCh);
    connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
    connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
    connect(editor, SIGNAL(redoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
  }
  else { 
     editor = qobject_cast<OptimTextEditor*>(DigCh->widget() );
     if (CtSt_.ClearText) {  
        editor->clear();
     }     else { 
       QTextCursor cursor = editor->textCursor(); 
       cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
     }
  }

  // Print results
  typeBetas(editor, v, alfa, -1., 0);

  // Substitutes values of betas and dispersions into the lattice text file
  substituteBetas (v);
  return  0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimMainWindow::typeBetas(OptimTextEditor* editor, Twiss& v, double alfa, double enr, int elem) 
{
  char buf[256]; 
   if(CtSt_.ClearText) {
      editor->clear();
   }
   else{
      editor->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
      editor->insertPlainText("\n");
   }

   if(elem) { 
     sprintf(buf,"Lattice functions at the downstream end of Element %d (%s)\n\n", elem, beamline_[elem-1]->fullName() );
   }
   else { 
     sprintf(buf,"Lattice functions at the downstream end of the line :\n\n");
   }
   
   editor->insertPlainText(buf);

   editor->insertPlainText(QString::asprintf("%-12s %1s %12g %-12s %1s %12g\n", "BetaX[cm]", "=", v.BtX,  "BetaY[cm]", "=",  v.BtY));
   editor->insertPlainText(QString::asprintf("%-12s %1s %12g %-12s %1s %12g\n", "AlphaX",    "=", v.AlX,  "AlphaY",     "=",  v.AlY));
   editor->insertPlainText(QString::asprintf("%-12s %1s %12g %-12s %1s %12g\n", "Qx",        "=", v.nuX,  "Qy",        "=",  v.nuY));
   editor->insertPlainText(QString::asprintf("%-12s %1s %12g %-12s %1s %12g\n", "DispX[cm]", "=", v.DsX,  "DispY[cm]", "=",  v.DsY));
   editor->insertPlainText(QString::asprintf("%-12s %1s %12g %-12s %1s %12g\n", "DispPrX",   "=", v.DsXp, "DispPrY",   "=",  v.DsYp));

  if( enr> 0.0 ) {
    editor->insertPlainText(QString::asprintf( "%-10s %1s %12g\n", "Energy [MeV]", "=", enr));
   }
   else {
     editor->insertPlainText("\n");
   }

   if(!elem){

     if(NmbPer>1){

       editor->insertPlainText(QString::asprintf("%-10s %1s %12g %-10s %1s %12g\n", "Qx*Nper", "=", v.nuX * NmbPer, "Qy*Nper", "=", v.nuY * NmbPer));
     }

     editor->insertPlainText(QString::asprintf("%-24s = %12g\n", "Momentum Compaction", alfa));
       
     if(CtSt_.CompAtExcitedOrb) {

       editor->insertPlainText(QString::asprintf("%-40s = %12g\n\n", "Path lengthening for excited orbit [cm]", DeltaL_));

       editor->insertPlainText(QString::asprintf( "%-40s  %12s\n", "Initial trajectory parameters for", (CtSt_.IsRingCh ? "RING":"LINE")));

       editor->insertPlainText(QString::asprintf("%-10s = %12f cm\n",   "x",     v_anlz[0]));
       editor->insertPlainText(QString::asprintf("%-10s = %12f mrad\n", "teta_x",v_anlz[1]*1000));

       editor->insertPlainText(QString::asprintf("%-10s = %12f cm\n",   "y",     v_anlz[2]));
       editor->insertPlainText(QString::asprintf("%-10s = %12f mrad\n", "teta_y",v_anlz[3]*1000));

       editor->insertPlainText(QString::asprintf("%-10s = %12f cm\n",   "s",     v_anlz[4]));
       editor->insertPlainText(QString::asprintf("%-10s = %12f mrad\n", "dp/p",  v_anlz[5]*1000));

     }
   }

   editor->insertPlainText("\n---------------------------------------------------------------------------------\n\n");

   editor->document()->setModified(false); 
   editor->show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::substituteBetas ( Twiss& v)
{

   char buf[LSTR+1];

   if (!LatticeCh_) return;

   OptimEditor* editor = qobject_cast<OptimEditor*>( LatticeCh_->widget() );

   BetaXin=v.BtX;        BetaYin=v.BtY;
   AlfaXin=v.AlX;        AlfaYin=v.AlY;
   DispXin=v.DsX;        DispYin=v.DsY;
   DispPrimeXin=v.DsXp;  DispPrimeYin=v.DsYp;

   int CurLine = editor->textCursor().blockNumber(); 

   int nline=0;
   while(true){ // "OptiM"
	
         nline=getLineCalc(editor, buf, LSTR, nline);
	 if(nline<0) return;
	 if(strcmpr("OptiM",buf))break;
   }
   
   nline = getLineCmt(editor, buf, LSTR, nline); //" Energy and Hr[kG*cm]=%g "
   nline = getLineCmt(editor, buf, LSTR, nline); //" Emittances "

   nline = getLineCmt(editor, buf, LSTR, nline);
   sprintf(buf,"Initial:\tBetaX[cm]=%g \tBetaY[cm]=%g ",BetaXin,BetaYin);
   replaceLine(editor, nline-1, buf);

   nline = getLineCmt(editor, buf, LSTR, nline);
   sprintf(buf,"\tAlphaX=%g \tAlphaY=%g ",AlfaXin,AlfaYin);
   replaceLine(editor, nline-1, buf);

   nline = getLineCmt(editor, buf, LSTR, nline);
   sprintf(buf,"\tDispersX[cm]=%g \tDispersY[cm]=%g ",DispXin,DispYin);
   replaceLine(editor, nline-1, buf);

   nline = getLineCmt(editor, buf, LSTR, nline);
   sprintf(buf,"\tDsp_PrimeX=%g \tDspPrimeY=%g ",DispPrimeXin,DispPrimeYin);
   replaceLine(editor, nline-1, buf);

   strcpy(buf, editor->getLine(CurLine).toUtf8().data() ); // editor->getLine(buf, LSTR, CurLine);
   replaceLine(editor, CurLine, buf);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  OptimMainWindow::cmdCloseSym ()
{
   closeLat2 (true);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  OptimMainWindow::cmdCloseLattice ()
{
   return closeLat2 (false);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdLBetas ()
{
  //std::cout << " OptimMainWindow::cmdLBetas" << std::endl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdTuneDiagram()
{
  static TuneDiagramDialog*  dialog = 0; 
  if (!dialog) { 

    dialog = new TuneDiagramDialog(0);
    dialog->data_.qxmin              = 0.0;
    dialog->data_.qxmax              = 1.0;
    dialog->data_.qymin              = 0.0; 
    dialog->data_.qymax              = 1.0;
    dialog->data_.minorder           = 0; 
    dialog->data_.maxorder           = 5;
    dialog->data_.sumresonances      = true; 
    dialog->data_.couplingresonances = true; 
    dialog->data_.captions           = false;
    dialog->data_.qxintervals        = 10; 
    dialog->data_.qyintervals        = 10; 
    dialog->data_.qx                 = 0.0;
    dialog->data_.qy                 = 0.0;

  };

  if ( dialog->exec() == QDialog::Rejected ) return;   

  OptimTuneDiagram* diagram  = 0;

  static QPointer<QMdiSubWindow> DiagramSw;
  
  if (!DiagramSw) { 
     DiagramSw = mdiArea->addSubWindow( (diagram = new OptimTuneDiagram(0)) );
     DiagramSw->setGeometry( mdiArea->geometry().width()/2,  0, mdiArea->geometry().width()/2,  mdiArea->geometry().height());
     DiagramSw->setWindowTitle("Resonance Diagram");
  } 

  
  diagram = qobject_cast<OptimTuneDiagram*>( DiagramSw->widget() );  
  diagram->setup( &dialog->data_);
  diagram->replot();
  diagram->show();
  
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdViewMatrix()
{
  if(interrupted_ ) { interrupted_ = false; return;}
  
  if( analyzed_) {
     if(analyze(false)) return;
  } else {
     if(analyze(true)) return;
  }
  
  static MatrixDialog* dialog = 0;
  if (!dialog) dialog = new MatrixDialog(0);
  if ( dialog->exec() == QDialog::Rejected) return;

  // get indices of first and last element  

  int m1   = dialog->data_.efirst;
  int m2   = dialog->data_.elast;
  bool symplecticity_test = dialog->data_.symplecticity_test;


  // ajust indices to prevent out-of-bound access
    
  if( m1<1      ) m1 = 1;
  if( m1 > nelm_) m1 = nelm_;
  if( m2<m1     ) m2 = m1;
  if( m2>nelm_  ) m2 = nelm_;

  RMatrix tm;
  RMatrix me;

  char  buf[256];
  char buf1[256];

  double Enr   = Ein;
  double tetaY = tetaYo0_;

  //multiply matrices
  
  for (int i=0; i<m1-1; ++i){ // this is needed to get the energy right at element m1-1
    auto ep1 = beamline_[i];
    me = ep1->rmatrix(Enr, ms, tetaY, 0.0, 3);
 }
  tm.toUnity();

  double en = Enr;
  double ty = tetaY;

  for (int i=m1-1; i<m2; ++i){
    auto ep1 = beamline_[i];
    me = ep1->rmatrix(Enr, ms, tetaY, 0.0, 3);
    tm = me*tm;  
  }

  //--------------------
  // symplecticity tests 
  //--------------------

  RMatrix smtx('J');// symplectic matrix
  RMatrix stest =  tm * smtx *tm.transpose() - smtx; 

  std::complex<double> lambda[6];
  std::complex<double> ev[6][6];
  tm.findEigenValues(lambda, ev); 

  std::complex<double> det = lambda[0];
  for (int i=1; i<6; ++i) { det *= lambda[i];};
  
  //  Print results
  
  OptimTextEditor* editor = 0 ;
  auto DigCh =  getAttachedSubWin(WindowId::DigCh);
  if (!DigCh) { 
    DigCh = createAttachedSubWin( (editor = new OptimTextEditor()), WindowId::DigCh);
    connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
    connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
    connect(editor, SIGNAL(redoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
  }
  else { 
    editor = qobject_cast<OptimTextEditor*>(  DigCh->widget() );  
    if (CtSt_.ClearText) editor->clear();
  }
  if (!editor)return;
  DigCh->raise();
  
  
  auto ep1 = beamline_[m1-1]; 
  auto ep2 = beamline_[m2-1];

  editor->insertPlainText(QString::asprintf("From Element %d(%s) to Element %d(%s)", m1,ep1->fullName(),m2,ep2->fullName()));
  editor->insertPlainText("\n");
  
  editor->insertPlainText(QString::asprintf("Matrix. Energy increase[MeV]=%g (From %g to %g) Delta TetaY=%g (From %g to %g)", Enr-en, en, Enr, tetaY-ty, ty, tetaY));
  editor->insertPlainText("\n");

  std::string str = fmt::format("{:>13s} {:>13s} {:>13s} {:>13s} {:>13s} {:>13s}\n","X[cm]", "Px", "Y[cm]", "Py", "ds[cm]", "dP/P");
  editor->insertPlainText(str.c_str());

  for (int i=0; i<6; i++){
    str = "";
    for (int j=0; j<6; j++){
      str += fmt::format("{:13.6g} ", tm[i][j]);

    }
    editor->insertPlainText(str.c_str());			
    editor->insertPlainText("\n");
  }

  editor->insertPlainText("\n\n\n");
  
  if ( symplecticity_test )  {
    editor->insertPlainText(QString::asprintf("Symplecticity Test [1]: Stest = ( S - M * S * M^T) (All entries should be 0.0)\n\n"));

    for (int i=0; i<6; i++){
      buf[0]=0;
      for (int j=0; j<6; j++){
        sprintf(buf1,"%13.6e\t", stest[i][j]);
        strcat(buf, buf1);
      }
      editor->insertPlainText(buf);			
      editor->insertPlainText("\n");
    }   
    editor->insertPlainText("\n");

    editor->insertPlainText(QString::asprintf("Symplecticity Test [2]: det(M) should be purely real with magnitude 1.0 \n\n"));
    editor->insertPlainText(QString::asprintf("det(M)        = %13.6e + i%13.6e\n", std::real(det), std::imag(det)));
    editor->insertPlainText(QString::asprintf("abs(det(M))-1 = %13.6e\n", std::abs(det)-1.0));
   }
  
  editor->document()->setModified(false);
  DigCh->show(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdViewControl ()
{
   if(interrupted_ ){ interrupted_ = false; return; }

   static ControlDialog* dialog = 0;
  
   if (!dialog)  {
     dialog = new ControlDialog(0);  
   }  

   dialog->data_ =  CtSt_;
   dialog->set();

   if ( dialog->exec() == QDialog::Rejected) return;

   // arraylen        = dialog->data_.ArrayLen; 
   n_cav_int       = dialog->data_.NStep; 
   // cav_accuracy    = dialog->data_.Accuracy; 
   // cav_accuracy_L  = dialog->data_.AccuracyL; 
   // CouplThreshold  = dialog->data_.CouplThreshold;

   CtSt_ = dialog->data_;

   char buf[256];
   sprintf(buf, "%s (%s", qApp->applicationName().toUtf8().data(), ( CtSt_.IsRingCh ? "RING": "LINE" ));
   strcat(buf, CtSt_.CompAtExcitedOrb ? ", reference orbit mode)" :  ")" );
   setWindowTitle(buf);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::PrintBdL(int n, std::string names[], int nln, char* buf)
{
  char const* cpname = 0;
  char  buf1[256];  // *** FIXME ***

  auto ep = beamline_[n];
  if(!NStLat_.printFirstLetter) {
     if(( (ep->name()[1]>='0') && (ep->name()[1]<='9') ) || (strlen(ep->name())==1) ) cpname=ep->name();
     else cpname = &ep->name()[1];
   }
   else cpname=ep->name();
   
  if(!Utility::filterName(ep->name(), NStLat_.Filter, NStLat_.MatchCase)) return 1;

   names[nln] =  std::string(cpname);
   strcpy(buf1, names[nln].c_str());
   std::string str;
   
   switch ( ep->etype() ) {
     case 'Q':
     case 'L':
       strcat(buf1,".BDL");
       //sprintf(buf,"%s 1 %-9.3f ", buf1, ep->length()*ep->G*1000.);
       str = fmt::format("{:24s} 1 {:-16.5g}", buf1, ep->length()*ep->G*1000.);
       strcpy(buf,str.c_str());
       break;
     case 'B':
     case 'D':
     case 'K':
     case 'C':
       strcat(buf1,".BDL");
       //       sprintf(buf,"%s 1 %-9.3f ", buf1, ep->length()*ep->B*1000.);
       str = fmt::format("{:24s} 1 {:-16.5g}", buf1, ep->length()*ep->B*1000.);
       strcpy(buf,str.c_str());
       break;
     case 'R':
     case 'S':
       strcat(buf1,".BDL");
       //sprintf(buf,"%s 1 %-9.3f ", buf1, ep->length()*ep->S*1000.);
       str = fmt::format("{:24s} 1 {:-16.5g}", buf1, ep->length()*ep->S*1000.);
       strcpy(buf,str.c_str());
       break;
     case 'A':
     case 'W':
       strcat(buf1,".GSET");
       //sprintf(buf,"%s 1 %-9.3f ", buf1, ep->G);
       str = fmt::format("{:24s} 1 {:-16.5g}",buf1, ep->G);
       strcpy(buf,str.c_str());
       break;
     default: buf[0]=0;
   }
 
   if(buf[0]){
     int i=0;
     while(i<nln){
       if(strcmpre(names[i++].c_str(), ep->name())) return 1;
     }
     return 0;
   }
   return 1;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdViewLattice()
{

  char buf[LSTR+1], buf1[LSTR+1] ; // what is buf1 ??? 
  char const* pname;
  int nln;
  double SL = 0.0;

  if(interrupted_ ) {interrupted_ = false; return;}

  if( analyzed_) {
    if(analyze(false)) return;
  } 
  else {
    if(analyze(true)) return;
  }

  static LatticeDialog* dialog = 0;

  //char  Filter[1024];     // Filter
  //bool  MatchCase;
  //int   MltpOrder;      // order for multipoles
  //bool  printBdL;
  //bool  printFirstLetter;
  //bool  StartSFromFile;

  if (!dialog) {
    dialog = new LatticeDialog(0);
    dialog->data_  = NStLat_;
    dialog->set();
   }

  if ( dialog->exec() == QDialog::Rejected) return; 
  NStLat_ = dialog->data_;
    
  OptimTextEditor* editor = 0 ;
  auto DigCh = getAttachedSubWin(WindowId::DigCh); 
  if (!DigCh) { 
    DigCh = createAttachedSubWin( (editor = new OptimTextEditor()), WindowId::DigCh);
    connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
    connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
    connect(editor, SIGNAL(redoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
  }
  else { 
    editor = qobject_cast<OptimTextEditor*>(  DigCh->widget() );  
    if (CtSt_.ClearText) editor->clear();
  }

  DigCh->raise();
  if (!editor)return;

    
  unsigned int lastline= editor->document()->blockCount();

  double tetaY = tetaYo0_;

  double    Enr;
  RMatrix   tm;

  
  if (NStLat_.printBdL) {    
  
    std::string names[nelm_];
  
    nln=0;
    for( int i=0; i<nelm_; ++i) {
      if( PrintBdL(i, names, nln, buf)) continue;
      nln++;
      editor->appendPlainText(buf);
     }
    goto endlat;
  }

  double alfa_exit, EnOld, Hrt, fi;  
  
  // editor->insertPlainText(QString::asprintf( "N \tName \tS[cm] \tL[cm] \tB[kG] \tG[kG/cm]   S[kG/cm/cm] \tTilt[deg] \tTilt_out \tBendAng[deg]\n");
  editor->insertPlainText(QString::asprintf( "#N \tName \tS[cm] \tL[cm] \tB[kG] \tG[kG/cm]   S[kG/cm/cm] \tTilt[deg] \tTilt_out \tBendAng[deg]"));

  Enr       = Ein;

  for(int i=0; i<nelm_; ++i){
    auto ep    = beamline_[i];
    SL       += ep->length(); 
    EnOld     = Enr;
    Hrt       = sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;
    tm = ep->rmatrix(alfa_exit, Enr, ms, tetaY, 0.0, 3);

    pname = (NStLat_.printFirstLetter) ? ep->fullName():  &ep->fullName()[1];  

    if(!Utility::filterName(ep->fullName(), NStLat_.Filter, NStLat_.MatchCase)) continue;
 
    if ( ep->etype() =='M' ) {
       if(  ep->N > NStLat_.MltpOrder ) continue;
    }

    sprintf(buf1,"%d \t%s \t%g \t%g ", i+1, pname, (NStLat_.StartSFromFile ? SL+so0_ : SL), ep->length());

    switch ( ep->etype() ) {
     case 'M':
       sprintf(buf, "  Order:m=%d   Bm*L[kG/cm**(m-1)]=%lg   Tilt[deg]=%lg", ep->N, ep->S, ep->tilt());
       break;
     case 'I':
       sprintf(buf, "  \t%g \t%g \t%g \t%g", 0., 0., 0., ep->tilt());
       break;
     case 'O':
       sprintf(buf," ");
       break;
     case 'H':
       switch(ep->N){
         case 1:
           sprintf(buf," Ellipse:   Ax[cm]=%g  Ay[cm]=%g  OffsetX[cm]=%g OffsetY[cm]=%g  Tilt[deg]=%g",
                  			ep->S, ep->A, ep->B, ep->G, ep->tilt());
           break;
         case 2:
           sprintf(buf," Hyperbole: Ax[cm]=%g  Ay[cm]=%g  OffsetX[cm]=%g OffsetY[cm]=%g  Tilt[deg]=%g",
                  			ep->S, ep->A, ep->B, ep->G, ep->tilt());
           break;
         default:
           sprintf(buf," Rectang.   Ax[cm]=%g  Ay[cm]=%g  OffsetX[cm]=%g OffsetY[cm]=%g  Tilt[deg]=%g",
                  			ep->S, ep->A, ep->B, ep->G, ep->tilt());
           break;
        }
	break;
      case 'A':
	sprintf(buf,"  D_E[MeV]=%g  E_out[MeV]=%g", ep->G*cos(PI/180.*ep->S), Enr);
	break;
      case 'W':
	sprintf(buf,"  D_E[MeV]=%g  E_out[MeV]=%g", Enr-EnOld, Enr);
	break;
      case 'Z':
	sprintf(buf,"  D_E[MeV]=%g  ", ep->G);
	break;
      case 'E':
	sprintf(buf,"  D_E[MeV]=%g  E_out[MeV]=%g", ep->B, Enr);
	break;
      case 'G':
	sprintf(buf,"\t%g  Angle[deg]=%g  Eff.Length_[cm]=%g  Tilt[deg]=%g",
			       ep->B, ep->G, ep->S, ep->tilt());
	break;
      case 'L':
	sprintf(buf,"\tGe[kV/cm**2]=%g  ", ep->G);
	break;
      case 'R':
        sprintf(buf,"\t%g \t%g  E[kV/cm]=%g  Ge[kV/cm**2]=%g",
			      ep->B, ep->G, ep->S, ep->tilt());
	break;
      case 'F':
	sprintf(buf,"   j[kA/cm**2]=%g ",ep->G);
	break;
      case 'X':
	sprintf(buf,"\tMatrix, \tD_E[MeV]=%g  L[cm]=%g", ep->G, ep->length());
	break;
      case 'B':    case 'D':
        fi = ep->length()*ep->B/Hrt*180./PI;
	sprintf(buf, "\t%g \t%g \t%g \t%g \t%g \t%g", ep->B, ep->G, ep->S, ep->tilt(), alfa_exit, fi);
	break;
      case 'T':
	sprintf(buf, "\tRms angle[mrad]=%lg  \t1/L*dL/dx[1/cm]=%lg \tTilt[deg]=%lg", ep->B,   ep->G, ep->tilt());
	break;
      case 'U':
	sprintf(buf, "\tRms Loss Spread[MeV]=%lg  \t1/L*dL/dx[1/cm]=%lg \tTilt[deg]=%lg", ep->B,   ep->G, ep->tilt());
	break;
      default:
	sprintf(buf, "\t%g \t%g \t%g \t%g", ep->B, ep->G, ep->S, ep->tilt());

    } // switch

    editor->appendPlainText(QString(buf1)+QString(buf));
  }
  sprintf(buf, " Total Length_ for one period = %g cm", SL);
  editor->appendPlainText(buf);
    
  sprintf(buf, " Final Energy = %g MeV", Enr);
  editor->appendPlainText(buf);
    
  sprintf(buf, " Total Length_ for %d period = %g cm",NmbPer, NmbPer*SL);
  editor->appendPlainText(buf);

endlat:

  // editor->UnlockBuffer(editBuffer, true);

  if(lastline>3){
      // **************  i = editor->GetLineIndex(lastline-1);
      // *************** editor->DeleteSubText(i-2,i);
    editor->insertPlainText("\n");
  }

  editor->moveCursor(QTextCursor::Start);
  editor->document()->setModified(false);
  editor->show();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::localBrowserOverride( QUrl const& url )
{
  // When it exists, use a local executable named optim_help_browser located
  // in the same directory as the optim executable
  // otherwise, use the default handler.
  QString path = QCoreApplication::applicationDirPath ();
  //path.append("/optim_help_browser");
  //QProcess::startDetached(path, QStringList(url.toEncoded().data() ));

  char* envoverride = std::getenv("OPTIMX_BROWSER");
  if (envoverride) {
    QProcess::startDetached(envoverride, QStringList(url.toEncoded().data() ));
  }
}  
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::help()
{
  QString path = QCoreApplication::applicationDirPath();
  // path.append("/optim_help_browser");

  auto helpurl = QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/help/htmldoc/optimx.html");

  QFileInfo check_file(path+ "/help/htmldoc/optimx.html");
  if ( !(check_file.exists() && check_file.isFile()) ) {
    OptimInformationMessage( this, "info", ("Can not open local help file:\n" + helpurl.toLocalFile()).toUtf8().data(),
			     QMessageBox::Ok);
    return;
  }
  
  // check if an override for the browser is defined via the env variable OPTIMX_BROWSER
  // if so, override the Url Handler 

  char* envoverride = std::getenv("OPTIMX_BROWSER");
  if (envoverride) {
     QDesktopServices::setUrlHandler("file", this, "localBrowserOverride");
  }
  
  bool success = QDesktopServices::openUrl(helpurl);
  if (!success) { 
    OptimInformationMessage( this, "info", "Request to launch default browser failed.", QMessageBox::Ok);
    return;
  }

  QDesktopServices::unsetUrlHandler("file");
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdTracker() 
{
  OptimEditor*  editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());

  // The tracker needs lattice info. Make sure that the 
  // lattice file has been processed. 
 
  if(analyzed_) {
    if(analyze(false, 1))  return;
  } 
  else {
    if(analyze(true, 1))  return;
  }
  
  OptimTracker*    tracker = 0;
  OptimTrackerNew* trackernew = 0;

  if (!Tracker_) {
   Tracker_ = mdiArea->addSubWindow( (trackernew = new OptimTrackerNew(this)) ); 
   // Note: once the subwindow has been added, it is reparented. Its parent 
   // is the viewport widget of the QMdiArea.
   connect( editor, SIGNAL( editorClosed(bool)), trackernew,  SLOT(setDisabled(bool)) );  
  }

  Tracker_->setGeometry(0, 0,  mdiArea->geometry().width()/2,  
                         mdiArea->geometry().height());
  Tracker_->show();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::stateNoLattice()
{
  lineMode();
  designOrbitMode();
  
  designReferenceOrbitToggleAct_->setEnabled(true);
  ringLineToggleAct_->setEnabled(true);       

  //      lineAct_->setEnabled(false);       
           newAct_->setEnabled(true);       
          openAct_->setEnabled(true);
       newTextAct_->setEnabled(true);
      openTextAct_->setEnabled(true);
      compressAct_->setEnabled(true);
   compressOldAct_->setEnabled(true);
    calculatorAct_->setEnabled(false);
    importMad8Act_->setEnabled(true);
    importMadXAct_->setEnabled(true);
   importDimadAct_->setEnabled(true);
 importResolveAct_->setEnabled(true);
    exportMad8Act_->setEnabled(false);
    exportMadXAct_->setEnabled(false);
   exportDimadAct_->setEnabled(false);
 exportResolveAct_->setEnabled(false);

         clearAllAct_->setEnabled(false);
   analyzeLatticeAct_->setEnabled(false);
    analyzeHeaderAct_->setEnabled(false);
      analyzeLineAct_->setEnabled(false);
 consistencyCheckAct_->setEnabled(false);
    invertLatticeAct_->setEnabled(false);
      scaleEnergyAct_->setEnabled(false);
readFieldIntegralAct_->setEnabled(false);
        incrementAct_->setEnabled(false);
        decrementAct_->setEnabled(false);
            stepsAct_->setEnabled(false);

   fitBetasAct_->setEnabled(false);
    fitUndoAct_->setEnabled(false);
       stopAct_->setEnabled(false);

           closeLatticeAct_->setEnabled(false);
       closeSymmetricalAct_->setEnabled(false);
                trackerAct_->setEnabled(false);


           trajectoryAct_->setEnabled(false);
        trajectoryNewAct_->setEnabled(false);
       typeTrajectoryAct_->setEnabled(false);
      closeTrajectoryAct_->setEnabled(false);
          tuneDiagramAct_->setEnabled(true);
     showExternalFileAct_->setEnabled(true);
         toolsControlAct_->setEnabled(true);

               betasAct_->setEnabled(false);
	       //betasLAct_->setEnabled(false);
               sizesAct_->setEnabled(false);
              phasesAct_->setEnabled(false);
 functionsAtElementsAct_->setEnabled(false);
           integralsAct_->setEnabled(false);
              matrixAct_->setEnabled(false);
     integrationStepAct_->setEnabled(false);
           functionsAct_->setEnabled(false);
       saveFunctionsAct_->setEnabled(false);
             latticeAct_->setEnabled(false);
        latticeTableAct_->setEnabled(false);
               orbitAct_->setEnabled(false);
            orbitNewAct_->setEnabled(false);
             controlAct_->setEnabled(true);
              detachAct_->setEnabled(false);
              attachAct_->setEnabled(false);
        attachDetachAct_->setEnabled(false);


       update4DBetasAct_->setEnabled(false);
             close4DAct_->setEnabled(false);
             betas4DAct_->setEnabled(false);
          betas4DNewAct_->setEnabled(false);
        dispersion4DAct_->setEnabled(false);
     dispersion4DNewAct_->setEnabled(false);
             sizes4DAct_->setEnabled(false);
           project4DAct_->setEnabled(false);
            phases4DAct_->setEnabled(false);
         functions4DAct_->setEnabled(false);
   functions4DAtElemAct_->setEnabled(false);
            control4DAct_->setEnabled(true);
          integrals4DAct_->setEnabled(false);

             cwBetasAct_->setEnabled(false);
             cwSizesAct_->setEnabled(false);
           cwProjectAct_->setEnabled(false);
            cwPhasesAct_->setEnabled(false);
         cwFunctionsAct_->setEnabled(false);
      cwCloseLatticeAct_->setEnabled(false);
           controlSCAct_->setEnabled(true);


   editorPreferencesAct_->setEnabled(true);
     plotPreferencesAct_->setEnabled(true);
     savePreferencesAct_->setEnabled(true);

          optimHelpAct_->setEnabled(true);
               aboutAct->setEnabled(true);
             aboutQtAct->setEnabled(true);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::stateLatticeLoaded()
{
   updateEditMenuState(); 

  designReferenceOrbitToggleAct_->setEnabled(true);
  ringLineToggleAct_->setEnabled(true);       

           newAct_->setEnabled(false);       
          openAct_->setEnabled(false);
       newTextAct_->setEnabled(true);
      openTextAct_->setEnabled(true);
      compressAct_->setEnabled(true);
   compressOldAct_->setEnabled(true);
    calculatorAct_->setEnabled(false);
    importMad8Act_->setEnabled(true);
    importMadXAct_->setEnabled(true);
   importDimadAct_->setEnabled(true);
 importResolveAct_->setEnabled(true);
    exportMad8Act_->setEnabled(true);
    exportMadXAct_->setEnabled(true);
   exportDimadAct_->setEnabled(true);
 exportResolveAct_->setEnabled(true);

         clearAllAct_->setEnabled(true);
   analyzeLatticeAct_->setEnabled(true);
    analyzeHeaderAct_->setEnabled(true);
      analyzeLineAct_->setEnabled(true);
 consistencyCheckAct_->setEnabled(true);
    invertLatticeAct_->setEnabled(true);
      scaleEnergyAct_->setEnabled(true);
readFieldIntegralAct_->setEnabled(true);
        incrementAct_->setEnabled(true);
        decrementAct_->setEnabled(true);
            stepsAct_->setEnabled(true);


   fitBetasAct_->setEnabled(true);
       stopAct_->setEnabled(true);

           closeLatticeAct_->setEnabled(true);
       closeSymmetricalAct_->setEnabled(true);
                trackerAct_->setEnabled(true);

           trajectoryAct_->setEnabled(true);
        trajectoryNewAct_->setEnabled(true);
       typeTrajectoryAct_->setEnabled(true);
      closeTrajectoryAct_->setEnabled(true);
          tuneDiagramAct_->setEnabled(true);
     showExternalFileAct_->setEnabled(true);
         toolsControlAct_->setEnabled(true);

               betasAct_->setEnabled(true);
	       //betasLAct_->setEnabled(true);
               sizesAct_->setEnabled(true);
              phasesAct_->setEnabled(true);
 functionsAtElementsAct_->setEnabled(true);
           integralsAct_->setEnabled(true);
              matrixAct_->setEnabled(true);
     integrationStepAct_->setEnabled(true);
           functionsAct_->setEnabled(true);
       saveFunctionsAct_->setEnabled(true);
             latticeAct_->setEnabled(true);
        latticeTableAct_->setEnabled(true);
               orbitAct_->setEnabled(true);
            orbitNewAct_->setEnabled(true);
             controlAct_->setEnabled(true);
              detachAct_->setEnabled(true);
              attachAct_->setEnabled(true);
        attachDetachAct_->setEnabled(true);


       update4DBetasAct_->setEnabled(true);
             close4DAct_->setEnabled(true);
             betas4DAct_->setEnabled(true);
          betas4DNewAct_->setEnabled(true);
        dispersion4DAct_->setEnabled(true);
     dispersion4DNewAct_->setEnabled(true);
             sizes4DAct_->setEnabled(true);
           project4DAct_->setEnabled(true);
            phases4DAct_->setEnabled(true);
         functions4DAct_->setEnabled(true);
   functions4DAtElemAct_->setEnabled(true);
           control4DAct_->setEnabled(true);
          integrals4DAct_->setEnabled(true);

             cwBetasAct_->setEnabled(true);
             cwSizesAct_->setEnabled(true);
           cwProjectAct_->setEnabled(true);
            cwPhasesAct_->setEnabled(true);
         cwFunctionsAct_->setEnabled(true);
      cwCloseLatticeAct_->setEnabled(true);
           controlSCAct_->setEnabled(true);

       fileSeparatorAct_->setEnabled(true);
    winMenuSeparatorAct_->setEnabled(true);


   editorPreferencesAct_->setEnabled(true);
     plotPreferencesAct_->setEnabled(true);
     savePreferencesAct_->setEnabled(true);

          optimHelpAct_->setEnabled(true);
               aboutAct->setEnabled(true);
             aboutQtAct->setEnabled(true);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdRunCalculator()
{

  auto& IncludeMode = appstate.IncludeMode;
  
  double result;
  char buf[LSTR+1], buf1[LSTR+1], buf2[LSTR+1], *bufpt1, *bufpt2, str_res[LSTR+1];
  int nline=0, i;
  
  SCalc calc;

  // determine if the active window is a text window.   
  
  if ( !activeMdiChild() ) return;
  OptimTextEditor* editor = 0;
  if ( ! (editor = qobject_cast<OptimTextEditor*>( activeMdiChild()->widget() ) ) ) return; 

  // Analysis of math header
  
  calc.zeroCalc();
  int BufLen = editor->document()->blockCount();
  do {
     nline = getLineCalc(editor, buf, LSTR, nline);
     if(strcmpr("stop",buf)||strcmpr("STOP",buf)||(nline<0)) return;
     bufpt1=buf;    
     bufpt2=buf2;
     while(*bufpt1){
       if(*bufpt1==';' || *bufpt1=='#')break;
       *bufpt2++=*bufpt1++;
     }
     *bufpt2='\0';
     i=calc.calcLine(buf2, &result, "%12.9lg", str_res, buf1);
     if(i>0){ 
       replaceLine(editor, nline-1, buf);
       editor->highlightCurrentBlock();
       OptimMessageBox::warning(this, "Calculator Error", buf1, QMessageBox::Ok);
       return;
     }
     if(i<0) { 
       strcat(buf2,";");
     }
     else {
       strcat(buf2,";  =>"); strcat(buf2, str_res);
     }
    if(*bufpt1==';' || *bufpt1=='#') {
      while(*bufpt1) {
        if(*bufpt1=='#')break;
        bufpt1++;
      }
    }
    if(*bufpt1=='#'){strcat(buf2, "  "); strcat(buf2, bufpt1);}

    if(!IncludeMode) replaceLine(editor, nline-1, buf2); // editor include mode ??? 

  } while(nline<BufLen);
 
  editor->document()->setModified(true); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::showPositionInDocument(QPoint pos)
{
  QString msg = QString("Line: %1 Column: %2").arg(pos.y()).arg(pos.x()); 
  statusBar()->showMessage(msg, 5000); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdScaleEnergy()
{

  char   buf[LSTR+1];
  double dat[5];
  int    nline=0;
  bool   result;
  
  static EnergyScaleDialog* dialog = 0;
  if (!dialog) dialog =  new EnergyScaleDialog(0); 

  if (!LatticeCh_)  return;

  double Enew = dialog->data_.energy = Ein;
  dialog->set();

  if (dialog->exec() == QDialog::Rejected) return;
  Enew = dialog->data_.energy;

  if( interrupted_ ) { interrupted_ = false; return;}
  
  if(analyzed_) { if(analyze(false)) return; } else { if( analyze(true)) return; }
  
  OptimEditor* editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());
   
  do{
    nline  = getLineCalc(editor, buf, LSTR, nline);
    result = strcmpr("OptiM",buf);
  } while(!result);
 
  nline = getLineCmt(editor, buf, LSTR, nline);
  decodeLine(buf,dat,2);
  Ein   = dat[0];  
  ms    = dat[1];
  Hr    = sqrt(2.*ms*Ein+Ein*Ein)/C_DERV1;

  double Hrnew = sqrt(2.*ms*Enew+Enew*Enew)/C_DERV1;

  sprintf(buf,"Energy[MeV]=%-8.8lg   Mass[MeV]=%-8.8lg ",Enew,ms);
  replaceLine(editor, nline-1, buf);

  double s  = Hrnew/Hr;
  double se = s*sqrt((2.*ms*Enew+Enew*Enew)/(2.* ms*Ein+Ein*Ein))*(Ein+ms)/(Enew+ms);

  for (int i=0; i<nelmlist_; i++){
    switch ( elmdict_[i]->etype() ) {
      case 'R': 
        elmdict_[i]->B *= s; 
        elmdict_[i]->G *= s;
	elmdict_[i]->S *= se; 
        elmdict_[i]->tilt(elmdict_[i]->tilt()*se); 
        break;
      case 'L': 
        elmdict_[i]->G *=se; 
        break;
      case 'Q': 
      case 'F': 
        elmdict_[i]->G *=s; 
        break;
      case 'B': 
      case 'D': 
        elmdict_[i]->G *=s;
      case 'G': 
        elmdict_[i]->B *=s; 
        break;
      case 'S': 
        elmdict_[i]->S *=s; 
        break;
      case 'A': 
      case 'W': 
        elmdict_[i]->G *= (Enew/Ein); 
        break;
      default:  
        break;
    }
  }

  nline=LineLIn-1;  //	 LineLIn:  index of the first line of the list of elements in editor window.

  do nline  = getLineCmt(editor, buf, 256, nline);

  while(!strcmpr("begin list",buf));

  for(int i=0; i< nelmlist_; ++i) { 
    nline = getLineCmt(editor, buf, 256, nline);
    print_elm( elmdict_[i].get(), buf);
    replaceLine(editor, nline-1, buf);
  } 

  
  Ein = Enew; 
  Hr  = Hrnew;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if  0

#define ARG_LEN 10
#define DATA_LEN 9

void OptimMainWindow::cmdFileExecute ()
{

  char buf[256], fn[256], *bp, arglist[ARG_LEN][80], *varg[ARG_LEN+1];
  int   nmblines, nline, i;
  FILE *fp = 0;

  OEditFile* edclient = TYPESAFE_DOWNCAST(this->GetClientWindow(), OEditFile);
  nmblines = edclient->GetNumLines();
 
  bp = getenv("os"); // Windows NT

  optiMDIClient* prnt = TYPESAFE_DOWNCAST(myparent, optiMDIClient);
 
  if(bp) 
    if(prnt->strcmpr("Windows_NT",bp)) {
      optiApp *theApp = TYPESAFE_DOWNCAST(GetApplication(), optiApp);
      theApp->GetModuleFileName(fn, 255);
      for( i=strlen(fn); i>0; i--) {
	 if((fn[i]=='\\')||(fn[i]==':')) break;
	 fn[i+1]=0;
	 strcat(fn,"tmp.bat");
    	 fp=fopen(fn,"w");
	 std::unique_ptr<FILE,decltype(fhdel)> sfp(fp,fhdel); 
         fprintf(fp,"title \"OptiM Command Window\"\n");
	 for (nline=0; nline<nmblines; nline++){
	    edclient->GetLine(buf, 255, nline);
	    if( buf[0] == '#')continue;
	    bp=buf; while(*bp!=0) { if(*bp=='\n')*bp=0; *bp++;}
    	    for(i=0; i<ARG_LEN; i++) varg[i]=arglist[i];
	    if(split(buf,ARG_LEN,varg)==0)continue;
            fprintf(fp,"%s\n",buf);
	 }
      //fclose(fp);
      sprintf(buf,"cmd /t:b0 /k \"%s\"", fn);
      for(i=0; i<ARG_LEN; i++)varg[i]=arglist[i];
      if(split(buf,ARG_LEN,varg)==0)return;
      spawnvp(P_NOWAIT,varg[0],varg);
      return;
    }
    // Windows 95
    for (nline=0; nline<nmblines; nline++){
      edclient->GetLine(buf, 255, nline);
      if( buf[0] == '#')continue;
      bp=buf; while(*bp!=0){if(*bp=='\n')*bp=0; *bp++;}
      for(i=0; i<ARG_LEN; i++)varg[i]=arglist[i];
      if(split(buf,ARG_LEN,varg)==0)return;
      if(spawnvp(P_WAIT,varg[0],varg)){
	MessageBox(buf,"Cannot perform the command", MB_OK);
	return;
       };
    }
  }
}

#endif

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  OptimMainWindow::InverseOrbit(double* tilt_out, int* ref)
{
  RMatrix tm;

  double xo     = xo0_; 
  double yo     = yo0_; 
  double zo     = zo0_;
  double tetaXo = tetaXo0_;

  double tetaYo = tetaYo0_;
  double tetaY  = tetaYo;
  
  double Enr    = Ein;
  double alfa_exit = 0.0;

  int j =0;
  for(int i=0; i<nelm_; ++i){
    auto ep  = beamline_[i];
    double Hrt    = sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;
    DeltOrb(Hrt,ms, *ep, xo, yo, zo, tetaXo, tetaYo);
    tm = ep->rmatrix(alfa_exit, Enr, ms, tetaY, 0.0, 3);
    char nm = ep->etype();
 
    if(nm=='B'||nm=='D'){
      ref[j] = i;
      tilt_out[j] = alfa_exit;
      ++j;
    }
  }

  return j; // j is the no of bends 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  OptimMainWindow::ReplaceLineForInv(OptimEditor* editor, int nline,
					 int n1, double dat1, int n2, double dat2)
{
  char buf1[LSTR+1];
  char buf2[LSTR+1];
  
  getLineCalc(editor, buf1, LSTR, nline);

  char* pt1 = buf1;
  char* pt2 = buf2;

  int j =  0;
  int i = -1;

  int    ref[] = {n1, n2, -2};
  double dat[] = {dat1, dat2};

  do{

    *pt2++ = *pt1++;

    if(*pt1 == '=') ++i;

    if(i == ref[j]) { 

      *pt2++ = *pt1++;

      while(*pt1 == ' ' || *pt1 =='\t') pt1++;
      while(*pt1 != ' ' && *pt1 !='\t' && *pt1 !=0 ) pt1++;

      sprintf(pt2, "%-8.8lg", dat[j]);

      while(*pt2 !=0 ) pt2++;
      ++j;
    }

  } while(*pt1 != 0);
  *pt2=0;

  QTextCursor cursor = editor->textCursor();
  cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
  cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, nline);

  cursor.select(QTextCursor::BlockUnderCursor);
  cursor.removeSelectedText();
  cursor.insertText("\n");
  cursor.insertText(buf2);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdAttachDetach()
{
  auto list  = mdiArea->subWindowList();
  QMdiSubWindow* active_sw = mdiArea->activeSubWindow();  

  if (!active_sw) return; 
  if (active_sw ->property("attached") == true ) {
    detach();
  }
  else {
    attach();
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::detach()
{
  auto list  = mdiArea->subWindowList();
  QMdiSubWindow* active_sw = mdiArea->activeSubWindow();  

  if (!active_sw) return; 

  for ( auto it = list.begin(); it != list.end(); ++it) {

    if ( (*it)->property("attached") == QVariant() ) continue;
    if ( (*it) ==  active_sw )  {
       (*it)->setProperty("attached", false);
       OptimPlot*       plot   = qobject_cast<OptimPlot*>((*it)->widget());
       OptimTextEditor* editor = qobject_cast<OptimTextEditor*>((*it)->widget());
       if (plot)   plot->replot();
       if (editor) editor->repaint();
       break;
    }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::attach()
{
  auto list  = mdiArea->subWindowList();
  QMdiSubWindow* active_sw = mdiArea->activeSubWindow();  

  if (active_sw->property("attached") == QVariant() ) return;
  if (active_sw->property("winid")    == QVariant() ) return;

  WindowId winid = (WindowId) active_sw->property("winid").toInt(); 
  
  for ( auto it = list.begin(); it != list.end(); ++it) {
    if ( (*it)->property("winid") == QVariant() )    continue;
    if ( WindowId((*it)->property("winid").toInt()) != winid ) continue;
    (*it)->setProperty("attached", false);
    OptimPlot*       plot   = qobject_cast<OptimPlot*>( (*it)->widget());
    OptimTextEditor* editor = qobject_cast<OptimTextEditor*>( (*it)->widget());
    if (plot)   plot->replot();
    if (editor) editor->repaint();
  }    

  active_sw->setProperty("attached", true);
  OptimPlot*       plot   = qobject_cast<OptimPlot*>(active_sw->widget());
  OptimTextEditor* editor = qobject_cast<OptimTextEditor*>(active_sw->widget());
  if (plot)   plot->replot();
  if (editor) editor->repaint();
 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::detachAll()
{
  auto list  = mdiArea->subWindowList();
  for ( auto it = list.begin(); it != list.end(); ++it) {
    if ( (*it)->property("attached") == QVariant() ) continue;
    auto geometry = (*it)->geometry();  
    (*it)->setProperty("attached", false);
    OptimPlot* plot = qobject_cast<OptimPlot*>((*it)->widget());
    if (plot) plot->replot();
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdFileGetFileName ()
{
  // insert filename at the cursor position in the editor that has focus.

  QString filename = QFileDialog::getOpenFileName(0, "Get File Name");

  if (filename == "") return; // rejected;

  QMdiSubWindow* sw = mdiArea->currentSubWindow();
  
  if (!sw) { return;}   // no subwindow has focus. 
   
  QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(sw->widget()); 

  if (!editor) return; // the window that has focus is not an editor  

  QTextCursor cursor = editor->textCursor();
  cursor.insertText( "\""+ filename +"\""); 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::cmdEditRead(char const* FileNm )
{
  char filename[128];

  char* str;
  char* chpt;
  char  name[20];
  char  buf[LSTR+1];
  char  buf1[20];
  char  buf2[NAME_LENGTH];

  double dat;
  double s;

  if(analyzed_) { 
     if(analyze(false,1)) return 1;
  } 
  else {
    if (analyze(true,1)) return 1;
  }
   
  EditReadDlgStruct edtrdSt; 
   
  static EditReadDialog* dialog = 0;
  if (!dialog) {
	 dialog = new EditReadDialog(0);
         strcpy(edtrdSt.Filter, "*");
         edtrdSt.MatchCase = false;
         strcpy(edtrdSt.FirstLetter, "");
         edtrdSt.CompareOnly   =  false;
         edtrdSt.Scale         =  scale_read;
         dialog->data_ = edtrdSt;
         dialog->set();
  };

  if (dialog->exec() == QDialog::Rejected) return 1; 
  edtrdSt = dialog->data_;
  strcpy(filename, edtrdSt.FileNam); 
  if (!filename[0]) return 1;  
  

  FILE *fp;
  if( !( fp = fopen(filename,"r")) ) {
     OptimMessageBox::warning( this, "Error opening file - ", FileNm, QMessageBox::Ok);
     return 1;
  }		

  scale_read = edtrdSt.Scale;

  sprintf(buf,"Import Field Integrals");

  // read line by line and fill ElmList

  int n=0;
   
  struct Element_name {
    char name[NAME_LENGTH];
  };

  std::vector<Element_name> elnm(nelmlist_);

  int i;
  while( !fgets(buf, LSTR-1, fp) ) {

     if( (*buf=='\n') || (*buf==0)) continue;
     str = name+1;
     sscanf(buf,"%15s %d %le ",str, &i, &dat);
     dat *= scale_read;
 
     if(!filterName(str, edtrdSt.Filter, edtrdSt.MatchCase)) continue;
  
     chpt = str; 
     while( (*chpt!=0) && (*chpt!='.')) ++chpt;  
     *chpt = 0;
     
     if( (edtrdSt.FirstLetter[0]!=0) && (edtrdSt.FirstLetter[0]!=' ')) {
       str=name;
       *str=edtrdSt.FirstLetter[0];
      }

     //if(!getElmName(ElemNameFCh,&str, buf1, buf2)) continue;
     getElmNameX(str, buf1, buf2);
     if (*str) continue;

      for( i=0; i< nelmlist_; ++i) { 
        if(!strcmp(buf1, elmdict_[i]->name())) break;
      }
      if(strcmp(buf1, elmdict_[i]->name()))  continue;

      switch ( elmdict_[i]->etype() ){
      
         case 'M':
	   s            = elmdict_[i]->S;  
           elmdict_[i]->S = dat;
	   sprintf(buf,"%s \told=%-9.3f \tnew*s=%-9.3f \tdelta=%-9.3f", buf1, s, dat, s-dat);
	   goto printgen1;
	 case 'Q': 
         case 'L':
	   s            = elmdict_[i]->G;  
           elmdict_[i]->G = dat*0.001/elmdict_[i]->length();
	   goto printgen;
	 case 'B':  
         case 'D':  
         case 'K':   
         case 'C':
	   s            = elmdict_[i]->B;  
           elmdict_[i]->B = dat*0.001/elmdict_[i]->length();
	   goto printgen;
	 case 'R':  
         case 'S':
	   s             = elmdict_[i]->S;  
           elmdict_[i]->S  = dat*0.001/elmdict_[i]->length();
printgen:  			
           sprintf(buf,"%s \told=%-9.3lf \tnew*s=%-9.3lf \tdelta=%-9.3lf",
		   buf1, elmdict_[i]->length()*s*1000., dat, elmdict_[i]->length()*s*1000.-dat);
	   if(elmdict_[i]->length()*s != 0)
           sprintf(&buf[strlen(buf)],"\t%c=%-9.3lf", '%', dat/(elmdict_[i]->length()*s*1000.)*100.-100.);
	   goto printgen1;
	 case 'A': 
         case 'W':  
         case 'Z':
	   s            = elmdict_[i]->G;  
           elmdict_[i]->G = dat;
	   sprintf(buf,"%s \told=%-9.3f \tnew*s=%-9.3f \tdelta=%-9.3f",
		   buf1, s, dat, s-dat);
printgen1: 
           strcpy(elnm[n].name, elmdict_[i]->name());
	   n++;
	   break;
	 default: 
           continue;
      }// switch
      /// *** if ( !FileNm) { edclient->Insert(buf); edclient->Insert("\n");}
   } // while
   fclose(fp);

   if( !FileNm) {
     if(edtrdSt.CompareOnly || (n==0)) {
        // edclient->ClearModify(); 
        return 0;
      }
   }

   OptimEditor* editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());
   if (!editor) return 0; 
 

   int nline = LineLIn-1;  //	 LineLIn   Number of First line for list of Elements in TEdit.
   
   do nline = getLineCmt(editor, buf, LSTR, nline); 
   while (!strcmpr("begin list",buf)) ;
   
   for( int i=0; i< nelmlist_; ++i){ //	 Nelmdict_  Number of different Elements in the Element list
     nline = getLineCmt(editor, buf, LSTR, nline);
     if( elmdict_[i]->etype() =='X'){
       for(int j=0; j<6; ++j) nline = getLineCmt(editor, buf, LSTR, nline);
       continue;
     }
     int j =0;
     for(j=0; j<n; ++j) {
       if( strcmpr(elnm[j].name, elmdict_[i]->name()) ) break;
     }
     if(j<n){
       print_elm(elmdict_[i].get(), buf);
       replaceLine(editor, nline-1, buf);
     }
   }

  if (!FileNm){
    editor->insertPlainText(QString::asprintf("BdL's for %d Elements were substituted\n", n));
    editor->document()->setModified(false);
   }
 
 return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdEditorFind(bool repeat)
{
  mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);

  static FindDialog* dialog = 0; // not static, since the editor 

  if (!dialog) {
    dialog = new FindDialog(this); 
    dialog->setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Dialog);
  }
  if ( repeat) {
    dialog->next();
  }
  else {   
    dialog->show();
  }

  dialog->setCallback([this](){

    QMdiSubWindow* sw = mdiArea->currentSubWindow();
    if (!sw) { return;}   // no subwindow had focus. 

    QPlainTextEdit* editor = qobject_cast<QPlainTextEdit*>(sw->widget()); 
    if (!editor) return; // the window that had focus is not an editor  

    QTextDocument::FindFlags flags;
    flags |= dialog->data_.up ?         QTextDocument::FindBackward        : QTextDocument::FindFlags();
    flags |= dialog->data_.match_case ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags();
    flags |= dialog->data_.match_word ? QTextDocument::FindWholeWords      : QTextDocument::FindFlags();
  
    bool found = editor->find( dialog->data_.what, flags);  

    if (!found) {
      OptimMessageBox::warning( this, "Find", ( dialog->data_.what + " not Found.").toUtf8().data(), QMessageBox::Ok);
    }
    return; });
     
  return;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdEditorReplace()
{

  //mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
  //mdiArea->activatePreviousSubWindow();	

  QMdiSubWindow* sw = mdiArea->currentSubWindow();
  if (!sw) { return;}   // no subwindow had focus. 

  QPlainTextEdit* editor = qobject_cast<QPlainTextEdit *>(sw->widget()); 
  if (!editor) return; // the window that had focus is not an editor  

  static ReplaceDialog* dialog =0;

  if (!dialog) {
    dialog = new ReplaceDialog(0); 
  }

  int result = 0; 
  while ( (result = dialog->exec()) != QDialog::Rejected) {

    QTextDocument::FindFlags flags;
    flags |= dialog->data_.match_case ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags();
    flags |= dialog->data_.match_word ? QTextDocument::FindWholeWords      : QTextDocument::FindFlags();
  
    QTextCursor cursor = editor->textCursor();
    editor->moveCursor(QTextCursor::Start);

  again:

   bool found = editor->find( dialog->data_.what, flags);  
    
   if (!found) {
         //store the original cursor position  
         editor->setTextCursor(cursor);
         if (result ==12 ) return;
         OptimMessageBox::warning( this, "Replace", (dialog->data_.what + " not Found.").toUtf8().data(), QMessageBox::Ok);
   };    
 
   switch(result) {
      case 10: // find
	break; 
      case 11: // replaceOnce
        cursor = editor->textCursor();
        cursor.removeSelectedText(); 	
        cursor.insertText( dialog->data_.with);
        break; 	
      case 12:  // replaceAll
        cursor = editor->textCursor();
        cursor.removeSelectedText(); 	
        cursor.insertText( dialog->data_.with);
        goto again;
        break;
      default:
        break;
      }
   }
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdEditorNext()
{
  cmdEditorFind(true);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::setSubWindowFocus()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void updateSubWindowListMenu() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::customizeSystemMenu(QMdiSubWindow* sw) 
{
    QMenu* sysmenu =  sw->systemMenu();
    OptimPlot* plot = qobject_cast<OptimPlot*>(sw->widget());

    if (plot) {
      sysmenu->addSeparator();
      QAction* plot_opts; 
      sysmenu->addAction( (plot_opts = new QAction("Plot Options",   plot)) );
      QAction* ext_opts;
      sysmenu->addAction( (ext_opts  = new QAction("Add External Data",  plot)) );
      QAction* edext_opts;
      sysmenu->addAction( (edext_opts  = new QAction("Edit External Data",  plot)) );

      QAction* copy_opts;
      sysmenu->addAction( (copy_opts  = new QAction("Copy to Clipboard",  plot)) );

      connect( copy_opts,   SIGNAL(triggered()), plot, SLOT(copyToClipboard()));
      connect( plot_opts,   SIGNAL(triggered()), plot, SLOT(options()));
      connect( ext_opts,    SIGNAL(triggered()), plot, SLOT(addMeasuredData()));
      connect( edext_opts,  SIGNAL(triggered()), plot, SLOT(editMeasuredData()));
    }
    sysmenu->addSeparator();
    sysmenu->addAction( attachAct_ );
    sysmenu->addAction( detachAct_ );

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::updateRecentFileActions()
 {
     QSettings settings;
     QStringList files = settings.value("recentFileList").toStringList();

     int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles_);

     for (int i=0; i<numRecentFiles; ++i) {
         QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
         recentFileActs_[i]->setText(text);
         recentFileActs_[i]->setData(files[i]);
         recentFileActs_[i]->setVisible(true);
     }
     for (int j=numRecentFiles; j < MaxRecentFiles_; ++j)
         recentFileActs_[j]->setVisible(false);

     fileSeparatorAct_->setVisible(numRecentFiles > 0);
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::openRecentFile()
 {

     QAction *action = qobject_cast<QAction *>(sender());

     if (action) {
       auto suffix = QFileInfo(action->data().toString()).suffix().toUpper();
       if ( (suffix == "OPT") || (suffix == "OPC" )) { 
	    open (action->data().toString());
       }
       else {
	   openTxt(action->data().toString());
       }
     }
 }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::updateSubWinMenu()
{
  // clear existing subwin related actions from the menu
  QAction* action; 
  auto actionlist = windowMenu_->actions(); 
    
  QListIterator<QAction*> it (actionlist);
  it.toBack();
  while (it.hasPrevious()) {
    action = it.previous();
    if (action == winMenuSeparatorAct_) break;
    windowMenu_->removeAction(action);
    delete action;
  }
  
  // add subwin related actions to the menu

  static QActionGroup* actiongroup = 0;
 
 if (!actiongroup) {
    actiongroup = new QActionGroup( windowMenu_);
    actiongroup->setExclusive(true);
  }
  auto swlist = mdiArea->subWindowList();
  for ( auto it = swlist.begin(); it != swlist.end(); ++it ) { 
    QMdiSubWindow* sw = (*it);
    windowMenu_->insertAction( 0, actiongroup->addAction((action =  new QAction( sw->windowTitle(), this))) );
    action->setVisible(true);
    action->setCheckable(true);
    action->setData( sw->windowTitle() );
    action->setChecked( (sw == mdiArea->activeSubWindow()) );
    connect( action, SIGNAL(toggled(bool)),  this, SLOT( setActiveSubWindow(bool)) ); 
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::setActiveSubWindow( bool state )
{
  if (!state) return;
  
  auto swlist = mdiArea->subWindowList();
  for ( auto it = swlist.begin(); it != swlist.end(); ++it ) { 
    if ( (*it)->windowTitle() == qobject_cast<QAction*>(sender())->data().toString() ) {
       mdiArea->setActiveSubWindow(*it);
       break;
    }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 	 OptimMainWindow::keyPressEvent ( QKeyEvent * event )
{   

  auto key       = event->key();
  auto modifiers = event->modifiers(); 
  QMainWindow::keyPressEvent( event );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdPlotPreferences()
{   

  static PlotPreferencesDialog* dialog = 0;
  
  if (!dialog ) { 
     dialog = new PlotPreferencesDialog(0);
     dialog->data_ = Globals::preferences().plot_preferences;
  }   
  
  dialog->set();
  dialog->exec();

  Globals::preferences().plot_preferences = dialog->data_;

  QList<OptimPlot*> widgetList = centralWidget()->findChildren<OptimPlot*>();

  for ( auto it = widgetList.begin(); it != widgetList.end(); ++it) {

    (*it)->setPreferences();
    (*it)->setOptions();
    (*it)->repaint();
    (*it)->replot();
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdCompPreferences()
{   
  static GeneralPreferencesDialog* dialog = 0;

  if (!dialog ) { 
     dialog = new GeneralPreferencesDialog(0);
     dialog->data_.ignore_autorepeat       =  Globals::preferences().ignore_autorepeat; 
     dialog->data_.quad_fringe_off         = !Globals::preferences().fringe_effects_on;
     dialog->data_.neg_phase_adv_threshold =  Globals::preferences().neg_phase_adv_threshold; 
     dialog->data_.parallel_tracking       =  Globals::preferences().parallel_tracking; 
  }   
  dialog->set();

  if ( dialog->exec() == QDialog::Rejected) return;

  Globals::preferences().ignore_autorepeat       =   dialog->data_.ignore_autorepeat;
  Globals::preferences().fringe_effects_on       =  !dialog->data_.quad_fringe_off;
  Globals::preferences().neg_phase_adv_threshold =   dialog->data_.neg_phase_adv_threshold;
  Globals::preferences().parallel_tracking       =   dialog->data_.parallel_tracking;

  Element::fringe_on               =  !dialog->data_.quad_fringe_off; 
  Element::neg_phase_adv_threshold =   dialog->data_.neg_phase_adv_threshold;

}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdEditorPreferences()
{   
  EditorPreferencesDialog* dialog = 0;
  if (!dialog) {
    dialog = new EditorPreferencesDialog(0);
  }

  dialog->data_.lattice_editor_tabs = Globals::preferences().lattice_editor_tabsp;
  dialog->data_.text_editor_tabs    = Globals::preferences().text_output_tabsp;

  dialog->data_.editor_hi_fore_r = Globals::preferences().editor_hi_fore_r;
  dialog->data_.editor_hi_fore_g = Globals::preferences().editor_hi_fore_g;
  dialog->data_.editor_hi_fore_b = Globals::preferences().editor_hi_fore_b;

  dialog->data_.editor_hi_back_r = Globals::preferences().editor_hi_back_r;
  dialog->data_.editor_hi_back_g = Globals::preferences().editor_hi_back_g;
  dialog->data_.editor_hi_back_b = Globals::preferences().editor_hi_back_b;
  dialog->data_.editor_curwidth  = Globals::preferences().editor_curwidth;
  dialog->data_.editor_showlines = Globals::preferences().editor_showlines;
  dialog->data_.editor_curline   = Globals::preferences().editor_curline;

  if (dialog->exec() == QDialog::Rejected ) return;

  Globals::preferences().lattice_editor_tabsp =  dialog->data_.lattice_editor_tabs;
  Globals::preferences().text_output_tabsp    =   dialog->data_.text_editor_tabs; 

  Globals::preferences().editor_hi_fore_r =  dialog->data_.editor_hi_fore_r;
  Globals::preferences().editor_hi_fore_g =  dialog->data_.editor_hi_fore_g;
  Globals::preferences().editor_hi_fore_b =  dialog->data_.editor_hi_fore_b;

  Globals::preferences().editor_hi_back_r =  dialog->data_.editor_hi_back_r;
  Globals::preferences().editor_hi_back_g =  dialog->data_.editor_hi_back_g;
  Globals::preferences().editor_hi_back_b =  dialog->data_.editor_hi_back_b;

  Globals::preferences().editor_curwidth  = dialog->data_.editor_curwidth;
  Globals::preferences().editor_showlines = dialog->data_.editor_showlines;
  Globals::preferences().editor_curline   = dialog->data_.editor_curline;
 
  resetEditorFont();			       

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdSavePreferences()
{   
  qobject_cast<OptimApp*>(qApp)->savePreferences( );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::initStateMachine()
{

  QState*      top               = new   QState(QState::ParallelStates);

  QState*      lattice           = new   QState(top);
  QState*      text              = new   QState(top);
  QState*      subwindows        = new   QState(top); // any subwindow
  QState*      editors           = new   QState(top); // any editor 

  QState*      subwindowpresent  = new   QState(subwindows);
  QState*      nosubwindow       = new   QState(subwindows);
  QState*      editorpresent     = new   QState(editors);
  QState*      noeditor          = new   QState(editors);


  QState*      nolattice         = new        NoLatticeState(lattice);
  QState*      latticeloaded     = new    LatticeLoadedState(lattice); // a lattice, no tracker

  QState*      trackerloaded     = new    TrackerLoadedState(latticeloaded );
  QState*      notracker         = new        NoTrackerState(latticeloaded ); 

  QState*      notexteditor      = new     NoTextEditorState(text);
  QState*      texteditorloaded  = new TextEditorLoadedState(text);

  // QState*         fitinprogress   = new   QState(); // a special state during fitting 
  fitinprogress_                  = new   QState(); // a special state during fitting 

  QHistoryState*  top_hist        = new   QHistoryState(QHistoryState::DeepHistory, top); 
  
  
  QFinalState* done              = new           QFinalState();


  qApp->installEventFilter( (machine_ =  new OptimStateMachine( this)) );

  machine_->addState(top);
  machine_->addState(fitinprogress_);

  // this is not required for parallel states
  //   top->setInitialState(lattice);
  //   top->setInitialState(text);

  lattice->setInitialState(nolattice);
  latticeloaded->setInitialState(notracker);

  text->setInitialState(notexteditor);
     editors->setInitialState(noeditor);
  subwindows->setInitialState(nosubwindow);

     
  machine_->addState(done);
  
          nolattice->addTransition( machine_,     SIGNAL( latticeLoaded()    ),  latticeloaded   );  
      latticeloaded->addTransition( machine_,     SIGNAL( noLattice()        ),  nolattice       ); 

          notracker->addTransition( machine_,     SIGNAL( trackerLoaded()    ),  trackerloaded   ); 
      trackerloaded->addTransition( machine_,     SIGNAL( noTracker()        ),  notracker       ); 

       notexteditor->addTransition( machine_,     SIGNAL( textEditorLoaded() ),  texteditorloaded); 
   texteditorloaded->addTransition( machine_,     SIGNAL( noTextEditor()     ),  notexteditor    ); 

           noeditor->addTransition( machine_,     SIGNAL( editorPresent()    ),  editorpresent   ); 
      editorpresent->addTransition( machine_,     SIGNAL( noEditor()         ),  noeditor    ); 
  
        nosubwindow->addTransition( machine_,     SIGNAL( subWindowPresent() ),  subwindowpresent ); 
   subwindowpresent->addTransition( machine_,     SIGNAL( noSubWindow()      ),  nosubwindow      ); 


  top->addTransition( this,     SIGNAL( fitInProgress()  ), fitinprogress_ ); 
  fitinprogress_->addTransition( this,     SIGNAL( fitDone()   ), top_hist ); 

  
  connect(fitinprogress_, SIGNAL( entered()), this, SLOT(stateFitInProgress()   ) );
  connect(fitinprogress_, SIGNAL( exited()),  this, SLOT(stateFitDone()   ) );


  connect( trackerloaded,    SIGNAL(entered()),  this, SLOT(stateTrackerLoaded()      ) );
  connect( notracker,        SIGNAL(entered()),  this, SLOT(stateNoTracker()          ) );
  connect( latticeloaded,    SIGNAL(entered()),  this, SLOT(stateLatticeLoaded()      ) );
  connect( nolattice,        SIGNAL(entered()),  this, SLOT(stateNoLattice()          ) );
  connect( texteditorloaded, SIGNAL(entered()),  this, SLOT(stateTextEditorLoaded()   ) );
  connect( notexteditor,     SIGNAL(entered()),  this, SLOT(stateNoTextEditor()       ) );
  connect( noeditor,         SIGNAL(entered()),  this, SLOT(stateNoEditor()           ) );
  connect( editorpresent,    SIGNAL(entered()),  this, SLOT(stateEditorPresent()      ) );
  connect( nosubwindow,      SIGNAL(entered()),  this, SLOT(stateNoSubWindow()        ) );
  connect( subwindowpresent, SIGNAL(entered()),  this, SLOT(stateSubWindowPresent()   ) );


  connect(nolattice,    SIGNAL(entered()), this, SLOT(stateNoLattice()   ) );
  connect(notexteditor, SIGNAL(entered()), this, SLOT(stateNoTextEditor()) );
  connect(noeditor,     SIGNAL(entered()), this, SLOT(stateNoEditor()    ) );
  connect(nosubwindow,  SIGNAL(entered()), this, SLOT(stateNoSubWindow() ) );
  connect(notracker,    SIGNAL(entered()), this, SLOT(stateNoTracker()   ) );

  machine_->setInitialState(top); // called when the machine is started 
  machine_->start();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::stateTextEditorLoaded()
{
          updateEditMenuState(); 

           newAct_->setEnabled(true);       
          openAct_->setEnabled(true);
       newTextAct_->setEnabled(true);
      openTextAct_->setEnabled(true);
    calculatorAct_->setEnabled(true);
    importMad8Act_->setEnabled(true);
    importMadXAct_->setEnabled(true);
   importDimadAct_->setEnabled(true);
 importResolveAct_->setEnabled(true);

       //windowStyleAct_->setEnabled(false);
       fileSeparatorAct_->setEnabled(false);
     //openedSubWinsAct_->setEnabled(false);
    winMenuSeparatorAct_->setEnabled(false);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::stateNoTextEditor()
{
 
    updateEditMenuState(); 
    calculatorAct_->setEnabled(false);

    importMad8Act_->setEnabled(true);
    importMadXAct_->setEnabled(true);
   importDimadAct_->setEnabled(true);
 importResolveAct_->setEnabled(true);

      executeAct_->setEnabled(false);

       //windowStyleAct_->setEnabled(false);
       fileSeparatorAct_->setEnabled(false);
     //openedSubWinsAct_->setEnabled(false);
    winMenuSeparatorAct_->setEnabled(false);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::stateTrackerLoaded()
{
  trackerPlotPositionsAct_->setEnabled(true);
     trackerSavePositionsAct_->setEnabled(true);
  trackerPlotDistributionAct_->setEnabled(true);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::stateNoTracker()
{
     trackerPlotPositionsAct_->setEnabled(false);
     trackerSavePositionsAct_->setEnabled(false);
  trackerPlotDistributionAct_->setEnabled(false);
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimMainWindow::updateFitMenuState(bool yes)
{
  fitUndoAct_->setEnabled(yes);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::updateEditMenuState(bool yes)
 {

   if( machine_->configuration().contains(fitinprogress_) ) return;  

   QMdiSubWindow* sw =  mdiArea->activeSubWindow();
    
   QPlainTextEdit* editor = sw ?  qobject_cast<QPlainTextEdit*>( sw->widget()) : 0;
   OptimPlot* plot   = sw ?  qobject_cast<OptimPlot*>( sw->widget()) : 0;

   if (editor ) {
     bool hasselection = editor->textCursor().hasSelection(); 
        cutAct_->setEnabled(hasselection);
       copyAct_->setEnabled(hasselection);
      pasteAct_->setEnabled(editor->canPaste() );
     deleteAct_->setEnabled(hasselection);
   clearAllAct_->setEnabled(true);
       findAct_->setEnabled(true);
    replaceAct_->setEnabled(true);
       nextAct_->setEnabled(true);
       undoAct_->setEnabled(editor->document()->isUndoAvailable() );
       redoAct_->setEnabled(editor->document()->isRedoAvailable() );
       saveAct_->setEnabled(editor->document()->isModified() );
   }
   else if (plot) {
     cutAct_->setEnabled(false);
       copyAct_->setEnabled(true);
      pasteAct_->setEnabled(false);
     deleteAct_->setEnabled(false);
   clearAllAct_->setEnabled(true);
       findAct_->setEnabled(true);
    replaceAct_->setEnabled(true);
       nextAct_->setEnabled(true);
       undoAct_->setEnabled(false);
       redoAct_->setEnabled(false);
       saveAct_->setEnabled(false);
   }
   else {
       saveAct_->setEnabled(false);
        cutAct_->setEnabled(false);
       copyAct_->setEnabled(false);
      pasteAct_->setEnabled(false);
     deleteAct_->setEnabled(false);
   clearAllAct_->setEnabled(false);
       findAct_->setEnabled(false);
    replaceAct_->setEnabled(false);
      nextAct_->setEnabled(false);
      undoAct_->setEnabled(false);
      redoAct_->setEnabled(false);
    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::updateEditMenuState(QMdiSubWindow* sw)
{
  if (!sw) return;  // QMdiArea returns 0 when it has just deactivated its last active window

  if (qobject_cast<QPlainTextEdit*>(sw->widget()) ) {
    searchMenu_->setEnabled(true);
  }
  else {
    searchMenu_->setEnabled(false);
  }  

  updateEditMenuState();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::resizeLatticeWindow()
{
  if (!LatticeCh_) return;
  LatticeCh_->setGeometry(0, 0, mdiArea->geometry().width()/2,  mdiArea->geometry().height());
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::stateNoSubWindow()
{  
  //  std::cout << " OptimMainWindow::stateNoSubWindow() "<< std::endl;

  printPreviewAct_->setEnabled(false);
         printAct_->setEnabled(false);
    printSetupAct_->setEnabled(true);
          exitAct_->setEnabled(true);
        saveAsAct_->setEnabled(false);

       cascadeAct_->setEnabled(false);
	  tileAct_->setEnabled(false);
  arrangeIconsAct_->setEnabled(false);
      closeAllAct_->setEnabled(false);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::stateSubWindowPresent()
{  
  //  std::cout << " OptimMainWindow::stateSubWindowPresent() "<< std::endl;
        saveAsAct_->setEnabled(true);
  printPreviewAct_->setEnabled(true);
         printAct_->setEnabled(true);
    printSetupAct_->setEnabled(true);
          exitAct_->setEnabled(true);

       cascadeAct_->setEnabled(true);
	  tileAct_->setEnabled(true);
  arrangeIconsAct_->setEnabled(true);
      closeAllAct_->setEnabled(true);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::stateNoEditor()
{  
  //std::cout << " OptimMainWindow::stateNoEditor() "<< std::endl;
         saveAct_->setEnabled(false);
  getFileNameAct_->setEnabled(false);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::stateEditorPresent()
{  
  //      std::cout << " OptimMainWindow::stateEditorPresent() "<< std::endl;
          // saveAct_->setEnabled(true);

	  // if an editor is present,    
	  
          getFileNameAct_->setEnabled(true);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


QMdiSubWindow* OptimMainWindow::getLatestDetached(WindowId id)  
{
  // check if a detached version exist. If so, return a pointer to it
  // otherwise return a null pointer. 

  auto wlist = mdiArea->subWindowList(QMdiArea::CreationOrder); 

  QMdiSubWindow* dw = 0; // detached windowof type id

  auto it=wlist.end();
  while ( it-- != wlist.begin() ) { // inverse creation order
    if ((*it)->property("attached").toBool())  continue;
    if ((*it)->property("winid").toInt() == id ) {
      dw = (*it);
      break;
    }
  };

  return dw;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimPlot* OptimMainWindow::addPlot(WindowId id, PlotSpec const& plotspecs,
				    std::vector<LegoData> const& legodata,
                                    DBSpecs const& dbspecs)
{
  // add a plot in the mdi area

  static std::vector<LegoData> empty; // an empty vector

  OptimPlot* plot  = 0; 
  QMdiSubWindow* previous = mdiArea->activeSubWindow();

  // if the global CtSt_.PlotBoxes is false. no lego widget is created;
  // in this case, no control of the latter is available from the plot local menu..
  
  auto sw = getAttachedSubWin(id); 
  if (!sw) { 
    sw = createAttachedSubWin( (plot = new OptimPlot(this, (CtSt_.PlotBoxes ? legodata : empty) )), id);
    plot->configure(plotspecs, dbspecs);
    sw->raise(); 
  }
  else {
    plot = qobject_cast<OptimPlot*>( sw->widget() );
    auto old_opts = plot->getPlotOpts();
    
    if (CtSt_.ClearPlot ) { // eliminate all but one of the curves (QwtPlotCurve)
      plot->removeDuplicateCurves();
     }; 
    
     plot->configureCurves(plotspecs,CtSt_.ClearPlot, dbspecs);  // refresh the curves only   

     plot->setLegoData(CtSt_.PlotBoxes ? legodata : empty);  // legodata needs to be reset because it may have changed.
                                                          // speed could be improved by refreshing only when hor axis
                                                          // is changed. The user may also simply hide the lego plot.  
     plot->setPlotOpts(old_opts);
     plot->setOptions();
  };
  

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

  return plot;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdFittingControl()
{
  static FitControlDialog* dialog = 0;

  if (!dialog) {
    dialog = new  FitControlDialog(); 
  }

  dialog->data_.use_fractional_tune = CtSt_.use_fractional_tune;
  dialog->set();

  if (dialog->exec() == QDialog::Rejected) return;

  CtSt_.use_fractional_tune = dialog->data_.use_fractional_tune; 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::closeEvent(QCloseEvent* event)
{
  int status = cmdExit();
  if (status ==  QMessageBox::Ok) {
    event->accept();
  }
  else {
    event->ignore();
  }
}

#if 0
void OptimMainWindow::closeEvent(QCloseEvent *event)
 { 
     interrupted_ = true;
     //mdiArea->closeAllSubWindows();
     //if (mdiArea->currentSubWindow()) {
     //    event->ignore();
     //} else {
     //    writeSettings();
         event->accept();
     //}
 }
#endif
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

