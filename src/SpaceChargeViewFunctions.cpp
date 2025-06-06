//  =================================================================
//
//  SpaceChargeViewFunctions.cpp
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
#include <fmt/format.h>

#include <Element.h>
#include <Constants.h>
#include <OptimMdiSubWindow.h>
#include <OptimMainWindow.h>
#include <OptimTextEditor.h>
#include <OptimMessages.h>
#include <Utility.h>
#include <RMatrix.h>
#include <Twiss.h>
#include <QGuiApplication>

#include <QMessageBox>
#include <QPointer>

using Constants::PI;
using Constants::C_DERV1;

using Utility::filterName;

using fmt::format_to;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::tabulateFunctionsSpCh ( char* filenm,  FunctionDlgStruct*  NStf )  
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
   
   char const* pname;
     
   char const fln1[] ={"{:>4s} {:>16s} {:>12s} {:>12s} {:>12s} "
		       "{:>12s} {:>12s} {:>12s} {:>12s} {:>12s} "
		       "{:>12s} {:>12s} {:>12s} {:>12s} {:>12s} "
		       "{:>12s} {:>12s} {:>12s} {:>8s} {:>8s} "
		       "{:>8s} {:>8s} {:>8s}"}; // 23 fields
   
   char const fln2[] ={"{:>4s} {:>12s} {:>12s} {:>12s} {:>12s} "
		       "{:>12s} {:>12s} {:>12s} {:>12s} {:>12s} "
		       "{:>12s} {:>12s} {:>12s} {:>12s} {:>12s} "
		       "{:>12s} {:>12s} {:>8s} {:>8s} {:>8s} "
		       "{:>8s} {:>8s}"}; //22 fields   %1d %1d"};

   int i0    = (startFrom > 0)                   ? startFrom-1 : 0;
   int nelm0 = ((stopAt>(i0+1))&&(stopAt<nelm_)) ? stopAt      : nelm_;
  
   OptimTextEditor* editor = 0;
   
   QPointer<QMdiSubWindow> Dig4Ch;

   auto outbufRaw = fmt::memory_buffer();
#if FMT_VERSION < 80000
  auto& outbuf    = outbufRaw;
#else
  auto outbuf    = std::back_inserter(outbufRaw);
