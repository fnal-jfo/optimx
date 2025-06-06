//  =================================================================
//
//  ViewLatticeTable.cpp
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
#include <Constants.h>
#include <GeneralPreferencesDialog.h>
#include <Element.h>
#include <Globals.h>
#include <RMatrix.h>
#include <Twiss.h>
#include <OptimApp.h>
#include <OptimMainWindow.h>
#include <OptimMdiSubWindow.h>
#include <OptimMessages.h>
#include <Utility.h>

#include <QtGlobal>
#include <QAction>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMap>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QStatusBar>
#include <QStyleFactory>
#include <QTableWidget>
#include <QtGui>


#include <iostream>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <fmt/format.h>

static const unsigned int LSTR = 1024;

using std::acosh;

using Constants::C_DERV1; 
using Constants::PI; 
using Constants::C_CGS;
using Constants::C_DERV3;
using Constants::KVSR;
using Constants::KE;

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

static int ierr = 0; // error code 


using std::acosh;

using Constants::PI;
using Constants::C_DERV1;
using Constants::C_DERV3;


void OptimMainWindow::cmdViewLatticeTable()
{

  char buf[LSTR+1], buf1[LSTR+1] ; // what is buf1 ??? 
  char const* pname;
  int nln;
  double SL = 0.0;

  if(interrupted_ ) {interrupted_ = false; return;}
  if(analyze(!analyzed_)) return;

  static LatticeDialog* dialog = 0;

  if (!dialog) {
    dialog = new LatticeDialog(0);
    dialog->data_  = NStLat_;
    dialog->set();
   }

  if ( dialog->exec() == QDialog::Rejected) return; 
  NStLat_ = dialog->data_;
    
  QTableWidget* table = 0;
  
  auto DigCh = getAttachedSubWin(WindowId(9999)); // FIXME 
  if (!DigCh) { 
    //DigCh = createAttachedSubWin( (table = new QTableWidget(0), );
    DigCh = createAttachedSubWin(  (table = new QTableWidget(0)), WindowId(9999) );
    //connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
    //connect(editor, SIGNAL(undoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
    // connect(editor, SIGNAL(redoAvailable(bool)), this, SLOT(updateEditMenuState(bool)) );
  }
  else { 
     table = qobject_cast<QTableWidget*>(  DigCh->widget() );  
     if (CtSt_.ClearText) table->clear();
  }

  DigCh->raise();
  if (!table)return;

  table->setRowCount(nelm_+10);

  table->setColumnCount(19);
  table->setHorizontalHeaderItem(1, new QTableWidgetItem("index"));
  table->setHorizontalHeaderLabels({"Type", "Name", "S[cm]", "L[cm]", "B[kG]", "G[kG/cm]", "S[kG/cm**2]", "Tilt[deg]",
	"Tilt_out", "BendAng[deg]", "D_E[MeV]", "E_out[MeV]", "angle", "Eff Len", "Shape", "Ax", "Ay", "OffX", "OffY"});

  int irow = 0; // table row idx

  double tetaY = tetaYo0_;

  double    Enr;
  RMatrix   tm;

  
  if (NStLat_.printBdL) {    
  
    std::string names[nelm_];
  
    nln=0;
    for( int i=0; i<nelm_; ++i) {
      if( PrintBdL(i, names, nln, buf)) continue;
      nln++;
      //editor->append(buf);

    }
    goto endlat;
  }

  double alfa_exit, EnOld, Hrt, fi;  
  
  //editor->sprintf(buf, "#N \tName \tS[cm] \tL[cm] \tB[kG] \tG[kG/cm]   S[kG/cm/cm] \tTilt[deg] \tTilt_out \tBendAng[deg]");

  Enr       = Ein;

  for(int i=0; i<nelm_; ++i){
    auto ep    = beamline_[i];
    SL       += ep->length(); 
    EnOld     = Enr;
    Hrt       = sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;
    tm = ep->rmatrix(alfa_exit, Enr, ms, tetaY, 0.0, 3);

    pname = (NStLat_.printFirstLetter) ? ep->fullName():  &ep->fullName()[1];  
    QString type(ep->str().c_str());
    
    if(!Utility::filterName(ep->fullName(), NStLat_.Filter, NStLat_.MatchCase)) continue;
 
    if ( ep->etype() =='M' ) {
       if(  ep->N > NStLat_.MltpOrder ) continue;
    }

    sprintf(buf1,"%d \t%s \t%g \t%g ", i+1, pname, (NStLat_.StartSFromFile ? SL+so0_ : SL), ep->length());

    table->setItem(irow, 0, new QTableWidgetItem(type) ); 
    table->setItem(irow, 1, new QTableWidgetItem(pname) ); 
    table->setItem(irow, 2, new QTableWidgetItem( QString("%1").arg(NStLat_.StartSFromFile ? SL+so0_ : SL)  ) ); 
    table->setItem(irow, 3, new QTableWidgetItem( QString("%1").arg(ep->length()) ) ); 


    
    switch (ep->etype() ){
     case 'M':
       sprintf(buf, "  Order:m=%d   Bm*L[kG/cm**(m-1)]=%lg   Tilt[deg]=%lg", ep->N, ep->S, ep->tilt());
       break;
     case 'I':
       sprintf(buf, "  \t%g \t%g \t%g \t%g", 0., 0., 0., ep->tilt());
       table->setItem(irow, 4, new QTableWidgetItem( QString("%1").arg(0.0) ));
       table->setItem(irow, 5, new QTableWidgetItem( QString("%1").arg(0.0) ));
       table->setItem(irow, 6, new QTableWidgetItem( QString("%1").arg(0.0) ));
       table->setItem(irow, 7, new QTableWidgetItem( QString("%1").arg(ep->tilt()) ));
       break;
     case 'O':
       sprintf(buf," ");
       break;
     case 'H':
       switch(ep->N){
         case 1:
           sprintf(buf," Ellipse:   Ax[cm]=%g  Ay[cm]=%g  OffsetX[cm]=%g OffsetY[cm]=%g  Tilt[deg]=%g",
                  			ep->S, ep->A, ep->B, ep->G, ep->tilt());
	   table->setItem(irow, 7,  new QTableWidgetItem( QString("%1").arg(ep->tilt()) ));
	   table->setItem(irow, 14, new QTableWidgetItem( QString("%1").arg("ellipse") ));
	   table->setItem(irow, 15, new QTableWidgetItem( QString("%1").arg(ep->A) ));
	   table->setItem(irow, 16, new QTableWidgetItem( QString("%1").arg(ep->B) ));
	   table->setItem(irow, 17, new QTableWidgetItem( QString("%1").arg(ep->A) ));
	   table->setItem(irow, 18, new QTableWidgetItem( QString("%1").arg(ep->B) ));
	    
           break;
         case 2:
           sprintf(buf," Hyperbola: Ax[cm]=%g  Ay[cm]=%g  OffsetX[cm]=%g OffsetY[cm]=%g  Tilt[deg]=%g",
                  			ep->S, ep->A, ep->B, ep->G, ep->tilt());
	   table->setItem(irow, 7,  new QTableWidgetItem( QString("%1").arg(ep->tilt()) ));
	   table->setItem(irow, 14, new QTableWidgetItem( QString("%1").arg("hyperbola") ));
	   table->setItem(irow, 15, new QTableWidgetItem( QString("%1").arg(ep->A) ));
	   table->setItem(irow, 16, new QTableWidgetItem( QString("%1").arg(ep->B) ));
	   table->setItem(irow, 17, new QTableWidgetItem( QString("%1").arg(ep->A) ));
	   table->setItem(irow, 18, new QTableWidgetItem( QString("%1").arg(ep->B) ));
           break;
         default:
           sprintf(buf," Rectang.   Ax[cm]=%g  Ay[cm]=%g  OffsetX[cm]=%g OffsetY[cm]=%g  Tilt[deg]=%g",
                  			ep->S, ep->A, ep->B, ep->G, ep->tilt());
	   table->setItem(irow, 7,  new QTableWidgetItem( QString("%1").arg(ep->tilt()) ));
	   table->setItem(irow, 14, new QTableWidgetItem( QString("%1").arg("rectangle") ));
	   table->setItem(irow, 15, new QTableWidgetItem( QString("%1").arg(ep->A) ));
	   table->setItem(irow, 16, new QTableWidgetItem( QString("%1").arg(ep->B) ));
	   table->setItem(irow, 17, new QTableWidgetItem( QString("%1").arg(ep->A) ));
	   table->setItem(irow, 18, new QTableWidgetItem( QString("%1").arg(ep->B) ));
           break;
        }
	break;
      case 'A':
	sprintf(buf,"  D_E[MeV]=%g  E_out[MeV]=%g", ep->G*cos(PI/180.*ep->S), Enr);
	table->setItem(irow, 10, new QTableWidgetItem( QString("%1").arg(ep->G*cos(PI/180.*ep->S)) ));
	table->setItem(irow, 11, new QTableWidgetItem( QString("%1").arg(Enr) ));
	break;
      case 'W':
	sprintf(buf,"  D_E[MeV]=%g  E_out[MeV]=%g", Enr-EnOld, Enr);
	table->setItem(irow, 10, new QTableWidgetItem( QString("%1").arg(Enr-EnOld) ));
	table->setItem(irow, 11, new QTableWidgetItem( QString("%1").arg(Enr) ));
	break;
      case 'Z':
	sprintf(buf,"  D_E[MeV]=%g  ", ep->G);
	table->setItem(irow, 10, new QTableWidgetItem( QString("%1").arg(ep->G) ));
	break;
      case 'E':
	sprintf(buf,"  D_E[MeV]=%g  E_out[MeV]=%g", ep->B, Enr);
	table->setItem(irow, 10, new QTableWidgetItem( QString("%1").arg(ep->B) ));
	break;
      case 'G':
	sprintf(buf,"\t%g  Angle[deg]=%g  Eff.Length[cm]=%g  Tilt[deg]=%g",
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
	table->setItem(irow, 4, new QTableWidgetItem( QString("%1").arg(ep->B) ));
        table->setItem(irow, 5, new QTableWidgetItem( QString("%1").arg(ep->G) )); 
        table->setItem(irow, 6, new QTableWidgetItem( QString("%1").arg(ep->S) )); 
        table->setItem(irow, 7, new QTableWidgetItem( QString("%1").arg(ep->tilt()) ));
	table->setItem(irow, 8, new QTableWidgetItem( QString("%1").arg(alfa_exit) ));
	table->setItem(irow, 9, new QTableWidgetItem( QString("%1").arg(fi)    )); 
	break;
      case 'T':
	sprintf(buf, "\tRms angle[mrad]=%lg  \t1/L*dL/dx[1/cm]=%lg \tTilt[deg]=%lg", ep->B,   ep->G, ep->tilt());
	break;
      case 'U':
	sprintf(buf, "\tRms Loss Spread[MeV]=%lg  \t1/L*dL/dx[1/cm]=%lg \tTilt[deg]=%lg", ep->B,   ep->G, ep->tilt());
	break;
      default:
	sprintf(buf, "\t%g \t%g \t%g \t%g", ep->B, ep->G, ep->S, ep->tilt());
	table->setItem(irow, 4, new QTableWidgetItem( QString("%1").arg(ep->B) ));
        table->setItem(irow, 5, new QTableWidgetItem( QString("%1").arg(ep->G) )); 
        table->setItem(irow, 6, new QTableWidgetItem( QString("%1").arg(ep->S) )); 
        table->setItem(irow, 7, new QTableWidgetItem( QString("%1").arg(ep->tilt()) )); 

    } // switch

    //editor->append(QString(buf1)+QString(buf));

    ++irow;
  }
  sprintf(buf, " Total Length for one period = %g cm", SL);
  //editor->append(buf);
    
  sprintf(buf, " Final Energy = %g MeV", Enr);
  //editor->append(buf);
    
  sprintf(buf, " Total Length for %d period = %g cm",NmbPer, NmbPer*SL);
  //editor->append(buf);

  table->show();

 endlat:return;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

