//  =================================================================
//
//  OptimFits.cpp
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
#include <iomanip>
#include <algorithm>

#include <OptimMainWindow.h>
#include <OptimMdiSubWindow.h>
#include <OptimEditor.h>
#include <OptimTextEditor.h>
#include <OptimMessages.h>
#include <Channel.h>
#include <Element.h>
#include <Fit.h>
#include <RMatrix.h>
#include <Utility.h>
#include <Twiss.h>
#include <OptimCalc.h>
#include <QMdiArea>
#include <QAction>
#include <QMdiSubWindow>
#include <QCoreApplication>
#include <QRegularExpression>
#include <memory>

using Utility::decodeLine;
using Utility::strcmpr;
using Utility::strcmpre;
using Utility::getElmName;
using Utility::getElmNameX;


unsigned int const LSTR = 1024;

char const  grname[] = "LBG";
char const* FitNames = "ODBQCL";

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::ostream& operator<<( std::ostream& os, FitElem const& o)
{
       os << "group----------------------------------\n";
       os << "no of elements     =  "  << o.n     << "\n";
       os << "parameter type     =  "  << o.param << "\n";
       os << "group member element indices: \n";
       for (int k=0; k < o.n; ++k) {
         os << "el[" << k << "] = " << o.el[k] << "\n";
       };
       os << "step = " <<  o.step  << "\n";
       os << "min  = " <<  o.min   << "\n";
       os << "max  = " <<  o.max   << "\n";
       os << "group----------------------------------\n";

       return os;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


std::ostream& operator<<( std::ostream& os, FitStep const& o)
{
       os << "step----------------------------------\n";
       os << "dL    = " << o.dL     << "\n"; 
       os << "dB    = " << o.dB     << "\n";
       os << "dG    = " << o.dG     << "\n";
       os << "btx   = " << o.btx    << "\n";
       os << "dbty  = " << o.dbtx   << "\n";
       os << "bty   = " << o.bty    << "\n";
       os << "dbty  = " << o.dbty   << "\n";
       os << "alfa  = " << o.alfa   << "\n";
       os << "dalfa = " << o.dalfa  << "\n";
       os << "step----------------------------------\n";

       return os;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
FitStep::FitStep()
{}

FitStep::FitStep( FitStep const& o)
:
  dL(o.dL), dB(o.dB), dG(o.dG), 
  btx(o.btx), dbtx(o.dbtx), 
  bty(o.bty), dbty(o.dbty), 
  alfa(o.alfa), dalfa(o.dalfa)
{}

FitStep& FitStep::operator=( FitStep rhs)
{
  std::swap(dL,    rhs.dL);
  std::swap(dB,    rhs.dB);
  std::swap(dG,    rhs.dG); 
  std::swap(btx,   rhs.btx);
  std::swap(dbtx,  rhs.dbtx); 
  std::swap(bty,   rhs.bty);
  std::swap(dbty,  rhs.dbty); 
  std::swap(alfa,  rhs.alfa);
  std::swap(dalfa, rhs.dalfa);

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::GetFitBeta(int& nline, Twiss& vfin, Twiss& dv, OptimEditor* editor) // V7
{
  double dat[4];
  char buf[LSTR];

  // decoding of the output values for fitting

  if( (nline = getLineCmt(editor, buf, LSTR, nline)) == -1 )return 1;

  decodeLine(buf,dat,4);

  vfin.BtX = dat[0]; dv.BtX = dat[1];
  vfin.AlX = dat[2]; dv.AlX = dat[3];

  if( (nline = getLineCmt(editor, buf, LSTR, nline)) == -1 )return 1;

  decodeLine(buf,dat,4);

  vfin.BtY = dat[0]; dv.BtY = dat[1];
  vfin.AlY = dat[2]; dv.AlY = dat[3];

  if( (nline=getLineCmt(editor, buf, LSTR, nline)) == -1 )return 1;

  decodeLine(buf,dat,4);

  vfin.DsX  = dat[0];  dv.DsX  = dat[1];
  vfin.DsXp = dat[2]; dv.DsXp = dat[3];

  if( (nline=getLineCmt(editor, buf, LSTR, nline)) == -1 )return 1;

  decodeLine(buf,dat,4);

  vfin.DsY  = dat[0];  dv.DsY  = dat[1];
  vfin.DsYp = dat[2];  dv.DsYp = dat[3];

  if( (nline=getLineCmt(editor, buf, LSTR, nline)) == -1 )return 1;

  decodeLine(buf,dat,2);

  vfin.nuX=dat[0]; dv.nuX=dat[1];
  
  if( (nline=getLineCmt(editor, buf, LSTR, nline)) == -1 )return 1;

  decodeLine(buf,dat,2);

  vfin.nuY = dat[0];  dv.nuY = dat[1];

  // detect constraint on phase advance

  phase_advance_constraint_  =  phase_advance_constraint_  ? phase_advance_constraint_ : ( dv.nuX > 0.0 || dv.nuY > 0.0 );
  
  return 0;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::GetFitParam (FitStep* fstep, Twiss vfin[],
				  Twiss dv[], int npoint[], FitElem group[], int& ngr, bool FirstPass) // V7
{
   // Initiation of a Fitting

   std::generate( &npoint[0], &npoint[FitElem::NPOINT+1], [](){return 0.0;});    

   char const* FitNames="ODBQCL";
   char const* bufa[]={ "BetaFitBlock dL[cm]=0.01 dB[kG]=0.01 dG[kG/cm]=0.001",
		       "#Requested parameters and corresponding accuracy are listed below (dPARM<=0. - no fitting)",
                       "#Maximum Betas[cm] and Momentum Compaction (Alfa) are on the next line",
		       "BtXmax=5000.  dBtXmax=0.  BtYmax=5000.  dBtYmax=0.  Alfa=0.  dAlfa=0.",
		       "#Fitting parameters at the end of the lattice",
		       "Beta_X[cm]=100.   dBeta_X[cm]=0.   Alpha_X=0.       dAlpha_X=0.",
		       "Beta_Y[cm]=100.   dBeta_Y[cm]=0.   Alpha_Y=0.       dAlpha_Y=0.",
		       "Disp_X[cm]=0.     dDisp_X[cm]=0.   D_prime_X=0.     dD_prime_X=0.",
		       "Disp_Y[cm]=0.     dDisp_Y[cm]=0.   D_prime_Y=0.     dD_prime_Y=0.",
		       "Qx=0.             dQx=0.",
		       "Qy=0.             dQy=0.",
		       "#Fit at Element with number =2 ",
		       "#To create a constraint at an intermediate element: uncomment the line above, ",
		       "# write the correct Element number and insert six lines describing the",
		       "# fit parameters. You can use up to 4 intermediate points",
		       "#Each point has to be specified as described above",
		       "#",
		       "#Insert groups of Elements below. Each group has to be located on one line.",
		       "#Start from the letter describing the type of changeable parameter such as: L:, B:, G: ",
		       "G: q1 q2: min=-1e10   max=1e10",
		       "EndBetaFitBlock"};


   int buflen = sizeof(bufa)/sizeof(char*);

   double dat[6];
   char buf[LSTR+1];
  
   char *bufpt = buf;
   int  nmblines, i;

   if (!LatticeCh_) return 1; 
   OptimEditor*   editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());     
   if (!editor ) return 1;

   nmblines = editor->document()->lineCount();

   int nline = 0; 
   for(nline=0; nline<nmblines; ++nline){
     strcpy(buf, editor->getLine(nline).toUtf8().data());
     if(strcmpr("BetaFitBlock",buf)) break;
   }
   
   if(nline == nmblines){
     if(OptimQuestionMessage(this, "Fit", "There is no fitting block. Do you want to create it?",
			     QMessageBox::Yes|QMessageBox::No ) == QMessageBox::No ) return 1;
     QTextCursor cursor = editor->textCursor();
     cursor.movePosition( QTextCursor::Start);
     cursor.movePosition( QTextCursor::QTextCursor::Down, 
                          QTextCursor::MoveAnchor, nline-1);
     // cursor.select(QTextCursor::LineUnderCursor); // This does not seem to work when lines wrap around. 

     cursor.select(QTextCursor::BlockUnderCursor);   // These 2 lines replace
     editor->insertPlainText("\n");                  // the line above. 
     cursor.removeSelectedText();	

     editor->setTextCursor(cursor);
     editor->insertPlainText(buf);
     for(int i=0; i<buflen; ++i){
        editor->insertPlainText("\n");	
        editor->insertPlainText(bufa[i]);
     }
        editor->insertPlainText("\n");
        editor->document()->setModified(false);
	return 1;
     }
     // decoding of the initial steps
     decodeLine(buf,dat,3);
     if( (dat[0] <=0.0) || (dat[1]<=0.0) || (dat[2] <= 0.0) ){
       OptimMessageBox::warning(this, "Fit", "Initial step cannot be zero or negative", QMessageBox::Ok);
       return 1;
     }
     fstep->dL = dat[0];  
     fstep->dB = dat[1]; 
     fstep->dG = dat[2];
     ++nline;
     
     // decoding of the maximums of betas  and required momentum compaction with its accuracies

     if( (nline=getLineCmt(editor, buf, LSTR, nline)) == -1 ) return 1;

     decodeLine(buf,dat,6);
     fstep->btx   = dat[0];
     fstep->dbtx  = dat[1];
     fstep->bty   = dat[2];
     fstep->dbty  = dat[3];
     fstep->alfa  = dat[4];
     fstep->dalfa = dat[5];
     
     // decoding of the betas and dispersion on exit
     if(GetFitBeta(nline, vfin[0], dv[0], editor)) return 1;
     npoint[0] = 1;
     
     // decoding of the betas and dispersion at additional points
     Twiss*  vp   =  vfin;	 
     Twiss* dvp   =  dv;
  
     static const QRegularExpression rx(R"((?i)\s*Fit\s+at\s+Element(?-i)\s*.+?=.+)"); 
  
       while(npoint[0] < FitElem::NPOINT) {
       // here we call getLineCalc (get a line for the calculator) instead of getLineCmt
       // this prevents the RHS of the equal sign to be "evaluated" to a double
       if( (nline = getLineCalc(editor, buf, LSTR, nline)) == -1 ) return 1;
       //if( (nline = getLineCmt(editor, buf, LSTR, nline)) == -1 ) return 1;

       QRegularExpressionMatch rm = rx.match(buf);
       if ( !rm.hasMatch() )  break;
       // if( !strcmpr("Fit at Element",buf) ) break;

       int i = npoint[0]++;
       decodeLine(buf,dat,1); // returns an integer 
       
       if(FirstPass){
         npoint[i] = dat[0];
	 if(npoint[i]>=nelm_){
	   OptimMessageBox::warning( this, 
				 "Fit","Element index for additional fitting exceeds no of elements in the line.",
                  	         QMessageBox::Ok);
	   return 1;
         }
         if( (i>2) && (npoint[i]<npoint[i-1])) {
	   OptimMessageBox::warning(this, "Fit", 
			       "Element indices for additional locations must be specified increasing order.",
			QMessageBox::Ok);
	   return 1;
         }
       }
       if(GetFitBeta(nline, *(++vp), *(++dvp), editor)) return 1;
     }
     // decoding of groups
 
     FitElem* groupt = group;
     nline--;

     char* buf1p = 0;
     char* buf2p = 0;
 
     std::unique_ptr<char[]> buf2(buf2p = new char[NAME_LENGTH]);
     std::unique_ptr<char[]> buf1(buf1p = new char[NAME_LENGTH]);
     
     
     for (i=0; i<FitElem::NGROUP; ++i) {

       if( (nline = getLineCmt(editor, buf, LSTR, nline)) == -1 ) return 1;

       if(strcmpr(bufa[buflen-1],buf)) break;    //exit at the EndBetaFitBlock

       int j = 0;
       for (j =0; j<(int)strlen(grname); ++j) { if(buf[0]==grname[j]) { groupt->param=j; break; } }

       if( j == (int) strlen(grname)) {
	 OptimMessageBox::warning(this, "Fit", "Requested Parameter does not exist or syntax error.",QMessageBox::Ok);
	 replaceLine(editor, nline-1, buf);
	 return 1;
        }
	if(buf[1]!=':'){
	  OptimMessageBox::warning(this, "Fit", "Error; <:> missing separator after parameter name", QMessageBox::Ok);
	  replaceLine(editor, nline-1, buf);
	  return 1; 
        }

	switch(groupt->param){ // assign initial steps for each group of Elements
      	  case 0:
      	    groupt->step = fstep->dL;
            break;
      	  case 1:
      	    groupt->step = fstep->dB;
            break;
      	  case 2:
      	    groupt->step = fstep->dG;
            break;
        }

	bufpt = buf+2;
	groupt->n=0;

	// kludge to prevent a memory leak. The real issue here the way  getElmName is (poorly) implemented.
        //..................................................................................................
	
        while( getElmName(FitNames,&bufpt,buf1p, buf2p) == 0 ){
            
	   if(groupt->n++ == FitElem::NGROUP){
	     replaceLine(editor, nline-1, buf);
	     OptimMessageBox::warning(this, "Too many elements in the group", buf, QMessageBox::Ok);
	     return 1;
           }
	   if (strlen(buf1.get()) > NAME_LENGTH ) { // buf1[NAME_LENGTH-1]!=0 ) { 
             replaceLine(editor, nline-1, buf);
             sprintf(buf,"Element %s cannot be used  (max allowed length exceeded).",buf1p);
	     OptimMessageBox::warning(this,"Conversion Error", buf, QMessageBox::Ok);
	     return 1;
           }

	   for(j=0; j<nelmlist_; ++j) {
	     if( strcmpre(elmdict_[j]->name(), buf1p) )break;
	   }
	     
	   if(j==nelmlist_) {
             replaceLine(editor, nline-1, buf);
	     sprintf(buf,"No such Element: %s",buf1p);
	     OptimMessageBox::warning(this, "Conversion Error", buf,  QMessageBox::Ok);
	     return 1;
           }
	   groupt->el[groupt->n-1] = j;
        }
	
        
        if(groupt->n == 0){
      	  replaceLine(editor, nline-1, buf);
          OptimMessageBox::warning( this, "The group contains no element usable for fitting", buf, QMessageBox::Ok);
          return 1;
        }

        // set min and max for the group

	bufpt = buf+2;

	while( (*bufpt != '\000') && (*bufpt != '\n') && (*bufpt !=';') ) {
	   ++bufpt;
        }
	
	if(*bufpt==';'){
      	  bufpt++;

          decodeLine(bufpt,dat,2);
          groupt->min = dat[0];
	  groupt->max = dat[1];

      	  if(groupt->min >= groupt->max) {
      	    replaceLine(editor, nline-1, buf);
	    static std::string msg = std::string("The minimum is larger than maximum for this group.\n") + buf;
	    OptimMessageBox::warning(this, "Error", msg.c_str(), QMessageBox::Ok);
	    return 1;
          }
        }
        else {
          groupt->min = -1.e10;   
          groupt->max =  1.e10;
        }
        groupt++;
     }
     
     
     if(i==0) { 
       OptimMessageBox::warning(this, "Fit", "No elements assigned for fitting", QMessageBox::Ok);
       return 1;
     }

     ngr = i; // number of groups  

     //................................................................................
#if 0
     // DBG: output group info 
     
     for (int i = 0; i < ngr; ++i) {  
       std::cout << "group index i    = " << i      << std::endl;
       std::cout <<  group[i];
     }
#endif     
     //................................................................................
     return 0;

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimMainWindow::PrintBetaParam (OptimTextEditor* editor, Twiss& v, Twiss& dv)
{
  char buf[257];
  bool first = true; 
  
  if (dv.BtX > 0.0){
    editor->insertPlainText(QString::asprintf("Beta_X[cm]=%g",      v.BtX));  
    editor->insertPlainText(QString::asprintf("\tdBeta_X[cm]=%g\n",   dv.BtX));
  }

  if (dv.AlX > 0.0){
    editor->insertPlainText(QString::asprintf("Alpha_X=%g",      v.AlX));
    editor->insertPlainText(QString::asprintf("\tdAlpha_X=%g\n", dv.AlX));
  }
  
  if (dv.BtY > 0.0){
    if (!first)  editor->insertPlainText("\t");
    editor->insertPlainText(QString::asprintf("Beta_Y[cm]=%g", v.BtY));
    editor->insertPlainText(QString::asprintf("\tdBeta_Y[cm]=%g\n", dv.BtY));
  }

  if (dv.AlX > 0.0){
    if (!first)  editor->insertPlainText("\t");
    editor->insertPlainText(QString::asprintf("Alpha_Y=%g",      v.AlY));
    editor->insertPlainText(QString::asprintf("\tdAlpha_Y=%g\n",     dv.AlY));
  }
 
 if (dv.DsX > 0.0){
   editor->insertPlainText(QString::asprintf("Dsp_X[cm]=%g",    v.DsX));    
   editor->insertPlainText(QString::asprintf("\tdDsp_X[cm]=%g\n",    dv.DsX));
 }
 
 if (dv.DsXp >0.0){
   editor->insertPlainText(QString::asprintf("Dsp_Xp=%g",    v.DsXp));
   editor->insertPlainText(QString::asprintf("\tdDsp_Xp=%g\n",    dv.DsXp));
 }

 if (dv.DsY > 0.0){
   editor->insertPlainText(QString::asprintf("Dsp_Y[cm]=%g", v.DsY));
   editor->insertPlainText(QString::asprintf("\tdDsp_Y[cm]=%g\n", dv.DsY));
 }

 if (dv.DsYp >0.0){
   editor->insertPlainText(QString::asprintf("Dsp_Yp=%g",   v.DsYp)); 
   editor->insertPlainText(QString::asprintf("\tdDsp_Yp=%g\n",  dv.DsYp));
 }

 if (dv.nuX > 0.){
   editor->insertPlainText(QString::asprintf("Q_X=%g \tdQ_X=%g  ",v.nuX, dv.nuX));
  }
  if (dv.nuY > 0.){
    editor->insertPlainText(QString::asprintf("Q_Y=%g\tdQ_Y=%g",v.nuY, dv.nuY));
  }
  if ((dv.nuX > 0.) || (dv.nuY > 0.))   editor->insertPlainText("\n");

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::PrintFitParam (OptimTextEditor* editor, FitStep* fstep, Twiss vfin[],
				     Twiss dv[], int npoint[], FitElem group[], int& ngr)
{
  FitElem* groupt = group;
  editor->insertPlainText(QString::asprintf("Mode = %s\n", CtSt_.IsRingCh ? "RING":  "LINE"));
  if ( phase_advance_constraint_ ) { 
    editor->insertPlainText(QString::asprintf("Phase advance = %s\n", (CtSt_.use_fractional_tune ? "Constrained (fractional part only)" : "Constrained (int + fractional)")));  
  }
  else {
    editor->insertPlainText(QString::asprintf("Phase advance = %s\n", "Unconstrained\n"));
  }
  editor->insertPlainText(QString::asprintf("Initial step sizes: dL[cm]=%g  dB[kGs]=%g  dG[kGs/cm]=%g\n", fstep->dL, fstep->dB, fstep->dG));
  editor->insertPlainText(QString::asprintf("\nFit to the following parameters:\n"));

  if ( (fstep->dbtx > 0.0) || (fstep->dbty > 0.0)) {
    editor->insertPlainText(QString::asprintf("Maximum Betas[cm]: Beta_X=%g \tdBeta_X=%g \tBeta_Y=%g \tdBeta_Y= %g\n", fstep->btx, fstep->dbtx, fstep->bty, fstep->dbty));
  }
  if(fstep->dalfa!=0.){
    editor->insertPlainText(QString::asprintf("Momentum compaction: Alfa=%g    dAlfa=%g\n", fstep->alfa, fstep->dalfa));
  }
  editor->insertPlainText("-----------------------------------------------------------\n");
  editor->insertPlainText("Exit parameters :\n");
  PrintBetaParam (editor, vfin[0], dv[0]);
  editor->insertPlainText("-----------------------------------------------------------\n");
  
  if(npoint[0]>1){
      editor->insertPlainText("-----------------------------------------------------------\n");
      editor->insertPlainText("Other Constrained Locations:\n");
      for (int i=1; i<npoint[0]; ++i){ 
	editor->insertPlainText(QString::asprintf("Downstream end of Element %d (%s)\n", npoint[i], beamline_[npoint[i]-1]->name()));
	PrintBetaParam (editor, vfin[i], dv[i]);
      }
      editor->insertPlainText("-----------------------------------------------------------\n");
  }
  int i = 0;
  while(i++ < ngr){
    editor->insertPlainText(QString::asprintf("Group %d: Fitting param. - %c; min=%g   max=%g; Element(s): ",
                                              i, grname[groupt->param], groupt->min, groupt->max));

    for (int j=0; j<groupt->n; ++j){
       int k = groupt->el[j];
       editor->insertPlainText(QString::asprintf("%s ",elmdict_[k]->name()));
    }
    editor->insertPlainText("\n");
    editor->insertPlainText(QString::asprintf("\n  min=%g   max=%g\n", groupt->min, groupt->max));
    groupt++;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  OptimMainWindow::addErr(double& err, Twiss const& v, Twiss const& vfin, Twiss const& dv)
{

  using std::abs;
  
  double s =0.0;

  // if a precision parameter is negative, the weight associated with
  // the variable is 0 i.e. it is ignored in the optimization 

  if ( dv.BtX  > 0.0)  { s = (vfin.BtX - v.BtX) / dv.BtX;  err += s*s;}
  if ( dv.BtY  > 0.0)  { s = (vfin.BtY - v.BtY) / dv.BtY;  err += s*s;}
  if ( dv.AlX  > 0.0)  { s = (vfin.AlX - v.AlX) / dv.AlX;  err += s*s;}
  if ( dv.AlY  > 0.0)  { s = (vfin.AlY - v.AlY) / dv.AlY;  err += s*s;}
  if ( dv.DsX  > 0.0)  { s = (vfin.DsX - v.DsX) / dv.DsX;  err += s*s;}
  if ( dv.DsY  > 0.0)  { s = (vfin.DsY - v.DsY) / dv.DsY;  err += s*s;}
  if ( dv.DsXp > 0.0)  { s = (vfin.DsXp -v.DsXp)/ dv.DsXp; err += s*s;}
  if ( dv.DsYp > 0.0)  { s = (vfin.DsYp -v.DsYp)/ dv.DsYp; err += s*s;}

  if ( dv.nuX  > 0.0)  {

      double intpart = 0.0;
      double nuXfin = vfin.nuX;
      double nuX    = v.nuX;
       
      s = ( CtSt_.use_fractional_tune ?  modf((nuXfin - nuX), &intpart) : (nuXfin - nuX)) / dv.nuX;
      err += s*s;
 
  }
  if ( dv.nuY  > 0.0)  {

      double intpart = 0.0;
      double nuYfin = vfin.nuY;
      double nuY    = v.nuY;

      s = ( CtSt_.use_fractional_tune ?  modf((nuYfin - nuY), &intpart) : (nuYfin - nuY)) / dv.nuY;
      err += s*s;
   
  }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  OptimMainWindow::FindError(  Twiss vfin[], Twiss dv[], int npoint[], FitStep* fstep) // V7
{

  Twiss v;
  RMatrix me; // single element matrix
  RMatrix tm;
  int irt;
  std::complex<double> ev[4][4];
  char buf[256];
  char const *cher[3] = { "X" , "Y" , "X&Y"};

  double tetaY, Enr, BetaXm=0., BetaYm=0., err=0., s, alfa;
  int i, j;
  
   if(  !CtSt_.IsRingCh ) {
     v.BtX  = BetaXin;
     v.BtY  = BetaYin;
     v.AlX  = AlfaXin;
     v.AlY  = AlfaYin;
     v.DsX  = DispXin;
     v.DsY  = DispYin;
     v.DsXp = DispPrimeXin;
     v.DsYp = DispPrimeYin;
     v.nuX  = 0.0;
     v.nuY  = 0.0;
   }
   else {
     findRMatrix(tm);
     irt=find_tunes(tm, Length_, v, &alfa);
     if(irt) {
       strcpy(buf,"Cannot close for ");
       strcat(buf, cher[irt-1]);
       OptimMessageBox::warning(this, "Close Error",buf, QMessageBox::Ok);
       return -1.;
     }
   }
   v.nuY = v.nuX=0.;
   tetaY = tetaYo0_; 
   Enr   = Ein;

   if( !CtSt_.IsRingCh ) tm.toUnity();

   v.eigenvectors(ev);

   for (i=0, j=1; i<nelm_; ++i){

     auto ep = beamline_[i];
     me = ep->rmatrix(Enr, ms, tetaY, 0.0, 3);
     
     if( !CtSt_.IsRingCh ) tm = me*tm;

     ep->propagateLatticeFunctions(me, v, ev); //  lattice functions downstream of element [matrix me]
     
     if (v.BtX > BetaXm) BetaXm = v.BtX;
     if (v.BtY > BetaYm) BetaYm = v.BtY;

     // contribution to objective function from error on lattice functions at intermediate locations, if any ...
 
     if (npoint[0]>1) {
          if(npoint[j]==(i+1)){
	   addErr(err, v, vfin[j], dv[j]); 
         ++j;
       }
     }
   } 

   // contribution to objective function from error on lattice functions at line downstream end 
   
   addErr(err, v, vfin[0], dv[0]);
   
   // contribution to objective function from exceeding maximum beta value 

   if(fstep->dbtx > 0.0 ) {
     s = (BetaXm - fstep->btx) / fstep->dbtx;
     if(s > 0.0) err += s*s;
   }

   if(fstep->dbty > 0.0){
     s = (BetaYm - fstep->bty) / fstep->dbty;
     if(s > 0.)err += s*s;
   }

   // contribution to objective function from error on momentum compaction

   double gamma = 1.0 + Ein/ms; 

   if(fstep->dalfa > 0.0){
     s     = -( DispXin * tm[4][0] + DispPrimeXin * tm[4][1] +
	        DispYin * tm[4][2] + DispPrimeYin * tm[4][3] +   (tm[4][5] - Length_/(gamma*gamma)) ) / Length_;
     s     = (s - fstep->alfa) / fstep->dalfa;
     err += s*s;
   }

   return err;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  OptimMainWindow::GetGradient(Twiss vfin[], Twiss dv[], int npoint[],
	                           FitElem group[], int ngr, FitStep* fstep, double* G) // V7
{
  bool er= false;

  double Q0 = FindError(vfin, dv, npoint, fstep); 

  if (Q0 < 0.0) return true;

  for(int i=0; i < ngr; ++i){

    ChangeGroupSetting(&group[i],  FitElem::STEP_MULT*group[i].step);

    double Qp = FindError(vfin, dv, npoint, fstep);   if(Qp < 0.0) er = true;

    ChangeGroupSetting(&group[i], -FitElem::STEP_MULT*group[i].step); 

    G[i] =  (Qp-Q0)/(FitElem::STEP_MULT*group[i].step);
  }
  return er;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool OptimMainWindow::SetGradientStep(Twiss vfin[], Twiss dv[], int npoint[],
				      FitElem group[], int ngr, FitStep* fstep) // V7
{
  bool er = false;
  char buf[256];

  double Q0 = FindError(vfin, dv, npoint, fstep);	

  if( Q0 < 0.0) { return true;}  //return -1 if cannot close; Message box

  for( int i=0; i < ngr; ++i){
     ChangeGroupSetting(&group[i],  group[i].step);
     double Qp = FindError(vfin, dv, npoint, fstep);    // Q(x+dx)
     if(Qp<0.0) er = true;

     ChangeGroupSetting(&group[i], -2.*group[i].step);  // Q(x-dx)
       double Qm = FindError(vfin, dv, npoint, fstep);
     if (Qm<0.0) er = true;

     ChangeGroupSetting(&group[i], group[i].step);      // Q(x)

     double sq = Qp+Qm-2.*Q0;                           // Q(x+dx)+ Q(x-dx) - 2 Q(x) =  (Q(x+dx)-Q(x))  - (Q(x) - Q(x-dx))  second order difference  2 dx2 * d2Q/dx2   

     if(group[i].step < FitElem::STEP_MIN*fstep->dG){
      	sprintf(buf,
          "Gradient step for group %d became less than InitStep*%g. Try reducing InitStep. Fitting stopped.",
                                          i+1, FitElem::STEP_MIN);
	OptimMessageBox::warning(this, "Fit", buf, QMessageBox::Ok);
      	er =true;
        break;
     }

     double s = fabs(0.5*(Qm-Qp)/sq);  //   0.5 Q(x+dx)-Q(x-dx)     Q'/Q''   

     if (s > 2.0) { group[i].step *= 2.0;  continue;}
     if (s < 0.5) { group[i].step *= 0.5;  continue;}
   }
   return er;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double OptimMainWindow::ChangeGroupSetting(FitElem group[], double delta_d) // V7
{
   // If there is is a single element in the group, change parameter (G,B, or L) by delta_d; 
   // If there are more than 1 element in the group, all other elements are changed *proportionally.
  //  that if the first element parameter is 0, this will FAIL. 

  std::shared_ptr<Channel> channel;

   int k1 = group->el[0];
   // first Element in the group
   switch (grname[group->param]) {
      case 'G': 
        channel = elmdict_[k1]->getChannel("gradient"); 
	break;
      case 'L': 
        channel = elmdict_[k1]->getChannel("length"); 
        break;
      case 'B':  
        channel = elmdict_[k1]->getChannel("bfield"); 
       break;
      default :
	OptimMessageBox::warning(this, "Fit", "Invalid name in element group.", QMessageBox::Ok); 
        return 0.0;
    }

   double value_old = channel->value();

   *channel += delta_d;    // NOTE: this changes the element parameter value !!!  

   // .......................................................................................................
   // for OTHER Elements in the group, change the parameter proportionally to the change in the first element   
   // .......................................................................................................

   int k;
   if(group->n > 1){

     double s = 1.0 + delta_d/value_old; // ORIGINAL ALGO: this fails when d_old = 0.0

     // double s = 1.0 + (fabs(d_old) > fabs(delta_d)) ? (delta_d/d_old) : delta_d; 

     switch (grname[group->param]) {
       case 'G':
         for (int j=1; j<group->n; ++j){ k = group->el[j];  auto channel = elmdict_[k]->getChannel("gradient"); (*channel) *= s; } // WHAT HAPPENS WHEN elmdict_[k]->G  = 0.0 ???
	 break;
       case 'L':
         for (int j=1; j<group->n; ++j){ k = group->el[j];  auto channel = elmdict_[k]->getChannel("length");   (*channel) *= s; }
         break;
       case 'B':
         for (int j=1; j<group->n; ++j){ k = group->el[j];  auto channel = elmdict_[k]->getChannel("bfield");   (*channel) *= s; }
         break;
     }
   }
  return value_old;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::DoStep(FitElem group[], int ngr, double* dG, double a) // V7
{
   int er = 0;
   for (int i=0; i<ngr; i++) { ChangeGroupSetting(&group[i], a*dG[i]); }
   for (int i=0; i<ngr; i++){

     if(grname[group[i].param]=='L'){
       int k = group[i].el[0];
       if(elmdict_[k]->length() <=0.0) {er=1;} // element length has become negative. This is is considered as an error. Should it ?   
     }
   }
   if(er){
     for (int i=0; i<ngr; i++) { ChangeGroupSetting(&group[i], -a*dG[i]); }
     OptimMessageBox::warning(this, "Fit", "An element length became negative. Fitting stopped. ", QMessageBox::Ok);
     return er;
   }
   return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::PrintGroupElement(OptimTextEditor* editor, FitElem group[], int ngr)
{

  char buf[257];
  
  for(int i=0; i < ngr; ++i) {
    sprintf(buf,"Group %d: Fitting param. - %c; ", i+1, grname[group[i].param]);

    for (int j=0; j<group[i].n; ++j){
      int k = group[i].el[j];
      switch (grname[group[i].param]) {
	case 'G':
	  editor->insertPlainText(QString::asprintf("\t %s \tG[kG/cm]=%g  (Old)=%g\n",
                                                    elmdict_[k]->name(), elmdict_[k]->G, ElmListFit_[k]->G));
	break;
      case 'L':
	editor->insertPlainText(QString::asprintf("\t %s \tL[cm]=%g  (Old)=%g\n",
                                                  elmdict_[k]->name(), elmdict_[k]->length(), ElmListFit_[k]->length()));
	break;
     case 'B':
	editor->insertPlainText(QString::asprintf("\t %s \tB[kG]=%g  (Old)=%g\n",
                                                  elmdict_[k]->name(), elmdict_[k]->B, ElmListFit_[k]->B));
	break;
    }

  }
 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::RewriteElementList( std::vector<std::shared_ptr<Element> >&  ElmL) 
{
  int    ngr;
  int    npoint[FitElem::NPOINT];
  int   npoint2[FitElem::NPOINT];
  bool     res;
  char      buf[LSTR];
  char  el_name[LSTR];
  char     buf2[NAME_LENGTH];
  FitElem group[FitElem::NGROUP];
  FitStep fstep;
  Twiss vfin[FitElem::NPOINT], dv[FitElem::NPOINT];

  if (!LatticeCh_) return; 

  OptimEditor*  editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());     
  if (!editor ) return;

  if(analyze( false,1)) return;

  if( GetFitParam(&fstep, vfin, dv, npoint, group, ngr, true)) {
     emit fitDone();
     return;//fitting parameters before lattice compression
  }
  //..............................................................
  // Use the full uncompressed lattice (no concatenation) only if
  // the user specified that he wants the integer part of the tune
  // AND a constraint on the phase advance has been specified.
  //...............................................................

  if (  !CtSt_.use_fractional_tune && phase_advance_constraint_)  {
    analyzeWithoutCompress(group, ngr, npoint);
  }
  else {
    analyzeCompress(group, ngr, npoint);
  }
  
  if((NElmListFit != nelmlist_ )  || ( ElmListFit_.empty() )) {
    OptimMessageBox::warning( this, "Fit", "Cannot undo fitting", QMessageBox::Ok);
    emit fitDone();
    return;
  }
  
  if(GetFitParam (&fstep, vfin, dv, npoint2, group, ngr, false)) {
     emit fitDone();
     return; //fitting parameters after lattice compression
  }

  int nline = LineLIn-1;  //	 LineLIn   Number of First line for list of Elements in TEdit.

  do { 
    nline = getLineCmt(editor, buf, LSTR, nline); 
  } while(!strcmpr("begin list",buf));
  
  while(true) {
    nline = getLineCmt(editor, buf, LSTR, nline);
    if(nline>LineLFin+1) break;      // added 1 here.  The convention for line no should be looked into. Start at 0 ? 

    char* bufpt = &buf[0];

    getElmName(ElemNameFCh, &bufpt, el_name, buf2);

    strcpy( buf, bufpt); 
    res   = false;
    int i = 0;
    for(int k=0; k < ngr; ++k){
       for (int j=0; j<group[k].n; ++j){
         i = group[k].el[j];
         if(strcmp(el_name, ElmL[i]->name()) == 0 ){
           res=true;
           break;
         }
       }
       if(res)break;
    }

    if(res){
      print_elm( ElmL[i].get(), buf);
      replaceLine(editor, nline-1, buf);
    }

    if( toupper(el_name[0])=='X'){
      for(int j=0; j<6; ++j) nline = getLineCmt(editor, buf, LSTR, nline);
    }	
  } // while
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::RewriteElementListFit( std::vector<std::shared_ptr<Element> >& ElmL, FitElem group[], int ngr) // V7
{
  char buf[LSTR];
  char el_name[LSTR];
  char buf2[NAME_LENGTH];


  if (!LatticeCh_ ) return;
  OptimEditor*   editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());     
  if (!editor ) return;
   
  int nline = LineLIn-1;  //	 LineLIn   Number of First line for list of Elements in TEdit.
  do nline = getLineCmt(editor, buf, LSTR, nline); 
  while(!strcmpr("begin list",buf));
  
  while(true){
    nline = getLineCmt(editor, buf, LSTR, nline);

    if(nline > LineLFin+1) break;  // added +1 here. Line nos start at 0 perhaps this should be changed. 

    char* bufpt = &buf[0];

    getElmName(ElemNameFCh, &bufpt,el_name,buf2);

    strcpy(buf,bufpt);
    bool res = false; 
    int i    = 0;

    for(int k=0; k < ngr; ++k){
      for (int j=0; j<group[k].n; ++j){
        i = group[k].el[j];
        if(strcmp(el_name, ElmL[i]->name()) ==0){
          res = true;
          break;
        }
      }
      if(res) break;
    }

    if(res){
      print_elm( ElmL[i].get(), buf);
      replaceLine(editor, nline-1, buf);
    }

    if( toupper(el_name[0])=='X')
    for(int j=0; j<6; ++j) nline = getLineCmt(editor, buf, LSTR, nline);
    
  }
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::saveElements()
{

  // save elements that will be varied
  // this is used for undo
  
  ElmListFit_.resize(0);
  
  for (int i=0; i<nelmlist_; ++i) {
    ElmListFit_.push_back( std::shared_ptr<Element>(elmdict_[i]->clone()) );       // use for fit 
  }

  NElmListFit = nelmlist_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimMainWindow::cmdFitBetas () //V7
{

  if(interrupted_) { interrupted_ = false; return; }

  if(analyze(!analyzed_, 1)) return;

  emit fitInProgress();
  
  phase_advance_constraint_ = false; // reset this. It will be set to true if an enabled phase advance constraint is read.
 
  RMatrix tm;
  Twiss v;
  Twiss   vfin[FitElem::NPOINT];
  Twiss     dv[FitElem::NPOINT];
  int           npoint[FitElem::NPOINT];  // npoint[0] - total number of points for fitting;
                                          // then, numbers of Elements where an additional fitting is required.
  int          npoint2[FitElem::NPOINT];  // npoint2 is used as a dummy array after lattice compression
  FitElem        group[FitElem::NGROUP];
  double             G[FitElem::NGROUP];  // gradients ?

  double  s;
  double  sq;
  double  Qold;
  double  step_dG;

  int ngr;
  int i;
  int iter;
  int er=0;

  FitStep fstep;

  char buf[257];
    
  if ( GetFitParam (&fstep, vfin, dv, npoint, group, ngr, true)) {
    emit fitDone();
    return;
  }

  OptimTextEditor* editor = 0;
  auto DigCh = getAttachedSubWin(WindowId::DigCh);
  
  if (!DigCh) { 
    DigCh = createAttachedSubWin( (editor = new OptimTextEditor()), WindowId::DigCh);
    }
    else { 
       editor = qobject_cast<OptimTextEditor*>(DigCh->widget() );
       if (CtSt_.ClearText) editor->clear();
    }

  DigCh->raise();
  editor->show();
  editor->setCloseAllowed(false); // cannot close window
  
  // the fitting parameters are reported **before** the lattice
  // is compressed (elements concatenated and replaced by matrices
  // when possible). 
  PrintFitParam (editor, &fstep, vfin, dv, npoint, group, ngr);

  if ( !CtSt_.use_fractional_tune && phase_advance_constraint_)  {
    analyzeWithoutCompress(group, ngr, npoint);
  }
  else {
    analyzeCompress(group, ngr, npoint);
  }

  if(GetFitParam (&fstep, vfin, dv, npoint2, group, ngr, false)) {
    emit fitDone();
    return; //fitting parameters after lattice compression
  }

  saveElements();

  // Optimization

  double Q0 = FindError(vfin, dv, npoint, &fstep);
  editor->insertPlainText("\n"); 
  editor->insertPlainText(QString::asprintf("Initial error %g\n", Q0));

  //if(Q0<1.0) { return; }

  interrupted_ = false;
  step_dG  = fstep.dG;
  
  int k = 0;
  do {
   iter = 0;
   Qold = Q0;
   for (int j=0; j<40; ++j){

     QCoreApplication::processEvents();

     if( interrupted_ ){ editor->insertPlainText("Calculation was interrupted."); er=1; goto err; }

     if(j){
        if(SetGradientStep(vfin, dv, npoint, group, ngr, &fstep)){er=2; goto err2;}
    } // return true if cannot close or step is too small; Message box; does not change settings
    
    if(GetGradient(vfin, dv, npoint, group, ngr, &fstep, G)) { 
      er=2; goto err2;
    } //return true if cannot close; Message box; does not change settings

    s = 0.0;

    for (int i=0; i<ngr; ++i)  s += G[i]*G[i];

    s = sqrt(s);
    for (int i=0; i<ngr; i++)  G[i] = -G[i]/s;
     
    int n=0;

    do{

      ++iter;

      Q0 = FindError(vfin, dv, npoint, &fstep);

      if ( Q0 < 1.0 ) break; 

      if(Q0<0.0){ er=2; goto err2; }                //return -1 if cannot close; Message box
      if(DoStep(group, ngr, G, step_dG)) {er=2;}    //return 1 if length is negative; undo changes if err; Message box

      double Q1 = FindError(vfin, dv, npoint, &fstep); 
      if(Q1<0.) {er= 2;}
      if(DoStep(group, ngr, G, step_dG)) {er=2;}

      double Q2 = FindError(vfin, dv, npoint, &fstep);
      if( Q2<0.0) er=2;
      sq = Q0 + Q2-2.0*Q1;

      if(sq > 0.0){ // second derivative gt zero

	s=0.5*(Q0-Q2)/sq - 1.;
	if(s < -2.){
          if(DoStep(group, ngr, G, -3.*step_dG)) {er=2;}
          step_dG *= 1.25;
          continue;
        }
	if(s > 0.){
          if(DoStep(group, ngr, G, step_dG)) {er=2;}
            step_dG *= 1.25;
            continue;
          }
	  if(DoStep(group, ngr, G, s*step_dG))er=2;
	  step_dG *= 0.75;
        } 

        else { //    negative second derivative

          if (Q2<Q0){
            if(DoStep(group, ngr, G, 2*step_dG))er=2;
            step_dG *= 1.25;
          } 
          else {
            if(DoStep(group, ngr, G, -step_dG))er=2;
            step_dG *= 0.5;
          }
          s=2.;
	}
        if(er)goto err2;

    }  while( (n++<20) && ((s>-0.75 ) || (s<-1.25)) );

   }
    
   // editor->moveCursor(QTextCursor::End); // use this to print every line 
   // clear the current line and overwrite with new iteration message
   QTextCursor itcursor = editor->textCursor();
   itcursor.select(QTextCursor::LineUnderCursor);
   itcursor.removeSelectedText();
   editor->setTextCursor(itcursor);

   editor->insertPlainText(QString::asprintf("n=%d \tQ0=%g \tDeltaG=%g \tN_iter=%d", ++k, Q0, step_dG, iter));
   
   if( (Qold-Q0) < 0.00001  && (Q0 > 1.0) ){
      sprintf(buf,"Cannot reach Q=1 (DeltaQ<0.00001)");
      goto err;
   }

  } while( Q0>1.0);
 
  if(Q0<1)  sprintf(buf,"\nFitting successful: Q0=%g",Q0);
   else     sprintf(buf,"\nFitting failed: Q0=%g",Q0);

err:  // er=1 - stop calculations
   QCoreApplication::processEvents();
   editor->insertPlainText("\n"); 
   editor->insertPlainText(buf); 
   editor->insertPlainText("\n");

   if( OptimQuestionMessage( this,"Fit", "Do you want to write these fitting results to main file?",QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {  
     RewriteElementListFit( elmdict_, group, ngr);
     emit fitUndoAllowed(true);
   }
err2:  // er=2 cannot close or step is too small or length is negative; comes here after message box

   editor->insertPlainText("\n Final element parameters values \n");
   PrintGroupElement(editor, group, ngr);

   #if  0
   //.........................................................
   // scroll down automatically  
   QTextCursor c =  editor->textCursor();
   c.movePosition(QTextCursor::End);
   editor->setTextCursor(c);
   editor->ensureCursorVisible(); // you might need this also
   // .........................................................
   #endif
   
   QCoreApplication::processEvents();
   if( (CtSt_.IsRingCh ) && (er==0) ){
     if(OptimQuestionMessage(this, "Fit", "Do you want to overwrite the initial betas with the new ones ?",
			       QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){
     findRMatrix(tm);
     if(find_tunes(tm, Length_, v, &s)){
       OptimMessageBox::warning(this, "Close Error", "Cannot close the lattice", QMessageBox::Ok);
       goto err2;
     }
     substituteBetas (v);
   }
 }
 editor->document()->setModified(false);
 interrupted_ = false;
 editor->setCloseAllowed(true); // ok to close window

 emit fitDone();

 return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::analyzeCompress(FitElem group[], int ngr, int npoint[]) // V7
{

   std::vector<std::shared_ptr<Element>>  edict; // tmp element dict  *ElmList2;
   Beamline bml;                                 // tmp beamline       Element **Elm2;       // references to Elements and their pointers

   RMatrix me;
   double tetaYold;
   double EnrOld;
   double Lelm;
   
  // ..................................................................................
  // count the total number of elements Nf participating in the fit
  // NOTE: some particpating elements may appear more than once in the lattice.
  // Such elements are represented by the same shared_ptr, but they are considered
  // as distinct locations.
  // ..................................................................................
   
  int  Nf= 0;
  for( int k=0; k < ngr; ++k) { // iterate over all the groups
    for( int j=0; j<group[k].n; ++j) { // iterate over elements of a group
	 int n = group[k].el[j];
         for (int i=0; i<nelm_; ++i) {
           if(strcmp(beamline_[i]->name(), elmdict_[n]->name())==0) { ++Nf;}
      }
    }
  }

  // Nf = "number of fitting elements" 
  // std::cout << "Nf = " << Nf << std::endl; 

  // the dimensions calc below need to be documented
                                          // npoint[0]= no of fitting constraints; npoint[i] i != 0, location index of the constraint i
  int NElm2     = 2*Nf + 1 + npoint[0];   // Max number of elements in the compressed beamline; npoint[0]=0 if fitting at the end lattice only
  int Nmat2     = Nf   + npoint[0] + 1;   // max number of new matrices in the compressed line 

  int NElmList2 = Nf   + npoint[0] + 1;   // max number of elm in the new element dict (see next line)
    
  for(int k=0; k < ngr; k++) NElmList2 += group[k].n; //number of Elements found in ElmList

  // Memory allocation
  bml.resize(NElm2);          // Pointer to Array of pointers with length NElm2
  edict.resize(NElmList2);    //Array keeping Element parameters,Length_ is equal to NElmList2

  std::vector<RMatrix> tmat2(Nmat2);
  //RMatrix tmat2[Nmat2];

  // create derivative structure

  double tetaY =  tetaYo0_;
  double Enr   = Ein;
  
  int n  = 0;
  int nm = 0;
  int nL = 0;    // current numbers for Element and matrix, and number in Element list
  int npnt = 1;
   
  int i    = 0;
  int br = 0;

  while ( i <nelm_) { // outer while loop
    tmat2[nm].toUnity();
    EnrOld   = Enr;
    tetaYold = tetaY;
    Lelm     = 0.0;

    // compute and write transfer matrix between active Elements

    std::shared_ptr<Element> ep;

    while (i <  nelm_){ // inner while loop 
      ep    = beamline_[i];
      Lelm += ep->length();

      br    = 0;
      for(int k=0; k < ngr; ++k) {
        for (int j=0; j<group[k].n; ++j) {
	   int nn = group[k].el[j];
	   if( strcmp(ep->name(), elmdict_[nn]->name())== 0 ){ br = 1; break;}
         }
      }

      if( npnt < npoint[0]) {
	if(npoint[npnt]==(i+1)) {
          npoint[npnt] = n+1;
          npnt++;
          br |= 2; // bitwise or with b00000010
        }
      }
      me = ep->rmatrix(Enr, ms, tetaY, 0.0, 3);

      if(br) break;

      tmat2[nm] = me*tmat2[nm];

      ++i;

     } // inner while loop  


     // make a xfermatrix element  
     // this process is really kludgy. 
     // ..................................................

     char tmpname[128];
     sprintf(tmpname,"X_matr%-d",nm+1);

     edict[nL] = std::shared_ptr<Element>( Element::makeElement(tmpname));

     auto etmp = std::dynamic_pointer_cast<XferMatrix>(edict[nL]);
 
     etmp->name(tmpname);
     etmp->S = edict[nL]->tilt(0.0);
     etmp->G = Enr-EnrOld;
     etmp->length(Lelm);
     etmp->B = tetaY-tetaYold;
     etmp->N = nm;
     etmp->setMatrix(tmat2[nm]);

     // ..................................................
     nm++;
     bml[n++] = edict[nL++];

 
     if(i==nelm_)break;    //continue if it was break'ed at Element where twiss parameters have to be fitted
                           // copying parameters of Element to be adjusted (fitted)
     if(br==2){ ++i; continue;}

     int br1 = 0;
     int k   = 0;

     for( k=0; k<nL; ++k) if(strcmp(ep->name(), edict[k]->name())==0) { br1=1; break;}

     if(br1){
        bml[n++] = edict[k];
     }
     else{

       edict[nL] = std::shared_ptr<Element>(ep->clone());
       bml[n++] = edict[nL++];
     }

     if (br==3){ //add empty Element if the same Element is used for Twiss and for adjustment

       tmat2[nm].toUnity();

       char tmpname[128];
       sprintf(tmpname,"X_matr%-d",nm+1);
       edict[nL] = std::shared_ptr<Element>( Element::makeElement(tmpname));
       auto etmp = std::dynamic_pointer_cast<XferMatrix>(edict[nL]);
       
       etmp->S = 0.0;
       etmp->tilt(0.0);
       etmp->G = 0.0;
       etmp->length(0.0);
       etmp->B = 0.0;
       etmp->N = nm;
       etmp->tmat_ = tmat2[nm];  nm++;
     }
     ++i;

   } // outer while loop 

   // bml and  edict need to be trimmed to their actual size 
   bml.erase(bml.begin() + n,  bml.end()); 
   edict.erase(edict.begin() + nL,  edict.end()); 
   
   // overwrite beamline and elmdict_ with the new ones
   
   beamline_ = bml;
   elmdict_  = edict;

   nelm_     =  beamline_.size();      
   nelmlist_ =  elmdict_.size();

   // for debugging 
#if 0
   std::cout << "beamline_ after compression " <<std::endl;
   std::cout << "----------------------------" <<std::endl;
 
   for (int i=0; i< beamline_.size(); ++i) {
     std::cout << "i = " << i << "  " << beamline_[i]->name() << std::endl; 
   }

   std::cout << "elmdict_ after compression " << std::endl;
   std::cout << "----------------------------" <<std::endl;
 
   for (int i=0; i< elmdict_.size(); ++i) {
     std::cout << "i = " << i << "  " << elmdict_[i]->name() << std::endl; 
   }

#endif   
   return Nf;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::analyzeWithoutCompress(FitElem group[], int ngr, int npoint[]) 
{

  // determine  Nf, the no of elements positions participating in a fit
  
  int  Nf= 0;
  for( int k=0; k < ngr; ++k) { // iterate over all the groups
    for( int j=0; j<group[k].n; ++j) { // iterate over elements of a group
	 int n = group[k].el[j];
         for (int i=0; i<nelm_; ++i) {
           if(strcmp(beamline_[i]->name(), elmdict_[n]->name())==0) { ++Nf;}
      }
    }
  }
   
 //*****************************************************************************
 // **** this dummy version does not replace segments of the line with matrices.
 //*****************************************************************************
 //  ********   DANGER WILL ROBINSON DANGER !!! **********
 // The fitting procedure expects elements in elmdict_ to point 
 // to elements in the beamline. An element in elmdict_ can point to
 // more than one location in the beamline, if the element occurs more than once.   
 // In when optim parses a file elmdict_ is constructed first.
 // When beamline_ is constructed, the element inserted in the beamline are cloned
 // from entries in elmdict_. This means that the pointers in elmdict_ are *NOT*
 // pointers to beamline elements. To ensure that this is the case, elmdict_  
 // neeeds to be rebuild. This is done below.  The beamline_ is  also rebuild
 // to ensure that all the variable elements in the beamline that share a label
 // point to a unique element instance in memory.  
 // ----------------------------------------------------------------------------------
  
  std::vector<std::shared_ptr<Element>>  edict; 

  // rebuild the dictionary.

  for( int k=0; k < ngr; ++k) { 
    for( int j=0; j<group[k].n; ++j) {
	int n = group[k].el[j];
        for( int i=0; i < nelm_; ++i) { 
           if(strcmp(beamline_[i]->name(), elmdict_[n]->name())==0) { 
             edict.push_back( beamline_[i] );
	     group[k].el[j] = edict.size()-1; // replace old index with new index in edict  
	     break; // no duplicate in dictionary (elm name may appear more than once in beamline)
	   }
        }
    }
  }

  // rebuild the beamline.

  for( int k=0; k < ngr; ++k) { 
    for( int j=0; j<group[k].n; ++j) {
	int n = group[k].el[j];
         for( int i=0; i < nelm_; ++i) { 
           if(strcmp(beamline_[i]->name(), edict[n]->name())==0) { 
             beamline_[i] = edict[n];
	   }
        }
    }
  }
  
 elmdict_  = edict;
 nelmlist_ = edict.size();
 
 return  Nf;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdFittingStop ()
{
     interrupted_ = true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdFittingUndo()
{
  RewriteElementList( ElmListFit_ );
  emit fitUndoAllowed(false);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 
   void  OptimMainWindow::stateFitDone()
   {
     // std::cout <<  "OptimMainWindow::stateFitDone()" << std::endl;
          fitBetasAct_->setEnabled(true);
           fitUndoAct_->setEnabled(true);
              stopAct_->setEnabled(true);
        fitControlAct_->setEnabled(true);

   }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

   void  OptimMainWindow::stateFitInProgress()
   {
      designReferenceOrbitToggleAct_->setEnabled(false);
      ringLineToggleAct_->setEnabled(false);       
      //lineAct_->setEnabled(false);       
                 newAct_->setEnabled(false);       
                openAct_->setEnabled(false);
             newTextAct_->setEnabled(false);
            openTextAct_->setEnabled(false);
            compressAct_->setEnabled(false);
         compressOldAct_->setEnabled(false);
          calculatorAct_->setEnabled(false);
          importMad8Act_->setEnabled(false);
          importMadXAct_->setEnabled(false);
         importDimadAct_->setEnabled(false);
       importResolveAct_->setEnabled(false);
          exportMad8Act_->setEnabled(false);
          exportMadXAct_->setEnabled(false);
         exportDimadAct_->setEnabled(false);
       exportResolveAct_->setEnabled(false);
                saveAct_->setEnabled(false);
              saveAsAct_->setEnabled(false);
             executeAct_->setEnabled(false);
         getFileNameAct_->setEnabled(false);
        printPreviewAct_->setEnabled(false);
               printAct_->setEnabled(false);
          printSetupAct_->setEnabled(false);
                exitAct_->setEnabled(true);



                   undoAct_->setEnabled(false);       
                   redoAct_->setEnabled(false);       
                    cutAct_->setEnabled(false);
                   copyAct_->setEnabled(false);
                  pasteAct_->setEnabled(false);
               clearAllAct_->setEnabled(false);
                 deleteAct_->setEnabled(false);
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

           findAct_->setEnabled(false);
        replaceAct_->setEnabled(false);
           nextAct_->setEnabled(false);

          fitBetasAct_->setEnabled(false);
           fitUndoAct_->setEnabled(false);
              stopAct_->setEnabled(true);
        fitControlAct_->setEnabled(false);

                 closeLatticeAct_->setEnabled(false);
             closeSymmetricalAct_->setEnabled(false);
                      trackerAct_->setEnabled(false);
         trackerPlotPositionsAct_->setEnabled(false);
         trackerSavePositionsAct_->setEnabled(false);
      trackerPlotDistributionAct_->setEnabled(false);
      //trackerPlotEmittanceAct_->setEnabled(false);
      //trackerPlotIntensityAct_->setEnabled(false);
 
                 trajectoryAct_->setEnabled(false);
             typeTrajectoryAct_->setEnabled(false);
            closeTrajectoryAct_->setEnabled(false);
                tuneDiagramAct_->setEnabled(false);
           showExternalFileAct_->setEnabled(false);
               toolsControlAct_->setEnabled(false);

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
                   controlAct_->setEnabled(false);
                    detachAct_->setEnabled(false);
                    attachAct_->setEnabled(false);
              attachDetachAct_->setEnabled(false);


             update4DBetasAct_->setEnabled(false);
                   close4DAct_->setEnabled(false);
                   betas4DAct_->setEnabled(false);
              dispersion4DAct_->setEnabled(false);
                   sizes4DAct_->setEnabled(false);
                 project4DAct_->setEnabled(false);
                  phases4DAct_->setEnabled(false);
               functions4DAct_->setEnabled(false);
         functions4DAtElemAct_->setEnabled(false);
               integrals4DAct_->setEnabled(false);
                 control4DAct_->setEnabled(false);

                   cwBetasAct_->setEnabled(false);
                   cwSizesAct_->setEnabled(false);
                 cwProjectAct_->setEnabled(false);
                  cwPhasesAct_->setEnabled(false);
               cwFunctionsAct_->setEnabled(false);
            cwCloseLatticeAct_->setEnabled(false);
                 controlSCAct_->setEnabled(false);
  
                   cascadeAct_->setEnabled(false);
                      tileAct_->setEnabled(false);
              arrangeIconsAct_->setEnabled(false);
                  closeAllAct_->setEnabled(false);
		  //windowStyleAct_->setEnabled(false);
		  //openedSubWinsAct_->setEnabled(false);


         editorPreferencesAct_->setEnabled(false);
           plotPreferencesAct_->setEnabled(false);
           compPreferencesAct_->setEnabled(false);
           savePreferencesAct_->setEnabled(false);

                optimHelpAct_->setEnabled(true);
    
	  // std::cout <<  "OptimMainWindow::stateFitInProgress()" << std::endl;
   }

