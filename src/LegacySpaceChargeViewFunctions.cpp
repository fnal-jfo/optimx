//  =================================================================
//
//  LegacySpaceChargeViewFunctions.cpp
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

#include <Element.h>
#include <Constants.h>
#include <OptimMdiSubWindow.h>
#include <OptimMainWindow.h>
#include <OptimTextEditor.h>
#include <OptimMessages.h>
#include <Utility.h>
#include <RMatrix.h>
#include <Twiss.h>

#include <QMessageBox>
#include <QPointer>

using Constants::PI;
using Constants::C_DERV1;

using Utility::filterName;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::ViewFunctionsSpCh ( char* filenm,  FunctionDlgStruct*  NStf )  // NEW
{

  BunchParam bunch;
  Twiss4D v;

  if ( spaceChargeInitTwiss4D(v, bunch) ) return 1; 

  // ... update global variables ... 

  strcpy(funcFilter, NStf->Filter);
  stepv         = NStf->Step;
  startFrom     = NStf->Start;
  stopAt        = NStf->Stop;
  printnames    = NStf->printname;
  NoFirstLetter = NStf->NoFirstLetter;
  MatchCase     = NStf->MatchCase;
  PrintEnerg    = NStf->PrintEnerg;
  StartSfrom0   = NStf->StartSfrom0;
   
   unsigned long lastline;
  
   char *pname;
   char buf[1024];
   char err[256]; err[0] = 0;
     
   char const fln1[] ={"%4s %16s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %6s %6s %6s %6s %6s"};
   char const fln2[] ={"%4s %5s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %6s %6s %6s %6s %6s"}; //  %1d %1d"};

   int i0    = (startFrom > 0)                   ? startFrom-1 : 0;
   int nelm0 = ((stopAt>(i0+1))&&(stopAt<nelm_)) ? stopAt      : nelm_;
  
   OptimTextEditor* editor = 0;
   
   QPointer<QMdiSubWindow> Dig4Ch;

   FILE *fp = 0;
   if (filenm) {
   fp = fopen(filenm,"w");
   if( !fp) {
       OptimCriticalMessage(this, "Error opening file for saving. File name -", filenm, QMessageBox::Ok);
       return 1;
     }
     if(printnames) fprintf(fp, fln1, "N", "NAME", "S[cm]", "BetaX1", "AlfaX1", "BetaY1", "AlfaY1", "Teta1/(2*PI)", "BetaX2", "AlfaX2", "BetaY2", "AlfaY2", "Teta2/(2*PI)", "U",
			              "DspX", "DspXp", "DspY", "DspYp", "NuX", "NuY", "Ax[cm]", "Ay[cm]", "AlphaXY");
     else fprintf(fp,fln2, "N", "S[cm]", "BetaX1", "AlfaX1", "BetaY1", "AlfaY1", "Teta1/(2*PI)", "BetaX2", "AlfaX2", "BetaY2", "AlfaY2", "Teta2/(2*PI)", "u",
  		           "DspX", "DspXp", "DspY", "DspYp", "NuX", "NuY", "Ax[cm]", "Ay[cm]", "AlphaXY");
   
     if(PrintEnerg)fprintf(fp,"\tEnerg[MeV]\n");  else fprintf(fp,"\n");
   } 
   else {
     Dig4Ch = getAttachedSubWin(WindowId::Dig4Ch);
     if (!Dig4Ch) { 
        Dig4Ch = createAttachedSubWin( (editor = new OptimTextEditor()), WindowId::Dig4Ch);
        connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
        connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
     }
     else { 
        editor = qobject_cast<OptimTextEditor*>(Dig4Ch->widget());
        if (!editor) return 1;
     } 
     Dig4Ch->raise();
     Dig4Ch->show();

     lastline = editor->document()->lineCount();
     if(printnames) sprintf(buf,fln1, "#  N", "NAME", "S[cm]", "BetaX1", "AlfaX1", "BetaY1", "AlfaY1", "Teta1/(2*PI)", "BetaX2", "AlfaX2", "BetaY2", "AlfaY2", "Teta2/(2*PI)", "U",
			              "DspX", "DspXp", "DspY", "DspYp", "NuX", "NuY", "Ax[cm]", "Ay[cm]", "AlphaXY" );
     else sprintf(buf,fln2, "#  N", "S[cm]", "BetaX1", "AlfaX1", "BetaY1", "AlfaY1",
		       "Teta1/(2*PI)", "BetaX2", "AlfaX2", "BetaY2", "AlfaY2", "Teta2/(2*PI)", "u",
		       "DspX", "DspXp", "DspY", "DspYp", "NuX", "NuY", "Ax[cm]", "Ay[cm]", "AlphaXY");
     if(PrintEnerg) { strcat(buf, "\tEnergy");}
     editor->appendPlainText(buf);
   }

   // char const formatn[]={"%4d\t%s\t%6.2f\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f"};
   // char const format[] ={"%4d\t%5.1f\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f %1d %1d"};

   char const formatn[]={"%4d %16s %12.2f %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %6.4f %6.4f %6.4f %6.4f %6.4f"};
   char const format[] ={"%4d %5.1f %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %6.4f %6.4f %6.4f %6.4f %6.4f %1d %1d"};

   //------------------------------------------------------------------------
   //  **** the computation starts here. v contains the 4D lattice functions.  
   //------------------------------------------------------------------------
   
   RMatrix tm, tmf;
   BeamSize bs;
   std::complex<double> lambda[4], ev[4][4];
  
   tmf.toUnity();
   
   v.eigenvectors(ev);  
   
   double x1   = sqrt(v.e1 * v.btx1);
   double x2   = sqrt(v.e2 * v.btx2);

   double y1   = sqrt(v.e1 * v.bty1);
   double y2   = sqrt(v.e2 * v.bty2);

   bs.a     = sqrt(x1*x1+x2*x2);
   bs.b     = sqrt(y1*y1+y2*y2);
   bs.alpha = -(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2))/(bs.a*bs.a);

   double  Lp = (StartSfrom0) ? 0.0  : so0_;
   v.qx = 0.0;
   v.qy = 0.0;

   if(printnames) {
     sprintf(buf,formatn, 0, "START", Lp,
             v.btx1, v.alfx1, v.bty1, v.alfy1, v.teta1/(2.*PI),
             v.btx2, v.alfx2, v.bty2, v.alfy2, v.teta2/(2.*PI),
             v.u, v.dx, v.dxp, v.dy, v.dyp, v.qx, v.qy, bs.a, bs.b, bs.alpha);
   } 
   else {
     sprintf(buf,format, 0, Lp,
             v.btx1, v.alfx1, v.bty1, v.alfy1, v.teta1/(2.*PI),
             v.btx2, v.alfx2, v.bty2, v.alfy2, v.teta2/(2.*PI),
             v.u, v.dx, v.dxp, v.dy, v.dyp, v.qx, v.qy, bs.a, bs.b, bs.alpha, 0, 0);
   }
   if(PrintEnerg)sprintf(&buf[strlen(buf)], "\t%g", Ein);
   if (fp) { 
     fprintf(fp,"%s \n", buf); 
   }
   else {
     editor->appendPlainText(buf); // editor->AddLine(&editBuffer,&buflen,peb,&iii,buf,"");
   } 

   double tetaY = tetaYo0_;
   double Enr   = Ein;

   int    jp = 0;
   int    jm = 0;

   for( int i=0; i<nelm0; ++i){
     auto ep = beamline_[i];
     pname = (NoFirstLetter) ? &ep->fullname[1] : ep->fullname;
     char nm = toupper(ep->name[0]);

     int ns = ((space_charge_step_ <stepv) || (stepv <=0.)) ? fabs(ep->L/space_charge_step_)+1 : fabs(ep->L/stepv)+1;

     double DL = 0.0;
     if( nm=='A' || nm=='W' ||nm=='X') { ns=1;}

     auto e = std::unique_ptr<Element>( ep->split(ns) );     

     switch (nm){
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
         jp  = 1; 
         jm = -1;	 
         break;
       default: 
         jp = 0;
	 jm = 0;
     }
     double dalfa=0.0;

     for(int j=0; j<ns; ++j){

       switch( toupper(nm) ){
	 case 'B':  
         case 'D':
           dalfa = GetCWSpaceChargeMatr(Enr, ms, bunch.I, bs, e.get(), tetaY, dalfa, tm, Element::checkEdge(j,ns)) - e->T;
	   break;
	 default:
           GetCWSpaceChargeMatr(Enr, ms, bunch.I, bs, e.get(), tetaY, 0., tm, Element::checkEdge(j,ns));
        }

       tmf = tm*tmf;

       for(int i=0; i<4; ++i){
          ev[1][i] = std::complex<double>(0.0, 0.0);
          ev[3][i] = std::complex<double>(0.0, 0.0);
          for(int j=0; j<4; ++j){
            ev[1][i] += tm[i][j]*ev[0][j];  
 	    ev[3][i] += tm[i][j]*ev[2][j];
          }
        }
 
       double s = -arg(ev[1][0]/ev[0][0])/(2.*PI);    if (s<-1.e-10) s += 1.0;  
       v.qx += s;

       s = -arg(ev[3][2]/ev[2][2])/(2.*PI);  if (s<-1.e-10) s += 1.0;
       v.qy += s;

       for(int i=0; i<4; ++i){
         ev[0][i] = ev[1][i];
         ev[2][i] = ev[3][i];
       }


       v.betatronFromEV(ev);
        v = tm * (Coordinates) v;
     
        double capa     = Ein*(2.*ms+Ein)/(Enr*(2.*ms+Enr));
        double capaP    = dpp*capa*(Enr+ms)/(Ein+ms);

        capaP    = capaP*capaP;
        capa     = sqrt(capa);
       
        x1       = sqrt(capa * v.e1 * v.btx1);
        x2       = sqrt(capa * v.e2 * v.btx2);
        y1       = sqrt(capa * v.e1 * v.bty1);
        y2       = sqrt(capa * v.e2 * v.bty2);

        // bs = beamsize  
        bs.a     = sqrt(x1*x1+x2*x2+capaP*v.dx*v.dx);
        bs.b     = sqrt(y1*y1+y2*y2+capaP*v.dy*v.dy);
        bs.alpha =-(x1*y1*cos(v.teta1)+x2*y2*cos(v.teta2)+capaP*v.dx*v.dy)/(bs.a*bs.b);

        Lp +=  e->L;
        DL +=  e->L;

        if( (stepv <= 0.) && (j<ns-1) )                continue;
        if( (stepv >  0.) && (j<ns-1) && ( DL<stepv) ) continue;

        DL = 0.0;


       if( (i>=i0) && filterName( e->fullname, funcFilter, MatchCase) ){

       if(printnames) {
       sprintf(buf, formatn, i+1, pname, Lp, v.btx1, v.alfx1, v.bty1, v.alfy1, v.teta1/(2.*PI),
       		               v.btx2, v.alfx2, v.bty2, v.alfy2, v.teta2/(2.*PI), v.u,
                                v.dx, v.dxp, v.dy, v.dyp, v.qx, v.qy, bs.a, bs.b, bs.alpha);
       } 
       else {
         sprintf(buf,format,i+1,Lp,v.btx1,v.alfx1,v.bty1,v.alfy1,v.teta1/(2.*PI),
       		            v.btx2,v.alfx2,v.bty2,v.alfy2,v.teta2/(2.*PI),v.u,
                            v.dx,v.dxp,v.dy,v.dyp,v.qx,v.qy, bs.a, bs.b, bs.alpha, jp,jm);
       }
       if(PrintEnerg) sprintf(&buf[strlen(buf)], "\t%g", Enr);

       if(fp) { 
           fprintf(fp,"%s \n", buf);
       } 
       else {
	 editor->appendPlainText(buf); //  AddLine(&editBuffer,&buflen,peb,&iii,buf,"");
       }

     }
    }
   }

   if(fp){ 
     OptimInformationMessage( this, "File saved.", filenm, QMessageBox::Ok);
     fclose(fp);
   }
   else {
     //editor->UnlockBuffer(editBuffer, true);
     if(lastline>3){
     {} ///i=editor->GetLineIndex(lastline-1);
	 ///editor->DeleteSubText(i-2,i);
	 ///editor->insertPlainText(bel);
     }
     editor->document()->setModified(false);
   }

 return 0;
}
