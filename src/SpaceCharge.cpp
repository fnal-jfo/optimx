//  =================================================================
//
//  SpaceCharge.cpp
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

#include <limits>
#include <iostream>
#include <cstdlib> // for gentenv

#include <Element.h>
#include <Cavity.h>
#include <Constants.h>
#include <FunctionDialog.h>
#include <OptimCalc.h>
#include <OptimEditor.h>
#include <OptimMainWindow.h>
#include <OptimPlot.h>
#include <OptimTextEditor.h>
#include <OptimMessages.h>
#include <RootFinder.h>
#include <SpaceChargeControlDialog.h>
#include <Structs.h>
#include <Twiss.h>
#include <Utility.h>
#include <RMatrix.h>

#include <QCoreApplication>
#include <QProgressDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPointer>

#include <QwtLegend>

#include <memory>
#include <iostream>

using Constants::PI;
using Constants::C_DERV1;

using Utility::strcmpr;
using Utility::filterName;
using Utility::decodeLine;

#define LSTR 1024


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::GetSpaceChargeParam(bool Reprint, BunchParam& bunch)
{
   int buflen=4;
   char const* bufa[] = {"SpaceChargeBlock","  BeamCurrent[A]=0.1",
			 "  BunchLength[cm]=1.0   dP/P=0.001   Alpha[-1,1]=0.   S_Offset[cm]=0.",
			 "EndSpaceChargeBlock"};

   double dat[6];
   char buf[257], bel[3]={'\015','\012','\000'};
   int i;
 
   if (!LatticeCh_) return 1; 
   OptimEditor* editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());
   if (!editor) return 1;

   int nmblines = editor->document()->lineCount(); 
   int nline    = LineLFin; 

   for( ; nline<nmblines; nline++) { 
     strcpy(buf, editor->getLine(nline).toUtf8().data()); // GetLine(buf, LSTR, nline);
     if(strcmpr(bufa[0],buf)) break;
   }
   // Creating Space Charge Block
   if(nline == nmblines){
     if( OptimQuestionMessage(this, "Space charge", "There is no SpaceChargeBlock. Do you want to create it?", QMessageBox::Yes|QMessageBox::No) ==  QMessageBox::No) return 1;
     replaceLine(editor, nline-1, buf);
     QTextCursor cursor = editor->textCursor();
     cursor.clearSelection();
     cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);   
     for(i=0; i<buflen; i++){ 
        cursor.insertText(bufa[i]);
        cursor.insertText(bel); 
     }
     return 1;
   }

   // decoding of the beam current
   nline++;
   nline = getLineCmt(editor, buf, LSTR, nline);
   decodeLine(buf,dat,1);
   bunch.I=dat[0];

   if(Reprint){
     sprintf(buf,"  BeamCurrent[A]=%lg",bunch.I);
     replaceLine(editor, nline-1, buf);
   }
  
   // decoding of the bunch parameters
   nline=getLineCmt(editor, buf, LSTR, nline);
   decodeLine(buf,dat,4);
   bunch.L       = dat[0];
   bunch.Dp      = dat[1];
   bunch.alpha   = dat[2];
   bunch.sOffest = dat[3];
   if(Reprint){
     sprintf(buf,"  BunchLength[cm]=%lg   dP/P=%lg   Alpha[-1,1]=%lg     S_Offset[sm]=%lg",
        	 bunch.L, bunch.Dp, bunch.alpha, bunch.sOffest);
     replaceLine(editor, nline-1, buf);
   }


   // Checking last comment
   getLineCmt(editor, buf, LSTR, nline);
   if(strcmpr(bufa[buflen-1],buf)) return 0;
   OptimMessageBox::warning(this, "Space charge", "Error: There is no EndSpaceChargeBlock statement",QMessageBox::Ok);
   return 1;


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdSpcharge_betas()
{
  if( interrupted_ ) { interrupted_= false; return; }

  if(analyzed_) {
     if(analyze(false)) return;
  } 
  else {
     if(analyze(true)) return;
  }

  RMatrix tm;
  std::complex<double> lambda[4], ev[4][4];
  char err[256];
   
  Twiss4D v;
  BunchParam bunch;
 
  if( Get4DBetasFromFile(false, false, v) ) return;

  if( GetSpaceChargeParam(false, bunch)   ) return;

  bool     const use4dblock =  SCSt_.use4dblock;
  
  if( CtSt_.IsRingCh ){
   
     // if specified, use lattice funtions from 4d beta block as a first guess  
     // else, compute the periodic solution for Ibeam =0
    
     if (use4dblock) {
       if(Get4DBetasFromFile(false, false, v)) return;
     }
     else {  
       findRMatrix(tm);
       if( RMatrix_t<4,double>(tm).findEigenValues(lambda, ev)!=0){
         OptimMessageBox::warning(this, "CW Betas", "Cannot close lattice.", QMessageBox::Ok);
        return;
       }
     
       v.betatronFromEV(ev);
       getDisp4D(tm, v, err);

       if(err[0]){ OptimMessageBox::warning(this, "Space Charge", err, QMessageBox::Ok); return;}
     }
     
     SpchargePeriodicBetas(v, bunch, true);
  }

  else { 
     if(Get4DBetasFromFile(false, false, v)) return;

#if 0
     char* envdbg = std::getenv("OPTIMX_LEGACY");

     if (envdbg) {
      std::cerr << "Calling LEGACY Spcharge_betas " << std::endl;  
      Spcharge_betas(v, bunch, true); // original
     }
     else {
       spChargeBetas(v, bunch, 1.0, true);
     }
#else
     spChargeBetas(v, bunch, 1.0, true);
#endif
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::SpchargePeriodicBetas(Twiss4D& v, BunchParam& bunch, bool display_on)   
{
  
  double   const alpha      =  SCSt_.relaxation;
  int      const maxiters   =  SCSt_.maxiters;
  double   const eps        =  SCSt_.eps;
  int      const isteps     =  SCSt_.isteps;
  bool     const use4dblock =  SCSt_.use4dblock;
  
  Twiss4D dv;

//..........................................................................
#ifdef USE_GSL  
//..........................................................................

  QProgressDialog progressdlg;
  progressdlg.setWindowModality(Qt::ApplicationModal);
  connect( &progressdlg, SIGNAL(canceled()), SLOT( cmdFittingStop()) );   
  progressdlg.resize(400, 150);
  progressdlg.setLabelText("Computing Periodic 4D Lattice Functions");

  double scale  = 1.0; 

  // .....................................
  // objective function for periodic match
  // ......................................
  
  auto objfunc = [this, &bunch, &scale, &v]( double* x, double* fx ) {

      for (int i=0; i<15; ++i) {

	// the code below prevents the betas to ever become negative.  

	if (x[0] < 0.0 ) { x[0] = 0.0; };
	if (x[2] < 0.0 ) { x[2] = 0.0; };
	if (x[4] < 0.0 ) { x[4] = 0.0; };
	if (x[6] < 0.0 ) { x[6] = 0.0; };

	(&v.btx1)[i] = x[i];
      }
    
      Twiss4D vout(v);

      //Spcharge_betas(vout, bunch, scale, false);
      spChargeBetas(vout, bunch, scale, false);

      for (int i=0; i<15; ++i) { fx[i] = (&vout.btx1)[i] -  (&v.btx1)[i];} //  12 parameters:  8 lattice functions + 2 dispersions + 2 disp derivatives +  (u + 2 angles)

    return 0;
  };

  // .....................................
  // callback (after every iteration) 
  // ......................................

  auto update = [this, &progressdlg, isteps ](void* p) {

    struct info_t { int iter;  int maxiters; int step; int maxsteps;  double residual; }; 

    auto info = reinterpret_cast< info_t*> (p);

    int    iter     = info->iter;
    int    step     = info->step;
    int    maxiters = info->maxiters;
    int    maxsteps = info->maxsteps;
    double residual = info->residual;

    
    progressdlg.setValue(int( 100*double(iter)/double(maxiters)) );  
    progressdlg.setLabelText(QString("Computing Periodic 4D Lattice Functions\n") +
			      QString("Current step: %1 of %2 \n").arg(step,5).arg(maxsteps,5) +
			      QString("Iteration %1 of %2 (max).\n Residual = %3\n").arg(iter,5).arg(maxiters,5).arg(residual,5));
    
    QCoreApplication::processEvents();
   };

  double    xinit[] = {  v.btx1, v.alfx1,  v.bty1, v.alfy1,  v.btx2, v.alfx2, v.bty2, v.alfy2, v.dx, v.dxp, v.dy, v.dyp, v.u, v.teta1, v.teta2 };
  double     dscale = 1/double(isteps); 
  bool   converged  = false;


  Twiss4D vold(v);
  try {

    RootFinder solver(15, objfunc, update, (RootFinder::Algorithm) SCSt_.algo);    // vary only the first 12 parameters

    for (int k=1; k<=isteps; ++k) { 
      scale = k*dscale; 
      converged = ( 0 == solver(xinit, eps, maxiters, isteps));
      if (!converged || interrupted_ ) break;
    }

    for (int i=0; i<15; ++i) { (&vold.btx1)[i]  = xinit[i]; }; 
  }
 
  catch( std::exception const& e) {
    //std::cout << e.what() << std::endl;
    converged = false;
  };


  if (converged ) {
   
     Spcharge_betas( vold, bunch, scale, display_on);
     //v.teta1 = v.teta2 = 0.0; 
     Print4DBetasToMain(vold);
     progressdlg.setValue(100);
   }
  else {
    OptimMessageBox::warning(this, "Space charge", "Error: Iterations failed to converge.\n Try increasing the no of current ramping steps. ", QMessageBox::Ok);
    progressdlg.setValue(100);
    return 1;
  }

//..........................................................................
#else 
//..........................................................................
  //scale is set to 1.0 (full current)
  //Spcharge_betas( v, bunch, 1.0, display_on);
  spChargeBetas( v, bunch, 1.0, display_on);
  return 0; // all ok
#endif


}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdSpcharge_sizes()
{
  double   const alpha      =  SCSt_.relaxation;
  int      const maxiters   =  SCSt_.maxiters;
  double   const eps        =  SCSt_.eps;
  int      const isteps     =  SCSt_.isteps;
  bool     const use4dblock =  SCSt_.use4dblock;
  
  Twiss4D v;
  RMatrix tm;
  BunchParam bunch;
  std::complex<double> lambda[4], ev[4][4];
  char err[256];

  err[0] = 0;
  
  if(interrupted_ ) { interrupted_ = false; return;}
  if(analyzed_) { if(analyze(false))return;} else {if(analyze(true))return;}

  if(Get4DBetasFromFile(false, false, v)) return;
  if(GetSpaceChargeParam(false, bunch)) return;

  if( CtSt_.IsRingCh ){
   
  // if specified, use lattice funtions from 4d beta block as a first guess  
  // else, compute the periodic solution for Ibeam =0
    
    if ( !use4dblock) {
      findRMatrix(tm);
      if( RMatrix_t<4,double>(tm).findEigenValues( lambda, ev)!=0){
         OptimMessageBox::warning(this, "CW Betas", "Cannot close lattice.", QMessageBox::Ok);
         return;
      }
      v.betatronFromEV(ev);
      getDisp4D(tm, v, err);
      if(err[0]){ OptimMessageBox::warning(this, "Space Charge", err, QMessageBox::Ok); return;}

    }

    if ( SpchargePeriodicBetas(v, bunch, false) ) return;

  }

#if 0
  char* envdbg = std::getenv("OPTIMX_LEGACY");
  if (envdbg) {
    std::cerr << "Calling LEGACY SpaceChargeSizes." << std::endl;
    SpaceChargeSizes(v, bunch);
  }
  else {
    spChargeSizes(v, bunch);
  }
#else
  spChargeSizes(v, bunch);
#endif
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdSpcharge_phases()
{

  double   const alpha      =  SCSt_.relaxation;
  int      const maxiters   =  SCSt_.maxiters;
  double   const eps        =  SCSt_.eps;
  int      const isteps     =  SCSt_.isteps;
  bool     const use4dblock =  SCSt_.use4dblock;
  
  Twiss4D v;
  RMatrix tm;
  BunchParam bunch;
  std::complex<double> lambda[4], ev[4][4];
  char err[256]; err[0] = 0;
  
  if(interrupted_ ) { interrupted_ = false; return;}
  if(analyzed_) { if(analyze(false))return;} else {if(analyze(true))return;}

  if(Get4DBetasFromFile(false, false, v)) return;
  if(GetSpaceChargeParam(false, bunch)) return;

  if( CtSt_.IsRingCh ){
   
  // if specified, use lattice funtions from 4d beta block as a first guess  
  // else, compute the periodic solution for Ibeam =0
    
    if ( !use4dblock) {
      findRMatrix(tm);
      if( RMatrix_t<4,double>(tm).findEigenValues( lambda, ev)!=0){
         OptimMessageBox::warning(this, "CW Betas", "Cannot close lattice.", QMessageBox::Ok);
         return;
      }
      v.betatronFromEV(ev);
     getDisp4D(tm, v, err);

     if(err[0]){ OptimMessageBox::warning(this, "Space Charge", err, QMessageBox::Ok); return;}
    }  

    if ( SpchargePeriodicBetas(v, bunch, false) ) return;
  }

#if 0
  char* envdbg = std::getenv("OPTIMX_LEGACY");
  if (envdbg) {
    std::cerr << "Calling LEGACY Spcharge_phases." << std::endl; 
    Spcharge_phases(v, bunch);
  }
  else {
    spChargePhases(v, bunch);
  }
#else
   spChargePhases(v, bunch);
#endif
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdSpChargeControl()
{

  static SpaceChargeControlDialog* dialog; 
  const  int maxsteps  = 100000;             // maximum number of steps for space charge integration  
  
  if (!dialog) {
    dialog = new SpaceChargeControlDialog(0); 
    dialog->data_ = SCSt_;
  }
  dialog->set();

  if (dialog->exec() == QDialog::Rejected) return;

  SCSt_ =  dialog->data_;
  space_charge_step_ = SCSt_.step; 

  if (!LatticeCh_) return; 

  OptimEditor* editor =  qobject_cast<OptimEditor*>( LatticeCh_->widget() ); 

  if(editor){
    if(interrupted_ ) { interrupted_=false; return;}
    if(analyzed_) {
       if (analyze(false)) return; 
    } 
    else {
       if(analyze(true))return;
    }
  }

  if(space_charge_step_ < Length_/maxsteps) space_charge_step_ = Length_/maxsteps;

  return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdSpacecharge_proj()
{

  double   const alpha      =  SCSt_.relaxation;
  int      const maxiters   =  SCSt_.maxiters;
  double   const eps        =  SCSt_.eps;
  int      const isteps     =  SCSt_.isteps;
  bool     const use4dblock =  SCSt_.use4dblock;
  
  Twiss4D v;
  RMatrix tm;
  BunchParam bunch;
  std::complex<double> lambda[4], ev[4][4];
  char err[256]; err[0] = 0;
  
  if(interrupted_ ) { interrupted_ = false; return;}
  if(analyzed_) { if(analyze(false))return;} else {if(analyze(true))return;}

  if(Get4DBetasFromFile(false, false, v)) return;
  if(GetSpaceChargeParam(false, bunch)) return;

  if( CtSt_.IsRingCh ){
   
  // if specified, use lattice funtions from 4d beta block as a first guess  
  // else, compute the periodic solution for Ibeam =0
    
    if ( !use4dblock) {
      findRMatrix(tm);
      if( RMatrix_t<4,double>(tm).findEigenValues( lambda, ev)!=0){
         OptimMessageBox::warning(this, "CW Betas", "Cannot close lattice.", QMessageBox::Ok);
         return;
      }

      v.betatronFromEV(ev);
      getDisp4D(tm, v, err);
    }  


    if(err[0]){ OptimMessageBox::warning(this, "Space Charge", err, QMessageBox::Ok); return;}
      
    if ( SpchargePeriodicBetas(v, bunch, false) ) return;
  }

#if 0
  char* envdbg = std::getenv("OPTIMX_LEGACY");
  if (envdbg) {
    std::cerr << "Calling LEGACY SpaceChargeProj." << std::endl;
    SpaceChargeProj(v, bunch);
  }
  else {
     spChargeProj(v, bunch);
  }
#else
  spChargeProj(v, bunch);
#endif
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::spaceChargeInitTwiss4D( Twiss4D& v, BunchParam& bunch) 
{

 bool     const use4dblock =  SCSt_.use4dblock;
   
 RMatrix tm;
 std::complex<double> lambda[4], ev[4][4];
 char err[256]; err[0] = 0;
 
 if(  Get4DBetasFromFile(false, false, v) )  return 1;
 if(  GetSpaceChargeParam(false, bunch  ) )  return 1;
   
  if( CtSt_.IsRingCh ){
   
     // if specified, use lattice functions from 4d beta block as a first guess  
     // else, compute the periodic solution for Ibeam =0
    
     if ( !use4dblock) {
        findRMatrix(tm);
        if( RMatrix_t<4,double>(tm).findEigenValues( lambda, ev)!=0){
           OptimMessageBox::warning(this, "CW Betas", "Cannot close lattice.", QMessageBox::Ok);
           return 1;
        }
        v.betatronFromEV(ev);
        getDisp4D(tm, v, err);
        if(err[0]){ OptimMessageBox::warning(this, "Space Charge", err, QMessageBox::Ok); return 1;}
     }  
      
     if ( SpchargePeriodicBetas(v, bunch, false) ) return 1;
  }

  return 0; // all ok.
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdSpacecharge_functions()
{
   static FunctionDialog*  dialog = 0;

   if (!dialog) {
     dialog = new  FunctionDialog(0);
   }
   
   strcpy(NStf_.Filter, funcFilter);
   NStf_.Step          = stepv;
   NStf_.Start         = startFrom;
   NStf_.Stop          = stopAt;
   NStf_.printname     = printnames;
   NStf_.NoFirstLetter = NoFirstLetter;
   NStf_.MatchCase     = MatchCase;
   NStf_.PrintEnerg    = PrintEnerg;
   NStf_.StartSfrom0   = StartSfrom0;
  
   dialog = new  FunctionDialog(0);
   dialog->data_ = NStf_;
   dialog->set();

   if ( dialog->exec() == QDialog::Rejected) return;
   NStf_ = dialog->data_;

   
   if(interrupted_)  { interrupted_ = false; return;  }
   if(analyzed_) { if(analyze(false)) return; } else { if(analyze(true))  return; }

#if 0
   char* envdbg = std::getenv("OPTIMX_LEGACY");
   if (envdbg) {
     std::cerr << "Calling LEGACY  ViewFunctionsSpCh." << std::endl;
     ViewFunctionsSpCh(0, &NStf_);
   }
   else {
     tabulateFunctionsSpCh(0, &NStf_);
   }
#else
  tabulateFunctionsSpCh(0, &NStf_);
#endif
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimMainWindow::cmdSpChargeCloseLattice()
{

 if(interrupted_){interrupted_=false; return;} 
 if(analyze( !analyzed_)) return;
 
 int ir = 0; // return code
 char err[256];
 Twiss4D v;
 RMatrix tm;
 std::complex<double> lambda[4];
 std::complex<double> ev[4][4];
   
 bool     const use4dblock =  SCSt_.use4dblock;
 BunchParam bunch;    

 
 // if specified, use lattice funtions from 4d beta block as a first guess  
 // else, compute the periodic solution for Ibeam =0

  if( Get4DBetasFromFile(false, false, v)) return; //  This sets the eigen-emittances
  if( GetSpaceChargeParam(false, bunch) )  return; //    

  if (!use4dblock) {  
    findRMatrix(tm); // finds the R matrix w/o space charge 
    if( RMatrix_t<4,double>(tm).findEigenValues( lambda, ev)!=0) {
      OptimMessageBox::warning(this, "CW Betas", "Cannot close lattice.", QMessageBox::Ok);
      return;
    }
    v.betatronFromEV(ev);
    getDisp4D(tm, v, err);
    if(err[0]){ OptimMessageBox::warning(this, "Space Charge", err, QMessageBox::Ok); return;}
  }
  else { 
     int ierr = SpchargePeriodicBetas(v, bunch, false);
    if (ierr != 0) return; // could not find periodic solution with sc 
  }

  // a periodic solution with sc must be found before the matrix can be computed.
  // since the beam size must be known  

  Print4DBetasToMain(v);
   
  OptimTextEditor* editor = 0;
  auto Dig4Ch = getAttachedSubWin(WindowId::Dig4Ch); 
  if (!Dig4Ch) { 
     Dig4Ch = createAttachedSubWin( (editor = new OptimTextEditor()), WindowId::Dig4Ch);
     connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
     connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
   }
   else { 
     editor =  qobject_cast<OptimTextEditor*>( Dig4Ch->widget() );
    if (CtSt_.ClearText) editor->clear();
   }

   Dig4Ch->raise();

#if 0
   // OPTIMX_LEGACY is now obsolete ..... 
   char* envdbg = std::getenv("OPTIMX_LEGACY");
   if (envdbg) {
     std::cerr << " Calling LEGACY spFindRMatrix(). " << std::endl;
     spFindRMatrix(tm, v, bunch, 1.0);
   }
   else {
     spChargeRMatrix(tm, v, bunch, 1.0);   
   }
#else
    spChargeRMatrix(tm, v, bunch, 1.0);
#endif
    
   RMatrix_t<4,double>(tm).findEigenValues( lambda, ev);
   //v.betatronFromEV(ev);
   //getDisp4D(tm, v, err);
 
   TypeEigenVectorsToOutW(editor, tm, lambda, ev, v, ir);

   editor->document()->setModified(false);
   editor->show();
}


