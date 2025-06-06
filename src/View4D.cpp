//  =================================================================
//
//  View4D.cpp
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
#include <complex>
#include <cmath>
#include <cstdio>
#include <exception>
#include <memory>
#include <cstring>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QObject>
#include <fmt/format.h>

#include <Constants.h>
#include <Element.h>
#include <Control4DDialog.h>
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

#include <QwtLegend>

using Constants::PI;
using Utility::matr_invert;
using Utility::balanc;
using Utility::elmhes;
using Utility::hqr;
using Utility::strcmpr;
using Utility::decodeLine;
using Utility::decodeExtLine;
using Utility::filterName;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#define LSTR 1024


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdView4DControl()
{
  static Control4DDialog* dialog = 0;
  if (!dialog) dialog = new Control4DDialog(0);

  Dlg4DCtrlStr& data = dialog->data_; 
  data.UpdateTwiss           = NSt4D.UpdateTwiss;
  data.ComputeAtExcitedOrbit = NSt4D.ComputeAtExcitedOrbit;

  dialog->set();   
  if ( dialog->exec() == QDialog::Rejected ) return;
 
  NSt4D.UpdateTwiss           =  data.UpdateTwiss;
  NSt4D.ComputeAtExcitedOrbit =  data.ComputeAtExcitedOrbit;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdViewUpdate4Dblock ()
{

  if(interrupted_ ) { interrupted_= false; return;}
  if(analyze( !analyzed_) ) return;

  Twiss4D v;
  Get4DBetasFromFile(true, true, v);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdView4DClose ()
{
   if(interrupted_){interrupted_=false; return;} 
   if(analyze( !analyzed_)) return;

   int ir;
   char err[256];
   Twiss4D v;
   RMatrix tm;
   std::complex<double> lambda[4], ev[4][4];
   
   // calculate transfer matrix and tunes

   findRMatrix(tm);
   if ( (ir = RMatrix_t<4,double>(tm).findEigenValues( lambda, ev)) !=0 ) {
     OptimMessageBox::warning(this, "4D View", "Cannot close lattice.", QMessageBox::Ok);
   }
   else{
     v.betatronFromEV(ev);
     getDisp4D(tm, v, err);

     if(err[0]) { OptimMessageBox::warning(this, "4D View", err, QMessageBox::Ok);   return; }
     Print4DBetasToMain(v);
   }
   
   OptimTextEditor* editor = 0;
   auto Dig4Ch = getAttachedSubWin(WindowId::Dig4Ch ); 
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

   TypeEigenVectorsToOutW(editor, tm, lambda, ev, v, ir);

   editor->document()->setModified(false);
   editor->show();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimMainWindow::cmdView4DSize ()
{
  Twiss4D v;
  RMatrix tm;
  std::complex<double> lambda[4], ev[4][4];
  char err[256];

  if (interrupted_){ interrupted_ = false; return;}
  if (analyze( !analyzed_ ) ) return;

  //..............................................................................
  // To compute the beam size, the eigen-emittances must be known.
  // The eigen-emittances are set in the 4DBetaBlock. For this reason,
  // we read this block first. In ring mode, the 4DBetaBlock lattice functions
  // in the Twiss4D struct v are later overwritten with the periodic solution.
  // However, the emittances e1 and e2 are PRESERVED.
  //..............................................................................
  
  if(Get4DBetasFromFile(false, false, v)) return;

  try{ 

    if( CtSt_.IsRingCh ) {
       findRMatrix(tm);
       if( RMatrix_t<4,double>(tm).findEigenValues(lambda, ev) !=0 ) {
	 OptimMessageBox::warning(this, "4D View", "Cannot close lattice.",QMessageBox::Ok);
         return;
       }
     
       v.betatronFromEV(ev);
 
       getDisp4D(tm, v, err);
       if(err[0]){ OptimMessageBox::warning(this, "4D View", err, QMessageBox::Ok); return;}
       
    }
  }

  catch ( std::runtime_error& e) {
      std::throw_with_nested(std::runtime_error("View4DSize"));
  }  
 

  Sizes4D(v);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdView4DPhases ()
{

   Twiss4D v;
  
   if(interrupted_) { interrupted_=false; return;     }
   if(analyze(!analyzed_)) return;  

   RMatrix tm;
   std::complex<double> lambda[4], ev[4][4];
   char err[256];
   if( CtSt_.IsRingCh ){
     findRMatrix(tm);
     if( RMatrix_t<4,double>(tm).findEigenValues(lambda, ev)!=0){
       OptimMessageBox::warning(this,"4D View", "Cannot close lattice.",QMessageBox::Ok);
       return;
     }

     v.betatronFromEV(ev);

     getDisp4D(tm, v, err);
     if(err[0]){ OptimMessageBox::warning(this,"4D View",err, QMessageBox::Ok); return;}
    }
   else { 
     if(Get4DBetasFromFile(false, false, v)) return;
   }
  Phases4D(v);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdView4DProjection()
{

   Twiss4D v;

   if(interrupted_){interrupted_= false; return;}
   if(analyze(!analyzed_)) return;

   if(Get4DBetasFromFile(false, false, v))return;

   //..............................................................................
   // To compute the beam size, the eigen-emittances must be known.
   // The eigen-emittances are set in the 4DBetaBlock. For this reason,
   // we read this block first. In ring mode, the 4DBetaBlock lattice functions
   // in the Twiss4D struct v are later overwritten with the periodic solution.
   // However, the emittances e1 and e2 are PRESERVED.
   //..............................................................................

   RMatrix tm;
   std::complex<double> lambda[4], ev[4][4];
   char err[256];
   if( CtSt_.IsRingCh ){
     findRMatrix(tm);
     if( RMatrix_t<4,double>(tm).findEigenValues(lambda, ev)!=0) {
       OptimMessageBox::warning(this,"4D View", "Cannot close lattice.",QMessageBox::Ok);
       return;
     }

     v.betatronFromEV(ev);

     getDisp4D(tm, v, err);
     if(err[0]){ OptimMessageBox::warning(this, "4D View", err, QMessageBox::Ok); return;}
   }
   Proj4D(v);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdToolsShowExtern ()
{
  #define MAXDAT 20
  
  char buf[256];  

  ExterPlotDlgStruct st;
  /*
   static ExterPlotDlgStruct st = 
   {"", "",  
      0,  1,  10 , 
     -1 , 1 , 10, 
     -1,  1 , 10,
      1, 
      2, 0, "Y1", true, true,
      0, 0, "Y2", true, true,
      0, 0, "Y3", true, true,
      0, 0, "Y4", true, true 
   };
  */

  static ExternalPlotDialog*  dialog = 0;

   if (!dialog) {
      dialog = new  ExternalPlotDialog(0);
      dialog->data_  = st;
      dialog->set();
   }

   if( dialog->exec() == QDialog::Rejected) return;
   st = dialog->data_;
   
   // count data length

   FILE* fp = 0;   
   if ( !(fp=fopen( dialog->data_.filename, "r")) ) {
      sprintf(buf,"Cannot plot file %s", dialog->data_.filename);
      OptimMessageBox::warning( this, "External Plot", buf, QMessageBox::Ok);
      return;
   }

   int n = 0; 
   while ( fgets(buf, 255, fp)) { 
     if (!(buf[0]=='#' || buf[0]==';')) ++n;
   }
   fclose(fp);

   std::vector<int>      column = {  dialog->data_.col[1], dialog->data_.col[2], dialog->data_.col[3], dialog->data_.col[4] };
   std::vector<double>   x(n);
   std::vector<std::vector<double>> y(4);
   std::vector<double>   dat(MAXDAT); 

   for(int i=0; i<4; ++i) { y[i].resize(n); }

   // Read data

   fp = fopen(dialog->data_.filename,"r");

   int i = 0 ;
   while ( fgets(buf, 255, fp) )  {
     if ( buf[0]=='#' || buf[0]==';') continue;
     decodeExtLine( buf,  &dat[0], MAXDAT);
     x[i] = dat[dialog->data_.col[0]-1];
     for(int j=0; j<4; ++j) {
       if (column[j]>0 ) y[j][i] = dat[column[j]-1];
     }
     ++i;
   }

   fclose(fp);

   sprintf(buf,"External plot from file %s",st.filename);

   OptimPlot* plot = 0;
   QMdiSubWindow* sw = mdiArea->addSubWindow( (plot = new OptimPlot(0)) );
   sw->setGeometry(mdiArea->geometry().width()/2, 0, mdiArea->geometry().width()/2,  mdiArea->geometry().height());
   sw->setProperty("attached", (QVariant()) );  // not related to main editor. 

   PlotSpec plotspecs;
   plotspecs.title =  buf;
   plotspecs.bottom_title = "";
   auto& curvespecs = plotspecs.curvespecs;

   //-----------------------------------------------------------------------------------------------------------------------------------
   //                     legendname xv       yv    n  axis             title                          bottom_title         vertical_title  
   //------------------------------------------------------------------------------------------------------------------------------------  
  
   for (int i=0; i<4; ++i) {
     if (column[i] == 0) continue; 
     curvespecs.push_back({ dialog->data_.legend[i+1],    &x[0], &y[i][0], n, QwtSymbol::NoSymbol,  (st.axis[i+1] == 1 ? QwtPlot::yRight:  QwtPlot::yLeft), ""  , "" });  
   }
   plot->configure(plotspecs); 
   plot->replot(); 
   plot->show();
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::Get4DBetasFromFile(bool Reprint, bool cmdUpdate, Twiss4D& v)
{

 auto oldtwiss4d = v; // save the old values
  
 char const* bufa[]={"4D_BetaBlock",
	       "\tBeta_X_1[cm]=100. \tBeta_X_2[cm]=0.",
	       "\tAlpha_X_1=.1      \tAlpha_X_2=0.",
	       "\tBeta_Y_1[cm]=0.   \tBeta_Y_2[cm]=100.",
	       "\tAlpha_Y_1=0.      \tAlpha_Y_2=0.1 ",
	       "\tNu_1[deg]=0.      \tNu_2[deg]=0    U=0",
	       "\tEmit1[cm]=1.e-5.  \tEmit2[cm]=1.e-5",
	       "\tDispX[cm]=0.      \tDispY[cm]=0",
	       "\tDispXpr[cm]=0.    \tDispYpr[cm]=0",
	       "End4DBetaBlock"};
 
 int buflen = sizeof(bufa)/sizeof(char const*);

 double dat[6];
 char buf[257];
 int nline, i, j, ret=0;
 double fi1[4], s;

 if (!LatticeCh_) return 1;
 OptimEditor* editor = qobject_cast<OptimEditor*>(LatticeCh_->widget()); 
 
  int nmblines = editor->document()->lineCount(); 
  for(nline=LineLFin; nline<nmblines; nline++) { 
    strcpy(buf, editor->getLine(nline).toUtf8().data()); //editor->GetLine(buf, LSTR, nline);
    if(strcmpr(bufa[0],buf)) break;
  }
  // Creating 4D beta block
  if(nline == nmblines){
    if (OptimInformationMessage(this, "4D View", "There is no 4D_BetaBlock. Do you want to create it?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::No )return 1;
    replaceLine(editor, nline-1, buf);
    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor); 
    for(i=0; i<buflen; ++i) { 
      cursor.insertText(bufa[i]);
      cursor.insertBlock();	
    }
    return 1;
  }
  // decoding of the beta-functions

  ++nline;

  nline = getLineCmt(editor, buf, LSTR, nline);
  if (nline == -1) return 1;
  
  decodeLine(buf,dat,2);

  v.btx1 = dat[0];
  v.btx2 = dat[1];

  if((v.btx1<=0.) || (v.btx2<0.)){
    OptimMessageBox::warning(this, "4D View", "Zero or negative beta-function",QMessageBox::Ok);
    replaceLine(editor, nline-1, buf);
    return 1;
  }
  if(Reprint){
    sprintf(buf,"\tBeta_X_1[cm]=%lg \tBeta_X_2[cm]=%lg",v.btx1, v.btx2);
    replaceLine(editor, nline-1, buf);
  }

  
  nline = getLineCmt(editor, buf, LSTR, nline);
  if (nline == -1) return 1;
  decodeLine(buf,dat,2);  v.alfx1=dat[0];  v.alfx2=dat[1];
  if(Reprint){
    sprintf(buf,"\tAlpha_X_1=%lg  \tAlpha_X_2=%lg",v.alfx1, v.alfx2);
    replaceLine(editor, nline-1, buf);
  }
  nline = getLineCmt(editor, buf, LSTR, nline);
  if (nline == -1) return 1;
  decodeLine(buf,dat,2);

  v.bty1 = dat[0];
  v.bty2 = dat[1];

  if((v.bty1<0.) || (v.bty2<=0.)){
    OptimMessageBox::warning(this, "4D View", "Zero or negative beta-function", QMessageBox::Ok);
    replaceLine(editor, nline-1, buf);
    return 1;
  }
  if(Reprint){
    sprintf(buf,"\tBeta_Y_1[cm]=%lg \tBeta_Y_2[cm]=%lg",v.bty1, v.bty2);
    replaceLine(editor, nline-1, buf);
  }

  nline=getLineCmt(editor, buf, LSTR, nline);
  if (nline == -1) return 1;
  decodeLine(buf,dat,2);

  v.alfy1 = dat[0];
  v.alfy2 = dat[1];

  if(Reprint){
    sprintf(buf,"\tAlpha_Y_1=%lg  \tAlpha_Y_2=%lg",v.alfy1, v.alfy2);
    replaceLine(editor, nline-1, buf);
  }


  // attempt to determine possible values for the parameters teta1, teta2 and u;

  double um1  = 0.0;
  double um2  = 0.0;
  double up1  = 0.0;
  double up2  = 0.0;
  double fip1 = 0.0;
  double fip2 = 0.0;
  double fim1 = 0.0;
  double fim2 = 0.0;

  if(cmdUpdate || NSt4D.UpdateTwiss) {

    double cpx  =  sqrt(v.btx2/v.btx1);
    double cpy  =  sqrt(v.bty1/v.bty2);
    double cpx2 =  cpx*cpx;
    double cpy2 =  cpy*cpy;
    double Ax   =  cpx2*v.alfx1-v.alfx2;
    double Ay   =  cpy2*v.alfy2-v.alfy1;
    double Cp   =   Ax*(cpx2-1.0)*cpy2 - Ay*(cpy2-1.0) *cpx2;
    double Ap   =  (Ax*(cpy2-1.0)      - Ay*(cpx2-1.0))*cpx*cpy;
    double B0   =  (cpx2-cpy2)*cpx*cpy;
    double cmd  =   Ax*(cpx2+1.)*cpy2  + Ay*(cpy2+1.0) *cpx2;
    double Am   = -(Ax*(cpy2+1.)       + Ay*(cpx2+1.0))*cpx*cpy;

    // Ap == A+
    // Am == A-
    
     
    if(Ap*Ap > 1.0e-8){
      double sp1 = asin(Cp/sqrt(Ap*Ap+B0*B0));
      double sp2 = atan2(Ap,B0);
      fip1 = PI-sp1+sp2;
      fip2 = sp1+sp2; 
      up1  = v.getU4Dp(fip1, ret);
      up2  = v.getU4Dp(fip2, ret);
    }
    else {
      up1  = up2 =0.5;
      fip1 = fip2 = PI/2.;
      ret  = 1;
    }
    if(Am*Am > 1.0e-8){
      double sm1  = asin(cmd/sqrt(Am*Am+B0*B0));
      double sm2  = atan2(Am,B0);
       fim1 = PI+sm1-sm2;
       fim2 = -sm1-sm2;
       um1 = v.getU4Dm(fim1, ret);
       um2 = v.getU4Dm(fim2, ret);
    }
    else {
      um1 = um2  = 0.5; 
     fim1 = fim2 = PI/2.;  
      ret = 1;
    }
    if( fabs(up1-um1) >= 0.5*fabs(up1-up2)) {
         s = um1; 
       um1 = um2; 
       um2 = s;
         s = fim1; 
      fim1 = fim2; 
      fim2 = s;
    }

    if( (fabs(up1-um1) >1.e-8) || (fabs(up2-um2)>1.e-8)) {
      if(ret){
	std::cout <<  "fabs(up1-um1) = " <<   fabs(up1-um1) << std::endl;
	std::cout <<  "fabs(up2-um2) = " <<   fabs(up2-um2) << std::endl;
	OptimMessageBox::warning(this, "4D View",
        		     "Cannot correctly update Nu_1, Nu_2 and U for given 4-D beta-functions",
			     QMessageBox::Ok);
	v = oldtwiss4d;  // restore twiss values
        return 1;
       }
      OptimMessageBox::warning(this, "4D View", "Internal err.1 (4Dcalc) )",QMessageBox::Ok);
      replaceLine(editor, nline-1, buf);
      v = oldtwiss4d;  // restore twiss values
      return 1;
    }
    fi1[0] = tnorm(0.5*(fip1-fim1));
    fi1[1] = tnorm(0.5*(fip2-fim2));
    fi1[2] = tnorm(fi1[0]+PI);
    fi1[3] = tnorm(fi1[1]+PI);
  }

  // reading and comparison for phases

  nline = getLineCmt(editor, buf, LSTR, nline);
  decodeLine(buf,dat,3);

  if(cmdUpdate || NSt4D.UpdateTwiss){
    s = tnorm(dat[0]*PI/180.);
    j=0;
    double dif = fabs(s-fi1[0]);
    for(i=1; i<4; i++)if(fabs(tnorm(s-fi1[i])) < dif ) {
      j=i;
      dif=fabs(tnorm(s-fi1[i]));
    }

    // phase settings

    v.teta1=fi1[j];

    switch (j){
      case 0:
      	v.teta2 = tnorm(0.5*(fip1+fim1));
   	v.u=up1;
        break;
      case 1:
      	v.teta2=tnorm(0.5*(fip2+fim2));
   	v.u=up2;
        break;
      case 2:
      	v.teta2=tnorm(0.5*(fip1+fim1)+PI);
   	v.u=up1;
        break;
      case 3:
      	v.teta2=tnorm(0.5*(fip2+fim2)+PI);
   	v.u=up2;
        break;
    }
  }
  else { // 
    v.teta1 = dat[0]*PI/180.0;
    v.teta2 = dat[1]*PI/180.0;
    v.u     = dat[2];

    double qint = 0.0;

    v.qx    = modf(dat[0]/PI, &qint);
    v.qx    = v.qx <  -0.5 ? (v.qx +=1.0)      : v.qx; 
    v.qx    = v.qx >   0.5 ? (v.qx = 1.0-v.qx) : v.qx; 

    v.qy    = modf(dat[1]/PI, &qint);
    v.qy    = v.qy <  -0.5 ? (v.qy +=1.0)      : v.qy; 
    v.qy    = v.qy >   0.5 ? (v.qy = 1.0-v.qy) : v.qy; 
    
  }
  if(Reprint){
    sprintf(buf,"\tNu_1[deg]=%lg \tNu_2[deg]=%lg \tU=%lg", v.teta1*180./PI, v.teta2*180./PI, v.u);
    replaceLine(editor, nline-1, buf);
  }

  // Emittances and dispersions

  nline=getLineCmt(editor, buf, LSTR, nline);
  decodeLine(buf,dat,2);  v.e1=dat[0];  v.e2=dat[1];
  if(Reprint){
    sprintf(buf,"\tEmit1[cm]=%lg  \tEmit2[cm]=%lg",v.e1, v.e2);
    replaceLine(editor, nline-1, buf);
  }
  nline=getLineCmt(editor, buf, LSTR, nline);
  decodeLine(buf,dat,2);  v.dx=dat[0];  v.dy=dat[1];

  if(Reprint){
    sprintf(buf,"\tDispX[cm]=%lg \tDispY[cm]=%lg",v.dx, v.dy);
    replaceLine(editor, nline-1, buf);
  }

  nline=getLineCmt(editor, buf, LSTR, nline);
  decodeLine(buf,dat,2);  v.dxp=dat[0];  v.dyp=dat[1];

  if(Reprint){
    sprintf(buf,"\tDispXpr[cm]=%lg \tDispYpr[cm]=%lg",v.dxp, v.dyp);
    replaceLine(editor, nline-1, buf);
  }

  if(ret){
    OptimMessageBox::warning(this,
			 "4D View", "Cannot correctly update Nu_1, Nu_2 and U for given 4-D beta-functions",
			 QMessageBox::Ok);
    v = oldtwiss4d;
    return 1;
  }
  // Checking last comment

  getLineCmt(editor, buf, LSTR, nline);
  if(strcmpr(bufa[buflen-1],buf)) return 0;

  OptimMessageBox::warning(this, "4D View", "Error: There is no End4DBetaBlock statement", QMessageBox::Ok);
  return 1;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::Print4DBetasToMain(Twiss4D& v)
{

 char const* bufa[]={"4D_BetaBlock", "End4DBetaBlock"};
 char buf[257], buf1[257];
 int nline;
 bool New4DBlock;

 if (!LatticeCh_) return;
 
 OptimEditor* editor = qobject_cast<OptimEditor*>( LatticeCh_->widget());

 if (!editor) return;

 int nmblines = editor->document()->lineCount(); 
 for(nline=LineLFin; nline<nmblines; nline++){
    strcpy(buf, editor->getLine(nline).toUtf8().data()); //editor->GetLine(buf, LSTR, nline);
    if(strcmpr(bufa[0],buf)) break;
 }
 // Creating 4D beta block
  QTextCursor cursor = editor->textCursor();
  cursor.movePosition( QTextCursor::End, QTextCursor::MoveAnchor);

  if(nline == nmblines){
   OptimMessageBox::warning(this,"4D View", "There is no 4D_BetaBlock. It will be created.", QMessageBox::Ok);
   replaceLine(editor, nline-1, buf);
   cursor.insertText(bufa[0]);
   cursor.insertText("\n");
   New4DBlock=true;
 }
 else { 
  replaceLine(editor, nline, buf);
  New4DBlock=false;
 }
 nline++;
 // decoding of the beta-functions

 sprintf(buf,"\tBeta_X_1[cm]=%-12g \tBeta_X_2[cm]=%-12g",v.btx1, v.btx2);
 if(New4DBlock) { 
   cursor.insertText(buf);
   cursor.insertText("\n");

 }
 else {
   nline=getLineCmt(editor, buf1, LSTR, nline);
   replaceLine(editor, nline-1, buf);
 }

 sprintf(buf,"\tAlpha_X_1=%-12g  \tAlpha_X_2=%-12g",v.alfx1, v.alfx2);
 if(New4DBlock) {
   cursor.insertText(buf);
   cursor.insertText("\n");
 }
 else {
   nline=getLineCmt(editor, buf1, LSTR, nline);
   replaceLine(editor, nline-1, buf);
 }

 sprintf(buf,"\tBeta_Y_1[cm]=%-12g \tBeta_Y_2[cm]=%-12g",v.bty1, v.bty2);
 if(New4DBlock) {
   cursor.insertText(buf);
   cursor.insertText("\n");
 }
 else {
   nline=getLineCmt(editor, buf1, LSTR, nline);
   replaceLine(editor, nline-1, buf);
 }

 sprintf(buf,"\tAlpha_Y_1=%-12g  \tAlpha_Y_2=%-12g",v.alfy1, v.alfy2);
 if(New4DBlock) {
   //cursor.insertText("\n");	
   cursor.insertText(buf);
   cursor.insertText("\n");
 }
 else {
   nline=getLineCmt(editor, buf1, LSTR, nline);
   replaceLine(editor, nline-1, buf);
 }

 sprintf(buf,"\tNu_1[deg]=%-12g \tNu_2[deg]=%-12g \tU=%-12g",
 v.teta1*180./PI, v.teta2*180./PI, v.u);
 if(New4DBlock) {
   //cursor.insertText("\n");	
   cursor.insertText(buf);
   cursor.insertText("\n");
 }
 else {
  nline=getLineCmt(editor, buf1, LSTR, nline);
  replaceLine(editor, nline-1, buf);
 }

 if(New4DBlock) {
   //cursor.insertText("\n");
   cursor.insertText("\tEmit1[cm]=1.e-5  \tEmit2[cm]=1.e-5");
   cursor.insertText("\n");
 }
 else {
   nline=getLineCmt(editor, buf1, LSTR, nline);
 }

 sprintf(buf,"\tDispX[cm]=%-12g \tDispY[cm]=%-12g",v.dx, v.dy);
 if(New4DBlock) {
    //cursor.insertText("\n");	
    cursor.insertText(buf);
    cursor.insertText("\n");
 }
 else {
   nline=getLineCmt(editor, buf1, LSTR, nline);
   replaceLine(editor, nline-1, buf);
 }

 sprintf(buf,"\tDispXpr[cm]=%-12g \tDispYpr[cm]=%-12g",v.dxp, v.dyp);
 if(New4DBlock) { 
   //cursor.insertText("\n");	
   cursor.insertText(buf);
   cursor.insertText("\n");
 }
 else {
   nline=getLineCmt(editor, buf1, LSTR, nline);
   replaceLine(editor, nline-1, buf);
 }

 if(New4DBlock) {
   //cursor.insertText("\n");	
   cursor.insertText(bufa[1]);
   cursor.insertText("\n");
 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::TypeEigenVectorsToOutW(  OptimTextEditor* editor,
	   RMatrix& tm, std::complex<double> lambda[4], std::complex<double> v[4][4], Twiss4D& vt, int err)
{

  char bel[3]={'\015','\012','\000'};
  char buf[256];

  if(CtSt_.ClearText) { 
    editor->clear();
  }
  else{ 
    editor->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    editor->insertPlainText(bel);
  }

  if(lambda){

    sprintf(buf,"Eigenvalues:");
    editor->insertPlainText(buf);
    editor->insertPlainText(bel);

    for(int i=0; i<4; ++i){
      editor->insertPlainText(QString::asprintf("  Lambda[%d] \t= %10lg + i%10lg \tabs(Lambda[%d])-1.0 \t= %10lg\n",
                              i,real(lambda[i]),imag(lambda[i]),i,abs(lambda[i])-1.0));
      editor->insertPlainText(QString::asprintf("  V[%d] \t= { ",i));
      for(int j=0; j<4; ++j){
   	editor->insertPlainText(QString::asprintf("%10lg + i%10lg", real(v[i][j]),imag(v[i][j])));
        if(i<3) {
	  editor->insertPlainText(", ");
	}
	else {
	  editor->insertPlainText(" }");
	}
      }//for  j
      editor->insertPlainText("\n");
    } //for  i 

    if(err)return;
    editor->insertPlainText("\n");
   }
  
   editor->insertPlainText("4D beta-functions:\n");

   editor->insertPlainText(QString::asprintf("  Beta_X_1[cm] \t= %10lg \tBeta_X_2[cm] \t= %10lg\n",vt.btx1, vt.btx2));
   editor->insertPlainText(QString::asprintf("  Alpha_X_1 \t= %10lg  \tAlpha_X_2 \t = %10lg\n",vt.alfx1, vt.alfx2));
   editor->insertPlainText(QString::asprintf("  Beta_Y_1[cm] \t= %10lg \tBeta_Y_2[cm] \t= %10lg\n",vt.bty1, vt.bty2));
   editor->insertPlainText(QString::asprintf("  Alpha_Y_1\t = %10lg \tAlpha_Y_2 \t= %10lg\n",vt.alfy1, vt.alfy2));
   editor->insertPlainText(QString::asprintf("  Nu_1[deg] \t= %10lg \tNu_2[deg] \t= %10lg \tU=%10lg\n",
                                             vt.teta1*180./PI, vt.teta2*180./PI, vt.u));

   editor->insertPlainText("\n");
   editor->insertPlainText("Dispersions:\n");
   
   editor->insertPlainText(QString::asprintf("  DispX[cm] \t= %10lg \tDispY[cm] \t= %10lg\n",vt.dx, vt.dy));
   editor->insertPlainText(QString::asprintf("  DispPrX \t= %10lg  \tDispPrY \t= %10lg\n",vt.dxp, vt.dyp));
   editor->insertPlainText("\nFractional part of tunes:\n");

   double qx =  lambda ?  -arg(lambda[0])/(2.*PI) : vt.qx;
   double qy =  lambda ?  -arg(lambda[2])/(2.*PI) : vt.qy;

   if ( qx< -1.0e-10) qx +=1.0;
   if ( qy< -1.0e-10) qy +=1.0;

   editor->insertPlainText(QString::asprintf("  Q1=%lg \tQ2=%lg \tQ1-Q2=%lg\n",qx, qy, qx-qy));


   // Tune split computations and output

   int er = 0;
   double btx = 0.0;
   double alx = 0.0;
   double bty = 0.0;
   double aly = 0.0;
   double a1  = 0.0;
   double a2  = 0.0;
   double tsplit = 0.0;
   
   btx  =  tm[0][0] - tm[1][1];
   btx  = 4.0 * tm[1][0]*tm[0][1] + btx*btx;
   if( btx == 0.0 ) { er=1;  goto tse;}

   btx  = 4.0 * tm[0][1]*tm[0][1] / btx;
   if( btx >= 0.0) { er=2;   goto tse;}
   btx  = sqrt(-btx);

   alx  = btx*(tm[0][0]-tm[1][1])/(2.*tm[0][1]);

   bty  = tm[2][2]-tm[3][3];
   bty  = 4.0*tm[3][2]*tm[2][3]+bty*bty;
   if (bty==0.0) { er=3; goto tse;}

   bty  = 4.*tm[2][3]*tm[2][3]/bty;
   if (bty >=0.0) { er=4; goto tse;}
   bty  = sqrt(-bty);

   aly  = bty*(tm[2][2]-tm[3][3])/(2.*tm[2][3]);

   a1     = tm[2][0]*sqrt(btx/bty) + tm[3][1]*sqrt(bty/btx) + tm[2][1]*(aly-alx)/sqrt(btx*bty);
   a2     = tm[3][0]*sqrt(btx*bty) - tm[2][1]*(1.0+alx*aly)/sqrt(bty*btx)+tm[2][0]*aly*sqrt(btx/bty) - tm[3][1]*alx*sqrt(bty/btx);

   tsplit = sqrt(a1*a1+a2*a2)/(2.*PI);

 tse:
   if(er) {
     editor->insertPlainText(bel);
     OptimMessageBox::warning(this,"4D View", "Cannot compute tune split.", QMessageBox::Ok);
   }
   else {
     editor->insertPlainText(QString::asprintf("\nTune split = %10.6g\n", tsplit));
   }

   double gamma = 1.0 + Ein/ms;
   double gm2   = gamma*gamma;
   
   double alfa = -( vt.dx * tm[4][0] + vt.dxp * tm[4][1] +
	     vt.dy * tm[4][2] + vt.dyp * tm[4][3] + (tm[4][5] - Length_/gm2) ) / Length_; // CHECK THIS !!!

   editor->insertPlainText(QString::asprintf("\nMomentum compaction= %g\n",alfa));

   if(CtSt_.CompAtExcitedOrb) {
     editor->insertPlainText(QString::asprintf("\nPath lengthening for excited orbit[cm] = %g\n",DeltaL_));
     editor->insertPlainText(QString::asprintf("\nInitial trajectory parameters for %s\n",  (CtSt_.IsRingCh ? "RING" : "LINE")));
     editor->insertPlainText(QString::asprintf("x \t= %10f cm \tteta_x \t= %10f mrad\n",  v_anlz.c[0], v_anlz.c[1]*1000.));
     editor->insertPlainText(QString::asprintf("y \t= %10f cm \tteta_y \t= %10f mrad\n",  v_anlz.c[2], v_anlz.c[3]*1000.));
     editor->insertPlainText(QString::asprintf("s \t= %10f cm \tDp/p*1000 \t= %10f\n",    v_anlz.c[4], v_anlz.c[5]*1000.));
   }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimMainWindow::Phases4D (Twiss4D& v)
{
  char err[256];

  int N = CtSt_.ArrayLen;
 
  std::vector<double> x(N+1); // possibly N samples + extra position for the end marker 
      
  std::vector<std::vector<double> > y(4);
  for (int i=0; i<4; ++i) {y[i] = std::vector<double>(N+ 1);}

  std::vector<LegoData> legodata;

  RMatrix tm;
  std::complex<double> ev[4][4];
     
  v.eigenvectors(ev);
  
  x[0]    = 0.0;
  y[0][0] = 0.0;
  y[1][0] = 0.0;
  y[2][0] = v.teta1/(2.*PI);
  y[3][0] = v.teta2/(2.*PI);

  double h     = Length_/N;
  double tetaY = tetaYo0_;
  double Enr   = Ein;
  double L     = 0.0;
  double Lp    = 0.0;
  int     k    = 1;

  for( int i=0; i<nelm_; ++i){
    auto ep = beamline_[i];
    char nm = ep->etype();
    int  ns = fabs(ep->length()/h)+1;
    if( nm=='A' || nm=='W' ||nm=='X'){ns=1;}

    auto e = std::shared_ptr<Element>(ep->split(ns) );

    legodata.push_back( { L*0.01, ep->length()*0.01, (ep->G>=0.0 ? 1 : -1), ep->fullName()});

    // calculate Element boxes located at the bottom of screan
    L     +=  ep->length();

    // calculate beta-functions
    double dalfa=0.0;
    for(int j=0; j<ns; j++){
      switch( nm ) {
	case 'B':  
        case 'D':
          tm = e->rmatrix(dalfa, Enr, ms, tetaY, dalfa, Element::checkEdge(j,ns));
	  dalfa -= e->tilt();
	  break;
	default:
          tm = e->rmatrix(Enr, ms, tetaY, 0.0, Element::checkEdge(j,ns));
      }

      e->propagateLatticeFunctions( tm, v, ev);

      Lp +=  e->length();

      // increment x  if the current sample is less  
      if(Lp  > Length_*k/(N-1) ){
          x[k]    =  Lp*.01;
	  
	  while(v.qx > 0.5 ) v.qx -= 0.5;
	  while(v.qy > 0.5 ) v.qy -= 0.5;
	  
	  while(v.qx < -0.5 ) v.qx += 0.5;
	  while(v.qy < -0.5 ) v.qy += 0.5;

	  y[0][k] =  v.qx; //  -arg(ev[0][0])/(2.*PI);
	  y[1][k] =  v.qy; //  -arg(ev[2][2])/(2.*PI);

	  y[2][k]   = v.teta1/(2.*PI);
	  y[3][k++] = v.teta2/(2.*PI);
      }
      if ( k == N) break;
    } // split element loop
    if ( k == N) break;
  } // main element loop

  // NOTE : just before the loop exit, k was incremented.   
  legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 
 
  PlotSpec plotspecs;
  plotspecs.title        = "Phases (4D)";
  plotspecs.bottom_title = "S [m]";

  auto& curvespecs = plotspecs.curvespecs;

    //-----------------------------------------------------------------------------------------------------------------------------------
    //                     legendname xv       yv    n  axis             title                          bottom_title         color 
    //------------------------------------------------------------------------------------------------------------------------------------  

    curvespecs.push_back({ "Q_1",     &x[0], &y[0][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Phase/(2*pi)", "" });  
    curvespecs.push_back({ "Q_2",     &x[0], &y[1][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "Phase/(2*pi)", "" });  
    curvespecs.push_back({ "Nu_1", &x[0], &y[2][0], k, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Phase/(2*pi)", "" });  
    curvespecs.push_back({ "Nu_2", &x[0], &y[3][0], k, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Phase/(2*pi)", "" });  
  
    addPlot(WindowId::Phase4Ch, plotspecs, legodata); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
void OptimMainWindow::Sizes4D(Twiss4D& v)
{

  char err[256];

  int N = CtSt_.ArrayLen;

  std::vector<double> x(N+1);
  
  std::vector< std::vector<double> > y(4);
  for (int i=0; i<4; ++i) {y[i] = std::vector<double>(N+ 1);}

  std::vector<LegoData> legodata;
  RMatrix tm;

  int brk = 0;

  std::complex<double> ev[4][4];
  v.eigenvectors(ev);

  
  x[0]=0.0;

  double x1 = sqrt(v.e1 * v.btx1);
  double x2 = sqrt(v.e2 * v.btx2);
  double y1 = sqrt(v.e1 * v.bty1);
  double y2 = sqrt(v.e2 * v.bty2);

  double xm = sqrt(x1*x1+x2*x2);
  double ym = sqrt(y1*y1+y2*y2);
  double alf =(x1*y1*cos(v.teta1) + x2*y2*cos(v.teta2))/(xm*ym);
  y[0][0] = (xm*ym)*sqrt(2.0*(1.0-alf*alf)/(xm*xm+ym*ym-sqrt(v.e2*v.e2+4.0*alf*alf*xm*xm*ym*ym)));
  y[1][0] = (xm*ym)*sqrt(2.0*(1.0-alf*alf)/(xm*xm+ym*ym+sqrt(v.e2*v.e2+4.0*alf*alf*xm*xm*ym*ym)));
  y[2][0] = (fabs(v.e2)<1.e-10) ? 90.0 : 90.0/PI*atan2(2.0*alf*xm*ym,v.e2);
  
  double h     = Length_/N;
  double tetaY = tetaYo0_;
  double Enr   = Ein;
  double L  = 0.0;
  double Lp = 0.0;  

  int k     = 1;
  for (int i = 0; i<nelm_; ++i) {
    if(brk) break;
    auto ep  = beamline_[i];
    char nm  = ep->etype();
    int  ns  = fabs(ep->length()/h) + 1;
    if( nm=='A' || nm=='W' ||nm=='X') {ns=1;}

    auto e = std::shared_ptr<Element>( ep->split(ns) );

    legodata.push_back( { L*0.01, ep->length()*0.01, (e->G>=0.0 ? 1:-1), ep->fullName() });

    // calculates Element's boxes located at the bottom of screan

    L    +=  ep->length();

    // calculate beta-functions

    double dalfa =0.0;
    for(int j=0; j<ns; ++j){

      switch(nm) {
        case 'B':  
        case 'D':
          tm = e->rmatrix(dalfa, Enr, ms, tetaY, dalfa, Element::checkEdge(j,ns));
	  dalfa -= e->tilt();
          break;
        default:
          tm = e->rmatrix(Enr, ms, tetaY, 0.0, Element::checkEdge(j,ns));
      }
      
      e->propagateLatticeFunctions( tm, v, ev);

      Lp +=  e->length();
   
      if (Lp > Length_*k/(N-1) ) {

        double capa = sqrt(Ein*(2.*ms+Ein)/(Enr*(2.*ms+Enr)));

	x[k] = Lp*0.01;

         x1   = sqrt( v.e1 * v.btx1);
         x2   = sqrt( v.e2 * v.btx2);
         y1   = sqrt( v.e1 * v.bty1);
         y2   = sqrt( v.e2 * v.bty2);
   
         xm  = sqrt(capa*(x1*x1+x2*x2));
         ym  = sqrt(capa*(y1*y1+y2*y2));
         alf = capa*(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2))/(xm*ym);
         double e2  = xm*xm-ym*ym;

         y[0][k] = xm*ym*sqrt(2.*(1-alf*alf)/(xm*xm+ym*ym-sqrt(e2*e2+4.*alf*alf*xm*xm*ym*ym))); 
         y[1][k] = xm*ym*sqrt(2.*(1-alf*alf)/(xm*xm+ym*ym+sqrt(e2*e2+4.*alf*alf*xm*xm*ym*ym)));
         y[2][k] = (fabs(e2)<1.0e-10)?  90.0 : 90.0/PI*atan2(2.0*alf*xm*ym,e2);

	 if(fabs(y[0][k])>100.0 || fabs(y[1][k])>100.0){
	    OptimMessageBox::warning( this, "View|Size", "Beam size is above threshold of 1 m ", QMessageBox::Ok);
            brk=1;
            break;
          }
       ++k;
      } // if (Lp*1.00001 > Length_*k/(N-1) ) 
      if (k == N) break; 
    }// split element loop 
    if (k == N) break; 
  } // main element loop
  
  int const n = k;
  
  PlotSpec plotspecs;
  plotspecs.title        = "Beam Sizes (4D)";
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         color  
  //------------------------------------------------------------------------------------------------------------------------------------  

  curvespecs.push_back({ "a",     &x[0], &y[0][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft, "Betatron Size [cm]",   "" });  
  curvespecs.push_back({ "b",     &x[0], &y[1][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft, "Betatron Size [cm]",   "" });  
  curvespecs.push_back({ "angle", &x[0], &y[2][0], n, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Angle [deg] [-90:90]", "" });  

  addPlot( WindowId::Size4Ch, plotspecs, legodata); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::Proj4D(Twiss4D& v)
{

  char err[256];
  int  N = CtSt_.ArrayLen;
 
  std::vector<double> x(N);
  
  std::vector<std::vector<double> > y(4);
  for (int i=0; i<4; ++i) {y[i] = std::vector<double>(N);} // N+ 1 ??? 

  std::vector<LegoData> legodata;

  RMatrix tm;
  std::complex<double> ev[4][4];

  std::vector<double>                 scraperS; 
  std::vector<std::vector<double> >   scraperPos(2);
  int nscrapers = 0;
  
  v.eigenvectors(ev);

  x[0] = 0.0;

  double x1 = sqrt(v.e1 * v.btx1);
  double x2 = sqrt(v.e2 * v.btx2);
  double y1 = sqrt(v.e1 * v.bty1);
  double y2 = sqrt(v.e2 * v.bty2);

  // are sizes computed correctly ?  FIXME !!!
  
  y[0][0] = sqrt(x1*x1+x2*x2 + (CtSt_.PlotTotalSize ? dpp_*v.dx*dpp_*v.dx : 0.0)  );
  y[1][0] = sqrt(y1*y1+y2*y2 + (CtSt_.PlotTotalSize ? dpp_*v.dy*dpp_*v.dy : 0.0) );
  y[2][0] = -(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2) + (CtSt_.PlotTotalSize ? dpp_*v.dx*dpp_*v.dy : 0.0))/(y[0][0]*y[1][0]);

  double h     = Length_/N;
  double tetaY = tetaYo0_;
  double Enr   = Ein;
  int     k    = 1;
  double L     = 0.0;
  double Lp    = 0.0;

  for(int i=0; i<nelm_; ++i){
    auto ep = beamline_[i];
    char nm = ep->etype();
    int  ns = fabs(ep->length()/h)+1;
    if( nm=='H') ++nscrapers;
    if( nm=='A' || nm=='W' ||nm=='X'){ns=1;}
 
    auto e = std::shared_ptr<Element>( ep->split(ns) );

    // calculates Element's boxes located at the bottom of screan

    legodata.push_back( { L*0.01, ep->length()*0.01, (ep->G>=0.0 ? 1 : -1), ep->fullName()});
    L    += ep->length();

    // calculates beta-functions

    double dalfa =0.0;

    for(int j=0; j<ns; ++j) {
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

      Lp +=  e->length();

      if (Lp > Length_*k/(N-1) ){

	double capa=sqrt(Ein*(2.*ms+Ein)/(Enr*(2.*ms+Enr)));
        double capaP    = dpp_*capa*(Enr+ms)/(Ein+ms);
        capaP    = capaP*capaP;
      
        x[k]    = Lp*0.01;
        x1      = sqrt(v.e1 * v.btx1);
	x2      = sqrt(v.e2 * v.btx2);
        y1      = sqrt(v.e1 * v.bty1);
	y2      = sqrt(v.e2 * v.bty2);
        y[0][k] = sqrt(capa*(x1*x1+x2*x2) + (CtSt_.PlotTotalSize ? capaP*v.dx*v.dx : 0.0 ) );
        y[1][k] = sqrt(capa*(y1*y1+y2*y2) + (CtSt_.PlotTotalSize ? capaP*v.dy*v.dy : 0.0 ) );
        y[2][k] = capa*(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2) + (CtSt_.PlotTotalSize ? capaP*v.dx*v.dy : 0.0) )/
                		        (y[0][k]*y[1][k]);
        ++k;
      }
    if (k == N) break; 
    } // split element loop
    if (k == N) break; 
  } // main element loop

  legodata.push_back( { L*0.01,  0,  0,   std::string("END") } ); 

  if( nscrapers > 0 ){
    scraperS.resize(nscrapers);
    scraperPos[0] = std::vector<double>(nscrapers);
    scraperPos[1] = std::vector<double>(nscrapers);

    for (int i=0; i<nscrapers; ++i) {
      scraperS[i] = scraperPos[0][i] =  scraperPos[1][i] = 0.0;  
    }				    

    L         = 0.0;
    nscrapers = 0;

    for( int i=0; i<nelm_; ++i) {
      auto ep = beamline_[i];
      char nm = ep->etype();

      if( nm == 'H' ) {
 	scraperS[nscrapers]                     = L*0.01;
        scraperPos[0][nscrapers]                = ep->S;
        scraperPos[1][nscrapers]   = (ep->N==2) ? ep->S : ep->A;
        ++nscrapers;
      }
      L +=  ep->length();
   } //for
  } // if 


  PlotSpec plotspecs;
  if (CtSt_.PlotTotalSize) {
    plotspecs.title        = " (4D) Total Size Projections (betatron + dispersion)";
  }
  else {
    plotspecs.title        = " (4D) Size Projections (betatron only)";
  }  
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  

  if ( (nscrapers > 0)  &&  (CtSt_.PlotApertures) ) {
    curvespecs.push_back({ "H-apert",  &scraperS[0], &scraperPos[0][0],   nscrapers, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "", "cyan" });  
    curvespecs.push_back({ "V-apert",  &scraperS[0], &scraperPos[1][0],   nscrapers, QwtSymbol::NoSymbol, QwtPlot::yLeft,   "", "magenta" });  
  }

  if (CtSt_.PlotTotalSize) {
    curvespecs.push_back({  "Ax_t", &x[0], &y[0][0], k, QwtSymbol::NoSymbol,QwtPlot::yLeft,  "Betatron + Disp Proj. [cm]",   "" });  
    curvespecs.push_back({  "Ay_t", &x[0], &y[1][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft, "Betatron + Disp Proj. [cm]",   "" });  
  }
  else {
    curvespecs.push_back({  "Ax", &x[0], &y[0][0], k, QwtSymbol::NoSymbol,QwtPlot::yLeft,  "Betatron  Proj. [cm]",         "" });  
    curvespecs.push_back({  "Ay", &x[0], &y[1][0], k, QwtSymbol::NoSymbol, QwtPlot::yLeft, "Betatron + Disp Proj. [cm]",   "" });
  }
  curvespecs.push_back({  "Alpha_xy",  &x[0], &y[2][0], k, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Alpha_xy [-1, +1]", "" });  
   
  if (CtSt_.PlotTotalSize) {
     addPlot( WindowId::Proj4TotCh, plotspecs, legodata);
  }
  else {
    addPlot( WindowId::Proj4Ch, plotspecs, legodata);
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++
#if 0
OLD VERSION 
void OptimMainWindow::Sizes4D(Twiss4D& v)
{

  char err[256];

  int N = CtSt_.ArrayLen;
 
  std::vector<double> x(N+1);
  
  std::vector< std::vector<double> > y(4);
  for (int i=0; i<4; ++i) {y[i] = std::vector<double>(N+ 1);}

  std::vector<LegoData> legodata;
  RMatrix tm;

  int brk = 0;
  std::complex<double> ev[4][4];

  v.eigenvectors(ev);

  x[0]=0.0;

  double x1 = sqrt(v.e1 * v.btx1);
  double x2 = sqrt(v.e2 * v.btx2);
  double y1 = sqrt(v.e1 * v.bty1);
  double y2 = sqrt(v.e2 * v.bty2);

  y[0][0] = sqrt(x1*x1+x2*x2 + (CtSt_.PlotTotalSize ? dpp_*v.dx*dpp_*v.dx*(dpp_*dpp_) : 0.0) );
  y[1][0] = sqrt(y1*y1+y2*y2 + (CtSt_.PlotTotalSize ? dpp_*v.dy*dpp_*v.dy*(dpp_*dpp_) : 0.0) );
  y[2][0] = -(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2) + (CtSt_.PlotTotalSize ? dpp_*v.dx*dpp_*v.dy*(dpp_*dpp_) : 0.0) ) /(y[0][0]*y[1][0]);

  
  
  double h     = Length_/N;
  double tetaY = tetaYo0_;
  double Enr   = Ein;
  int k     = 1;
  double L  = 0.0;
  double Lp = 0.0;  


  for (int i = 0; i<nelm_; ++i) {
    if(brk) break;
    auto ep  = beamline_[i];
    char nm  = ep->etype();
    int  ns  = fabs(ep->length()/h) + 1;
    if( nm=='A' || nm=='W' ||nm=='X'){ns=1;}

    auto e = std::shared_ptr<Element>( ep->split(ns) );

    legodata.push_back( { L*0.01, ep->length()*0.01, (e->G>=0.0 ? 1:-1), ep->fullName() });

    // calculates Element's boxes located at the bottom of screan

    L    +=  ep->length();
    double dalfa =0.0;

    // calculate beta-functions

    for(int j=0; j<ns; j++){

      switch(nm) {
        case 'B':  
        case 'D':
          tm = e->rmatrix(dalfa, Enr, ms, tetaY, dalfa, Element::checkEdge(j,ns));
	  dalfa -= e->tilt();
          break;
        default:
          tm = e->rmatrix(Enr, ms, tetaY, 0.0, Element::checkEdge(j,ns));
      }
      
      for(int i=0; i<4; ++i){
        ev[1][i] = {0.0, 0.0};
        ev[3][i] = {0.0, 0.0};

	for( int j=0; j<4; ++j){
          ev[1][i] += tm[i][j]*ev[0][j];
          ev[3][i] += tm[i][j]*ev[2][j];
	}
      }

      for( int i=0; i<4; ++i){
          ev[0][i] = ev[1][i];
          ev[2][i] = ev[3][i];
      }
    
      v.betatronFromEV(ev);
      v = tm * (Coordinates) v; // propagate dispersion (NOTE: the matrix product is overloaded)
       Lp +=  e->length();

      double capa = sqrt(Ein*(2.*ms+Ein)/(Enr*(2.*ms+Enr)));
      double capaP    = dpp_*capa*(Enr+ms)/(Ein+ms);
      capaP    = capaP*capaP;
      capa     = sqrt(capa);

      x[k] = Lp*0.01;

      x1   = sqrt( capa * v.e1 * v.btx1);
      x2   = sqrt( capa * v.e2 * v.btx2);
      y1   = sqrt( capa * v.e1 * v.bty1);
      y2   = sqrt( capa * v.e2 * v.bty2);

      double xm = sqrt( x1*x1+x2*x2 + (CtSt_.PlotTotalSize ? capaP*v.dx*v.dx*(dpp_*dpp_)  : 0.0 ) );
      double ym = sqrt( y1*y1+y2*y2 + (CtSt_.PlotTotalSize ? capaP*v.dy*v.dy*(dpp_*dpp_)  : 0.0 ) );
      double alf = (x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2) + (CtSt_.PlotTotalSize ? capaP*v.dx*v.dy*(dpp_*dpp_) : 0.0) )/(xm*ym);
      double e2  = xm*xm-ym*ym;

      if (Lp  > Length_*k/(N-1) ) {

	y[0][k] = xm*ym*sqrt(2.*(1-alf*alf)/(xm*xm+ym*ym-sqrt(e2*e2+4.*alf*alf*xm*xm*ym*ym))); 
        y[1][k] = xm*ym*sqrt(2.*(1-alf*alf)/(xm*xm+ym*ym+sqrt(e2*e2+4.*alf*alf*xm*xm*ym*ym)));

	y[2][k] = (fabs(e2)<1.e-10) ? 90.0 : (90.0/PI)*atan2(2.*alf*xm*ym,e2);

	if(fabs(y[0][k])>100.0 || fabs(y[1][k])>100.0){
	  OptimMessageBox::warning( this, "View|Size", "Beam size is above threshold of 1 m ", QMessageBox::Ok);
          brk=1;
          break;
        }
        ++k;
      }
    }
  }
  
  int const n = k;

  PlotSpec plotspecs;
  plotspecs.title        =  (CtSt_.PlotTotalSize ? "Total Beam Sizes (4D)" : "Beam Sizes (4D)");
  plotspecs.bottom_title = "S [m]";
  auto& curvespecs = plotspecs.curvespecs;

  //-----------------------------------------------------------------------------------------------------------------------------------
  //                     legendname xv       yv    n  axis             title                          bottom_title         vertical_title  
  //------------------------------------------------------------------------------------------------------------------------------------  

  curvespecs.push_back({ (CtSt_.PlotTotalSize ? "a_tot": "a"), &x[0], &y[0][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft, (CtSt_.PlotTotalSize ? "Betatron + Disp. Size [cm]" : "Betatron Size [cm]"),   "" });  
  curvespecs.push_back({ (CtSt_.PlotTotalSize ? "b_tot": "b"), &x[0], &y[1][0], n, QwtSymbol::NoSymbol, QwtPlot::yLeft, (CtSt_.PlotTotalSize ? "Betatron + Disp. Size [cm]" : "Betatron Size [cm]"),   "" });  
  curvespecs.push_back({ "angle", &x[0], &y[2][0], n, QwtSymbol::NoSymbol, QwtPlot::yRight,  "Angle [deg] [-90:90]", 0 });  

  addPlot( WindowId::Size4Ch, plotspecs, legodata); 
}
//++++++++++++++++++++++++++++++++++++++++++++++
#endif

