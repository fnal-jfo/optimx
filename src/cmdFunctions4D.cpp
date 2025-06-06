//  =================================================================
//
//  cmdFunctions4D.cpp
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
// OptimX   (c) FRA/Fermilab 
// Please refer to OptiM.Copyright.txt for copyright and license 
// information. All other rights reserved.
//
// File: cmdFunctions4D.cpp
// ------------------------------------------------------------------

#include <iostream>
#include <complex>
#include <cmath>
#include <cstdio>
#include <exception>
#include <memory>
#include <iterator>
#include <fmt/format.h>
#include <QObject>

#include <Constants.h>
#include <Element.h>
#include <ElementSelectionDialog.h>
#include <OptimCalc.h>
#include <OptimEditor.h>
#include <OptimTextEditor.h>
#include <OptimMainWindow.h>
#include <OptimMdiSubWindow.h>
#include <OptimMdiArea.h>
#include <OptimPlot.h>
#include <OptimMessages.h>
#include <OptimApp.h>
#include <Structs.h>
#include <Twiss.h>
#include <Utility.h>
#include <RMatrix.h>

//#include <QwtLegend>
#include <qwt_legend.h>

using Constants::PI;
using Utility::filterName;
using fmt::format_to;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#define LSTR 1024


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdView4dFunctionsAtElem ()
{

  if(interrupted_){interrupted_= false; return;}
  if( analyze(!analyzed_) ) return;

  static ElementSelectionDialog* dialog = 0;
  if (!dialog) dialog = new ElementSelectionDialog(0);

  dialog->data_.elmno =  view_elem;
  dialog->set();

  if (dialog->exec() == QDialog::Rejected) return;
  
  view_elem = dialog->data_.elmno;

  if( (view_elem>=nelm_) || (view_elem<0) ) view_elem=0;

  double tetaY = tetaYo0_;
  double Enr   = Ein;

  RMatrix tm;
  tm.toUnity();

  for(int i=0; i<nelm_; ++i){
    auto ep =beamline_[i];
    RMatrix me = ep->rmatrix(Enr, ms, tetaY, 0.0, 3);
    tm  = me*tm;
    if( (i+1)==view_elem )break;
  }

  RMatrix tmf;
  tmf.toUnity();
  
  std::complex<double> lambda[4], ev[4][4];
  char err[256];
  
  Twiss4D v;
  if( CtSt_.IsRingCh ) {
    findRMatrix(tmf);
    if( RMatrix_t<4,double>(tmf).findEigenValues(lambda, ev) !=0) {
      OptimMessageBox::warning(this, "4D View", "Cannot close lattice.", QMessageBox::Ok);
      return;
    }

    v.betatronFromEV(ev);
    
   getDisp4D( tm, v, err);
   if(err[0]) {
      OptimMessageBox::warning( this, "4D View", err, QMessageBox::Ok );
      return;
   }
  }
  else if( Get4DBetasFromFile(false, false, v) !=0 ) {
     return;
  }
   
  v.qx = v.qy = 0.0;
  View4dFunctionsAtElem(tm, v);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::View4dFunctionsAtElem(RMatrix& tm, Twiss4D& v)
{

  std::complex<double> ev[4][4];
  char err[256];
  
  OptimTextEditor* editor = 0;
  auto Dig4Ch = getAttachedSubWin(WindowId::Dig4Ch); 
  if (!Dig4Ch) { 
     Dig4Ch = createAttachedSubWin( (editor = new OptimTextEditor()), WindowId::Dig4Ch );
     connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
     connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
  }
   else { 
     editor = qobject_cast<OptimTextEditor*>( Dig4Ch->widget() );
  }
  
  Dig4Ch->raise();
  
  if(CtSt_.ClearText) { 
     editor->clear();
  }
  else{ 
    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    cursor.insertBlock();
  }

  Dig4Ch->show();

  v.eigenvectors(ev);
  Element::propagateLatticeFunctions( tm, v, ev);

  // printing

  char const bel[3] = {'\015','\012','\000'};
    
  if(CtSt_.ClearText) editor->clear();
  else{ editor->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor); } // { PutCursorToEnd(editor); editor->insertPlainText(bel);}

  editor->insertPlainText(bel);
  if(view_elem) {
    editor->insertPlainText(QString::asprintf("4D functions at Element %d (%s)",view_elem, beamline_[view_elem-1]->fullName()));
  }
  else {
    editor->insertPlainText("4D functions at lattice downstream end");
  }  

  editor->insertPlainText(bel);
  editor->insertPlainText(QString::asprintf("  Beta_X_1[cm]=%lg \tBeta_X_2[cm]=%lg",v.btx1, v.btx2));
   
  editor->insertPlainText(bel);
  editor->insertPlainText(QString::asprintf("  Alpha_X_1=%lg  \tAlpha_X_2=%lg",v.alfx1, v.alfx2));
 
  editor->insertPlainText(bel);
  editor->insertPlainText(QString::asprintf("  Beta_Y_1[cm]=%lg \tBeta_Y_2[cm]=%lg",v.bty1, v.bty2));
  
  editor->insertPlainText(bel);
  editor->insertPlainText(QString::asprintf("  Alpha_Y_1=%lg  \tAlpha_Y_2=%lg",v.alfy1, v.alfy2));
         
  editor->insertPlainText(bel);
  editor->insertPlainText(QString::asprintf("  Nu_1[deg]=%lg \tNu_2[deg]=%lg \tU=%lg",
                                            v.teta1*180./PI, v.teta2*180./PI, v.u));
  editor->insertPlainText(bel);
  editor->insertPlainText("Dispersions:");
 
  editor->insertPlainText(bel);
  editor->insertPlainText(QString::asprintf("  DispX[cm]=%lg \tDispY[cm]=%lg\n",v.dx, v.dy));

  editor->insertPlainText(QString::asprintf("  DispPrX=%lg  \tDispPrY=%lg\n",  v.dxp, v.dyp));
  
  editor->insertPlainText("Fractional part of tunes:\n");
  
  editor->insertPlainText(QString::asprintf("  Q1=%lg \tQ2=%lg\n",v.qx, v.qy));
  
  double alfa = 0; 
  if(!view_elem) {

    double gamma = 1.0 + Ein/ms;
    double gm2   = gamma*gamma;
    
    alfa = - (  v.dx * tm[4][0] + v.dxp *  tm[4][1] +
                v.dy * tm[4][2] + v.dyp *  tm[4][3] + ( tm[4][5]- Length_ /gm2 ) ) / Length_;

    editor->insertPlainText(QString::asprintf("  Momentum compaction=%lg",alfa));
    editor->insertPlainText(bel);
   }
  if(CtSt_.CompAtExcitedOrb){
    if(!view_elem){
      editor->insertPlainText(QString::asprintf("Path lengthening for excited orbit[cm]=%g ",DeltaL_));
              editor->insertPlainText(bel);
      }
    editor->insertPlainText(QString::asprintf("Initial trajectory parameters for %s", (CtSt_.IsRingCh ? "RING":"LINE")));
    editor->insertPlainText(bel);
    
    editor->insertPlainText(QString::asprintf("\tx=%f cm   teta_x=%f mrad",v_anlz.c[0], v_anlz.c[1]*1000.));
    editor->insertPlainText(bel);
   
    editor->insertPlainText(QString::asprintf("\ty=%f cm   teta_y=%f mrad",v_anlz.c[2], v_anlz.c[3]*1000.));
    editor->insertPlainText(bel);
    
    editor->insertPlainText(QString::asprintf("\ts=%f cm   Dp/p*1000=%f",v_anlz.c[4], v_anlz.c[5]*1000.));
    editor->insertPlainText(bel);
   }
   
   editor->document()->setModified(false);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdView4Dfunctions()
{
  FunctionDialog* dialog = 0;
  if (!dialog) dialog = new FunctionDialog(0);
 
  FunctionDlgStruct& NStf = dialog->data_;  

  strcpy(NStf.Filter, funcFilter);
  NStf.Step          = stepv;
  NStf.Start         = startFrom;
  NStf.Stop          = stopAt;
  NStf.printname     = printnames;
  NStf.NoFirstLetter = NoFirstLetter;
  NStf.MatchCase     = MatchCase;
  NStf.PrintEnerg    = PrintEnerg;
  NStf.StartSfrom0   = StartSfrom0;
  
  dialog->set();

  if (dialog->exec() == QDialog::Rejected)  return;

  ViewFunctions4D(0, &NStf);
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int OptimMainWindow::ViewFunctions4D (char* filenm, FunctionDlgStruct* NStf)
{


  if(interrupted_ ) { interrupted_ = false; return 1; }
  if( analyze(!analyzed_) ) return 1;

  char err[1024];

  // ................................................
  
  double dalfa = 0.0;
  double s     = 0.0;
  std::complex<double> lambda[4];
  std::complex<double> ev[4][4];

  Twiss4D v;
  RMatrix tm;

  if( CtSt_.IsRingCh ) {
    findRMatrix(tm);
    if(RMatrix_t<4,double>(tm).findEigenValues(lambda, ev)!=0){
      OptimMessageBox::warning(this, "4D View", "Cannot close lattice.", QMessageBox::Ok);
      return 1;
    }

    v.betatronFromEV(ev);
    
    getDisp4D(tm, v, err);
    if(err[0]){ OptimMessageBox::warning(this, "4D View", err, QMessageBox::Ok); return 1;}
  }
  else{
    if(Get4DBetasFromFile(false, false, v)) return 1;
    v.eigenvectors(ev);
  }

  OptimTextEditor* editor = 0;
  auto Dig4Ch = getAttachedSubWin(WindowId::Dig4Ch);
  if (!Dig4Ch) { 
     Dig4Ch = createAttachedSubWin((editor = new OptimTextEditor()), WindowId::Dig4Ch);
     connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
     connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
  }
  else { 
     editor = qobject_cast<OptimTextEditor*>( Dig4Ch->widget() );
     if (!editor) return 1;  
   }
  
 Dig4Ch->raise();

 if(CtSt_.ClearText) { 
    editor->clear();
  }
  else{ 
    QTextCursor cursor = editor->textCursor(); 
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    cursor.insertBlock();
  }

  Dig4Ch->show();

  strcpy(funcFilter, NStf->Filter);
  stepv          = NStf->Step;
  startFrom      = NStf->Start;
  stopAt         = NStf->Stop;
  printnames     = NStf->printname;
  NoFirstLetter  = NStf->NoFirstLetter;
  MatchCase      = NStf->MatchCase;
  PrintEnerg     = NStf->PrintEnerg;
  StartSfrom0    = NStf->StartSfrom0;

  //--------------------------------------------
  //   for libfmt < 8   
  //   use format_to(outbuf)
  //   for libfmt>= 8
  //   use format_to(std::back_inserter(outbuf)
  // ----------------------------
  
  auto outbufRaw = fmt::memory_buffer();

#if FMT_VERSION < 80000
  auto& outbuf    = outbufRaw;
#else
  auto outbuf    = std::back_inserter(outbufRaw);
#endif
  
  FILE* fp = 0;

   if(filenm) {
     if((fp=fopen(filenm,"w"))==0){
        OptimMessageBox::warning(this,  "Error opening file for saving. File name -", filenm, QMessageBox::Ok);
        return 1;
     }
   }
  
   if (!editor) return 1;
   int lastline =  editor->document()->lineCount();

  
    if (printnames) {
      format_to(outbuf,"#{:>11s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s}",
                "N",   "NAME",  "S[cm]",  "BetaX1[cm]",  "AlphaX1",  "BetaY1[cm]",  "AlphaY1",  "Nu1/(2*PI)",
                                          "BetaX2[cm]",  "AlphaX2",  "BetaY2[cm]",  "AlphaY2",  "Nu2/(2*PI)",
                "U",       "DspX[cm]",  "DspXp",  "DspY[cm]",  "DspYp",  "Q1",  "Q2", "M56[cm]");
    }
    else {
      format_to(outbuf,"#{:>11s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s} {:>12s}",
                "N",  "S[cm]",  "BetaX1{cm]",  "AlphaX1",  "BetaY1[cm]",  "AlphaY1",  "Nu1/(2*PI)",
                "BetaX2[cm]",  "AlphaX2",  "BetaY2[cm]",  "AlphaY2",  "Nu2/(2*PI)",
                "U",       "DspX[cm]",  "DspXp",  "DspY[cm]",  "DspYp",  "Q1",  "Q2", "M56[cm]");
   } 

   if(PrintEnerg) {
      format_to(outbuf, " {:>12s}\n", "Energy");
   }
   else {
      format_to(outbuf, "\n");
   }      

  double Lp = (StartSfrom0) ? 0.0 : so0_;


  char const formatn[] = "{:12d} {:>12s} {:12.2f} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12.5f} {:12.5f} {:12g}";
  char const format[]  = "{:12d} {:12.1f} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12g} {:12.5f} {:12.5f} {:12g} {:12g}";
  
  int i0    = (startFrom > 0.0) ? startFrom-1 : 0;
  int nelm0 = ((stopAt>(i0+1)) && (stopAt<nelm_)) ? stopAt : nelm_;

  v.qx = v.qy = 0.0 ;

  if(printnames ) {
    format_to(outbuf,formatn, 0, "START", Lp,
             v.btx1, v.alfx1, v.bty1, v.alfy1, v.teta1/(2.*PI),
             v.btx2, v.alfx2, v.bty2, v.alfy2, v.teta2/(2.*PI),
	      v.u, v.dx, v.dxp, v.dy, v.dyp, v.qx, v.qy, 0.0);
  }	
  else {
    format_to(outbuf, format, 0, Lp,
              v.btx1, v.alfx1, v.bty1, v.alfy1, v.teta1/(2.*PI),
              v.btx2, v.alfx2, v.bty2, v.alfy2, v.teta2/(2.*PI),
              v.u, v.dx, v.dxp, v.dy, v.dyp, v.qx, v.qy, 0.0);
  }

  if(PrintEnerg) {
    format_to(outbuf, " {:12g}\n", Ein);
  }
  else {
    format_to(outbuf, "\n");
  }
  
  
  auto cursor_guard = OptimApp::cursorWait();

  double tetaY = tetaYo0_;
  double Enr   = Ein;
  RMatrix tmf;
  tmf.toUnity();
 
  int jp = 0;
  int jm = 0;


  for (int i=0; i<nelm0; ++i) {
     auto ep = beamline_[i];
       char const* pname = (NoFirstLetter) ? &ep->fullName()[1] : ep->fullName();

       char nm    = ep->etype();
       int  ns    = (stepv <=0 ) ? 1 : fabs(ep->length()/stepv)+1;

       if( nm=='A' || nm=='W' ||nm=='X') {ns = 1;}

       auto e = std::shared_ptr<Element>( ep->split(ns));

       double dalpha = 0.0;
       
       for(int j=0; j<ns; j++){
         switch(nm ) {
	   case 'B':  
           case 'D':
             tm = e->rmatrix(dalfa, Enr, ms, tetaY, dalfa, Element::checkEdge(j,ns));
	     dalfa -= e->tilt();
	     break;
	   default:
             tm = e->rmatrix(Enr, ms, tetaY, 0.0, Element::checkEdge(j,ns));
	 }

	 e->propagateLatticeFunctions( tm, v, ev);
         tmf = tm*tmf; 
	 
         Lp +=  e->length();

	 if( (i>=i0) && filterName( e->fullName(), funcFilter, MatchCase) ){
	    if(printnames) {
              format_to(outbuf,formatn, i+1, pname, Lp, v.btx1, v.alfx1, v.bty1, v.alfy1, v.teta1/(2.*PI),
       		    v.btx2,v.alfx2,v.bty2,v.alfy2,v.teta2/(2.*PI),v.u,v.dx,v.dxp,v.dy,v.dyp,v.qx,v.qy, tmf[4][5] );
	    }
	    else { 
              format_to(outbuf,format,i+1,Lp,v.btx1,v.alfx1,v.bty1,v.alfy1,v.teta1/(2.*PI),
       		    v.btx2,v.alfx2,v.bty2,v.alfy2,v.teta2/(2.*PI),v.u,v.dx,v.dxp,v.dy,v.dyp,v.qx,v.qy, tmf[4][5] );
	    }
	    if(PrintEnerg) {
              format_to(outbuf," {:12g}\n", Enr);
            }
            else { 
              format_to(outbuf,"\n");
            }
	} 
      } //for j=0; j<ns 
   } // for i =0; i<elm_

    format_to(outbuf, "{:c}", '\0');

    if(filenm) {
      fprintf(fp, "%s", outbufRaw.data());
      fclose(fp);
      OptimInformationMessage( this, "File has been saved.", filenm, QMessageBox::Ok);
    }
    else {
      editor->appendPlainText(outbufRaw.data());
      editor->moveCursor(QTextCursor::Start);
      editor->document()->setModified(false);
    }
    
    return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||













