//  =================================================================
//
//  OptimFileMenu.cpp
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

//#include <Constants.h>
#include <QtGui>
#include <QPixmap>
#include <QFontDialog>

#include <OptimMainWindow.h>
#include <OptimEditor.h>

#include <regex>
#include <cfloat>
#include <memory>
#include <string>
#include <vector>

#include <Constants.h>
#include <Utility.h>
#include <OptimCalc.h>
#include <OptimPlot.h>
#include <TuneDiagramDialog.h>
#include <ControlDialog.h>
#include <MatrixDialog.h>

#include <QwtLegend>


static unsigned int LSTR = 1024;
using Constants::PI; 
using Constants::C_CGS;
using Constants::C_DERV1; 
using Constants::C_DERV3;
using Constants::KVSR;
using Constants::KE;

using Utility::gauss; 

#define ARG_LEN 10
#define DATA_LEN 9

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdFileExecute ()
{
   char buf[256], fn[256], *bp, arglist[ARG_LEN][80], *varg[ARG_LEN+1];
   int nmblines, nline, i;
   FILE *fp;
   
   OEditFile* edclient = TYPESAFE_DOWNCAST(this->GetClientWindow(), OEditFile);
   
   nmblines=edclient->GetNumLines();
   bp=getenv("os");
   
   // Windows NT
   optiMDIClient* prnt = TYPESAFE_DOWNCAST(myparent, optiMDIClient);
   if(bp) {
     if(prnt->strcmpr("Windows_NT",bp)){
       optiApp *theApp = TYPESAFE_DOWNCAST(GetApplication(), optiApp);
       theApp->GetModuleFileName(fn, 255);
     
       for(i=strlen(fn); i>0; i--) {
	  if((fn[i]=='\\')||(fn[i]==':')) break;
       }
       fn[i+1]=0;
       strcat(fn,"tmp.bat");
       fp=fopen(fn,"w");
       fprintf(fp,"title \"OptiM Command Window\"\n");
       for(nline=0; nline<nmblines; nline++){
	  edclient->GetLine(buf, 255, nline);
	  if( buf[0] == '#') continue;
	  bp=buf; 
          while(*bp!=0){if(*bp=='\n')*bp=0; *bp++;}
    	  for(i=0; i<ARG_LEN; i++) varg[i]=arglist[i];
	  if(split(buf,ARG_LEN,varg) ==0 ) continue;
          fprintf(fp,"%s\n",buf);
       }
       fclose(fp);
       sprintf(buf,"cmd /t:b0 /k \"%s\"", fn);
       for(i=0; i<ARG_LEN; i++)varg[i]=arglist[i];
       if(split(buf,ARG_LEN,varg)==0) return;
       spawnvp(P_NOWAIT,varg[0],varg);
       return;
     }
   }
    
   // Windows 95
	 
   for (nline=0; nline<nmblines; nline++){
     edclient->GetLine(buf, 255, nline);
     if( buf[0] == '#') continue;
     bp = buf; 
     while(*bp!=0) {if(*bp=='\n')*bp=0; *bp++;}
     for(i=0; i<ARG_LEN; i++)varg[i]=arglist[i];
     if(split(buf,ARG_LEN,varg)==0)return;
     if(spawnvp(P_WAIT,varg[0],varg)){
       OptimWarningMessage(this, "Cannot perform the command", buf, QMessageBox::Ok);
     return;
      
    }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

