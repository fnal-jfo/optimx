//  =================================================================
//
//  Export.cpp
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

#include <OptimMainWindow.h>
#include <Constants.h>   
#include <Utility.h>   
#include <Element.h>   
#include <RMatrix.h>   
#include <ExportDialog.h>   
#include <LatticeDialog.h>  // old 
#include <OptimMessages.h>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QMdiSubWindow>

#include <ctime>

using Constants::PI;
using Constants::C_CGS;
using Constants::C_DERV1;
using Constants::C_DERV2;
using Utility::splitText;

void cutname(char* b) // trim trailing blanks 
{
	while ((*b!=' ')&&(*b!=0))b++;
	*b='\000';
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdExportResolve()
{
  char *pname, nm, name[20];
  double s, c, fi, dfi, gap, k_in, k_out, L, Hrt, Enr;
  FILE *fp;

  ExportDlgStr NSt;
  if(interrupted_ ){interrupted_= false; return;}
  NSt.HideFirstLetter=false;
 
  ExportDialog dialog(0);

  if ( dialog.exec() == QDialog::Rejected ) return; 

  if(analyzed_) { 
    if( analyze(false) ) return;
  } 
  else { 
    if( analyze(true) )  return;
  }
  
  QString fileto = dialog.data_.filename;
  if ( fileto == "" ) return;
  
  // start of the export

  if((fp = fopen( fileto.toUtf8().data(), "w"))==0 ) {
    OptimMessageBox::warning(this, "Error opening file for export. File name -", fileto.trimmed().toUtf8().data(), QMessageBox::Ok);
    return;
  }

  std::unique_ptr<FILE, int(*)(FILE*)> fpu( fp, &std::fclose);
  
  s = 0.001*sqrt((Ein+ms)*(Ein+ms)-ms*ms);
  fprintf(fp,"START_VALUES, ENERGY=%g, DISTANCE=%g;\n", s, so0_*0.01);
  
  // writing of the Elements
  
  Enr=Ein;
  for ( int i=0; i<nelm_-1; i++) {
    auto ep = beamline_[i];
    strcpy(name, ep->name());
    nm = toupper(name[0]);
    
    if(NSt.HideFirstLetter) {
      if(((name[1]>='0')&&(name[1]<='9'))||(strlen(name)==1)) {
        pname=name;
      }
      else { 
        pname=name+1;
      }
    } 
    else { 
      pname=name;
    }
   
    L=ep->length()*0.01;
    Hrt=sqrt(2.*ms*Enr+Enr*Enr)/C_DERV1;
    switch (nm){
      case 'O':  
      case 'H': 
      case 'I':
	if (L==0.) { 
          fprintf(fp,"%s : MONI;\n",pname);
	}
        else { 
          fprintf(fp,"%s : DRIFT, LENGTH=%g;\n",pname, L);
        }
        break;
      case 'Q':
	if(ep->tilt() != 0.0) fprintf(fp,"R%s: ROTATION, ANGLE=%g;\n",pname, -PI*ep->tilt()/180.);
        fprintf(fp,"%s : QUAD,  LENGTH=%g,  B1=%g;\n", pname, L, 100.*ep->G);
	if(ep->tilt() != 0.0) fprintf(fp,"R%s: ROTATION, ANGLE=%g;\n",pname,  PI*ep->tilt()/180.);
	break;
      case 'S':
	fprintf(fp,"%s : SEXT,  LENGTH=%g,  B2=%g;", pname, L, 10000.*ep->S);
	break;
      case 'B':
	if(ep->tilt() != 0.) fprintf(fp,"R%s: ROTATION, ANGLE=%g;\n",pname, -PI*ep->tilt()/180.);
	fprintf(fp,"%s : BEND,  LENGTH=%g,  B0=%g, B1=%g", pname, L, ep->B, 100.*ep->G);
	gap=0.;
	if(i>0){
	  if( beamline_[i-1]->etype()  == 'G'){
	     fi   = PI*beamline_[i-1]->G/180.; if(ep->B < 0.) fi = -fi;
	     fprintf(fp,", \n   BETA_IN=%g ", fi);
	     s    = sin(fi);
	     c    = cos(fi);
	     k_in = 0.5;
	     gap=0.01*beamline_[i-1]->S/k_in*c*c*c/(1+s*s);
	     fprintf(fp,",  GAP=%g, K_IN=%g, H_IN=0.", gap, k_in);}
	   }
	   if((i+1) < nelm_){
	     if(  beamline_[i+1]->etype() == 'G'){
	        fi = PI*beamline_[i+1]->G/180.; if(ep->B < 0.) fi = -fi;
		fprintf(fp,", \n   BETA_OUT=%g ", fi);
		s = sin(fi);
		c = cos(fi);
		if(gap >0.0){
                   k_out = 0.01*beamline_[i+1]->S/gap*c*c*c/(1+s*s);
		   fprintf(fp,",  K_OUT=%g, H_OUT=0.", k_out);
                }
		else{ 
                   k_out = 0.5;
		   gap   = 0.01*beamline_[i+1]->S/k_out*c*c*c/(1+s*s);
		   fprintf(fp,",  GAP=%g, K_OUT=%g, H_OUT=0.", gap, k_out);
                }
	     }
	   }
	  fprintf(fp,";\n");
	  if(ep->tilt() != 0.) fprintf(fp,"R%s: ROTATION, ANGLE=%g;\n",pname, PI*ep->tilt()/180.);
	  break;
       case 'D':
	  if(ep->tilt() != 0.) fprintf(fp,"R%s: ROTATION, ANGLE=%g;\n",pname, -PI*ep->tilt()/180.);
	  fprintf(fp,"%s : BEND,  LENGTH=%g,  B0=%g, B1=%g", pname, L, ep->B, 100.*ep->G);
	  gap=0.;
	  dfi=fabs(0.5*ep->B*ep->length()/Hrt);
	  if(i>0){
	    if( beamline_[i-1]->etype() == 'G'){
	       fi  = PI*beamline_[i-1]->G/180.;
	       if(ep->B < 0.0) fi = -fi;
	       fi += dfi;
	       fprintf(fp,", \n   BETA_IN=%g ", fi);
	       s    = sin(fi);
	       c    = cos(fi);
	       k_in = 0.5;
	       gap  = 0.01*beamline_[i-1]->S/k_in*c*c*c/(1+s*s);
	       fprintf(fp,",  GAP=%g, K_IN=%g, H_IN=0.", gap, k_in);
	     }
             else {
	       fprintf(fp,", \n   BETA_IN=%g ", dfi);
             }
	  } else { 
            fprintf(fp,", \n   BETA_IN=%g ", dfi);
          }
	  if((i+1) < nelm_){
            if( beamline_[i+1]->etype() == 'G'){
	       fi  = PI*beamline_[i+1]->G/180.;
	       if(ep->B < 0.) fi = -fi;
	       fi += dfi;
	       fprintf(fp,", \n   BETA_OUT=%g ", fi);
	       s = sin(fi);
	       c = cos(fi);
	       if(gap > 0.0){
                  k_out = 0.01*beamline_[i+1]->S/gap*c*c*c/(1+s*s);
	          fprintf(fp,",  K_OUT=%g, H_OUT=0.", k_out);
               }
	       else{ 
                  k_out = 0.5;
		  gap   = 0.01*beamline_[i+1]->S/k_out*c*c*c/(1+s*s);
		  fprintf(fp,",  GAP=%g, K_OUT=%g, H_OUT=0.", gap, k_out);
               }
	     } else {
	       fprintf(fp,", \n   BETA_OUT=%g ", dfi);
	     }
	  } else {
             fprintf(fp,", \n   BETA_OUT=%g ", dfi);
          }
	  fprintf(fp,";\n");
	  if(ep->tilt() != 0.) fprintf(fp,"R%s: ROTATION, ANGLE=%g;\n",pname, PI*ep->tilt()/180.);
	  break;
	case 'A':  
        case 'W':
	  fprintf(fp,"%s: ACCEL, LENGTH=%g, EGAIN=%g, PHASE=%g, FREQUENCY=%g;\n",
						pname,L, ep->G*0.001, PI*ep->S/180., C_DERV2/ep->tilt());
	  Enr += ep->G*cos(PI/180.*ep->S);
	  break;
	case 'K':
	  if( fabs(sin(PI*ep->tilt()/180))<0.707){
	    if (fabs(L)<1.2e-6) fprintf(fp,"%s: XCOR; \n", pname);
	    else fprintf(fp,"D%s: DRIFT, LENGTH=%g;\n%s: XCOR;\nD%s: DRIFT, LENGTH=%g;\n",
								pname, L/2., pname, pname, L/2.);
	  }
	  else {
	    if(fabs(L)<1.2e-6)fprintf(fp,"%s: YCOR;\n", pname);
	    else fprintf(fp,"D%s: DRIFT, LENGTH=%g;\n%s: YCOR;\nD%s: DRIFT, LENGTH=%g;\n",
								pname, L/2., pname, pname, L/2.);
	  }
	  break;
	case 'C': 
        case 'T': 
        case 'U':
	  if (L==0.){fprintf(fp,"%s : MONI;\n",pname); break;}
          else{fprintf(fp,"%s : MONI;\n",pname); break;}
	default :
	  fprintf(fp,"%s: UNKNOWN, LENGTH=%g;\n",pname, L);
	case 'G':
	  break;
    } // switch 
  } // for
  fprintf(fp, " END_BEAM_LINE;\n");

OptimInformationMessage(this, "Export is done to file", fileto.trimmed().toUtf8().data(), QMessageBox::Ok);
  return;
}

//======================================================================
// Presently, this is a routine to export to MAD

std::string CorrectName(char const* buf)
{
  std::string name(buf);
  
  for( auto it = name.begin(); it != name.end(); ++it ) { 
      if( *it == ':') *it ='_';

   }
  return name;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdExportDimad ()  // ExportMad
{
  char *pname, nm, *flnm, name[20];
  double* Enr, L, Hrt, s, c, tetaY;

  // unit conversion factors for transfer matrix  
  double const mt[6][6] = {{1.,   0.01, 1.,   0.01, 1.,   0.01},
			   {100., 1.,   100., 1.,   100., 1.},
			   {1.,   0.01, 1.,   0.01, 1., 0.01},
			   {100., 1.,   100., 1.,   100., 1.},
			   {1.,   0.01, 1.,   0.01, 1., 0.01},
			   {100., 1.,   100., 1.,   100., 1.}};

  FILE *fp;
  int i, j, n;
  time_t tim;
  RMatrix tm;
  RMatrix me;
  ExportDlgStr NSt;
  if(interrupted_ ){interrupted_= false; return;}
  NSt.HideFirstLetter=false;
  
  ExportDialog dialog(0);
  if ( dialog.exec() == QDialog::Rejected ) return;  
  QString fileto = dialog.data_.filename;
   
  if(analyzed_) {if(analyze(false))return;} else {if(analyze(true))return;}
  if (fileto == "") return;
  

  // start of the export
  
  if((fp=fopen(fileto.toUtf8().data(),"w"))==0){
    OptimMessageBox::warning(this, "Error opening file for export. File name -", fileto.trimmed().toUtf8().data(), QMessageBox::Ok);
    return;
  }

  
  fprintf(fp,"!TITLE\n");
  time(&tim);
  
  flnm = LatticeCh_->windowTitle().toUtf8().data();

  fprintf(fp,"!  Converted from %s.  %s\n", flnm, ctime(&tim));
  fprintf(fp,"!  Energy=%g MeV, massa=%g MeV\n", Ein, ms);
  fprintf(fp,"!  BetaX=%g m,  AlphaX=%g,  BetaY=%g m,  AlphaY=%g \n",
		 BetaXin*0.01, AlfaXin, BetaYin*0.01, AlfaYin);
  fprintf(fp,"!  DispX=%g m,  DispPX=%g,  DispY=%g m,  DispPY=%g \n",
		 DispXin*0.01, DispPrimeXin, DispYin*0.01, DispPrimeYin);
  fprintf(fp,"\nOPTION, -INTER, -ECHO\n");
  fprintf(fp,"ASSIGN, PRINT=\"line.out\"\n");
  fprintf(fp,"ASSIGN, ECHO=\"line.echo\"\n\n");

  // Energy at entrances of Elements
  
  Enr=new double [nelm_];
  Enr[0]=Ein;
  for (i=0; i<nelm_-1; i++){
    auto ep = beamline_[i];
    nm = ep->etype();
    switch (nm){
      case 'E':
	Enr[i+1]=ep->S=Enr[i]+ep->B; ep->G=Enr[i]; 	
        break;
      case 'A':
	Enr[i+1] = Enr[i] + ep->G*cos(PI/180.*ep->S);  
        break;
      case 'W':
        Enr[i+1]=Enr[i];
	me = ep->rmatrix(Enr[i+1], ms, tetaY, 0.0,3); 
        break;
      case 'X':
	Enr[i+1] = Enr[i] + ep->G;  break;
      default:
	Enr[i+1] = Enr[i];  
        break;
    }
  }
  
  // writing of the List of Elements
  
  for(i=0; i<nelmlist_; ++i){
     auto ep= elmdict_[i];
     strcpy(name, ep->name());
     cutname(name);
     nm = ep->etype();
     n=-1;
     for(j=0; j<nelm_; j++)if(strcmp(ep->name(), beamline_[j]->name())==0){n=j; break;}
     if(n == -1) continue;
     if(NSt.HideFirstLetter) {
       if(((name[1]>='0')&&(name[1]<='9'))||(strlen(name)==1))pname=name;
       else pname=name+1;
     } 
     else { 
       pname=name;
     }
     Hrt=sqrt(2.*ms*Enr[n]+Enr[n]*Enr[n])/C_DERV1;
     L=ep->length()*0.01;
//   if((L==0.)&&(nm!='K')){fprintf(fp,"%s: MONITOR \n",CorrectName(pname)); continue;}
     switch (nm){
       case 'Q':
	 fprintf(fp,"%s: QUADRUPOLE,  L=%g,  K1=%g",CorrectName(pname).c_str(), L, 10000.*ep->G/Hrt);
	 if(ep->tilt() == 0.) fprintf(fp,"\n");
	 else fprintf(fp,",  TILT=%g \n", ep->tilt()*PI/180.);
	 break;
       case 'B':  case 'D':
	 fprintf(fp,"%s: SBEND,  L=%g,  ANGLE=%g",CorrectName(pname).c_str(), L, ep->B*ep->length()/Hrt);
	 if(ep->G != 0.)fprintf(fp,",  K1=%g", 10000.*ep->G/Hrt);
	 if(ep->tilt() != 0.)fprintf(fp,",  TILT=%g ", ep->tilt()*PI/180.);
	 if(n>0){
	   if( beamline_[n-1]->etype() == 'G'){
	     if(ep->B >= 0.)fprintf(fp,"& \n    ,  E1=%g ", beamline_[n-1]->G*PI/180.);
	     else fprintf(fp,"& \n    ,  E1=%g ", -beamline_[n-1]->G*PI/180.);
	     if(beamline_[n-1]->S != 0.){
	       s=sin(PI/180.*beamline_[n-1]->G);
	       c=cos(PI/180.*beamline_[n-1]->G);
	       fprintf(fp,",  HGAP=%g, FINT=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );}
	     }
	  }
	  if((n+1) < nelm_){
	    if( beamline_[n+1]->etype() == 'G'){
	      if(ep->B >= 0.)fprintf(fp," & \n    ,  E2=%g ", beamline_[n+1]->G*PI/180.);
	      else fprintf(fp," & \n    ,  E2=%g ", -beamline_[n+1]->G*PI/180.);
	      if(beamline_[n+1]->S != 0.){
	        s=sin(PI/180.*beamline_[n+1]->G);
	        c=cos(PI/180.*beamline_[n+1]->G);
	        //fprintf(fp,",  HGAP=%g, FINTX=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
	        fprintf(fp,",  HGAP=%g, FINT=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
              }
	    }
	  }
	  fprintf(fp,"\n");
	  break;
	case 'S':
	  fprintf(fp,"%s: SEXTUPOLE, L=%g, K2=%g\n",CorrectName(pname).c_str(), L, 1.e6*ep->S/Hrt);
	  break;
	case 'C':
	  fprintf(fp,"%s: SOLENOID, L=%g, KS=%g\n",CorrectName(pname).c_str(), L, 50.*ep->B/Hrt);
	  break;
	case 'K':
	  s=PI/180.*ep->tilt();
	  fprintf(fp,"%s:  kicker, L=%g, hkick=%g, vkick=%g\n",
		  CorrectName(pname).c_str(), L, cos(s)*L*ep->B/Hrt, sin(s)*L*ep->B/Hrt);
	  break;
	case 'A': 
        case 'W':
	  fprintf(fp,"%s: RFCAVITY, L=%g, VOLT=%g, LAG=%g, FREQ=%g\n",CorrectName(pname).c_str(),L,
		     ep->G*1000., ep->S, C_CGS/ep->tilt());
	  break;
	case 'X':
	  fprintf(fp,"%s: MATRIX, & \n",CorrectName(pname).c_str());
	  tm = std::dynamic_pointer_cast<XferMatrix>(ep)->rmatrix();
	  for(n=0; n<5; n++){
	     for(j=0; j<6; j++) fprintf(fp,"R%1d%1d=%g ,  ", n+1, j+1, tm[n][j] * mt[5][j]);
	     fprintf(fp," & \n");
	  }
	  for(j=0; j<5; j++) fprintf(fp,"R6%1d=%g ,  ", j+1, tm[5][j]*mt[5][j]);
	  fprintf(fp,"R66=%g \n", tm[5][5]);
	  break;
	case 'I':
	  if(L!=0)fprintf(fp,"d%s: DRIFT, L=%g \n",CorrectName(pname).c_str(), L);
	  fprintf(fp,"%s: MONITOR \n",CorrectName(pname).c_str());
	  break;
        case 'T': 
        case 'U': 
        case 'H':
	  if(L!=0) //MessageBox("Element does not have zero length in OptiM file",pname);
	    fprintf(fp,"%s: MONITOR \n",CorrectName(pname).c_str());
	  break;
	case 'O':  
        case 'Z':
	  default :
	    fprintf(fp,"%s: DRIFT, L=%g \n",CorrectName(pname).c_str(), L);
	case 'G':
	  break;
     }
  }
  
  // structure
  
  fprintf(fp,"\nMACHINE: LINE=( ");
  j=2;
  for(i=0; i<nelm_; i++){
     auto ep=beamline_[i];
     strcpy(name, ep->name());
     cutname(name);
     if( toupper(name[0])=='G') continue;
     if(NSt.HideFirstLetter) {
       if(((name[1]>='0')&&(name[1]<='9'))||(strlen(name)==1))pname=name;
       else pname=name+1;
     } 
     else { 
       pname=name;
     }
     if( ((ep->length())!=0) && (toupper(name[0])=='I') ) fprintf(fp,"d%s, ",CorrectName(pname).c_str());
     fprintf(fp, "%s", CorrectName(pname).c_str());  if(i != (nelm_-1))fprintf(fp, ", ");
     if(j++ ==5) {
         j=0; fprintf(fp, " & \n     ");
     }
   }

   fprintf(fp, ")\nuse,machine\n");
 
   // Mad start lines
   fprintf(fp, "\ntwiss energy=%e, betx=%e, alfx=%e, &\n",
    	       (Ein+ms)*0.001, BetaXin*0.01, AlfaXin);
   fprintf(fp, "     bety=%e, alfy=%e, &\n",
    	       BetaYin*0.01, AlfaYin);
   fprintf(fp, "     dx=%e, dpx=%e, &\n",
    	       DispXin*0.01, DispPrimeXin);
   fprintf(fp, "     dy=%e, dpy=%e, TAPE=line.twiss \n",
    	       DispYin*0.01, DispPrimeYin);
   fprintf(fp, "\nsurvey,\n\nstop\n");
   fclose(fp);
   delete [] Enr;
   OptimInformationMessage(this, "Done exporting to file", fileto.trimmed().toUtf8().data(), QMessageBox::Ok);
   return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdExportCebafDimad ()
{
  char *pname, nm, *flnm, name[20], buf[256], buf1[40];
  double* Enr, L, Hrt, s, c, tetaY;
  const double mt[6][6]={ {1., 0.01, 1., 0.01, 1., 0.01}, {100., 1., 100., 1., 100., 1.},
			  {1., 0.01, 1., 0.01, 1., 0.01}, {100., 1., 100., 1., 100., 1.},
			  {1., 0.01, 1., 0.01, 1., 0.01}, {100., 1., 100., 1., 100., 1.}};
  FILE *fp;
  int i, j, n, iCebafCav=0;
  time_t tim;
  RMatrix tm;
  RMatrix me;
  ExportDlgStr NSt;

  if(interrupted_){interrupted_= false; return;}
  NSt.HideFirstLetter=true;

  ExportDialog dialog(0);
  if ( dialog.exec() == QDialog::Rejected ) return;  
  QString fileto = dialog.data_.filename;

  if(analyzed_) {if(analyze(false))return;} else {if(analyze(true))return;}
  
  if (fileto == "") return;

  // start of the export

  if( (fp = fopen(fileto.toUtf8().data(),"w"))==0 ){
    OptimMessageBox::warning(this, "Error opening file for saving.  File name -", fileto.trimmed().toUtf8().data(), QMessageBox::Ok);
    return;
  }
  fprintf(fp,"UTRANSPORT\n\nTITLE\n");
  time(&tim);
  
  flnm = LatticeCh_->windowTitle().toUtf8().data();

  fprintf(fp,"Converted from %s to CEBAF DIMAD file.  %s \n", flnm, ctime(&tim));
  fprintf(fp,"!  Energy=%g MeV, massa=%g MeV\n", Ein, ms);
  fprintf(fp,"!  BetaX=%g m,  AlphaX=%g,  BetaY=%g m,  AlphaY=%g \n",
		 BetaXin*0.01, AlfaXin, BetaYin*0.01, AlfaYin);
  fprintf(fp,"!  DispX=%g m,  DispPX=%g,  DispY=%g m,  DispPY=%g \n",
		 DispXin*0.01, DispPrimeXin, DispYin*0.01, DispPrimeYin);
  // Energy at entrances of Elements
  Enr=new double [nelm_];
  Enr[0]=Ein;
  for(i=0; i<nelm_-1; i++){
    auto ep=beamline_[i];
    nm = ep->etype();
     switch (nm){
      case 'E':
	Enr[i+1]=ep->S=Enr[i]+ep->B; ep->G=Enr[i]; 	
        break;
      case 'A':
	Enr[i+1] = Enr[i] + ep->G*cos(PI/180.*ep->S);  
        break;
      case 'W':
        Enr[i+1]=Enr[i];
	me = ep->rmatrix( Enr[i+1], ms, tetaY, 0.0, 3); 
        break;
      case 'X':
	Enr[i+1] = Enr[i] + ep->G;  
        break;
      default:
	Enr[i+1] = Enr[i];  
        break;
    }
  }
  // writing of the List of Elements

  /************************************************************************
  CEBAF conventions:
  Elements which name begins from a capital letter are converted to drifts
  ---------------------------------------------------------------
  name   | purpose                                  | converted to
  ---------------------------------------------------------------
  oM...  removed magnet                             drift
  oI...  removed instrumentation                    drift
  kM...  corrector with BdL			    corrector
  KM...  corrector disconnected from power supply   drift
  kaM... corrector without BdL (30 Hz, fast         monitor 
	 feedback system, Hall A parity experiment
  krR... RF separator, raster                       sbend
  iI...  instrumentation (must have zero length)    monitor
  II...  currently disconected instrumentation      drift
  qMQ... quad                                       quad
  QMQ... disconected quad                           drift
  beMQ.. extraction line quad (listed as dipole)    quad
  a...   accelerating cavity

  ***************************************************************************/

  for(int i=0; i<nelmlist_; i++){
    auto ep = elmdict_[i];
    strcpy(name, ep->name());
    cutname(name);
    nm = ep->etype();
    n=-1;
    for(j=0; j<nelm_; j++)if(strcmp(ep->name(), beamline_[j]->name())==0){n=j; break;}
    if(n == -1) continue;
    if(NSt.HideFirstLetter) {
      if(((name[1]>='0')&&(name[1]<='9'))||(strlen(name)==1))pname=name;
      else pname=name+1;
    } 
    else { 
        pname=name;
    }
    Hrt=sqrt(2.*ms*Enr[n]+Enr[n]*Enr[n])/C_DERV1;
    L=ep->length()*0.01;
    if( (!islower(ep->name()[0])) && (nm!='G')){
      fprintf(fp,"%s: DRIFT, L=%g \n",pname, L); continue;
    }
    switch (nm) {
      case 'Q':
prquad:	fprintf(fp,"%s: QUADRUPOLE,  L=%g,  K1=%g",pname, L, 10000.*ep->G/Hrt);
	if(ep->tilt() == 0.) fprintf(fp,"\n");
	else fprintf(fp,",  TILT=%g \n",-ep->tilt());
	break;
      case 'B':
	if(ep->name()[1]=='e'){pname++; goto prquad;}
	fprintf(fp,"%s: SBEND,  L=%g,  ANGLE=%g",pname, L, 180./PI*ep->B*ep->length()/Hrt);
	if(ep->G != 0.)fprintf(fp,",  K1=%g", -ep->G*Hrt/(ep->B*ep->B));
	if(ep->tilt() != 0.)fprintf(fp,",  TILT=%g ",-ep->tilt());
	if(n>0) {
	  if( beamline_[n-1]->etype() == 'G'){
	    if(ep->B >= 0.)fprintf(fp,"& \n   ,  E1=%g ", beamline_[n-1]->G);
	    else fprintf(fp,"& \n    ,  E1=%g ", -beamline_[n-1]->G);
	    if(beamline_[n-1]->S != 0.){
	      s=sin(PI/180.*beamline_[n-1]->G);
	      c=cos(PI/180.*beamline_[n-1]->G);
	      fprintf(fp,",  HGAP=%g, FINT=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
            }
	  }
	}
	if((n+1) < nelm_){
	  if( beamline_[n+1]->etype() == 'G'){
	    if(ep->B >= 0.)fprintf(fp," & \n    ,  E2=%g ", beamline_[n+1]->G);
	    else fprintf(fp," & \n    ,  E2=%g ", -beamline_[n+1]->G);
	    if(beamline_[n+1]->S != 0.){
	      s=sin(PI/180.*beamline_[n+1]->G);
	      c=cos(PI/180.*beamline_[n+1]->G);
	      //fprintf(fp,",  HGAPX=%g, FINTX=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
	      fprintf(fp,",  HGAP=%g, FINT=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
            }
	  }
	}
	fprintf(fp,"\n");
	break;
      case 'D':
	fprintf(fp,"%s: RBEND,  L=%g,  ANGLE=%g",pname, L, 180./PI*ep->B*ep->length()/Hrt);
	if(ep->G != 0.)fprintf(fp,",  K1=%g", -ep->G*Hrt/(ep->B*ep->B));
	if(ep->tilt() != 0.)fprintf(fp,",  TILT=%g ",-ep->tilt());
	if(n>0){
	  if( ( beamline_[n-1]->etype() == 'G') && (beamline_[n-1]->S != 0.) ){
	     s=sin(PI/180.*beamline_[n-1]->G);
	     c=cos(PI/180.*beamline_[n-1]->G);
	     fprintf(fp,",  HGAP=%g, FINT=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
	   }
	 }
	 if((n+1) < nelm_){
	   if( (beamline_[n+1]->etype() == 'G') && (beamline_[n+1]->S != 0.)){
	     s=sin(PI/180.*beamline_[n+1]->G);
	     c=cos(PI/180.*beamline_[n+1]->G);
	     //fprintf(fp,",  HGAPX=%g, FINTX=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
	     fprintf(fp,",  HGAP=%g, FINT=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
	   }
	 }
	 fprintf(fp,"\n");
	 break;
      case 'S':
	 fprintf(fp,"%s: SEXTUPOLE, L=%g, K2=%g\n",pname, L, 1.e6*ep->S/Hrt);
	 break;
      case 'C':
	 fprintf(fp,"%s: SOLENOID, L=%g, KS=%g\n",pname, L, 50.*ep->B/Hrt);
	 break;
      case 'K':
	 if(ep->name()[1]=='a'){fprintf(fp,"%s: MONITOR \n",++pname); break;}
	 if(ep->name()[1]=='y'){fprintf(fp,"%s: MONITOR \n",++pname); break;}
	 if(ep->name()[1]=='r'){fprintf(fp,"%s: SBEND,  L=%g",++pname, L); break;}
	 s=-PI/180.*ep->tilt();
	 if(L < 1.01e-8) L=0.;
	 fprintf(fp,"%s: GKICK, L=%g, DXP=%g, DYP=%g\n",
			     pname, L, cos(s)*L*ep->B/Hrt, sin(s)*L*ep->B/Hrt);
	 break;
      case 'A': 
      case 'W':
	 if(fabs(L-0.5)<1e-6){
	   if(iCebafCav==0){
             fprintf(fp,"dCEB0CV: DRIFT, L=-0.1 \n"); iCebafCav=1;
           }
	   fprintf(fp,"%s: CEBCAV, L=0.7, EN0=%g, DELTAE=%g, PHI0=%g\n",
			       pname, sqrt((Ein+ms)*(Ein+ms)-ms*ms), ep->G, ep->S);
	 } 
         else {
	   fprintf(fp,"%s: RFCAVITY, L=%g, VOLT=%g, LAG=%g, FREQ=%g\n",pname,L,
			       ep->G*1000., ep->S, C_CGS/ep->tilt());
	 }
	 break;
       case 'X':
	 fprintf(fp,"%s: MATRIX, & \n",pname);
	 tm = std::dynamic_pointer_cast<XferMatrix>(ep)->rmatrix();
	 for(n=0; n<5; n++){
	    for(j=0; j<6; j++) fprintf(fp,"R%1d%1d=%g ,  ", n+1, j+1, tm[n][j]*mt[5][j]);
	    fprintf(fp," & \n");
	 }
	 for(j=0; j<5; j++) fprintf(fp,"R6%1d=%g ,  ", j+1, tm[5][j]*mt[5][j]);
	 fprintf(fp,"R66=%g \n", tm[5][5]);
	 break;
       case 'I': 
       case 'T': 
       case 'U':  
       case 'H':
	 fprintf(fp,"%s: MONITOR \n",pname);
	 break;
       case 'O': 
       case 'Z':
       default :
	 fprintf(fp,"%s: DRIFT, L=%g \n",pname, L);
       case 'G':
	 break;
      }
    }
    // structure
    fprintf(fp,"\nMACHINE: LINE=( ");
    j=16;
    buf[0]=0;
    for(i=0; i<nelm_; i++){
	auto ep=beamline_[i];
	strcpy(name, ep->name());
	cutname(name);
	if(toupper(name[0])=='G')continue;
	if(NSt.HideFirstLetter) {
	   if(((name[1]>='0')&&(name[1]<='9'))||(strlen(name)==1))pname=name;
	   else pname=name+1;
	} else pname=name;
        if( (name[0]=='k') && ((name[1]=='a')||(name[1]=='r')) )pname++;
	if( (name[0]=='b') && (name[1]=='e') )pname++;
	if( (name[0]=='a') && (fabs(ep->length()-50.)<1.e-5) )
          sprintf(buf1, "dCEB0CV, %s, dCEB0CV", pname);
	else 
           sprintf(buf1, "%s", pname);
        j += strlen(buf1)+2;
        if(j<70){
          strcat(buf, buf1);
          if(i != (nelm_-1))strcat(buf, ", ");
          else {fprintf(fp, "%s", buf); break;}
        }
        else{
          fprintf(fp, "%s &\n", buf);
    	  strcpy(buf, "   ");
          strcat(buf, buf1);
	if(i != (nelm_-1)){
	  j=3+strlen(buf1)+2;
          strcat(buf, ", ");
        }
        else {
          fprintf(fp, "%s", buf); break;}
         }
    }
    fprintf(fp, ")\nuse,machine\ndimat\ncavity setup\n%f,\n\nmachine\n1 2 1 0 1 1 1\n",
    	     sqrt((Ein+ms)*(Ein+ms)-ms*ms));
    fprintf(fp, "%g %g %g %g\n",BetaXin*0.01, AlfaXin,DispXin*0.01, DispPrimeXin);
    fprintf(fp, "%g %g %g %g\n",BetaYin*0.01, AlfaYin,DispYin*0.01, DispPrimeYin);
    fprintf(fp, "0,\n!matrix\n!1 -1,\n!layout\n");
    fprintf(fp, "!%g %g %g %g %g %g 0.\n!1 0,\n",
		 so0_*0.01,xo0_*0.01,yo0_*0.01,zo0_*0.01,tetaXo0_,tetaYo0_);
    s=0.001*sqrt((Ein+ms)*(Ein+ms)-ms*ms);
    fprintf(fp, "hardware\n");
    fprintf(fp, "%g %g %g %g %g %g %g 0.\n1 0;\n",
		 s,so0_*0.01,xo0_*0.01,yo0_*0.01,zo0_*0.01,tetaXo0_,tetaYo0_);
    fprintf(fp, "\nstop\n");
    fclose(fp);
    delete [] Enr;
    OptimInformationMessage(this, "Done exporting to file ", fileto.toUtf8().data(), QMessageBox::Ok);
    return;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdExportMadX() 
{    

 // Export to Mad-X

 char *pname;
 char  nm;
 char *flnm;
  
 char name[32]; // ***FIXME***
 char buf[128];
 char buf1[128];
 double L, Hrt, s, c, tetaY;
 
 double const mt[6][6]={
  		         {1., 0.01, 1., 0.01, 1., 0.01},
			 {100., 1., 100., 1., 100., 1.},
		         {1., 0.01, 1., 0.01, 1., 0.01},
			 {100., 1., 100., 1., 100., 1.},
                         {1., 0.01, 1., 0.01, 1., 0.01},
			 {100., 1., 100., 1., 100., 1.}};


  int i, j, n, nMarker, iMarker, ii;
  time_t tim;
  RMatrix tm;
  RMatrix me;
  ExportDlgStr NSt;

  if(interrupted_){ interrupted_ = false; return; }
  
  NSt.HideFirstLetter = false;
  NSt.LinacCav        = false;

  ExportDialog dialog(0);
  if ( dialog.exec() == QDialog::Rejected ) return;  

  QString fileto = dialog.data_.filename;
  

  if(analyzed_) {if(analyze(false))return;} else {if(analyze(true))return;}
  

  // start of export

  FILE *fp = 0;
  if( !(fp=fopen(fileto.toUtf8().data(),"w") ) ) {
    OptimMessageBox::warning(this, "Error opening file for saving. File name -", fileto.toUtf8().data(), QMessageBox::Ok);
    return;
  }

  std::unique_ptr<FILE, int(*)(FILE*)> fpu( fp, &std::fclose);
 
  fprintf(fp,"!TITLE ");
  time(&tim);
  flnm = LatticeCh_->windowTitle().toUtf8().data();

  fprintf(fp,"Converted from %s.  %s\n", flnm, ctime(&tim));
  fprintf(fp,"!  Kinetic energy=%g MeV, mass=%g MeV\n", Ein, ms);
  fprintf(fp,"!  BetaX=%g m,  AlphaX=%g,  BetaY=%g m,  AlphaY=%g \n",
		 BetaXin*0.01, AlfaXin, BetaYin*0.01, AlfaYin);
  fprintf(fp,"!  DispX=%g m,  DispPX=%g,  DispY=%g m,  DispPY=%g \n",
		 DispXin*0.01, DispPrimeXin, DispYin*0.01, DispPrimeYin);
  fprintf(fp,"\nOPTION, -ECHO;\n");
  fprintf(fp,"ASSIGN, ECHO=\"line.echo\";\n\n");

// Energy at entrances of Elements and list of full names for "i" Elements

  std::vector<double>      Enr(nelm_+1);
  std::vector<std::string> MarkerName(nelm_); 

  //for(i=0; i<nelm_; i++) MarkerName[i] = new char [NAME_LENGTH];
  
  nMarker = 0;
  Enr[0]  = Ein;
  
  for (i=0; i<nelm_; i++){
    auto ep = beamline_[i];
    nm = ep->etype();
     switch (nm){
      case 'E':
	Enr[i+1]=ep->S=Enr[i]+ep->B; ep->G=Enr[i]; 	
        break;
      case 'A':
	Enr[i+1] = Enr[i] + ep->G*cos(PI/180.*ep->S);  
        break;
      case 'W':
        Enr[i+1]=Enr[i];
	me = ep->rmatrix( Enr[i+1], ms, tetaY, 0.0, 3); 
        break;
      case 'X':
	Enr[i+1] = Enr[i] + ep->G;  
        break;
      case 'I':
	MarkerName[nMarker++] = CorrectName(ep->fullName() );
      default:
	Enr[i+1] = Enr[i];  
        break;
    }
  }

  // writing the List of Elements
  
  for(i=0; i<nelmlist_; i++){
    auto ep=elmdict_[i];
    strcpy(name, ep->name());
    cutname(name);  // removes spaces at the name end
    nm = ep->etype();
    n  = -1;
    
     for(j=0; j<nelm_; j++) if(strcmp(ep->name(), beamline_[j]->name())==0){n=j; break;}
     if(n == -1) continue;
     if(NSt.HideFirstLetter) {
       if(isalpha(name[1])&&(strlen(name)>2))pname=name+1;
       else pname=name;
     } 
     else pname=name;
    
     Hrt=  sqrt(2.*ms*Enr[n]+Enr[n]*Enr[n])/C_DERV1;
     L   = ep->length()*0.01;
     
     switch (nm){
       case 'Q':
	 fprintf(fp,"%s: QUADRUPOLE,  L=%-.10lg,  K1=%-.10lg",CorrectName(pname).c_str(), L, 10000.*ep->G/Hrt);
	 if(ep->tilt() == 0.) fprintf(fp,";\n");
	 else fprintf(fp,",  TILT=%-.10lg ;\n",ep->tilt()*PI/180.);
	 break;
       case 'B':  
       case 'D':
	 fprintf(fp,"%s: SBEND,  L=%-.10lg,  ANGLE=%-.10lg",CorrectName(pname).c_str(), L, ep->B*ep->length()/Hrt);
	 if(ep->G != 0.)fprintf(fp,",  K1=%-.10lg", 10000.*ep->G/Hrt);
	 if(ep->tilt() != 0.)fprintf(fp,",  TILT=%-.10lg ", ep->tilt()*PI/180.);
	 if(n>0) {
	   if( beamline_[n-1]->etype() == 'G') {
	     if(ep->B >= 0.)fprintf(fp,"   \n    ,  E1=%-.10lg ", beamline_[n-1]->G*PI/180.);
	     else fprintf(fp,"   \n    ,  E1=%-.10lg ", -beamline_[n-1]->G*PI/180.);
	     if(beamline_[n-1]->S != 0.){
	       s = sin(PI/180.*beamline_[n-1]->G);
	       c = cos(PI/180.*beamline_[n-1]->G);
	       fprintf(fp,",  HGAP=%-.10lg, FINT=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
             }
	   }
	 }
	 if((n+1) < nelm_) {
	   if( beamline_[n+1]->etype() == 'G') {
	     if(ep->B >= 0.)fprintf(fp,"   \n    ,  E2=%-.10lg ", beamline_[n+1]->G*PI/180.);
	     else fprintf(fp,"   \n    ,  E2=%-.10lg ", -beamline_[n+1]->G*PI/180.);
	     if(beamline_[n+1]->S != 0.0 ){
	       s=sin(PI/180.*beamline_[n+1]->G);
	       c=cos(PI/180.*beamline_[n+1]->G);
	       //fprintf(fp,",  HGAPX=%-.10lg, FINTX=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
	       fprintf(fp,",  HGAP=%-.10lg, FINT=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
             }
	   }
	 }
	 
         fprintf(fp,";\n");
	 break;
       case 'S':
	 fprintf(fp,"%s: SEXTUPOLE, L=%-.10lg, K2=%-.10lg;\n",CorrectName(pname).c_str(), L, 1.e6*ep->S/Hrt);
	 break;
       case 'C':
	 fprintf(fp,"%s: SOLENOID, L=%-.10lg, KS=%-.10lg;\n",CorrectName(pname).c_str(), L, 100.*ep->B/Hrt);
	 break;
       case 'K':
	 s=PI/180.*ep->tilt();
	 fprintf(fp,"%s:  kicker, L=%-.10lg, hkick=%-.10lg, vkick=%-.10lg;\n",
		 CorrectName(pname).c_str(), L, cos(s)*L*ep->B/Hrt, sin(s)*L*ep->B/Hrt);
	 break;
       case 'A': 
       case 'W':
         if(NSt.LinacCav) {
	   fprintf(fp,"%s: LCAV, L=%-.10lg, DELTAE=%-.10lg, PHI0=%-.10lg, FREQ=%-.10lg;\n",CorrectName(pname).c_str(),L,
	            ep->G, ep->S/360., C_CGS/ep->tilt());
         } 
         else {
	   fprintf(fp,"%s: RFCAVITY, L=%-.10lg, VOLT=%-.10lg, LAG=%-.10lg, HARMON=%d;\n",CorrectName(pname).c_str(),L,
		      ep->G, (ep->S/360.-0.25), (int)(Length_/ep->tilt()));
	 }
         break;
       case 'M':
	 fprintf(fp,"%s: MULTIPOLE, ",CorrectName(pname).c_str());
	 if(ep->tilt()!=0.)fprintf(fp,"TILT=%-.10lg, ", ep->tilt()*PI/180.);
	 fprintf(fp," KNL:={");
         ii=0; while(ii<ep->N){fprintf(fp,"0,"); ii++;}
         fprintf(fp, "%-.10lg};\n", exp(ep->N*log(100.))*ep->S/Hrt);
	 break;
       case 'X':
	 fprintf(fp,"%s: MATRIX,   \n",CorrectName(pname).c_str());
	 tm = std::dynamic_pointer_cast<XferMatrix>(ep)->rmatrix();
	 for(n=0; n<5; n++){
	   for(j=0; j<6; j++) fprintf(fp,"R%1d%1d=%-.10lg ,  ", n+1, j+1, tm[n][j]*mt[5][j]);
	   fprintf(fp,"   \n");
	 }
	 for(j=0; j<5; j++) fprintf(fp,"R6%1d=%-.10lg ,  ", j+1, tm[5][j]*mt[5][j]);
	 fprintf(fp,"R66=%-.10lg ;\n", tm[5][5]);
	 break;
       case 'I':
	 if(L!=0)fprintf(fp,"d%s: DRIFT, L=%-.10lg ;\n",CorrectName(pname).c_str(), L);
 	 break;
       case 'T': 
       case 'U': 
       case 'H':
         if(L!=0) OptimMessageBox::warning(this, pname, "Element does not have zero length in OptiM file", QMessageBox::Ok );
	 fprintf(fp,"%s: MONITOR ;\n",CorrectName(pname).c_str());
	 break;
       case 'O':  
       case 'Z':
       default :
	 fprintf(fp,"%s: DRIFT, L=%-.10lg ;\n",CorrectName(pname).c_str(), L);
       case 'G':
	  break;
     } // switch
  } //for
 
  for(iMarker=0; iMarker<nMarker; iMarker++)
    fprintf(fp,"%s: MONITOR ;\n",MarkerName[iMarker].c_str());

  // structure
  
  sprintf(buf,"\nMACHINE: LINE=(");
  for(iMarker=0, i=0; i<nelm_; i++){
    auto ep=beamline_[i];
    strcpy(name, ep->name());
    cutname(name);
    if( toupper(name[0]) =='G' ) continue;
    if(NSt.HideFirstLetter) {
      if(isalpha(name[1])&&(strlen(name)>2))pname=name+1;
      else pname=name;
    } 
    else pname=name;
    if( toupper(name[0]) == 'I') {
      if( ep->length() != 0) sprintf(buf1,"d%s, %s, ",CorrectName(pname).c_str(),MarkerName[iMarker++].c_str());
      else sprintf(buf1,"%s, ",CorrectName(MarkerName[iMarker++].c_str()).c_str());
    }
    else sprintf(buf1,"%s, ",CorrectName(pname).c_str());
    if( (strlen(buf)+strlen(buf1)+2)>78){
      fprintf(fp,"%s \n", buf);
      *buf=0;
    }
    strcat(buf, buf1);
  }// for
  
  buf[strlen(buf)-2]=0;
  fprintf(fp,"%s);\n", buf);
  fprintf(fp, "\nbeam, MASS=%-.10lg,CHARGE=1, ENERGY=%-.10lg,\n",ms/1000., (Ein+ms)/1000.);
  fprintf(fp, "      EX=%-.10lg, EY=%-.10lg, SIGE=%-.10lg;\n", ex_*0.01, ey_*0.01, dpp_);
  fprintf(fp, "\nuse,period=machine;\n");
  
  // Mad start lines
  
  fprintf(fp, "\ntwiss energy=%-.10lg, betx=%-.10lg, alfx=%-.10lg, \n",
    							(Ein+ms)*0.001, BetaXin*0.01, AlfaXin);
  fprintf(fp, "     bety=%-.10lg, alfy=%-.10lg, \n",
    							BetaYin*0.01, AlfaYin);
  fprintf(fp, "     dx=%-.10lg, dpx=%-.10lg, \n",
    						DispXin*0.01, DispPrimeXin);
  fprintf(fp, "     dy=%-.10lg, dpy=%-.10lg;\n",
    						DispYin*0.01, DispPrimeYin);
  fprintf(fp, "\nsurvey,;\n\nstop;\n");

  OptimMessageBox::warning( this, "Export to file successful.", splitText(fileto.toUtf8().data()), QMessageBox::Ok);
  
  //for(i=0; i<nelm_; i++)delete MarkerName[i];

  return;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdExportMad8 ()  
{

 //-----------------------
 // Export to MAD8 format
 //-----------------------
  
  char  *pname;
  char   nm;
  char   name[32]; // the array length is defined in Element.h **FIXME*** this should not be hardwired;
  char   buf[128];
  char  buf1[128];

 double L, Hrt, s, c, tetaY;

 double const mt[6][6]={
  		{1., 0.01, 1., 0.01, 1., 0.01}, {100., 1., 100., 1., 100., 1.},
		{1., 0.01, 1., 0.01, 1., 0.01}, {100., 1., 100., 1., 100., 1.},
                {1., 0.01, 1., 0.01, 1., 0.01}, {100., 1., 100., 1., 100., 1.}};

 time_t tim;
 
 RMatrix tm;
 RMatrix me;

 QString flnm = LatticeCh_->windowTitle();

 flnm.remove("[*]");
 flnm.remove("LATTICE EDITOR:");
 flnm = flnm.trimmed();

 
 if(interrupted_) { interrupted_= false; return;}

 ExportDlgStr NSt;
 NSt.HideFirstLetter = false;
 NSt.LinacCav        = false;
 NSt.filename        = QFileInfo(QFileInfo(".").absoluteFilePath(), flnm.remove(".opt").remove(".OPT") + ".mad8").filePath();

 ExportDialog dialog(0);

 dialog.data_.filename = NSt.filename;
 dialog.updateFileName();
 
 if ( dialog.exec() == QDialog::Rejected ) return;  
 QString fileto = dialog.data_.filename;

 NSt.HideFirstLetter = dialog.data_.HideFirstLetter;
 NSt.LinacCav        = dialog.data_.LinacCav;
 
 if(analyzed_) {
    if(analyze(false,1)) return;
 } else {
    if(analyze(true,1)) return;
 }
 
// start export to MAD8
 
FILE* fp = 0;
if( !(fp=fopen(fileto.toUtf8().data(),"w")) ) {
   OptimMessageBox::warning( this,  "Error opening file for writing: ", fileto.toUtf8().data(), QMessageBox::Ok);
   return;
 }

 std::unique_ptr<FILE, int(*)(FILE*)> fpu( fp, &std::fclose);

 fprintf(fp,"! TITLE\n");
 time(&tim);

 fprintf(fp,"! ----------------------------------------------------------------------------\n");
 fprintf(fp,"! %s\n", flnm.toUtf8().data());
 fprintf(fp,"! Converted by OptimX to MAD8 format.\n");
 fprintf(fp,"! %s\n", ctime(&tim));
 fprintf(fp,"! ----------------------------------------------------------------------------\n");
 
 fprintf(fp,"!  Kinetic energy=%g MeV, mass=%g MeV\n", Ein, ms);
 fprintf(fp,"!  BetaX=%g m,  AlphaX=%g,  BetaY=%g m,  AlphaY=%g \n",
	 BetaXin*0.01, AlfaXin, BetaYin*0.01, AlfaYin);
 fprintf(fp,"!  DispX=%g m,  DispPX=%g,  DispY=%g m,  DispPY=%g \n",
	 DispXin*0.01, DispPrimeXin, DispYin*0.01, DispPrimeYin);
 fprintf(fp,"\nOPTION, -INTER, -ECHO\n");
 fprintf(fp,"ASSIGN, PRINT=\"line_out.dat\"\n");
 fprintf(fp,"ASSIGN, ECHO=\"line_echo.txt\"\n\n");

// Energy at Element entrances and list of full names for "i" Elements
 
  std::vector<double>      Enr(nelm_+1);       // Energy  at Element entrance
  std::vector<std::string> MarkerName(nelm_);  // Element full name

  //for(i=0; i<nelm_; i++) MarkerName[i] = std::unique_ptr(new char[NAME_LENGTH]);

  int nMarker = 0;
  Enr[0]  = Ein;
  for (int i=0; i<nelm_; ++i){
    auto ep =beamline_[i];
    nm = ep->etype();
    switch (nm){
      case 'E':
	Enr[i+1]=ep->S=Enr[i]+ep->B; ep->G=Enr[i]; 	
        break;
      case 'A':
	Enr[i+1] = Enr[i] + ep->G*cos(PI/180.*ep->S);  
        break;
      case 'W':
        Enr[i+1]=Enr[i];
        me = ep->rmatrix(Enr[i+1], ms, tetaY, 0.0, 3);
	break;
      case 'X':
	Enr[i+1] = Enr[i] + ep->G;
	break;
      case 'I':
	MarkerName[nMarker++] = CorrectName( ep->fullName() );
      default:
	Enr[i+1] = Enr[i];  break;
    }
  }

  // writing element list
  
  int n = 0;
  for(int i=0; i<nelmlist_; ++i){
    auto ep = elmdict_[i];
    strcpy(name, ep->name());
    cutname(name);  // removes trailing spaces
    nm = ep->etype();
    n  = -1;
    for(int j=0; j<nelm_; ++j)if(strcmp(ep->name(), beamline_[j]->name())==0){n=j; break;}
    if(n == -1) continue;
    if(NSt.HideFirstLetter) {
      if(isalpha(name[1]) && (strlen(name)>2)) pname=name+1;
      else pname=name;
    }
    else pname=name;
    Hrt=sqrt(2.*ms*Enr[n]+Enr[n]*Enr[n])/C_DERV1;
    L=ep->length()*0.01;
    switch (nm){
      case 'Q':
	fprintf(fp,"%s: QUADRUPOLE,  L=%-.10lg,  K1=%-.10lg",CorrectName(pname).c_str(), L, 10000.*ep->G/Hrt);
	if(ep->tilt() == 0.) fprintf(fp,"\n");
	else fprintf(fp,",  TILT=%-.10lg \n", ep->tilt()*PI/180.);
	break;
      case 'B':  
      case 'D':
	fprintf(fp,"%s: SBEND,  L=%-.10lg,  ANGLE=%-.10lg",CorrectName(pname).c_str(), L, ep->B*ep->length()/Hrt);
	if(ep->G != 0.)fprintf(fp,",  K1=%-.10lg", 10000.*ep->G/Hrt);
	if(ep->tilt() != 0.)fprintf(fp,",  TILT=%-.10lg ", ep->tilt()*PI/180.);
	if(n>0){
	  if( beamline_[n-1]->etype() == 'G'){
	    if(ep->B >= 0.)fprintf(fp,"& \n    ,  E1=%-.10lg ", beamline_[n-1]->G*PI/180.);
	    else fprintf(fp,"& \n    ,  E1=%-.10lg ", -beamline_[n-1]->G*PI/180.);
	    if(beamline_[n-1]->S != 0.){
	      s = sin(PI/180.*beamline_[n-1]->G);
	      c = cos(PI/180.*beamline_[n-1]->G);
	      fprintf(fp,",  HGAP=%-.10lg, FINT=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
            }
	  }
	}
	
        if((n+1) < nelm_){
	  if( beamline_[n+1]->etype() == 'G'){
	    if(ep->B >= 0.)fprintf(fp," & \n    ,  E2=%-.10lg ", beamline_[n+1]->G*PI/180.);
	    else fprintf(fp," & \n    ,  E2=%-.10lg ", -beamline_[n+1]->G*PI/180.);
	    if(beamline_[n+1]->S != 0.){
	      s=sin(PI/180.*beamline_[n+1]->G);
	      c=cos(PI/180.*beamline_[n+1]->G);
	      //fprintf(fp,",  HGAPX=%-.10lg, FINTX=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
	      fprintf(fp,",  HGAP=%-.10lg, FINT=0.5",0.01*beamline_[n-1]->S*c*c*c/(1+s*s) );
            }
	  }
	}
        fprintf(fp,"\n");
	break;
      case 'S':
	fprintf(fp,"%s: SEXTUPOLE, L=%-.10lg, K2=%-.10lg\n",CorrectName(pname).c_str(), L, 1.e6*ep->S/Hrt);
	break;
      case 'C':
	fprintf(fp,"%s: SOLENOID, L=%-.10lg, KS=%-.10lg\n",CorrectName(pname).c_str(), L, 100.*ep->B/Hrt);
	break;
      case 'K':
        s=PI/180.*ep->tilt();
	fprintf(fp,"%s:  kicker, L=%-.10lg, hkick=%-.10lg, vkick=%-.10lg\n",
	CorrectName(pname).c_str(), L, cos(s)*L*ep->B/Hrt, sin(s)*L*ep->B/Hrt);
	break;
      case 'A': 
      case 'W':
        if(NSt.LinacCav) {
	  fprintf(fp,"%s: LCAV, L=%-.10lg, DELTAE=%-.10lg, PHI0=%-.10lg, FREQ=%-.10lg\n",CorrectName(pname).c_str(),L,
		  ep->G, ep->S/360., C_CGS/ep->tilt());
	}
        else {
	  fprintf(fp,"%s: RFCAVITY, L=%-.10lg, VOLT=%-.10lg, LAG=%-.10lg, HARMON=%d\n",CorrectName(pname).c_str(),L,
		  ep->G, (ep->S/360.-0.25), (int)(Length_/ep->tilt()));
	}
        break;
      case 'M':
	fprintf(fp,"%s: MULTIPOLE, K%-dL=%-.10lg, T%-d=%-.10lg\n",CorrectName(pname).c_str(),
            	ep->N, exp(ep->N*log(100.))*ep->S/Hrt,ep->N, ep->tilt()*PI/180.);
	break;
      case 'X':
        fprintf(fp,"%s: MATRIX, & \n",CorrectName(pname).c_str());
	tm = std::dynamic_pointer_cast<XferMatrix>(ep)->rmatrix();
	for(n=0; n<5; n++){
	  for(int j=0; j<6; ++j) fprintf(fp,"R%1d%1d=%-.10lg ,  ", n+1, j+1, tm[n][j]*mt[5][j]);
	  fprintf(fp," & \n");
	}
	for(int j=0; j<5; ++j) fprintf(fp,"R6%1d=%-.10lg ,  ", j+1, tm[5][j]*mt[5][j]);
	fprintf(fp, "R66 = %-.10lg \n", tm[5][5]);
	break;
      case 'I':
	if(L!=0)fprintf(fp,"d%s: DRIFT, L=%-.10lg \n",CorrectName(pname).c_str(), L);
 	break;
      case 'T': 
      case 'U': 
      case 'H':
	if(L!=0) OptimMessageBox::warning(this,pname, "Element does not have zero length in OptiM file",QMessageBox::Ok);
        print_elm(ep.get(),buf);
	fprintf(fp,"! %s \n",buf);
	fprintf(fp,"%s: MONITOR \n",CorrectName(pname).c_str());
	break;
      case 'O':  
      case 'Z':
      default :
	  fprintf(fp,"%s: DRIFT, L=%-.10lg \n",CorrectName(pname).c_str(), L);
      case 'G':
	   break;
    } 
  }
  
  for (int iMarker=0; iMarker<nMarker; ++iMarker) {
    fprintf(fp,"%s: MONITOR \n",MarkerName[iMarker].c_str());
  } 
  
  // structure
  
  sprintf(buf,"\nMACHINE: LINE=(");
  
  int iMarker=0;
  for(int i=0; i<nelm_; ++i){
      auto ep = beamline_[i];
      strcpy(name, ep->name());
      cutname(name);
      if( toupper(name[0])=='G')continue;
      if(NSt.HideFirstLetter) {
	if(isalpha(name[1])&&(strlen(name)>2))pname=name+1;
        else pname=name;
      } 
      else { 
      pname=name;
      }
      if( toupper(name[0]) == 'I'){
        if( ep->length() != 0) sprintf(buf1,"d%s, %s, ",CorrectName(pname).c_str(),MarkerName[iMarker++].c_str());
        else sprintf(buf1,"%s, ", CorrectName(MarkerName[iMarker++].c_str()).c_str() );
      }
      else sprintf(buf1,"%s, ", CorrectName(pname).c_str());
      if( (strlen(buf)+strlen(buf1)+2)>78){
        fprintf(fp,"%s &\n", buf);
        *buf=0;
      }
      strcat(buf, buf1);
    }
    buf[strlen(buf)-2]=0;
    fprintf(fp,"%s)\n", buf);

    fprintf(fp, "\nUSE, machine\n");
    fprintf(fp, "! ---------------------------------------------------------------------------------\n");
    fprintf(fp, "! Set all print flags.\n");
    fprintf(fp, "! ---------------------------------------------------------------------------------\n");
    fprintf(fp, "SELECT, FLAG=TWISS, FULL\n");
 
    fprintf(fp, "! ---------------------------------------------------------------------------------\n");
    fprintf(fp, "! Generate TWISS table.\n");
    fprintf(fp, "! ---------------------------------------------------------------------------------\n");

    fprintf(fp, "\nTWISS, ENERGY=%-.10lg, COUPLE, BETX=%-.10lg, ALFX=%-.10lg, &\n",
    		(Ein+ms)*0.001, BetaXin*0.01, AlfaXin);
    fprintf(fp, "     BETY=%-.10lg, ALFY=%-.10lg, &\n",
    		BetaYin*0.01, AlfaYin);
    fprintf(fp, "     DX=%-.10lg, DPX=%-.10lg, &\n",
    		DispXin*0.01, DispPrimeXin);
    fprintf(fp, "     DY=%-.10lg, DPY=%-.10lg, TAPE=line.twiss \n",
    	        DispYin*0.01, DispPrimeYin);

    fprintf(fp, "! ---------------------------------------------------------------------------------\n");
    fprintf(fp, "! Plot the uncoupled lattice functions.\n");
    fprintf(fp, "! ---------------------------------------------------------------------------------\n");

    fprintf(fp, "PLOT, HAXIS=S, VAXIS1=BETX,BETY, VAXIS2=DX,DY\n");
    
    fprintf(fp, "! ---------------------------------------------------------------------------------\n");
    fprintf(fp, "! Generate ENVELOPE table (sigma matrix) \n");
    fprintf(fp, "! ---------------------------------------------------------------------------------\n");

    fprintf(fp, "ENVELOPE, SAVE=ENVELOPE\n");

    fprintf(fp, "! ---------------------------------------------------------------------------------\n");
    fprintf(fp, "! Generate SURVEY table (elements position & orientation in global coordinates).\n");
    fprintf(fp, "! ---------------------------------------------------------------------------------\n");
    fprintf(fp, "SURVEY, SAVE=SURVEY\n");
    fprintf(fp, "STOP\n");
    
    //OptimInformationMessage(this, "Export to MAD8", splitText(fileto.toUtf8().data()), QMessageBox::Ok);
     OptimInformationMessage(this, "Export to MAD8", fileto.toUtf8().data(), QMessageBox::Ok);

    return;
}