#endif
  
   FILE *fp = 0;
   if (filenm) {
     fp = fopen(filenm,"w");
     if( !fp) {
       OptimCriticalMessage(this, "Error opening file for saving. File name -", filenm, QMessageBox::Ok);
       return 1;
     }
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

     //lastline = editor->document()->lineCount();
   }

  if(CtSt_.ClearText) { 
     editor->clear();
  }
  else{ 
    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    cursor.insertBlock();
  }

  Dig4Ch->raise();
  Dig4Ch->show();

  if(printnames) {
      format_to(outbuf,fln1, "#  N", "NAME", "S[cm]", "BetaX1[cm]", "AlphaX1",
		             "BetaY1[cm]", "AlphaY1", "Nu1/(2*PI)", "BetaX2[cm]", "AlphaX2",
		             "BetaY2[cm]", "AlphaY2", "Nu2/(2*PI)", "U", "DspX[cm]",
		             "DspXp", "DspY[cm]", "DspYp[cm]", "Q1", "Q2",
		             "Ax[cm]", "Ay[cm]", "AlphaXY"); // 23 fields

   }
   else {
       format_to(outbuf,fln2, "#  N", "S[cm]", "BetaX1[cm]", "AlphaX1", "BetaY1[cm]",
		              "AlphaY1", "Nu1/(2*PI)", "BetaX2[cm]", "AlphaX2", "BetaY2[cm]",
		              "AlphaY2", "Nu2/(2*PI)", "U", "DspX[cm]", "DspXp",
		              "DspY[cm]", "DspYp", "Q1", "Q2", "Ax[cm]",
		              "Ay[cm]", "AlphaXY"); // 22 fields
     
   }

   if (PrintEnerg) {
     format_to(outbuf, " {:>12s}\n", "Energy[MeV]");
   }
   else {
       format_to(outbuf, "\n");
   }

   const char formatn[] = "{: 4d} {:>16s} {: 12.2f} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} "
                          "{: 8.4f} {: 8.4f} {: 8.4f} {: 8.4f} {: 8.4f}";
   const char  format[] = "{: 4d} {: 12.2f} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} {: 12g} "
                          "{: 8.4f} {: 8.4f} {: 8.4f} {: 8.4f} {: 8.4f}";

   //------------------------------------------------------------------------
   //  **** the computation starts here. v contains the 4D lattice functions.  
   //------------------------------------------------------------------------

   auto restore = [](int* p){ QGuiApplication::restoreOverrideCursor(); delete p;};
   std::unique_ptr<int,decltype(restore)> cursor_guard(new int, restore); 

   QGuiApplication::setOverrideCursor(Qt::WaitCursor);

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
     format_to(outbuf,formatn, 0, "START", Lp,
             v.btx1, v.alfx1, v.bty1, v.alfy1, v.teta1/(2.*PI),
             v.btx2, v.alfx2, v.bty2, v.alfy2, v.teta2/(2.*PI),
	       v.u, v.dx, v.dxp, v.dy, v.dyp, v.qx, v.qy, bs.a, bs.b, bs.alpha, Ein);
   } 
   else {
     format_to(outbuf,format, 0, Lp,
             v.btx1, v.alfx1, v.bty1, v.alfy1, v.teta1/(2.*PI),
             v.btx2, v.alfx2, v.bty2, v.alfy2, v.teta2/(2.*PI),
	       v.u, v.dx, v.dxp, v.dy, v.dyp, v.qx, v.qy, bs.a, bs.b, bs.alpha, Ein );
   }

   if (PrintEnerg) {
      format_to(outbuf, " {:>12g}\n", Ein);
   }
   else {
      format_to(outbuf, "\n");
   }
   
   double tetaY = tetaYo0_;
   double Enr   = Ein;

   for( int i=0; i<nelm0; ++i){

     auto ep = beamline_[i];
     pname = (NoFirstLetter) ? &ep->fullName()[1] : ep->fullName();
     char nm = toupper(ep->name()[0]);

     int ns = ((space_charge_step_ <stepv) || (stepv <=0.)) ? fabs(ep->length()/space_charge_step_)+1 : fabs(ep->length()/stepv)+1;

     double DL = 0.0;
     if( nm=='A' || nm=='W' ||nm=='X') { ns=1;}

     auto e = std::unique_ptr<Element>( ep->split(ns) );     

     double dalfa =0.0; //  bend angle step ?? 
     double alfap =0.0; //  integrated bend angle ??

     for(int j=0; j<ns; ++j) {

       int st = Element::checkEdge(j,ns);
       RMatrix tm = e->rmatrixsc( alfap, Enr, ms, bunch.I, bs, tetaY, dalfa, st );

       switch( toupper(nm) ){
	 case 'B':  
         case 'D':
           dalfa = alfap - e->tilt();
	   break;
	 default:
	   break;
       };

       e->propagateLatticeFunctions( tm, v, ev);
        tmf = tm*tmf; 
	 
        double capa     = Ein*(2.*ms+Ein)/(Enr*(2.*ms+Enr));
        double capaP    = dpp_*capa*(Enr+ms)/(Ein+ms);

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

        Lp +=  e->length();
        DL +=  e->length();

        if( (stepv <= 0.) && (j<ns-1) )                continue;
        if( (stepv >  0.) && (j<ns-1) && ( DL<stepv) ) continue;

        DL = 0.0;


       if( (i>=i0) && filterName( e->fullName(), funcFilter, MatchCase) ) {

         if(printnames) {
	   format_to(outbuf, formatn, i+1, pname, Lp, v.btx1, v.alfx1, v.bty1, v.alfy1, v.teta1/(2.*PI),
       		               v.btx2, v.alfx2, v.bty2, v.alfy2, v.teta2/(2.*PI), v.u,
		     v.dx, v.dxp, v.dy, v.dyp, v.qx, v.qy, bs.a, bs.b, bs.alpha, Enr);
         } 
         else {
	   format_to(outbuf,format,i+1,Lp,v.btx1,v.alfx1,v.bty1,v.alfy1,v.teta1/(2.*PI),
       		            v.btx2,v.alfx2,v.bty2,v.alfy2,v.teta2/(2.*PI),v.u,
		     v.dx,v.dxp,v.dy,v.dyp,v.qx,v.qy, bs.a, bs.b, bs.alpha, Enr );
         }
	 if (PrintEnerg) {
            format_to(outbuf, " {:>12g}\n", Enr);
         }
	 else {
           format_to(outbuf, "\n");
         } 
       }
     }
   }

 
   format_to(outbuf, "{:c}", '\0'); // fmt library does not automatically add a zero terminator to raw data 

   if(fp) {
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

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
