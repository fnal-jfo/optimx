//  =================================================================
//
//  OptimCLTxt.cpp
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
#include <iterator>
#ifdef  _MSC_VER 
#include <io.h>	
#else
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <fmt/format.h>

#include <OptimMainWindow.h>
#include <OptimMdiSubWindow.h>
#include <OptimTextEditor.h>
#include <OptimCalc.h>
#include <Element.h>
#include <RMatrix.h>
#include <Twiss.h>
#include <TrackParam.h>
#include <Utility.h>
#include <Constants.h>
#include <LatticeDialog.h>
#include <MatrixDialog.h>
#include <OptimMessages.h>
#include <ParticleTrackingDialog.h> 
#include <ToolsControlDialog.h> 
#include <IntegrationStepDialog.h> 
#include <ElementSelectionDialog.h> 
#include <OrbitDialog.h> 
#include <fcntl.h>

#include <QTextEdit>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QApplication>
#include <QPointer>

using Constants::PI;
using Constants::C_DERV1;
using Constants::KVSR;
using Constants::KE;
using Constants::C_CGS;
using Utility::filterName;
using Utility::matr_invert;

#define LSTR 1024

#ifdef __linux__
#define O_BINARY 0
#endif



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdSaveFunctions ()
{

   QString filename = QFileDialog::getSaveFileName(this, "Save Functions", 0, 0); 
   if (filename == "" ) return;

   FunctionDlgStruct NStf;
   static FunctionDialog* dialog = 0; 
   if (!dialog) {
     dialog = new  FunctionDialog(0); 
     strcpy(NStf.Filter, funcFilter);
     NStf.Step          = stepv;
     NStf.Start         = startFrom;
     NStf.Stop          = stopAt;
     NStf.printname     = printnames;
     NStf.NoFirstLetter = NoFirstLetter;
     NStf.MatchCase     = MatchCase;
     NStf.PrintEnerg    = PrintEnerg;
     NStf.StartSfrom0   = StartSfrom0;
     dialog->data_      = NStf;
     dialog->set();
   }

   if (dialog->exec() == QDialog::Rejected) return;
   NStf = dialog->data_;;

   strcpy(funcFilter, NStf.Filter);
   stepv         = NStf.Step;
   startFrom     = NStf.Start;
   stopAt        = NStf.Stop;
   printnames    = NStf.printname;
   NoFirstLetter = NStf.NoFirstLetter;
   MatchCase     = NStf.MatchCase;
   PrintEnerg    = NStf.PrintEnerg;
   StartSfrom0   = NStf.StartSfrom0;

   ViewFunctions(filename.toUtf8().data(), &NStf, 0, false);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdViewFunctions ()
{

  static FunctionDialog* dialog = 0; 
  FunctionDlgStruct NStf;
  NStf.Filter[0] = 0; // empty string
    
  if (!dialog) {
    dialog = new FunctionDialog(0); 
      
    strcpy(NStf.Filter, funcFilter);
    NStf.Step          =  stepv;
    NStf.Start         =  startFrom;
    NStf.Stop          =  stopAt;
    NStf.printname     =  printnames;
    NStf.NoFirstLetter =  NoFirstLetter;
    NStf.MatchCase     =  MatchCase;
    NStf.PrintEnerg    =  PrintEnerg;
    NStf.StartSfrom0   =  StartSfrom0;
    dialog->data_ = NStf;

    dialog->set();
  }
  if (dialog->exec() == QDialog::Rejected) return;

  NStf = dialog->data_;
  
  if( CtSt_.IsRingCh ) {
    ViewFunctions(0, &NStf, 0, true);
  }
  else { 
    ViewFunctions(0, &NStf, 0, false);
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int OptimMainWindow::ViewFunctions (char const* filenm, FunctionDlgStruct* NStf, char const* comment, bool ClsLat)
{

  auto restore = [](int* p){ QGuiApplication::restoreOverrideCursor(); delete p;};
  std::unique_ptr<int,decltype(restore)> cursor_guard(new int, restore); 
  QGuiApplication::setOverrideCursor(Qt::WaitCursor);

   FILE *fp;
   char const *pname;
   char buf[512];
   unsigned  lastline;
   int ierr = 0;  
   
   OptimTextEditor* editor = 0;

   QPointer<QMdiSubWindow> DigCh(0);
   
   if (!filenm) {
     DigCh = getAttachedSubWin(WindowId::DigCh);

     if (!DigCh) { 
       DigCh = createAttachedSubWin( (editor = new OptimTextEditor()), WindowId::DigCh);
    }
    else { 
       editor = qobject_cast<OptimTextEditor*>(DigCh->widget() );
       if (CtSt_.ClearText) editor->clear();
    }

    DigCh->raise();

   }

   if (interrupted_) { interrupted_= false; return 0;}

   if(analyzed_) {
     if( analyze(false)) return 0;
   } 
   else { 
     if(analyze(true)) return 0;
   }

   strcpy(funcFilter, NStf->Filter);
   stepv         = NStf->Step;
   startFrom    =  NStf->Start;
   stopAt       =  NStf->Stop;
   printnames   =  NStf->printname;
   NoFirstLetter=  NStf->NoFirstLetter;
   MatchCase    =  NStf->MatchCase;
   PrintEnerg   =  NStf->PrintEnerg;
   StartSfrom0  =  NStf->StartSfrom0;

   char const* fln1[] = {"#    N", "NAME",  "S[cm]", "BetaX", "AlphaX", "BetaY", "AlphaY", "DspX", "DspXp", "DspY", "DspYp", "NuX", "NuY"};
   char const* fln2[] = {"#    N", "S[cm]", "BetaX", "AlphaX", "BetaY", "AlphaY", "DspX", "DspXp", "DspY", "DspYp", "NuX", "NuY"};

   char const* format_t  = "%6d\t%12.1f\t%12g\t%12g\t%12g\t%12g\t%12g\t%12g\t%12g\t%12g\t%12g\t%12g\t%12d\t%12d";
   char const* formatn_t = "%6d\t%12s\t%12.2f\t%12g\t%12g\t%12g\t%12g\t%12g\t%12g\t%12g\t%12g\t%12g\t%12g";

   char const* formatn  = "{:8d} {:>16s} {:12.2f} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g}";
   char const* format   = "{:8d} {:12.2f} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g}";

   auto mbufRaw = fmt::memory_buffer();

#if FMT_VERSION < 80000
   auto& mbuf = mbufRaw;
#else
   auto mbuf = std::back_inserter(mbufRaw);
#endif
    
   if(filenm) {
     if((fp = fopen(filenm,"w"))==0) {
       OptimMessageBox::warning( this, "Error opening file for save operation. File name -", filenm, QMessageBox::Ok);
       return 1;
     }
   }
   else {
     lastline = editor->document()->lineCount();
   }

  if (printnames) {
       format_to(mbuf,"#{:>7s} {:>16s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s}",
                 "N", "NAME",  "S[cm]", "BetaX[cm]", "AlphaX", "BetaY[cm]", "AlphaY", "DspX[cm]", "DspXp", "DspY[cm]", "DspYp", "NuX", "NuY");
  }
  else {
    format_to(mbuf,"#{:>7s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s}",
                 "N", "S[cm]", "BetaX[cm]", "AlphaX", "BetaY[cm]", "AlphaY", "DspX[cm]", "DspXp", "DspY[cm]", "DspYp", "NuX", "NuY");
  }

  if(PrintEnerg) {
    format_to(mbuf, " {:>12s}\n", "Energy[MeV]");
  }
  else {
    format_to(mbuf, "\n");
   }
  

   Twiss v;
   Element e;
   
   int i;

   std::complex<double> ev[4][4];

   if( (filenm) && (comment!=0)) { fprintf(fp,"%s\n", comment); }

   int i0    = (startFrom > 0.0) ? startFrom-1 : 0;
   int nelm0 = ((stopAt>(i0+1))&&(stopAt<nelm_)) ? stopAt : nelm_;

   v.BtX  = BetaXin;        v.BtY  = BetaYin;
   v.AlX  = AlfaXin;        v.AlY  = AlfaYin;
   v.DsX  = DispXin;        v.DsY  = DispYin;
   v.DsXp = DispPrimeXin;   v.DsYp = DispPrimeYin;
   v.nuX  = QXin;	    v.nuY  = QYin;

   RMatrix tm;
   double dalfa = 0.0;

   if(ClsLat){
     findRMatrix(tm);
     i = find_tunes(tm, 100.0, v, &dalfa);
     if(i){
       OptimMessageBox::warning(this, "Close Error", "Cannot close for X or Y", QMessageBox::Ok);
       return 1;
     }
     v.nuY=v.nuX=0.;
   }

   double Lp = StartSfrom0 ? 0.0 : so0_;
   
   if(printnames) {
     format_to(mbuf, formatn, 0, "START", Lp, v.BtX, v.AlX, v.BtY, v.AlY, v.DsX,
	     v.DsXp, v.DsY, v.DsYp, v.nuX, v.nuY);
   }
   else {
     format_to(mbuf, format, 0, Lp, v.BtX, v.AlX, v.BtY, v.AlY, v.DsX,
	     v.DsXp, v.DsY, v.DsYp, v.nuX, v.nuY);
   }  

   if(PrintEnerg) {
     format_to(mbuf, " {:12g}\n", Ein);
   }
   else {
     format_to(mbuf, "\n");
   }

   double tetaY = tetaYo0_;
   double Enr   = Ein;

   v.eigenvectors(ev);

   double   coupling = 0.0;
   unsigned int nc   = 0; 

   for(i=0; i<nelm0; ++i){

     auto ep = beamline_[i];

     pname = ep->fullName();

     if(NoFirstLetter) {
       if( ( (ep->name()[1] >='0') ||
	     (ep->name()[1] <='9') ) && (strlen(ep->name()) > 1) ) {
	 pname = &ep->fullName()[1];
       }
     }

     char nm = ep->etype();

     unsigned int ns = ( stepv <= 0) ? 1 : fabs(ep->length()/stepv) + 1; //  is this correct ?
     if ( nm=='A' || nm=='W' ||  nm=='X') {ns=1;}

     // split the Element into ns identical pieces

     std::shared_ptr<Element> e( ep->split(ns) );

     // ***FIXME *** what are jm, jp ???
     // this looks like some sort of id for end type 
     
     int jp = 0;
     int jm = 0;

     switch ( nm ) {
       case 'Q':
       case 'L':
       case 'F':
         jp = (e->G >= 0) ? 3 : -3;
	 jm = jp; // *** FIXME: perhaps this should be jm = -jp ??? 
	 break;
       case 'B': 
       case 'D':
       case 'R':
         jp =  2;
	 jm = -2;
	 break;
       case 'S':
         jp =  1;
	 jm = -1;
	 break;
       default: 
         jp = 0;
	 jm = 0;
      }

     nc +=ns;        // counter to estimate coupling

     dalfa = 0.0;

     for(unsigned int j=0; j<ns; ++j ){
	switch( nm ){
	  case 'B':
	  case 'D':
	    tm = e->rmatrix(dalfa, Enr, ms, tetaY, dalfa, Element::checkEdge(j,ns));
	    dalfa -= e->tilt();
	    break;
	  default:
	    tm = e->rmatrix(Enr, ms, tetaY, 0.0, Element::checkEdge(j,ns) );
        }

	e->propagateLatticeFunctions(tm, v, ev);

        coupling += sqrt(fabs(tm[2][0]*tm[3][1] - tm[2][1]*tm[3][0]));
	Lp +=  e->length();


	if( (i>=i0) && filterName(e->fullName(), funcFilter, MatchCase) ){
	  if(printnames) {
	    format_to(mbuf,formatn, i+1, pname, Lp, v.BtX, v.AlX, v.BtY, v.AlY, v.DsX,
	       	      v.DsXp, v.DsY, v.DsYp, v.nuX, v.nuY);
	  }
	  else { 
	    format_to(mbuf, format, i+1, Lp, v.BtX, v.AlX, v.BtY, v.AlY, v.DsX,
		      v.DsXp, v.DsY, v.DsYp, v.nuX, v.nuY);
	  }
	  if(PrintEnerg) {
            format_to(mbuf, " {:12g}\n",Enr);
          }
          else {
            format_to(mbuf, "\n");
           } 
	}
      } //for
   } //for

   if(printnames){
     format_to(mbuf, formatn, i+1, "END", Lp, v.BtX, v.AlX, v.BtY, v.AlY, v.DsX,
		    v.DsXp, v.DsY, v.DsYp, v.nuX, v.nuY);
     if(PrintEnerg) {
       format_to(mbuf, " {:12g}\n", Enr);
     }
     else {
       format_to(mbuf, "\n", Enr);
     }   
   }
   
   if ( (coupling /= nc) > CtSt_.CouplThreshold){
     sprintf(buf,"XY-coupling (%.2e) above threshhold (%.2e). Change threshold or use View_4D menu instead", coupling,  CtSt_.CouplThreshold);
     OptimMessageBox::warning(this,"View|Betas", buf, QMessageBox::Ok);
   }

   format_to(mbuf, "{:c}",'\0');
   
   if(filenm){
     fprintf(fp,"%s \n", mbufRaw.data());
     if  ( !fclose(fp) ) { 
	  OptimInformationMessage( this, "File Saved -", filenm, QMessageBox::Ok);
     }
   }

   editor->appendPlainText( mbufRaw.data());
   editor->moveCursor(QTextCursor::Start);
   editor->document()->setModified(false); 
   editor->show();
   
   return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdIntegrStep ()
{
  static IntegrationStepDialog* dialog = 0;
  if (!dialog) dialog = new IntegrationStepDialog(0);

  dialog->data_.step = stepint;
  dialog->set();

  if (dialog->exec() == QDialog::Rejected) return;  
  
  stepint =  dialog->data_.step;

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdViewElement ()
{
  if(interrupted_ ) { interrupted_ = false; return; }

  if(analyzed_) { 
    if(analyze(false)) return;
  } 
  else { 
    if(analyze(true)) return;
  }

  static ElementSelectionDialog* dialog = 0;
  if (!dialog) dialog = new ElementSelectionDialog(0);
  
  dialog->data_.elmno = view_elem;
  dialog->set();
 
  if (dialog->exec() == QDialog::Rejected) return; 

  view_elem = dialog->data_.elmno;

  if( (view_elem>=nelm_) || (view_elem<0)) view_elem=0;

  OptimTextEditor* editor = 0;
  auto DigCh = getAttachedSubWin(WindowId::DigCh);
  if ( !DigCh) {
    DigCh = createAttachedSubWin( (editor = new OptimTextEditor()), WindowId::DigCh);
  }
  else  { 
    editor = qobject_cast<OptimTextEditor*>(DigCh->widget() );
     if (CtSt_.ClearText) editor->clear(); 
  } 
  DigCh->raise();
		
  Twiss v;
  RMatrix tm, me;
  std::complex<double> ev[4][4];
  
  v.BtX  = BetaXin;
  v.BtY  = BetaYin;
  v.AlX  = AlfaXin;
  v.AlY  = AlfaYin;
  v.DsX  = DispXin;
  v.DsY  = DispYin;
  v.DsXp = DispPrimeXin;
  v.DsYp = DispPrimeYin;

  v.nuY = v.nuX = 0.;

  if( CtSt_.IsRingCh ) {
    double dalfa = 0.0;
    findRMatrix(tm);
    if ( find_tunes(tm, 100.0, v, &dalfa) != 0) { 
      OptimMessageBox::warning(this, "Close Error", "Cannot close for X or Y", QMessageBox::Ok);
    }
    v.nuY = v.nuX=0.0;
  }

  double tetaY = tetaYo0_;
  double Enr   = Ein;

  tm.toUnity();
  v.eigenvectors(ev);

  double alfa = 0.0;

  for (int i=0; i<nelm_; i++){
    auto ep = beamline_[i];
    me = ep->rmatrix(Enr, ms, tetaY, 0.0, 3);
    ep->propagateLatticeFunctions(me, v, ev);
    if (!view_elem) tm = me*tm;
    if( (i+1)==view_elem ) typeBetas(editor, v, alfa, Enr, view_elem);
  }
  if (!view_elem){

    double gamma = 1 + Ein/ms;
    double gm2   = gamma*gamma;
    
    alfa = - ( v.DsX * tm[4][0] + v.DsXp * tm[4][1] +
	       v.DsY * tm[4][2] + v.DsYp * tm[4][3] + ( tm[4][5] - Length_/gm2) )/ Length_;

    typeBetas(editor, v, alfa, Enr, 0);
  }

  editor->document()->setModified(false);
  editor->show();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdTypeTrajectory ()
{

  static PTrackingDialog* dialog = 0;
  if (!dialog) dialog = new PTrackingDialog(0);

  PTracStruct&  NSt = dialog->data_;

  if(interrupted_ ) {interrupted_ = false; return;}

  strcpy(NSt.Filter, funcFilter);
  NSt.Step      = stepv;
  NSt.Start     = startFrom;
  NSt.Stop      = stopAt;
  NSt.printname = printnames;
  NSt.MatchCase = MatchCase;
  dialog->set();

  if( dialog->exec() == QDialog::Rejected ) return;

  TypeTrajectory(0, &NSt, false, 'T');
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::TypeTrajectory(char const* filenm, PTracStruct *NSt, bool ClsLat, char OutputType)
{

  auto restore = [](int* p){ QGuiApplication::restoreOverrideCursor(); delete p;};
  std::unique_ptr<int,decltype(restore)> cursor_guard(new int, restore); 
  QGuiApplication::setOverrideCursor(Qt::WaitCursor);

  OptimTextEditor* editor = 0;
  auto DigCh = getAttachedSubWin(WindowId:: DigCh);
  if (!DigCh) {
    DigCh = createAttachedSubWin( (editor = new OptimTextEditor()), WindowId:: DigCh);
  }
  else { 
    editor = qobject_cast<OptimTextEditor*>(  DigCh->widget() );
     if (CtSt_.ClearText) editor->clear(); 
  }
  DigCh->raise();
  
  char buf[1024];
  int fb;
  FILE* fp;
  struct BinOutStr {char name[16]; double x, y;} bo;

  strcpy(funcFilter, NSt->Filter);
  stepv      = NSt->Step;
  startFrom  = NSt->Start;
  stopAt     = NSt->Stop;
  printnames = NSt->printname;
  MatchCase  = NSt->MatchCase;

  if (analyze(!analyzed_)) return 0;

  Coordinates v;
  
  if(getTrajParamFromFile(false, false, v)) return 1; // Gets Dp/p from Traj. block

  if ( CtSt_.IsRingCh  || ClsLat ){
     closeTraject(v);
     if (NstTool.FullClosure ) {
       if (!NstTool.Closure4D ) getTrajParamFromFile(false, false, v); // 6D 
       if(closeTrajectFull(v)) {
         OptimMessageBox::warning( this, "Close Error", "Cannot close trajectory. Make sure that Twiss parameters can be closed; then, reduce the convergence tolerance.", QMessageBox::Ok);
       }
     }
  }

  char const*  fln1[] = {"#  N", "L[cm]",  "NAME", "X[cm]", "Px[mrad]", "Y[cm]", "Py[mrad]", "S[cm]", "dP/P[0.1%]", "dt[ns]"};
  char const*  fln2[] = {"#  N", "L[cm]",          "X[cm]", "Px[mrad]", "Y[cm]", "Py[mrad]", "S[cm]", "dP/P[0.1%]", "dt[ns]"};

  char const*  formatn = "{: >8d} {: > 16.6f} {:>16s} {: > 12.4g} {: > 12.4g} {: > 12.4g} {: > 12.4g} {: > 12.4g} {: > 12.4g} {: > 12.4g}\n"; 
  char const*  format  = "{: >8d} {: > 16.6f} {: > 12.4g} {: > 12.4g} {: > 12.4g} {: > 12.4g} {: > 12.4g} {: > 12.4g} {: > 12.4g} {:2d} {:2d}\n"; 

  auto mbufRaw = fmt::memory_buffer();

#if FMT_VERSION < 80000  
  auto& mbuf = mbufRaw;
#else
  auto mbuf = std::back_inserter(mbufRaw);
#endif
  
  
  buf[0] = 0;

  if (printnames) { 
    format_to(mbuf,"#{:>7s} {:>16s} {:>16s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s}\n",
              "N", "L[cm]",  "NAME", "X[cm]", "Px[mrad]", "Y[cm]", "Py[mrad]", "S[cm]", "dP/P[0.1%]", "dt[ns]"); 
  }
  else {
    format_to(mbuf,"#{:>7s} {:>16s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s}\n",
              "N", "L[cm]", "X[cm]", "Px[mrad]", "Y[cm]", "Py[mrad]", "S[cm]", "dP/P[0.1%]", "dt[ns]"); 
  }

  if (filenm) {
    if( toupper(OutputType)=='T'){
      fp=fopen(filenm, "w");
      fprintf(fp, "%s \n", buf);
    }
    else {
#if defined(_MSC_VER)  
      fb = _open(filenm, O_CREAT|O_TRUNC|O_BINARY|O_WRONLY, S_IREAD | S_IWRITE); // binary file ??? 
#else      
      // unix
      fb = ::open(filenm, O_CREAT|O_TRUNC|O_BINARY|O_WRONLY, S_IRUSR | S_IWUSR);
#endif
    }
  }
  else {
    editor->appendPlainText(buf);
  } 

  RMatrix me;
  Element e;
  Coordinates vexact;
  TrackParam prm;
    double EnrNew, dalfa, capa, dtx, dty, c, s, EnrNew0;
  char nm;

  int   ns, jp, jm;
  bool brk = false;
  
  int i0 = (startFrom > 0) ? startFrom-1 : 0;
  
  int nelm0 = ((stopAt>(i0+1)) && (stopAt<nelm_)) ? stopAt : nelm_;
  double bta = sqrt(1.0 - 1.0/((1.0+Ein/ms)*(1.0+Ein/ms)) );

  if(printnames) {
    format_to(mbuf, formatn, 0, 0.0, "START", v[0], v[1]*1000, v[2], v[3]*1000., v[4], v[5]*1000.0,  -v[4]/(bta*C_CGS)*1.0e9 );    
  } 
  else {
    format_to(mbuf, format,  0, 0.0, v[0], v[1]*1000, v[2], v[3]*1000., v[4], v[5]*1000.,  -v[4]/(bta*C_CGS)*1.0e9, 0, 0);
  }
  
  if(filenm) { 
    if( toupper(OutputType) =='T') { 
      fprintf(fp, "%s \n", buf);
    }  
    else {
      strcpy(bo.name,"START");
      bo.x=v[0];
      bo.y=v[2];

#if defined(_MSC_VER)
      _write(fb, &bo, 2*sizeof(double) + sizeof(bo.name) );
#else
      auto status = ::write(fb, &bo, 2*sizeof(double) + sizeof(bo.name) );
#endif    
	}
  }
 
  double tetaY = tetaYo0_;
  double Enr   = Ein;
  double Hrt   = Hr;
  
  vexact = v;

  double Lp = 0.0;
  int i = 0;
  for( auto it=beamline_.begin(); it != beamline_.end(); ++it) {

     if(brk) break;
     auto ep = (*it); //beamline_[i];

      ep->preTrack (ms, Enr,  tetaY, i, prm, me);
      EnrNew0 = Enr;

      ep->trackOnce(ms, EnrNew0, i, 0, prm, me, vexact);
      nm = ep->etype();
      ns = (stepv<=0) ? 1 : fabs(ep->length()/stepv)+1;

      if( nm=='A' || nm=='W' ||nm=='X' ||nm=='Z'|| nm=='G' ){ns=1;}
    
      std::shared_ptr<Element> e( ep->split(ns) );

      switch ( nm ){
	case 'Q':
        case 'L':
        case 'F':
          jp = (e->G>=0) ? 3 : -3; 
          jm = jp; 
          break;
	case 'B': 
        case 'D': 
        case 'R': 
          jp =  2; 
          jm = -2;	 
          break;
	case 'S':
          jp=1; jm=-1;	 
          break;
	default: 
          jp=jm=0;
      } // switch
      
      dalfa=0.;
      for(int j=0; j<ns; j++){
	 switch(nm){
	   case 'E': 
           case 'X': 
           case 'A': 
           case 'W':
             EnrNew = Enr;
	     me = e->rmatrix(EnrNew, ms, tetaY, 0.0, Element::checkEdge(j,ns));
	     v.c = me*v.c;
	     capa=sqrt(sqrt((2.*Enr*ms+Enr*Enr)/(2.*EnrNew*ms+EnrNew*EnrNew)));
	     v[0] *=  capa;
	     v[1] *=  capa;
	     v[2] *=  capa;
	     v[3] *=  capa;
             v[5] *=  capa*capa*(ms+EnrNew)/(ms+Enr);
	     Enr     =  EnrNew;
	     Hrt     =  sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;
	     break;
	   case 'Q':
	     me = e->rmatrix(Enr, ms, tetaY, 0.0, Element::checkEdge(j,ns));
             v[0] -= ep->offsX();
             v[2] -= ep->offsY();
	     v.c = me*v.c;
             v[0] += ep->offsX();
             v[2] += ep->offsY();
	     break;
	   case 'B': 
           case 'D':
	     me = e->rmatrix(dalfa, Enr, ms, tetaY, dalfa, Element::checkEdge(j,ns));
	     dalfa -= e->tilt();
	     v.c = me*v.c;
             if(ep->tiltErr()!=0.) {  //simple formulas to describe the kick due to dipole tilt
               c = -cos(ep->tilt()/180.*PI)*PI*ep->tiltErr()/180.*e->length()*e->B/Hrt;
               s =  sin(ep->tilt()/180.*PI)*PI*ep->tiltErr()/180.*e->length()*e->B/Hrt;
               v[0] += 0.5*e->length()*s;
               v[1] += s;
               v[2] += 0.5*e->length()*c;
               v[3] += c;
             }
	     break;
	   case 'S':
	     Sextupole::sext_trans(e.get(), Hrt, &v, &v);
	     break;
           case 'M':
	     Multipole::multipole_trans(e.get(), Hrt, &v, &v);
             break;
	   case 'Z':
	     v[0] += e->length() * v[1];
	     v[2] += e->length() * v[3];
	     v[5] += e->G*(Enr+ms)/(Enr*Enr+2.*Enr*ms);
	     break;
	   default:
	     me = e->rmatrix(Enr, ms, tetaY, 0.0, Element::checkEdge(j,ns));
	     v.c = me*v.c;
	 } // switch
	 
         Lp +=  fabs(e->length());

	 if(j==ns-1) for(int i=0; i<6; i++) { v[i]=vexact[i]; }
         
	 if( (i >= i0) && filterName( e->fullName(), funcFilter, MatchCase) ){

	   double bta = sqrt(1.0 - 1.0/((1.0+Enr/ms)*(1.0+Enr/ms)) );

	   if(printnames){
             format_to(mbuf, formatn, i+1, Lp, e->fullName(), v[0], v[1]*1000., v[2], v[3]*1000., v[4], v[5]*1000.0, -v[4]/(bta*C_CGS)*1.0e9);
	   }
	   else {
             format_to(mbuf, format, i+1, Lp, v[0], v[1]*1000., v[2], v[3]*1000., v[4], v[5]*1000.0, -v[4]/(bta*C_CGS)*1.0e9, jp, jm);
           }

           if( filenm && toupper(OutputType)!='T') { 
      	       strcpy(bo.name, e->fullName());
               bo.x=v[0];  bo.y=v[2];
      	       auto status = ::write(fb, &bo, 2*sizeof(double)+sizeof(bo.name) );
      	   }
	   
	 }
         if(fabs(v[0])>1000. || fabs(v[2])>1000. ||  v[5] <= -.9999999){
	   OptimMessageBox::warning(this, "Tool|Trajectory", "Beam displacement is above threshold of 10 m or Energy became negative", QMessageBox::Ok);
           brk = true;
           break;
         }
      }
       for( int i=0; i<6; ++i) {v[i]=vexact[i];}
       Enr=EnrNew0;
       ++i;
   }
     
   if(!filenm) {
     format_to(mbuf, "{:c}", '\0');
     editor->appendPlainText(mbufRaw.data());
     editor->moveCursor(QTextCursor::Start);
     editor->document()->setModified(false);
     editor->show(); 
   }
   else {
     if( toupper(OutputType)=='T') {
       fprintf(fp, "%s", mbufRaw.data());
       fclose(fp);
     }
     else { 
       ::close(fb);
     }
   }

   return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimMainWindow::cmdConsistencyCheck()
{

  /********************************************************************************
   Checks for:
      1. Elements repeated in the Element list
      2. Unused elements in the lattice
      3. Field and tilt arguments of edge focusing Elements correspond to 
         those of the dipoles they are associated with  
  **********************************************************************************/
  
  char buf[256];

  OptimTextEditor* editor = 0;
  auto DigCh = getAttachedSubWin(WindowId:: DigCh);
  if ( !DigCh) { 
    DigCh = createAttachedSubWin( (editor = new OptimTextEditor()), WindowId:: DigCh);
  }
  else { 
     editor = qobject_cast<OptimTextEditor*>(  DigCh->widget() ); 
     if (CtSt_.ClearText) editor->clear();
  }
  DigCh->raise();

  if(analyzed_) { 
    if( analyze(false)) return;
  } 
  else { 
    if(analyze(true)) return;
  }

  // Analysis of OptiM file

  editor->insertPlainText("------------------------\n");
  editor->insertPlainText("Consistency Check Report\n");
  editor->insertPlainText("------------------------\n\n");


  // this code is very inefficient ... // FIXME 
  
  for(int i=0; i<nelmlist_-1; ++i){
    for( int j=i+1; j< nelmlist_; ++j) {
      if(strcmp(elmdict_[i]->name(), elmdict_[j]->name()) == 0 ){
	editor->insertPlainText(QString::asprintf(
            	"Element name <%s> is repeated at lines %d and %d of the Element list ",
               	elmdict_[i]->name(), i, j));
        }
     }
  }

  std::vector<int> res(nelmlist_);

  for ( int i=0; i< nelmlist_; ++i) res[i]=0;

  for ( int i=0; i<nelm_; ++i){
    for( int j=0; j< nelmlist_; ++j) {
      if( strcmp(elmdict_[j]->name(), beamline_[i]->name() ) == 0 ) {
	res[j] = 1;
	break;
      }
    }
   }

  editor->insertPlainText("(1) Elements\n");
  editor->insertPlainText("------------\n");

  for(int i=0; i< nelmlist_; ++i){
     if(res[i]==0){
       editor->insertPlainText(QString::asprintf(
                               "<%s> is present in the Element list but is not used in the lattice.\n", elmdict_[i]->name()));
    }
  }

  // Checking for dipole and edge focusing consistency
   
  double  alfa_exit;

  double tetaY = tetaYo0_; // ***WARNING*** This is the lattice file initial value
  double Enr   = Ein;
   
  editor->insertPlainText("\n(2) Edge Focusing\n");
  editor->insertPlainText("-----------------\n");

  for(int i=0; i<nelm_; ++i){

    auto ep=beamline_[i];
//      Hrt=sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;

    RMatrix tm = ep->rmatrix(alfa_exit, Enr, ms, tetaY, 0.0, 3);

    if( (ep->etype() =='B') || (ep->etype() =='D')){
      if(i>0){
        if( beamline_[i-1]->etype() =='G'){
          if((fabs(beamline_[i-1]->B - beamline_[i]->B)/(fabs(beamline_[i]->B)+0.0001)>1.e-5)){
            editor->insertPlainText(QString::asprintf("Magnetic field is different for %s and %s by %f kGs\n",
                                    beamline_[i-1]->name(), beamline_[i]->name(), beamline_[i-1]->B - beamline_[i]->B));
          }
          if( fabs(beamline_[i-1]->tilt() - beamline_[i]->tilt()) > 1.e-3){
           editor->insertPlainText(QString::asprintf("Tilt angles are is different for %s and %s by %f deg\n",
                                  beamline_[i-1]->name(), beamline_[i]->name(), beamline_[i-1]->tilt() - beamline_[i]->tilt()));
          }
        }
    }
    if(i<nelm_-1){
      if( beamline_[i+1]->etype() =='G'){
        if((fabs(beamline_[i+1]->B - beamline_[i]->B)/(fabs(beamline_[i]->B)+0.0001)>1.e-5)){
          editor->insertPlainText(QString::asprintf("Magnetic field is different for %s and %s by %f kGs\n",
                                  beamline_[i+1]->name(), beamline_[i]->name(), beamline_[i+1]->B - beamline_[i]->B));
        }
        if( fabs(beamline_[i+1]->tilt() - alfa_exit) > 1.e-3){
           editor->insertPlainText(QString::asprintf("Tilt angles are is different for %s and %s by %f deg\n",
                                   beamline_[i+1]->name(), beamline_[i]->name(), beamline_[i+1]->tilt() - alfa_exit));
         }
       }
     }
   } //if 
     

   bool r;
   if( ep->etype() =='G'){
      r = false;
      if((i>0) && (i<nelm_-1)){
        r = r || ( beamline_[i+1]->etype() =='B') || (beamline_[i+1]->etype() =='D') ||
                 ( beamline_[i-1]->etype() =='B') || (beamline_[i-1]->etype() =='D');
      }
      if(i==0){
        r = r || ( beamline_[i+1]->etype() =='B') || ( beamline_[i+1]->etype() =='D');
      }
      if(i==nelm_-1){
        r = r || ( beamline_[i-1]->etype() =='B') || ( beamline_[i-1]->etype() =='D');
      }
      if(!r){
        editor->insertPlainText(QString::asprintf("Edge focusing Element %s does not have nearby dipole.\n", ep->name()));
      }
    }
 }

 editor->insertPlainText("\nConsistency check done.\n"); 
 editor->document()->setModified(false);
 editor->show();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdToolsControl()
{
   static ToolsControlDialog* dialog = 0; 
   if (!dialog) dialog = new  ToolsControlDialog(0);

   if(interrupted_ ) {interrupted_ = false; return;}

   NstTool.ConvParam  = cnvg;
   NstTool.ConvErr    = CvgErr;
   NstTool.Nmax       = Ncvg;
   
   dialog->data_ = NstTool; 
   dialog->set();

   int ret = dialog->exec(); 
   if (ret == QDialog::Rejected) return; 

   NstTool = dialog->data_; 

   cnvg    = NstTool.ConvParam;
   CvgErr  = NstTool.ConvErr;
   Ncvg    = NstTool.Nmax;



}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdToggle_mode()
{

  if(interrupted_ ) { interrupted_ = false;  return; }

  CtSt_.CompAtExcitedOrb = ! CtSt_.CompAtExcitedOrb;

  if  ( CtSt_.CompAtExcitedOrb ) {
     excitedOrbitMode(); 
  }
  else  {
     designOrbitMode();
  }
  

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::designOrbitMode() 
{
  QString OptimName = qApp->applicationName();
  char buf[256];

  
  sprintf(buf, "%s (%s", OptimName.toUtf8().data(), (CtSt_.IsRingCh ? "RING" : "LINE" ));
  if(CtSt_.CompAtExcitedOrb)strcat(buf, ", reference orbit mode)");
  else strcat(buf, ")");

  setWindowTitle(buf);

}  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimMainWindow::excitedOrbitMode()
{
  QString OptimName = qApp->applicationName();
  char buf[256];

  sprintf(buf, "%s (%s", OptimName.toUtf8().data(), (CtSt_.IsRingCh ? "RING" : "LINE" ));
  if(CtSt_.CompAtExcitedOrb)strcat(buf, ", reference orbit mode)");
  else strcat(buf, ")");

  setWindowTitle(buf);
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdToggle_line()
{
  if( interrupted_ ){ interrupted_= false; return;}

  CtSt_.IsRingCh = !CtSt_.IsRingCh;  

  if (CtSt_.IsRingCh) {
    ringMode();
  }
  else {
    lineMode();
  }  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::lineMode()
{
  char buf[256];
  QString OptimName = qApp->applicationName();
 
  CtSt_.IsRingCh = false;
  sprintf(buf, "%s (%s",  OptimName.toUtf8().data(), (CtSt_.IsRingCh ? "RING": "LINE") );

  if(CtSt_.CompAtExcitedOrb)strcat(buf, ", reference orbit mode)");
  else strcat(buf, ")");
  setWindowTitle(buf);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::ringMode()
{
  char buf[256];
  QString OptimName = qApp->applicationName();

  // if ( cmdCloseTraject() != 0 ) return;  
  // if ( cmdCloseLattice() != 0 ) return;

  CtSt_.IsRingCh = true;
  sprintf(buf, "%s (%s",  OptimName.toUtf8().data(), (CtSt_.IsRingCh ? "RING": "LINE") );

  strcat(buf, CtSt_.CompAtExcitedOrb ? ", reference orbit mode)" :  ")" ); 

  setWindowTitle(buf);
}
