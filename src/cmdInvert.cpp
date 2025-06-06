//  =================================================================
//
//  cmdInvert.cpp
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
#include <Element.h>
#include <Globals.h>
#include <RMatrix.h>
#include <Twiss.h>
#include <OptimApp.h>
#include <OptimEditor.h>
#include <OptimMainWindow.h>
#include <OptimMessages.h>
#include <Utility.h>
#include <QTextBlock>
#include <QMdiArea>
#include <QMdiSubWindow>

#include <iostream>
#include <algorithm>
#include <iomanip>

static const unsigned int LSTR = 1024;

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


void OptimMainWindow::cmdEditInvert ()
{

  if(interrupted_ ) {interrupted_ = false; return;}

  if(analyze(!analyzed_,1 )) return; 

  if (!LatticeCh_) return;
  OptimEditor* editor = qobject_cast<OptimEditor*>(LatticeCh_->widget() ); 
  if (!editor) return;

  if( OptimInformationMessage( this, "Inverse Lattice",
     "Lattice inversion requires an inversion of dipole and cavity fields. This operation may destroy the connection with the math-header"
			       "for these elements. Continue ?", (QMessageBox::Yes | QMessageBox::No)) == QMessageBox::No ) return;
			       
  Element elm;
  int i, j, nline=0;
  char buf[LSTR+1], buf1[LSTR+1], buf2[NAME_LENGTH];
  
  std::complex<double> ev[4][4];
  
  // Getting tilt angles for dipoles and exit coordinates

  // count bends ... 
  int nb = 0; // no of bends
  for( int i = nb =0; i<nelm_; ++i){
    char nm = beamline_[i]->etype();
    if(nm=='B'||nm=='D') ++nb;
  }

  std::vector<double> tilt_out(nb);
  std::vector<int>    ref(nb);
  
  InverseOrbit(&tilt_out[0], &ref[0]);
  
  // Get exit Twiss parameters
  
  RMatrix tm;
  double Enew = findRMatrix(tm);

  Twiss v;
  v.BtX  = BetaXin;
  v.BtY  = BetaYin;
  v.AlX  = AlfaXin;
  v.AlY  = AlfaYin;
  v.DsX  = DispXin;
  v.DsY  = DispYin;
  v.DsXp = DispPrimeXin;
  v.DsYp = DispPrimeYin;
  v.nuY  = v.nuX =0.0;

  v.eigenvectors(ev);
  v.btfTransform(tm,ev);
 
  QTextCursor cursor;

  do {
     nline=getLineCalc(editor, buf, LSTR, nline);
  } while(!strcmpr("OptiM",buf));
  
  nline = getLineCmt(editor, buf, LSTR, nline);
  sprintf(buf,"Energy[MeV]=%-8.8lg   Mass[MeV]=%-8.8lg ",Enew,ms);
  replaceLine(editor, nline-1, buf);

  nline = getLineCmt(editor, buf, LSTR, nline);
  nline = getLineCmt(editor, buf, LSTR, nline);
  sprintf(buf,"Initial:\tBetaX[cm]=%g \tBetaY[cm]=%g ",v.BtX,v.BtY);
  replaceLine(editor, nline-1, buf);

  nline = getLineCmt(editor, buf, LSTR, nline);
  sprintf(buf,"\tAlphaX=%g \tAlphaY=%g ",-v.AlX,-v.AlY);
  replaceLine(editor, nline-1, buf);

  nline = getLineCmt(editor, buf, LSTR, nline);
  sprintf(buf,"\tDispersX[cm]=%g \tDispersY[cm]=%g ", -v.DsX,   v.DsY);  // note reversal of final value of DsX 
  replaceLine(editor, nline-1, buf);

  nline = getLineCmt(editor, buf, LSTR, nline);
  sprintf(buf,"\tDsp_PrimeX=%g \tDspPrimeY=%g ",       v.DsXp, -v.DsYp); // note reversal of final value of DsYp
  replaceLine(editor, nline-1, buf);

  nline = getLineCmt(editor, buf, LSTR, nline);
  sprintf(buf,"\tX[cm]=%-8.3lf \tY[cm]=%-8.3lf \tZ[cm]=%-8.3lf \tS[cm]=%-8.3lf ",xo_, yo_, zo_, 0.0);
  replaceLine(editor, nline-1, buf);

  nline=getLineCmt(editor, buf, LSTR, nline);
  tetaXo_ += (tetaXo_ >0.0 ) ? -180.0 : 180.0;

  sprintf(buf,"\ttetaX[deg]=%-8.8lg \ttetaY[deg]=%-8.8lg ",tetaXo_, -tetaYo_);
  replaceLine(editor, nline-1, buf);

  while(true){
    nline=getLineCmt(editor, buf, LSTR, nline);
    if(nline<0) return;
    if(strcmpr("begin lattice",buf)) break;
   }
  
   cursor = editor->textCursor();
   cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
   cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, nline);
   
   do { 
     strcpy(buf, cursor.block().text().toUtf8().data() ); nline++;  // editor->GetLine(buf, LSTR, nline++); 
     cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
   } while(!strcmpr("end lattice",buf));

   cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::KeepAnchor);
   cursor.removeSelectedText();

   for (i=j=0; i<nelm_; i++) {
     auto e =  beamline_[nelm_-i-1]; 
     copyname(buf, e->name(), e->fullName());
     cursor.insertText(buf);
     if((j++ == 6) && (i!=nelm_-1)){j=0; cursor.insertText("\n");}
   }
   cursor.insertText("\n");
   nline = cursor.blockNumber()+ 1;

   while(true){     // First line for list of Elements in TEdit.
     nline=getLineCmt(editor, buf, LSTR, nline);
     if(strcmpr("begin list",buf))break;
     if(nline<=0) return; // goto EdInvEnd;
   }

   double dat[7];
   do{
     nline=getLineCmt(editor, buf, LSTR, nline);
     if(nline<0) break; // goto EdInvEnd;
     if(strcmpr("end list",buf))break;
     char* bufpt = buf; 
     //getElmName(ElemNameFCh, &bufpt,buf1,buf2);
     getElmNameX(bufpt,buf1,buf2);
     elm.name(buf1);
     decodeLine(buf,dat,6);
 
     char ename = elm.etype();

     switch (ename) {
       
     case 'A':  // pillbox cavity 
       ReplaceLineForInv(editor, nline-1, 3, -dat[3], -1, dat[0]); 
       break;
     case 'W': // general pi-mode cavity 
       ReplaceLineForInv(editor, nline-1, 1, -dat[1], -1, dat[1]);
       break;
     case 'R': // wien filter
       ReplaceLineForInv(editor, nline-1, 2, -dat[2],  3, -dat[3]);
       break;
     case 'B':
     case 'D': // CF bends
       for(i=0; i<nb; i++){
          if(!strcmp(beamline_[ref[i]]->name(),elm.name())) break;
       }
       if (i<nb) {
            ReplaceLineForInv(editor, nline-1, 1, -dat[1], 3, -tilt_out[i]);
       }
       else {
         sprintf(buf, "Element <%s> is defined but not used.", elm.name());
	 OptimMessageBox::warning(this, "Inverse Lattice Warning", buf, QMessageBox::Ok);
       }
       break;
     case 'G': // edge
       ReplaceLineForInv(editor, nline-1, 0, -dat[0], 3, -dat[3]);
       break;
     case 'K': // transverse corrector
       ReplaceLineForInv(editor, nline-1, 2, -dat[2], -1, -dat[2]);  //does not updates B
       break;
     case 'Q': // quad
       ReplaceLineForInv(editor, nline-1, 2, -dat[2], -1, -dat[2]);
       break;
     case 'C': // solenoid
       ReplaceLineForInv(editor, nline-1, 1, -dat[1], -1, -dat[2]);
       break;
     default:
       break;
     }

   } while (nline>0);

   OptimInformationMessage(this, "Inverse Lattice", 
   "Remember that corrector fields were not reversed. Should this be required, it will have to be done manually.",
			    QMessageBox::Ok);

   EdInvEnd: nline=0;

   return; 
}

