//  =================================================================
//
//  Import.cpp
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
#include <iostream>

#include <Globals.h>
#include <OptimMainWindow.h>
#include <OptimMdiArea.h>
#include <OptimTextEditor.h>
#include <Constants.h>   
#include <Element.h>   
#include <ImportDialog.h>   
#include <ExportDialog.h>   // old
#include <LatticeDialog.h>  // old 
#include <Utility.h>  
#include <OptimMessages.h>

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFileDialog>
#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
 // std::regex not supported for g++ < 4.9.X and MVC C++ < 15.0
#if GCC_VERSION >= 40900 || _MSC_VER >= 1900
#include <regex>
namespace compat = std;
#else
#include <boost/regex.hpp>
namespace compat = boost;
#endif

 using Constants::PI;
 using Constants::C_CGS;
 using Constants::C_DERV1;
 using Constants::C_DERV2;

 using Utility::correctNames;
 using Utility::strcmpr;
 using Utility::readLineCmt;
 using Utility::readLineCalc;
 using Utility::splits;
 using Utility::strcmpf; 
 using Utility::strcmpp;
 using Utility::skipspaces;
 using Utility::cpname;
 using Utility::copystr;
 using Utility::decodeLine;
 using Utility::decodeLineCalc;
 using Utility::getExpression;
 using Utility::getElmName;
 using Utility::getElmNameX;
 using Utility::getVariableName;
 using Utility::fPrint;
 
 #define N_NAMES 20000
 #define LBUF    256
 #define NLINES  4
 #define NELEM   8
 #define LSTRL   512
 #define LSTR    512
 #define ARG_LEN 10

 struct ename {char n[11]; double L; char newname[11];};

 static char const *res_par[] = {"LENGTH",   "B0",   "B1",    "B2",   "ANGLE", "BETA_IN",
 				"BETA_OUT", "H_IN", "H_OUT", "K_IN", "K_OUT", "GAP",
 				"EGAIN",    "PHASE", "FREQUENCY"};

 int n_res_par = sizeof(res_par)/sizeof(char*);


 static char const *res_type[] = { "DRIFT", "BEND", "QUAD", "SEXT", "XCOR", "YCOR",
 				  "MARK", "MONI", "ROTATION", "ACCEL"};

 int n_res_type = sizeof(res_type)/sizeof(char*);

 static char opti_type[]={"obqskkoooA"};



 //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


 void	testname(ename* nm, int *knm, resolve *v)
 {
   int i;
   // if name was not used yet
   i=0;
   while(i < *knm) {
     if(!strcmp(v->name, nm[i].n)) break;
     i++;
   }
   if(i==*knm){nm[*knm].L=v->L;
     strcpy(nm[*knm].n, v->name);
     strcpy(nm[*knm].newname, v->name);
     (*knm)++;
     return;
   }
   // Name already was used. If there is the same length
   i=0;
   while(i < *knm) { 
     if( (strcmp(v->name, nm[i].n)==0) && (v->L==nm[i].L)){
       strcpy(v->name, nm[i].newname);
       return;
     }
     i++;
   }
   // Generate new name
   strcpy(nm[*knm].n, v->name);
   sprintf(&nm[*knm].newname[1],"%-4d", *knm);   nm[*knm].newname[0]=v->name[0];
   nm[*knm].L=v->L;
   strcpy(v->name, nm[*knm].newname);
   (*knm)++;
 }

 //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 //||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 //------------------------------------------------------------------------------
 // 0 - OK,
 // 1 - too long name,
 // 2 - no ':' after name,
 // 3 - too long identificator of Element type,
 // 4 - not existing identificator of Element type,
 // 5 - too long parameter name,
 // 6 - no '=' after parameter name,
 // 7 - not existing identificator of parameter,
 // 8 - length of the number is longer 19 symbols.

 int  OptimMainWindow::decodeXLine(char* str, resolve* v, int* ntp)
 {
   int i, n;
   char buf[50], *bp;
   double d;

   // Element name
   bp = v->name; *bp++=' ';
   for(i=0; i<10; i++) {
     if(*str==':')break;
     if(*str=='\000')return 2;
     *bp++=*str++;
   }
   *bp='\000';
   if(i==10)return 1;
   str++;

   // initialization

   v->L     = 0.0;
   v->B     = 0.0; 
   v->G     = 0.0;
   v->S     = 0.0;
   v->T     = 0.0;
   v->beta1 = 0.0;
   v->beta2 = 0.0;
   v->Hin   = 0.0;
   v->Hout  = 0.0;
   v->Kin   = 0.0;
   v->Kout  = 0.0;
   v->GAP   = 0.0;

   // Element type

   bp=buf;
   for(int i=0; i<10; ++i) {
      if( (*str==',') || (*str=='\000') )break;
      *bp++ =*str++;
   }
   *bp=0;

   if(i==10)return 3;

   for(int i=0; i < n_res_type; ++i) if(strcmpr(res_type[i],buf)) break;

   if(i==n_res_type)return 4;
   *v->name=opti_type[i];
   *ntp=i;

   if(*str++ == 0) return 0;

  // parameters

  if(*ntp==9)v->name[1]=0;   // accelerating stracture name is always "A"

  while(*str) {
    bp=buf;
    for(int i=0; i<10; ++i) {
      if( (*str=='=') || (*str=='\000') )break;
      *bp++=*str++;
    }
    if(i==10)return 5;
    if(*str++=='\000')return 6;
    *bp='\000';

    for(n=0; n<n_res_par; n++) if(strcmpr(res_par[n],buf))break;
    if(n==n_res_par)return 7;

    bp=buf;
    for(i=0; i<20; i++) {
      if( (*str==',') || (*str=='\000') )break;
      *bp++=*str++;
    }
    *bp='\000';

    if(i==20)return 8;

    d = (double)atof(buf);

    switch  (n)  {
      case  0:   v->L     = d;   break;     // LENGTH
      case  1: 	 v->B     = d;   break;     // B0
      case  2: 	 v->G     = d;   break;     // B1
      case  3: 	 v->S     = d;   break;     // B2
      case  4: 	 v->T     = d;   break;     // ANGLE
      case  5: 	 v->beta1 = d;   break;     // BETA_IN
      case  6: 	 v->beta2 = d;   break;     // BETA_OUT
      case  7: 	 v->Hin   = d;   break;     // H_IN
      case  8: 	 v->Hout  = d;   break;     // H_OUT
      case  9: 	 v->Kin   = d;   break;     // K_IN
      case  10:  v->Kout  = d;   break;     // K_OUT
      case  11:  v->GAP   = d;   break;     // GAP
      case  12:  v->G     = d;   break;     // EGAIN
      case  13:  v->S     = d;   break;     // PHASE
      case  14: 	         break;     // FREQUENCY
    }
    if ( (!*str++) )break;

  }
  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int readline(FILE *fp, char* str)
{  
   //1 - OK; 0 - reading error or EOF; 2 - no ';' after 5 lines
   char buf[LBUF], *bp;
   int i;
   for(i=0; i<NLINES; i++){
     if(fgets(buf, LBUF, fp)==NULL)return 0;
     bp=buf;
     while((*bp!='\000') && (*bp!='\n')){
       if(*bp==';') {*str='\000'; 	return 1;}
       if((*bp!=' ') && (*bp!='\t'))*str++ = *bp;
       bp++;
     }
   }
   *str='\000';
   return 2;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdImportResolve ()
{
 
  char  buf[256];
  char nbuf[256];
  char bel[3]={'\015','\012','\000'};
  int i, j, k, err, nelm, ntp, knm;
  resolve v;
  Element e;
  double dat[5], s, mass=0.511006, enrg, tilt;

  char filenamefrom[255];
  strcpy(filenamefrom, QFileDialog::getOpenFileName().toUtf8().data()); 
  if (!filenamefrom[0]) return;

  ImportDialog dialog(0);
  if ( dialog.exec() == QDialog::Rejected ) return;

  char const* filenameto  = dialog.data_.filenameto.toUtf8().data(); 

  sprintf(buf,"Import from RESOLVE");
  
  OptimTextEditor* editor = 0;
  mdiArea->addSubWindow( editor = new OptimTextEditor() );
 
  // open files ... 

  auto del = [](FILE* p) { (p ? fclose(p) : 0);};  
  std::unique_ptr<FILE, decltype(del)>  fp1 (fopen(filenamefrom,"r"),del);
  std::unique_ptr<FILE, decltype(del)>  fp2(fopen(filenameto,"w"), del);

  if(!fp1) {
    OptimMessageBox::warning(this,  "Error opening file - ", filenamefrom, QMessageBox::Ok);
    return;
  }

  if(!fp2){
    OptimMessageBox::warning(this, "Error opening file - ",  filenameto, QMessageBox::Ok);
    return;
  }

  // first line analysis

  char str [LSTRL];
 
  if(readline(fp1.get(), str)!=1) return;

  if(strcmpr("START_VALUES", str)){
    decodeLine(str,dat,2);  enrg=dat[0];  s=dat[1];
    enrg=enrg*1000.-mass;
  }
  else { 
    sprintf(buf,"ERROR: The first line is not START_VALUES");
    editor->insertPlainText(buf); editor->insertPlainText(bel);
    return;
  }
  fprintf(fp2.get(),"OptiM\n");
  fprintf(fp2.get(),"Energy[MeV]=%-8.8lg   Mass[MeV]=%-8.8lg  \n",enrg,mass);
  fprintf(fp2.get(),"Emittance: ex[cm]=%g  ey[cm]=%g  DP/P=%g \n",2.e-7,2.e-7,2.e-5);
  fprintf(fp2.get(),"Initial:\tBetaX[cm]=%g \tBetaY[cm]=%g \n",1000.,1000.);
  fprintf(fp2.get(),"\tAlphaX=%g \tAlphaY=%g \n",0.,0.);
  fprintf(fp2.get(),"\tDispersX[cm]=%g \tDispersY[cm]=%g \n",0.,0.);
  fprintf(fp2.get(),"\tDsp_PrimeX=%g \tDspPrimeY=%g \n",0.,0.);
  fprintf(fp2.get(),"\tX[cm]=%g \tY[cm]=%g \tZ[cm]=%g \tS[cm]=%-8.3lf \n",0.,0.,0., s);
  fprintf(fp2.get(),"\ttetaX[deg]=%g \ttetaY[deg]=%g \n",0.,0.);
  // First reading to get Names of Elements
  fprintf(fp2.get(),"begin lattice. Number of periods=%d \n",1);

  std::unique_ptr<ename[]> nm( new ename [N_NAMES]); 

  if(!nm){
    sprintf(buf,"Not enough memory");
    editor->insertPlainText(buf);
    editor->insertPlainText(bel); 
    return;
  }
  for (i=j=nelm=knm=0; i<N_NAMES-1; i++){
    if((err=readline(fp1.get(), str)) != 1) break;
    if(err == 2){
      sprintf(buf,"Error. The following line has no the end mark ';'. Line number is %d", i+1);
      editor->insertPlainText(buf);
      editor->insertPlainText(bel);
      editor->insertPlainText(str);
      editor->insertPlainText(bel);
      break;
    }
    if(strcmpr("START_VALUES", str))continue;
    if(strcmpr("END_BEAM_LINE", str))break;
    if((err=decodeXLine(str, &v, &ntp))!=0){
      sprintf(buf,"Error %d in decodeXLine function at line %d.",err, i+1);
      editor->insertPlainText(buf); editor->insertPlainText(bel);
      editor->insertPlainText(str); editor->insertPlainText(bel);
      break;
    }

    testname(nm.get(), &knm, &v);

    switch (ntp){
      //DRIFT, QUAD,  SEXT",  XCOR,  YCOR,   MARK,   MONI
      case 0:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
	fPrint(fp2.get(), v.name);
	if( j++ == NELEM ){j=0;   fprintf(fp2.get()," \n");}
	break;
      case 1: // BEND
	v.name[0]='g';
	fPrint(fp2.get(),v.name);
	if(j++ == NELEM) {
           j=0; fprintf(fp2.get()," \n");
        }
	v.name[0]='b'; fPrint(fp2.get(),v.name);
	if(j++ == NELEM){
          j=0; fprintf(fp2.get()," \n"); 
        }
	v.name[0]='G'; fPrint(fp2.get(),v.name);
        if(j++ == NELEM){
          j=0; fprintf(fp2.get()," \n");
        }
      case 8:  
        break;
      case 9:  
        fprintf(fp2.get(),"O A O ");
	if(j++ == NELEM){
           j=0; fprintf(fp2.get()," \n");
        }
	break;
    }
  };
 
  if(j)fprintf(fp2.get()," \n");
  
  sprintf(buf,"File %s include %d Elements", filenamefrom, i+1);
  
  editor->insertPlainText(buf); editor->insertPlainText(bel);
  if(i==N_NAMES){ 
    OptimMessageBox::warning(this, "Import", "Error. File has more than 1900 names(20000 for WIN32)",QMessageBox::Ok);
    return;
  }
  fprintf(fp2.get(),"end lattice\n");
  // getting Elements

  std::unique_ptr<ename[]> name( new ename [N_NAMES]); 

  if(!name){
    sprintf(buf,"Cannot allocate enough memory"); 
    editor->insertPlainText(buf);
    editor->insertPlainText(bel); 
    return;
  }
  if(!name) { 
    OptimMessageBox::warning(this, "Import", "Cannot allocate enough memory.", QMessageBox::Ok);
    return;
  }

  rewind(fp1.get());
  readline(fp1.get(), str);

  fprintf(fp2.get(),"begin list \n");
  for (i=j=knm=0, tilt=0.; i<N_NAMES-1; i++){
    if((err=readline(fp1.get(), str)) != 1) break;
    if(strcmpr("START_VALUES", str))continue;
    if(strcmpr("END_BEAM_LINE", str))break;
    if((err=decodeXLine(str, &v, &ntp))!=0)	break;
    testname(nm.get(), &knm, &v);
    if(ntp==8) {
       tilt += v.T*180/PI;      // ROTATION
       if(fabs(tilt)<1.e-7) tilt =0.;
       continue;
    }

  err=1.; k=0;
  while(k<nelm){
    if(!strcmp(&name[k++].n[0], v.name)) {err=0; break;}
  }

  
  if(err){
    strcpy(&name[nelm++].n[0], v.name);
    strcpy(nbuf, v.name); // a copy that can be modified (see below)
    e.name(v.name);

    e.length( 100.*v.L );
    e.B  = v.B,
    e.G  = 0.01*v.G;
    e.S  = 0.0001*v.S;
    e.N  = 0;
    e.tilt(tilt);
    
    switch (ntp){
      case 8:  break;      // ROTATION
			   //DRIFT, QUAD,  SEXT",  XCOR,  YCOR,   MARK,   MONI
      case 0: case 2: case 3: case 4: case 5: case 6: case 7:
	print_elm(&e,buf);
	fprintf(fp2.get(),"%s \n", buf);
	break;
      case 1: // BEND
        nbuf[0] = 'g';
	e.name(nbuf);
        e.G=v.beta1*180./PI;
	e.S=v.GAP*v.Kin*100.*(1.+sin(v.beta1)*sin(v.beta1))
			     /cos(v.beta1)/cos(v.beta1)/cos(v.beta1);
        if(e.B < 0.) e.G = -e.G;
	print_elm(&e,buf);
	fprintf(fp2.get(),"%s \n", buf);
	nbuf[0] = 'b'; e.name(nbuf); e.G=0.01*v.G;
	print_elm(&e,buf);
	fprintf(fp2.get(),"%s \n", buf);
	nbuf[0] = 'G'; e.name(nbuf);
	e.G=v.beta2*180./PI;
	e.S=v.GAP*v.Kout*100.*(1.+sin(v.beta2)*sin(v.beta2))
			     /cos(v.beta2)/cos(v.beta2)/cos(v.beta2);
	if(e.B < 0.) e.G = -e.G;
	print_elm(&e,buf);
	fprintf(fp2.get(),"%s \n", buf);
	break;
      case 9: // ACCEL  **************************************************** CHECK THIS !!!!
	e.length( 50.0 );
	e.N = 5;
	e.B = 7.0,
	e.G = 1000*v.G;
	e.S = 180.*v.S/PI;
	e.tilt(20.);
	print_elm(&e,buf);
	fprintf(fp2.get(),"O \tL[cm]=10. \n%s \n", buf);

     }
   }
  }
  fprintf(fp2.get(),"end list \n");
  sprintf(buf,"File %s includes %d different Element names.",filenamefrom, nelm);
  editor->insertPlainText(buf);
  editor->insertPlainText(bel);
  sprintf(buf, "Import done to file %s", filenameto);
  editor->insertPlainText(buf);
  editor->insertPlainText(bel);
  editor->document()->setModified(false);
  
  return;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//=====================================================================
// DIMAD import
//=====================================================================

#define NLINES_ELM 9
#define N_PARAM_NAME_TOTAL 17
#define N_UNIT_NAME 7

static char const* param_name[N_PARAM_NAME_TOTAL] = {
	"LENGTH",  "BENDING ANGLE", 	"BENDING RADIUS",  "HALFGAP",
	"FIELD INDEX N", "ENTRANCE ANGLE",  "EXIT ANGLE",  "TILT ANGLE",// dipole
	"STRENGTH",                                                     // quad & sextupole
	"INDUCTION",	"FIELD INDEX BETA",	"ENTRANCE CURVATURE",	"EXIT CURVATURE",//dipole
	"HALF APERTURE","FOCAL LENGTH", 	"POLE TIP INDUCTION",	"INTEGRATED STRENGTH"};

static char const* unit_name[N_UNIT_NAME] = {"/M)", "/M**2)", "/M**3)", "M)", "M.)", "DEG)",  "KG)"};


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::PrintToWindow(char const* buf, char const* buf2, QPlainTextEdit* editor)
{
  editor->insertPlainText(buf);
  editor->insertPlainText("    ");
  editor->insertPlainText(buf2);
  editor->insertPlainText("\n");
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//return: 1 - OK; 0 - reading error, or EOF, or syntax error; 2 - End list of Element properties
// buf - keeps last line which was read

#define LEN_BUF 1200

int OptimMainWindow::ReadElemDimad(FILE *fp, char* buf, dimad* el, QPlainTextEdit* editor)
{
   char *bp, *btmp, b[LEN_BUF], buf1[128];
   int i,j;
   // skips empty lines, reads the first line and takes Element name
   
   do { 
     if(fgets(buf, LBUF, fp)==0){
enrd:		
       PrintToWindow("ReadDimad: Premature end of fileE while reading element parameters"," ", editor); 
       return 0;
     }
     if(buf[strlen(buf)-1]=='\n')buf[strlen(buf)-1]=0;
     bp=skipspaces(buf);
   } while(*bp==0);
   
   bp=strcmpf("ELEMENT:",buf);
   if(bp==0) return 2;
   cpname(el->name,bp);
   //reads lines with parameters to b[]
   b[0]=0;
   for(i=0; i<NLINES_ELM; i++){
     if(fgets(buf, LBUF, fp)==0)goto enrd;
     j=strlen(buf);
     if(buf[j-1]=='\n')buf[j-1]=0;
     bp=skipspaces(buf);
     if(*bp=='\000')break;
     if(strlen(b)>(LEN_BUF-128)){
       PrintToWindow("Line describing an Element is too long:",b, editor);
       return 0;
     }
     strcat(b," ");
     strcat(b,buf);
   }
   
   // determines parameters

   for(j=0; j<N_PARAM_NAME; j++)el->v[j]=0.;
   bp = b;
   el->tip = 'o';
   
   while(*bp) {
     for(j=0; j<N_PARAM_NAME_TOTAL; j++){
       btmp=strcmpp(param_name[j],bp);
       if(btmp!=0){bp=btmp; break;}
     }
     if(btmp){
       bp=copystr(buf1, bp);
       if(j<N_PARAM_NAME){el->v[j] = atof(buf1);
         switch (j){
	   case 1: el->tip='b'; break;     // dipole
	   case 8: el->tip='q'; break;     // quad
	   default: 	break;
         }
       }
       if(*bp=='('){
	 for(j=0; j<N_UNIT_NAME; j++){
           btmp=strcmpf(unit_name[j], bp+1);
           if(btmp!=0){bp=btmp; break;}
         }
         if(btmp==0){
           PrintToWindow("Unknown dimension in the line:",bp, editor);
           return 0;
         }
         if((el->tip=='q')&&(j==2)) el->tip='s';      //sextupole
       }
     } 
     else { 
       PrintToWindow("Unknown parameter in line:",bp, editor);
       return 0;
     }
   }
   return 1;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char* OptimMainWindow::read_one_line(char* buf, int len, FILE* fp, QPlainTextEdit* editor)
{
 char *bp;
 do{
   bp=fgets(buf, len, fp);
   if(bp==0){
     PrintToWindow("Premature End of FILE"," ", editor);
     return 0;
   }
   bp=skipspaces(bp);
 } while((*bp==0)||(*bp=='\n'));
 return bp;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//return address of the next symbol after the end of src in buf
char*OptimMainWindow::FindReadLine(char* buf, int len, FILE* fp, QPlainTextEdit* editor, char const* src)
{
  char *bp;
  do{
    bp=fgets(buf, len, fp);
    if(bp==0){
      PrintToWindow("Premature End of FILE or no operator ",src, editor);
      return 0;
    }
    bp=strcmpf(src,buf);
  } while(bp==0);
  return bp;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::DecodeDimadLine(dimad* v, double Hrt, FILE* fp)
{
  Element e;
  char buf[128];
  char nbuf[128];
  double s,c;
  e.length( 100*v->v[0] );

  strcpy(&nbuf[1], v->name);
  buf[0]=v->tip;
  e.name(nbuf);

  e.tilt(v->v[7]);

  switch (v->tip) {
    case 'b': // BEND
      e.B = PI*v->v[1]/180*Hrt/e.length();
      //
      nbuf[0] = 'g';
      e.name(nbuf);
      e.G = s   =  v->v[5];                   // entrance angle
      c   = cos(PI*e.G/180.);
      s   = sin(PI*e.G/180.);
      e.S = v->v[3]*100.*(1.+s*s)/c;          // effective length
      if(e.B < 0.) e.G = -e.G;
      print_elm(&e,buf);
      fprintf(fp,"%s \n", buf);
      //
      nbuf[0] = 'b';
      e.name(nbuf);
      e.G = -e.B*e.B/Hrt*v->v[4];             //gradient
      print_elm(&e,buf);
      fprintf(fp,"%s \n", buf);
      //
      nbuf[0] = 'G';
      e.name(nbuf);
      e.G = v->v[6];                         // exit angle
      c  = cos(PI*e.G/180.);
      s  = sin(PI*e.G/180.);
      e.S  = v->v[3]*100.*(1.+s*s)/c;        // effective length
      if(e.B < 0.) e.G = -e.G;
      break;
    case 'q': // Quad
      e.G = v->v[8]*Hrt*0.0001;
      break;
    case 's': // Sextupole
      e.S = v->v[8]*Hrt*0.000001;
      break;
    default: break;
  }
  print_elm(&e,buf);
  fprintf(fp,"%s \n", buf);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdImportDimad () 
{

  struct drift {
     char name[NAME_LENGTH]; 
     double L;
  };


  FILE *fp1, *fp2;

  char buf[LBUF];
  char name[80];
  int i, j, k, nelem, ndrift;
  double btx,bty,alfx,alfy,Dx,Dy,Dxp,Dyp;
  double mass=0.511006, enrg, Hrt, s, s_old, len, s0, x, y, z, tetx, tety;
  
  char filenamefrom[255];
  strcpy(filenamefrom, QFileDialog::getOpenFileName().toUtf8().data());
  if (!filenamefrom[0]) return;
    
  ImportDialog dialog(0);
  if ( dialog.exec() == QDialog::Rejected ) return;

  char const* filenameto  = dialog.data_.filenameto.toUtf8().data(); 

  sprintf(buf, "%-8.8lg", mass);
 
  sprintf(buf,"Import from DIMAD"); 
  OptimTextEditor* editor = 0; 
  mdiArea->addSubWindow(editor = new OptimTextEditor()); // optiMDIChild(*this,buf,new QTextEdit(0, 0, 0, 0, 0, 0, 0, 0));
  editor->show();
  
  // Files opening
  if((fp1 = fopen(filenamefrom,"r"))==0){
    OptimMessageBox::warning(this, "Error opening file - ", filenamefrom, QMessageBox::Ok);
    return;
  }
  if((fp2=fopen(filenameto,"w"))==0){
    OptimMessageBox::warning(this, "Error opening file - ", filenameto, QMessageBox::Ok);
    fclose(fp1);
    return;
  }

  std::unique_ptr< dimad[] > v (new dimad [N_NAMES]);  // if(!v) { PrintToWindow("Cannot allocate enough memory.", "", editor); goto send2;}
  std::unique_ptr< drift[] > d (new drift [N_NAMES]);  // if(!d) { PrintToWindow("Cannot allocate enough memory.", "", editor); goto send1;}

  // first line analysis
  
  PrintToWindow("Begin reading of initial beta-functions and hardware"," ", editor);

  char* bp = FindReadLine(buf, LBUF, fp1, editor, "ELEMENT");

  if(!bp) goto send;

  bp = strcmpf("#", bp);                       if(!bp)goto send;
  bp = strcmpf("BETAX", bp);                   if(!bp)goto send;
  bp = read_one_line(buf, LBUF, fp1, editor);  if(!bp)goto send;

  sscanf(bp, "%le %le %le %le %le %le %le %le ",&btx,&alfx,&bty,&alfy,&Dx,&Dxp,&Dy,&Dyp);
  //

  if(!FindReadLine(buf, LBUF, fp1, editor, "HARDWARE"))goto send;
  bp=read_one_line(buf, LBUF, fp1, editor); if(!bp)goto send;
  sscanf(bp, "%le %le %le %le %le %le %le ",&enrg, &s0, &x, &y, &z, &tetx, &tety);
  s0 *= 100;  x *= 100; y *= 100; z *= 100;
  enrg = enrg*1000.-mass;
  Hrt  = sqrt(2.*mass*enrg+enrg*enrg)/C_DERV1;
  bp   = read_one_line(buf, LBUF, fp1, editor); if(!bp)goto send;

  if(!FindReadLine(buf, LBUF, fp1, editor, "VALUES ARE FOR ENERGY"))goto send;

  // OptiM header
  fprintf(fp2,"OptiM\n");
  fprintf(fp2,"# Converted from DIMAD output file: %s\n", filenamefrom );
  fprintf(fp2,"Energy[MeV]=%-8.8lg   Mass[MeV]=%-8.8lg   \n",enrg,mass);
  fprintf(fp2,"Emittance: ex[cm]=%g  ey[cm]=%g  DP/P=%g \n",2.e-7,2.e-7,2.e-5);
  fprintf(fp2,"Initial:\tBetaX[cm]=%g \tBetaY[cm]=%g \n", 100*btx, 100*bty);
  fprintf(fp2,"\tAlphaX=%g \tAlphaY=%g \n",alfx,alfy);
  fprintf(fp2,"\tDispersX[cm]=%g \tDispersY[cm]=%g \n",100*Dx, 100*Dy);
  fprintf(fp2,"\tDsp_PrimeX=%g \tDspPrimeY=%g \n",Dxp,Dyp);
  fprintf(fp2,"\tX[cm]=%-8.4lf \tY[cm]=%-8.4lf \tZ[cm]=%-8.4lf \tS[cm]=%-8.4lf \n",x,y,z, s0);
  fprintf(fp2,"\ttetaX[deg]=%-8.8lg \ttetaY[deg]=%-8.8lg \n",tetx,tety);
  fprintf(fp2,"begin lattice. Number of periods=1 \n");
  
  //    read Element parameters

  PrintToWindow("Begin reading of magnet parameters "," ", editor);
  nelem = 0;
  while(ReadElemDimad(fp1, buf, &v[nelem], editor)==1){
    nelem++;
    if(nelem==N_NAMES){
      PrintToWindow("Number of magnets in the source file exceeds limit "," ", editor);
      goto send;
    }
  }
  PrintToWindow("Begin reading of the machine layout "," ", editor);
  if(!FindReadLine(buf, LBUF, fp1, editor, "THE SXYZ COORDINATES"))goto send;
  if(!FindReadLine(buf, LBUF, fp1, editor, "#"))goto send;
  ndrift=k=0;
  s_old=s0;
  while((bool)(bp=read_one_line(buf, LBUF, fp1, editor))){
    if( (strlen(buf)<100)&&(ndrift>1) ) break;
      sscanf(bp, "%d %s %le ",&i, name, &s);
      s *= 100.;
      len=s-s_old;
      i=j=0;
      while(i<nelem){ if(strcmp(name, v[i].name)==0){j=1; break;} i++;}
      if(j){
	s_old=s_old+100.*v[i].v[0];
	if(v[i].tip=='b'){fprintf(fp2, "g%s ", v[i].name); k++;}
	fprintf(fp2, "%c%s ", v[i].tip, v[i].name);
	if(v[i].tip=='b'){fprintf(fp2, "G%s ", v[i].name); k++;}
      } 
      else {
       s_old=s;
       i=j=0;
       while(i<ndrift){ if(strcmp(name, d[i].name)==0){j=1; break;} i++;}
       if(j==0){strcpy(d[ndrift].name, name);  d[ndrift].L=len; ndrift++;}
       fprintf(fp2, "o%s ", name);
      }
      if(k++ >8) {fprintf(fp2, "\n"); k=0;}
    }
    if(k!=0) fprintf(fp2, "\n");
    fprintf(fp2,"end lattice \nbegin list \n");
    for(i=0; i<nelem; i++) DecodeDimadLine(&v[i], Hrt, fp2);
    for(i=0; i<ndrift; i++) fprintf(fp2, "o%s   L[cm]=%-8.8lg \n", d[i].name, d[i].L);
    fprintf(fp2,"end list \n");
    PrintToWindow("Conversion done to file ", filenameto, editor);

 send:	

    if(fp1)fclose(fp1);
    if(fp2)fclose(fp2);
    editor->document()->setModified(false);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#define LMADBUF 1000

static char const* mad_types[] = { "MARKER"
				 "DRIFT",
				 "SBEND",
				 "RBEND",
				 "QUADRUPOLE",
				 "SEXTUPOLE",
				 "OCTUPOLE",
				 "MULTIPOLE",
				 "SOLENOID",
				 "HKICKER",
				 "VKICKER",
				 "KICKER",
				 "RFCAVITY",
				 "ELSEPARATOR",
				 "HMONITOR",
				 "VMONITOR",
				 "MONITOR",
				 "INSTRUMENT",
				 "ECOLLIMATOR",
				 "RCOLLIMATOR",
				 "YROT",
				 "SROT",
				 "BEAMBEAM",
				 "MATRIX",
				 "LUMP" };



static char const* mad_type[]={ "DRIF", "RBEN", "SBEN", "QUAD", "SEXT",
                                "KICK", "HKIC", "VKIC", "MULT", "LCAV",
                                "RFCA", "MONI", "HMON", "VMON", "MARK",
                                "GLBL", "INST", "WIRE", "SLMO", "OCTU"};

int const MAD_TYPE_LEN = sizeof(mad_type)/ sizeof(char*);


static char opti_prfx[] = {"obbqskkkmaaiiiIIiiio"};


struct MadElm {

   char optnm[256];
   char type[25];
   char name[128];

   double L;
   double ang;
   double k1;
   double k2;
   double tilt;
   double e1;
   double e2;
   double gap1;
   double gap2;
   double h1;
   double h2; 
};


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int MakeMadName(char* OptiName, char* name, char* type)
{

   for(int i=0; i< MAD_TYPE_LEN; ++i) {
     if(strcmpr(type, mad_type[i])) {
      	 OptiName[0] = opti_prfx[i];
	 OptiName[1] = 0;
        strcat(OptiName, name);
      	return 0;
     }
   }
   strcpy(OptiName, "UNKNOWN");
   return 1;
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DecodeMadLine(double *enrg, double *Hrt, double mass, MadElm* v, Element* el, int modif)
{

   double s,c;
   double fi = 0;
   char   nbuf[128]; // a mutable buffer for the elemnt name
     
   *Hrt=sqrt(2.*mass*(*enrg)+(*enrg)*(*enrg))/C_DERV1;
   el->length( 100*v->L );
   el->tilt( v->tilt*180./PI);

   strcpy(nbuf, el->name(v->optnm));
  
   if(el->name()[0]=='b'){

     if(modif==1) { nbuf[0] = 'g'; el->name(nbuf); }
     if(modif==2) {nbuf[0] = 'G'; el->name(nbuf); }

     if(strcmpr("RBEN",v->type)){
       fi=0.5*v->ang;
       if(fi!=0.0) el->length( el->length()*fi/sin(fi) );
     }
     else fi=0.0;
   }

   switch (el->name()[0]){
     case 'b': // BEND
       el->B = v->ang*(*Hrt)/el->length();
       el->G = (*Hrt)*v->k1*0.0001;     //gradient
       break;
     case 'g': // BEND
       el->B = v->ang*(*Hrt)/el->length();
       el->G = (v->e1+fi)*180./PI;     // entrance angle
       c     = cos(el->G*PI/180.);
       s     = sin(el->G*PI/180.);
       el->S = v->gap1*100.*(1.+s*s)/c;  // effective length
       if(el->B < 0.) el->G = -el->G;
       break;
     case 'G': // BEND
       el->B =  v->ang*(*Hrt)/el->length();
       el->G = (v->e2+fi)*180./PI;     // entrance angle
       c     = cos(el->G*PI/180.);
       s     = sin(el->G*PI/180.);
       el->S = v->gap2*100.*(1.+s*s)/c;  // effective length
       if(el->B < 0.) el->G = -el->G;
       break;
     case 'q': // Quad
       el->G = (*Hrt)*v->k1*0.0001;     //gradient
       break;
     case 's': // Sextupole
       el->G = (*Hrt)*v->k2*0.000001;     //gradient
       break;
     case 'm': // Multipole
       if(modif==0){
      	 el->N=1;
         el->S=(*Hrt)*v->k1*0.01;}  //quadrupole gradient
       else{
      	 el->N=2;
         el->S=(*Hrt)*v->k1*0.0001;  //sextupole gradient
         strcpy(nbuf, v->optnm);
         strcat(nbuf,"S");
         el->name(nbuf);
       }
       break;
     case 'a': // accelerating section
       if(v->e1>0)el->tilt(C_DERV2/v->e1);//frequency-to-wavelength conversion
       else el->tilt( el->length()*2.);
       el->G = v->e2;              // total acceleration [MeV]
       el->S = 360.*v->h1;         // accelerating phase
       el->N = 4.*el->length()/el->tilt();     // number of cells
       el->B = 0.9*el->length()/el->N;    // effective cell length
       *enrg += el->G*cos(PI/180.*el->S);
       *Hrt=sqrt(2.*mass*(*enrg)+(*enrg)*(*enrg))/C_DERV1;
       break;
     default: 
       break;
   }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int cmdpMadElem(MadElm* v1, MadElm* v2)
{
   if(strcmp(v1->optnm, v2->optnm))return 0;
   if(v1->L   !=v2->L    || v1->ang!=v2->ang || v1->k1!=v2->k1 || v1->k2!=v2->k2 ||
   v1->tilt!=v2->tilt || v1->e1 !=v2->e1  || v1->e2!=v2->e2)return 2;
   return 1;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdImportMad8()
{
  Element el;

  char  buf[LMADBUF];
  char  buf1[24];
  char  arglist[ARG_LEN][80];
  char* varg[ARG_LEN+1];


  int i;
  int j;
  int k;
  int dk;
  int nelem;
  double s;
  double btx;
  double bty;
  double alx;
  double aly;
  double Dx;
  double Dy;
  double Dxp;
  double Dyp;

  double mux;
  double muy;
  double X;
  double PX;
  double Y;
  double PY;
  
  double btx0;
  double alx0;
  double Dx0;
  double Dxp0;
  double bty0;
  double aly0;
  double Dy0;
  double Dyp0;

  double mass, enrg, Hrt; // s_old, len, s0, x, y, z, tetx, tety;

  ImportDialogStruct NStI;

  for(int i=0; i<ARG_LEN; ++i) varg[i] = arglist[i];

  // open text window for conversion status report  

  OptimTextEditor* editor = 0;
  QMdiSubWindow*   sw = mdiArea->addSubWindow( editor = new OptimTextEditor() );
  editor->show();
  sw->setWindowTitle("Import from MAD8");

  // open the MAD8 file (in twiss table format)  

  char fname1[255];
  strcpy(fname1, QFileDialog::getOpenFileName().toUtf8().data()); 
  if (!fname1[0]) return; 
  
  auto del = [](FILE* p) { (p ? fclose(p) : 0);};  
  std::unique_ptr<FILE, decltype(del)>  fp1 (fopen(fname1,"r"), del);
  
  if( !fp1 ){
    OptimMessageBox::warning(this,  "Error opening file - ", fname1, QMessageBox::Ok);
    return;
  }

  // .................................................................................... 
  // Read the first line and confirm that we are dealing with a MAD8 file in TWISS table format.
  // if so, get the total number of elements 

  if ( fgets(buf, LMADBUF, fp1.get() )==0 ) {
    PrintToWindow("File is empty.", fname1, editor);
    return;
  }

  QString header(buf);
 
  if((i=splits(buf,ARG_LEN,varg))<5){
    PrintToWindow("First line has less than 5 fields", fname1, editor);
    return;
  }

  for (j=0; j<i; ++j) if (strcmpr("TWISS", arglist[j])) break;

  if (!strcmpr("TWISS", arglist[j])) {
    PrintToWindow("This is not a MAD8 Twiss table file ", fname1, editor);
    return;
  }

  nelem = atoi(arglist[i-1])-1;

  if(nelem< 2){
    PrintToWindow("Number of Elements is less than 2 ", fname1, editor);
    return;
  }

  
  // Find the "INITIAL" marker line

  while(fgets(buf, LMADBUF, fp1.get())){
    i = splits(buf,ARG_LEN,varg);
    if(strcmpr("INITIAL", varg[0])) break;
  }
  if(!strcmpr("INITIAL", varg[0])){
    PrintToWindow("Error: INITIAL line marker was not found.", fname1, editor);
    return;
  }

  // get the energy and mass of the particle 

  if (i>5) {
    enrg         = atof(varg[i-1])*1000.;
    NStI.energy  = enrg;
  }
  else { 
    NStI.energy = 8000.;
   };

  // NStI.mass = "$Mp";  // this is currently obtained from the dialog.
                         // dialog needs to parse expression 
  

  static ImportDialog* dialog = 0;

  if (!dialog) {
    dialog = new ImportDialog(0);
  }

  dialog->data_.energy = NStI.energy;
  dialog->data_.mass   = "938.272046"; // default to proton mass

  QFileInfo finfo1(fname1);
  dialog->data_.filenameto  = finfo1.absoluteDir().path() + QDir::separator() + finfo1.completeBaseName() +  ".opt";
  dialog->set();

  if ( dialog->exec() == QDialog::Rejected )  { sw->close(); return;} 

  NStI = dialog->data_;

  /*
  if( NStI.mass[0]=='$'){
     if( NStI.mass == "$Mp" || NStI.mass == "$Me" ) {
       if( NStI.mass == "$Mp")  { 
          mass = 938.2796;
       }
       else { 
          mass = 0.5110034;
       }
     }
     else {
       OptimMessageBox::warning( this, "MAD import", "Particle mass not correctly specified.", QMessageBox::Ok);
       return;
     }
   }
   else { 
     mass = NStI.mass.toDouble();
   }
  */
  
  enrg = NStI.kenergy;
  mass = NStI.mass.toDouble();

  auto fname2 = dialog->data_.filenameto;
  
  std::unique_ptr<FILE, decltype(del)>  fp2 (fopen(fname2.toUtf8().data(),"w"), del);

  if( !fp2 ){
    OptimMessageBox::warning(this, "Error opening file - ", fname2.toUtf8().data(), QMessageBox::Ok);
    return;
  }

  fprintf(fp2.get(),"#Title of the MAD output file: %s\n", header.toUtf8().data());

   // getting initial beta-functions
   if(fgets(buf, LMADBUF, fp1.get())==0){ // reading empty line

maderr:    	
     PrintToWindow("Premature end of file at INITIAL Element ", "", editor);
     return;
   }

   if(fgets(buf, LMADBUF, fp1.get())==0)goto maderr; // initial x-betas
   sscanf(buf, "%le %le %le %le %le ", &alx0, &btx0, &mux, &Dx0, &Dxp0);

   if(fgets(buf, LMADBUF, fp1.get())==0)goto maderr; // initial y-betas
   sscanf(buf, "%le %le %le %le %le ", &aly0, &bty0, &muy, &Dy0, &Dyp0);

   if(fgets(buf, LMADBUF, fp1.get())==0)goto maderr; // reading empty line

   // Reading beamline element data in sequence 

   // allocate mad element list.
   std::vector<MadElm> v(nelem);
   

   PrintToWindow("Reading MAD8 TWISS Table file"," ", editor);

   for(int i=0; i<nelem; ++i){
     if(fgets(buf, LMADBUF, fp1.get())==0){

 maderr1:
        sprintf(buf, "Premature end of file at Element %d of %d",i , nelem);
	PrintToWindow(buf, "", editor);
      	return;
      }

      sscanf(buf, "%4s%s %le %le %le %le ", &(v[i].type[0]), &(v[i].name[0]), &v[i].L, &v[i].ang, &v[i].k1, &v[i].k2);
      v[i].name[9] = 0; // truncate long name
      v[i].type[4] = 0;

      for(j=0; j<MAD_TYPE_LEN; ++j) {  // find madtype
	if(strcmpr(v[i].type, mad_type[j])) break;
      } 

      if( j == MAD_TYPE_LEN) { // madtype was not found 
      	sprintf(buf, "Line %d : element %s type %s is unsupported type.", (5*i+8), v[i].name, v[i].type);
        PrintToWindow(buf, "", editor);
        sprintf(buf, "DRIFT has been substituted.");
	PrintToWindow(buf, "", editor);
	strcpy(v[i].type, "DRIF");
	v[i].ang = 0.0;
	v[i].k1  = 0.0,
	v[i].k2  = 0.0;
	if(i>1){
            sprintf(buf, "upstream element is: %s %s ",v[i-1].type, v[i-1].name);
            PrintToWindow(buf, "", editor);
        }
	
        // strcpy(v[i].type, mad_type[j]);
	// return; // returning here treats the unreconized element an ERROR and stops conversion ! 
      }
      else strcpy(v[i].type, mad_type[j]);

      if(fgets(buf, LMADBUF, fp1.get())==0)  goto maderr1;
      sscanf(buf, "%le %le %le %le %le ", &v[i].tilt, &v[i].e1, &v[i].e2, &v[i].h1, &v[i].h2);

      if(fgets(buf, LMADBUF, fp1.get())==0)  goto maderr1;
      sscanf(buf, "%le %le %le %le %le ", &alx, &btx, &mux, &Dx, &Dxp);

      if(fgets(buf, LMADBUF, fp1.get())==0)  goto maderr1;
      sscanf(buf, "%le %le %le %le %le ", &aly, &bty, &muy, &Dy, &Dyp);

      if(fgets(buf, LMADBUF, fp1.get())==0)  goto maderr1;
      sscanf(buf, "%le %le %le %le %le ", &X, &PX, &Y, &PY, &s);

      v[i].gap1=v[i].gap2=0;
   }
//			D. McGinnis stripper format
//			sscanf(buf,
//"%d %s %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %s %le %le %le %le %le %le %le ",
//				&k, 		v[i].name, 		&s,
//				&btx, 	&alx, 		&mux, 	&Dx, 		&Dxp,
//				&bty, 	&aly, 		&muy, 	&Dy, 		&Dyp,
//				&x0, 		&xp0, 		&y0, 		&yp0, 	v[i].type,
//				&v[i].L, &v[i].ang, 	&v[i].k1,&v[i].k2,&v[i].tilt,
//				&v[i].e1, &v[i].e2);
   // OptiM prefix
   fprintf(fp2.get(),"OptiM\n");
   fprintf(fp2.get(),"# Converted from MAD output file: %s\n", fname1);
   fprintf(fp2.get(),"Energy[MeV]=%-8.8lg   Mass[MeV]=%-8.8lg   \n",enrg,mass);
   fprintf(fp2.get(),"Emittance: ex[cm]=%g  ey[cm]=%g  DP/P=%g \n",2.e-7,2.e-7,2.e-5);
   fprintf(fp2.get(),"Initial:\tBetaX[cm]=%g \tBetaY[cm]=%g \n", 100.*btx0, 100.*bty0);
   fprintf(fp2.get(),"\tAlphaX=%g \tAlphaY=%g \n",alx0,aly0);
   fprintf(fp2.get(),"\tDispersX[cm]=%g \tDispersY[cm]=%g \n",100.*Dx0, 100.*Dy0);
   fprintf(fp2.get(),"\tDsp_PrimeX=%g \tDspPrimeY=%g \n",Dxp0,Dyp0);
   fprintf(fp2.get(),"\tX[cm]=%-8.4lf \tY[cm]=%-8.4lf \tZ[cm]=%-8.4lf \tS[cm]=%-8.4lf \n",0.,0.,0.,0.);//,x,y,z,s0);
   fprintf(fp2.get(),"\ttetaX[deg]=%-8.8lg \ttetaY[deg]=%-8.8lg \n",0.,0.);// tetx,tety);
   
   // Writing lattice
   PrintToWindow("Writing lattice ..."," ", editor);
   fprintf(fp2.get(),"begin lattice. Number of periods=1 \n");

   for(i=k=0; i<nelem; i++){
      if(MakeMadName(v[i].optnm, v[i].name, v[i].type)) {
	sprintf(buf, "Unknown Element type <> at line %d", i*5+8);
	PrintToWindow("%s",buf, editor);
      }
      
      switch (v[i].optnm[0]) {
        case 'b':
	  dk=3*strlen(v[i].optnm);
          break;
        case 'm':
	  dk=2*strlen(v[i].optnm);
          break;
        default:
	  dk=strlen(v[i].optnm);
          break;
      }
      if(dk+k>70){fprintf(fp2.get(), "\n"); k=0;}
      k=k+dk;
      switch(v[i].optnm[0]){
        case 'b':
	  fprintf(fp2.get(), "g%s %s G%s ", v[i].name, v[i].optnm, v[i].name);
          break;
        case 'm':
	  fprintf(fp2.get(), "%s %sS ", v[i].optnm, v[i].optnm);
          break;
        default:
	  fprintf(fp2.get(), "%s ", v[i].optnm);
          break;
      }
   }
   if(k!=0) fprintf(fp2.get(), "\n");

   // Writing Elements
   PrintToWindow("Writing Element parameters ..."," ", editor);
   fprintf(fp2.get(),"end lattice \nbegin list \n");
   for(i=k=0; i<nelem; i++) {
      if(i>0)for(j=0; j<i; j++) {
	k=cmdpMadElem(&v[j],&v[i]);
	if(k){
	  if(k==2){
	    sprintf(buf,"Elements at lines %d and %d have identical names %s but different parameters",
                        i,j,v[i].optnm);
            PrintToWindow("Done. ",fname2.toUtf8().data(), editor);
          }
          break;
      	}
      }
      DecodeMadLine(&enrg, &Hrt, mass, &v[i], &el, 0);
      if(k)continue;
      print_elm(&el, buf);
      fprintf(fp2.get(), "%s\n", buf);
      if(v[i].optnm[0]=='b'){
      	DecodeMadLine(&enrg, &Hrt, mass, &v[i], &el, 1);
        print_elm(&el, buf);
        fprintf(fp2.get(), "%s\n", buf);
        DecodeMadLine(&enrg, &Hrt, mass, &v[i], &el, 2);
        print_elm(&el, buf);
        fprintf(fp2.get(), "%s\n", buf);
      }
      if(v[i].optnm[0]=='m'){
      	DecodeMadLine(&enrg, &Hrt, mass, &v[i], &el, 1);
        print_elm(&el, buf);
        fprintf(fp2.get(), "%s\n", buf);
     }
   }

   fprintf(fp2.get(),"end list \n");
   PrintToWindow("Done.",fname2.toUtf8().data(), editor);
   editor->document()->setModified(false);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

