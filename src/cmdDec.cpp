//  =================================================================
//
//  cmdDec.cpp
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
#include <GeneralPreferencesDialog.h>
#include <Element.h>
#include <Globals.h>
#include <RMatrix.h>
#include <Twiss.h>
#include <OptimApp.h>
#include <OptimEditor.h>
#include <OptimTextEditor.h>
#include <OptimMainWindow.h>
#include <OptimMessages.h>
#include <OptimPlot.h>
#include <Utility.h>
#include <StepsDialog.h>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QRegularExpression>
#include <QTextBlock>

#include <iostream>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <functional>

static const unsigned int LSTR = 1024;


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
using Utility::correctNames;
using Utility::getVariableName;
using Utility::strippedName;
using Utility::isValidType;

// ..........................................................................................

void OptimMainWindow::cmdIncrement() 
{
  DecInc(1);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdDecrement() 
{
  DecInc(-1);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::DecInc(int ddt)
{
  
  char const* cher[3]={"X","Y","X&Y"};
  char *b;
  int k, irt;

  int nd = 0;
  unsigned int ir = 0;
  unsigned int jr = 0;
  char et = 0;
  
  static QRegularExpression rx("[^\\s]+\\s*=\\s*[0-9,\\.,Ee-]+");  
      
  if( interrupted_ ) { interrupted_ = false; return; }

  if(!LatticeCh_) return;

  OptimEditor*   editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());     
  if (!editor ) return;
   
  if (!analyzed_) return;

  QTextCursor cursor       = editor->textCursor();
  unsigned int const pos0  = cursor.position();

  int nline         = cursor.blockNumber();
  unsigned int pos  = cursor.position();

  char  buf[LSTR+1];
  char* bufpt = &buf[0];

  // math header increment case ......
  
  if( (nline >=0) && (nline < lineOptiM) ) {

    if(analyze(false,1)) return;
    strcpy(buf, cursor.block().text().toUtf8().data() );   
    
    if(buf[0]=='#') return;
    b = buf;

    while ((*b != 0) && (*b != '=')) ++b;
    if(*b == 0) return;
    ++b;

    bufpt = strchr(buf, '>');
    bufpt ?  (++bufpt) : (bufpt = b); 
    double V = atof(bufpt)+ddt*dV_;
    sprintf(b,"%-12.9lg",V);

    replaceLine(editor, nline, buf);
    cursor = editor->textCursor();
    // preserve the selection 
    cursor.select(QTextCursor::LineUnderCursor);
    editor->setTextCursor(cursor); 
    
    goto dicont;
  }

  // Element increment case

  if( (nline>LineLFin) || (nline<LineLIn)) return; // LineLFin is the final line for the  Element list
  
  if(analyze(false)) return;

  strcpy(buf, cursor.block().text().toUtf8().data() );  // GetLine(buf, LSTR, nline);

  // make sure that this is not a comment
  bufpt = strchr(buf, '#');
  if (bufpt ) *bufpt = 0;
   
  cursor.movePosition(QTextCursor::StartOfBlock);
  pos  = cursor.position();

  {

    jr             = pos;
    unsigned int p = pos;
    do { 
         cursor = editor->document()->find(rx, p);
	 if ( cursor.isNull() ) {nd = 0; break; }
         ++nd; 
	 if ( (jr = cursor.selectionEnd()) >= pos0 )  break;
	 ir = cursor.selectionStart(); 
         jr = cursor.selectionEnd();
	 p = jr;
	 
      } while ( (pos0 > ir ) );
      
  }

  editor->setTextCursor(cursor); 

  if ( nd==0 )   return;

  char buf1[LSTR+1];
  char buf2[NAME_LENGTH]; 

  bufpt = buf;
  //if( !getElmName(ElemNameFCh, &bufpt, buf1, buf2)   ) return;
  if ( !getElmNameX(bufpt, buf1, buf2) ) return ;
 
  for(k=0; k<nelmlist_; k++) { 
    if(!strcmp(buf1,elmdict_[k]->name())) break; 
  }

  if(strcmp(buf1,elmdict_[k]->name())) return;

  et = elmdict_[k]->etype(); // element type
  switch (nd) { // nd: element attribute index 
    case 1:	
      elmdict_[k]->length( elmdict_[k]->length() + ddt*dL_ ); 
      break;
    case 2:	
      switch (et) {
	case 'D': 
        case 'B': 
        case 'C': 
        case 'R': 
        case 'K':
	  elmdict_[k]->B += ddt*dB_; 
          break;
        case 'Q':
        case 'L': 
        case 'F': 
          elmdict_[k]->G += ddt*dG_; 
          break;
        case 'S':
          elmdict_[k]->S += ddt*dS_; 
          break;
	case 'W':
          elmdict_[k]->G += ddt*dA_; 
          break;
	default: 
          return;
      } // switch
      break;
    case 3:	
      switch (et) {
	case 'D': 
        case 'B': 
          elmdict_[k]->G += ddt*dG_; 
          break;
	case 'W': 
          elmdict_[k]->S += ddt*dPhase_; 
          break;
      default: return;
      }
      break;
    case 4:	
      switch (et) {
	case 'A': elmdict_[k]->G += ddt*dA_; 
          break;
        case 'Q':
          elmdict_[k]->offsX(elmdict_[k]->offsX() + ddt*dofsX_); 
          break;
      default: 
          return;
      }
      break;
    case 5:	
      switch (et) {
	case 'A': 
          elmdict_[k]->S += ddt*dPhase_; 
          break;
        case 'Q':
          elmdict_[k]->offsY( elmdict_[k]->offsY()+ ddt*dofsY_); 
         break;
        default: return;
      }
      break;
    }

    print_elm(elmdict_[k].get(), buf);

    cursor = editor->textCursor();
    cursor.select(QTextCursor::BlockUnderCursor); // LineUnderCursor does not work correctly when text is wraping around.
                                                  // This looks like a bug.
    editor->setTextCursor(cursor);
    cursor.removeSelectedText(); 
    cursor.insertText("\n");                      // We need to insert a new \n here. For some reason, the QCursor Block 
                                                  // selection includes the \n that preceeds the text rather than the one 
                                                  // that follows it. Is this a bug ?    
    {
      QTextCharFormat cfmt;
      cfmt.setForeground(QBrush("red"));
      cursor.insertText(buf, cfmt);                       
    }
    // At this point the old line has been replaced.   
    // We need to make sure the selection persists.  
    // To do so, we just look for nd th arg 
    // and select it. Using regex is simpler 
    // than trying to find the cursor positions
    // at the beginning and end of an attribute.

    {
      for (int i=0; i<nd; ++i ) { 
         cursor = editor->document()->find(rx, pos); 
         pos    = cursor.position();
      }
    }

    editor->setTextCursor(cursor); 
      
dicont:
     bool status = analyze(false);
     if (status) return;    
 
    // Analyze exit conditions
   
    std::complex<double> lambda[4], ev[4][4], ev1[4][4];
    Coordinates vc;

    BunchParam bunch;
    double alfa;
    char err[256];

    RMatrix tm;
    double Enr2 = findRMatrix(tm);
    
    auto DigCh     = getAttachedSubWin(WindowId::DigCh);
    auto BetaCh    = getAttachedSubWin(WindowId::BetaCh);
    auto SizeCh    = getAttachedSubWin(WindowId::SizeCh);
    auto PhaseCh   = getAttachedSubWin(WindowId::PhaseCh);
    auto TracCh    = getAttachedSubWin(WindowId::TracCh);
  
    auto Dig4Ch    = getAttachedSubWin(WindowId::Dig4Ch);
    auto Beta4Ch   = getAttachedSubWin(WindowId::Beta4Ch);
    auto Size4Ch   = getAttachedSubWin(WindowId::Size4Ch);
    auto Phase4Ch  = getAttachedSubWin(WindowId::Phase4Ch);
    auto Disp4Ch   = getAttachedSubWin(WindowId::Disp4Ch);
    auto Proj4Ch   = getAttachedSubWin(WindowId::Proj4Ch);

    auto BetaSpCh  = getAttachedSubWin(WindowId::BetaSpCh); 
    auto PhaseSpCh = getAttachedSubWin(WindowId::PhaseSpCh);
    auto SizeSpCh  = getAttachedSubWin(WindowId::SizeSpCh);
    auto ProjSpCh  = getAttachedSubWin(WindowId::ProjSpCh);

    Twiss v,v1;
    if( CtSt_.IsRingCh ){ // Ring case

      // ... 2D

      if(DigCh||BetaCh||SizeCh||PhaseCh||TracCh) {

	
	irt = find_tunes(tm, Length_, v1, &alfa); // on return v1 contains periodic lattice functions. 

	if(irt) { 
          strcpy(buf,"Cannot close for ");
	  strcat(buf, cher[irt-1]);
	  OptimMessageBox::warning(this, "Close Error", buf, QMessageBox::Ok);
	  return;
        }
	
	if(CtSt_.AutoBeta && BetaCh) { v = v1; betas(v);  }
	if(CtSt_.AutoBeta && SizeCh) { v = v1; Sizes(v);  }
	if(CtSt_.AutoBeta && PhaseCh){ v = v1; Phases(v); }
	if(CtSt_.AutoBeta && TracCh) {	
          if(getTrajParamFromFile(false, false, vc)) return;
	  closeTraject(vc); // always use linear closure as first guess

	  if(NstTool.FullClosure) {
            if (!NstTool.Closure4D) getTrajParamFromFile(false, false, vc); // 6D: all initial parameters from file 
            if (closeTrajectFull(vc))
	        OptimMessageBox::warning(this,"Close Error", 
      				 "Cannot close trajectory. Make sure that periodic Twiss parameters can be determined.\n"
					 "If so, try reducing the relaxation factor and/or increasing the convergence threshold.",
				 QMessageBox::Ok);
	  }
	  v = v1;
	  TrajectoryNew(vc, v);
        }
      }

      // ... 4D

      Twiss4D v4, v4_2;

      if(Dig4Ch||Beta4Ch||Size4Ch||Phase4Ch||Disp4Ch||Proj4Ch ){
	if((ir=RMatrix_t<4,double>(tm).findEigenValues(lambda, ev))!=0){
	   OptimMessageBox::warning(this, "4D View", "Cannot close lattice.", QMessageBox::Ok);
           return;
         }
    	 else {

	   v4.betatronFromEV(ev);
	   
	   getDisp4D(tm, v4, err);
    	   if(err[0]){ OptimMessageBox::warning( this, "4D View", err, QMessageBox::Ok); return; }
      	   if(Dig4Ch && CtSt_.AutoLattice){
	     OptimTextEditor* editor = qobject_cast<OptimTextEditor*>( Dig4Ch->widget() ); 
             TypeEigenVectorsToOutW(editor, tm, lambda, ev, v4, ir);
           }
	   if(CtSt_.AutoBeta && Beta4Ch ) { Betas4DNew(v4);      }
	   if(CtSt_.AutoBeta && Size4Ch ) { Sizes4D(v4);      }
	   if(CtSt_.AutoBeta && Proj4Ch ) { Proj4D(v4);       }
	   if(CtSt_.AutoBeta && Phase4Ch) { Phases4D(v4);     }
	   if(CtSt_.AutoBeta && Disp4Ch ) { Dispersion4DNew(v4); }
         }                                                     
       }
    }  
    else { // Line case 
 
      Twiss4D v4, v4_2;
      double gamma = 1.0+Ein/ms;   
      double gm2   = gamma*gamma;
      std::complex<double> ev[4][4];
      
      setInitialBetas(v);

      alfa = - ( v.DsX * tm[4][0] + v.DsXp * tm[4][1] +
	         v.DsY * tm[4][2] + v.DsYp * tm[4][3] + (tm[4][5] - Length_/gm2) ) / Length_;
      
      if(CtSt_.AutoBeta && BetaCh) {
	betas(v);
      }
      else {
	v.eigenvectors(ev1);
	v.btfTransform(tm, ev1);
      }

      if(CtSt_.AutoBeta && SizeCh) { setInitialBetas(v1); Sizes(v1);}
      if(CtSt_.AutoBeta && PhaseCh){ setInitialBetas(v1); Phases (v1);}
      if(CtSt_.AutoBeta && TracCh) {
	getTrajParamFromFile(false, false, vc);
	setInitialBetas(v1);
	TrajectoryNew (vc, v1);
      }
      v1 = v;
 
     // ... 4D
      if(Dig4Ch||Beta4Ch||Size4Ch||Phase4Ch||Disp4Ch||SizeSpCh||BetaSpCh||PhaseSpCh||Proj4Ch|| ProjSpCh ) {
        Get4DBetasFromFile(false, false, v4);

	if(Dig4Ch && CtSt_.AutoLattice) { v4_2 = v4;  View4dFunctionsAtElem(tm, v4_2);}

        if(CtSt_.AutoBeta && Beta4Ch ) { v4_2 = v4; Betas4DNew(v4_2);     } 
        if(CtSt_.AutoBeta && Size4Ch ) { v4_2 = v4; Sizes4D(v4_2);     }
        if(CtSt_.AutoBeta && Proj4Ch ) { v4_2 = v4; Proj4D(v4_2);      }
        if(CtSt_.AutoBeta && Phase4Ch) { v4_2 = v4; Phases4D(v4_2);    }
        if(CtSt_.AutoBeta && Disp4Ch ) { v4_2 = v4; Dispersion4DNew(v4_2);}
        if(CtSt_.AutoBeta && BetaSpCh) { 
          v4_2 = v4;
          GetSpaceChargeParam(false, bunch); 
          // Spcharge_betas(v4_2, bunch);
          spChargeBetas(v4_2, bunch);
        }
        if(CtSt_.AutoBeta && PhaseSpCh){
          v4_2 = v4;
          GetSpaceChargeParam(false, bunch); 
          //Spcharge_phases(v4_2, bunch );
          spChargePhases(v4_2, bunch );
        }
        if( CtSt_.AutoBeta && SizeSpCh){ 
          v4_2= v4;
          GetSpaceChargeParam(false, bunch);
	  //SpaceChargeSizes(v4_2, bunch);
	  spChargeSizes(v4_2, bunch);
        }
        if( CtSt_.AutoBeta && ProjSpCh){ 
          v4_2= v4;
          GetSpaceChargeParam(false, bunch);
          //SpaceChargeProj(v4_2, bunch);
          spChargeProj(v4_2, bunch);
        }
      }
    } // LINE  


    if(DigCh && CtSt_.AutoLattice){
      OptimTextEditor*  editor = qobject_cast<OptimTextEditor*>(DigCh->widget());
      typeBetas(editor, v1, alfa, Enr2, 0);
    }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdSteps ()
{
 
  static StepsDialog* dialog = 0;

  if (!dialog) {
    dialog = new StepsDialog(0);
  
    dialog->data_.dL     = dL_;
    dialog->data_.dOfsx  = dofsX_;
    dialog->data_.dOfsy  = dofsY_;
    dialog->data_.dB     = dB_;
    dialog->data_.dG     = dG_;  
    dialog->data_.dS     = dS_;
    dialog->data_.dV     = dV_;
    dialog->data_.dA     = dA_;
    dialog->data_.dPhase = dPhase_;
    dialog->set();
  }

  QDialog::DialogCode result = static_cast<QDialog::DialogCode>(dialog->exec());   
  if (result == QDialog::Rejected) return;

  
  dL_     =  dialog->data_.dL;
  dofsX_  =  dialog->data_.dOfsx;
  dofsY_  =  dialog->data_.dOfsy;
  dB_     =  dialog->data_.dB;
  dG_     =  dialog->data_.dG;
  dS_     =  dialog->data_.dS;
  dV_     =  dialog->data_.dV;
  dA_     =  dialog->data_.dA;
  dPhase_ =  dialog->data_.dPhase;
  
}

