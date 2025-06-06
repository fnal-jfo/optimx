//  =================================================================
//
//  Analyze.cpp
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
#include <OptimCalc.h>
#include <OptimEditor.h>
#include <OptimMainWindow.h>
#include <OptimMessages.h>
#include <Utility.h>

#include <QDir>
#include <QSettings>
#include <QTextBlock>

#include <iostream>
#include <cfloat>
#include <string>

// std::regex not supported if g++ < 4.9.X MSV C++ < v15.0
#if GCC_VERSION >= 40900 || _MSC_VER >= 1900 ||  __clang__ 
#include <regex>
namespace compat = std;
#else
#include <boost/regex.hpp>
namespace compat = boost;
#endif

static const unsigned int LSTR = 1024; // max string length (including 0 end marker) 

using std::acosh;

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

static int ierr = 0; // error code 

#define MAX_NMB_WHILE_CYCLES 10000

static int const nmaxattr = 12; // max no of attributes

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdAnalyze()
{

  if (!LatticeCh_)  return; 

  QMessageBox::StandardButton  ret = 
          OptimMessageBox::warning( this, "OptiM",
			       "Analyze Lattice will destroy relation to the variables. Do you want to continue ?",
                               QMessageBox::Yes | QMessageBox::No);
  
  if ( ret ==  QMessageBox::No ) { 
       interrupted_ = false; 
       return;
  }
  
  bool s = CtSt_.RewriteBuf;
  CtSt_.RewriteBuf = true;
  analyze(true);
  CtSt_.RewriteBuf = s;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// This is still experimental code. The ultimate goal is for each beamline element type to correspond to a class derived from
// the base class Element. Once this is accomplished, Element will become a pure interface class. 
// A new mechanism is needed for errors. Right now  OptimMessageBox::warning is called when the element parameters are set.
// The element parameters should be sanitized first.

int OptimMainWindow::analyzeElement(OptimEditor* editor, int nline, char *buf, int nmtr, std::shared_ptr<Element>& ep)
{
  double dat[nmaxattr];
  std::vector<double> vdat;
  char buferr[1024];       // buffer holding element attributes (as a string) 
  
  int np = 0; // number of parameters in the dat[] array;

  //Utility::decodeLineNew(buf,dat,nmaxattr); //**** DEBUG

  //if( (np = Utility::decodeLineNew(buf,vdat, nmaxattr) )== -1) { // DEBUG
  if( (np = Utility::decodeLine(buf,dat, nmaxattr) )== -1) { // DEBUG
   	sprintf(buferr,"An argument value exceeds the allowed limit %e on line %d <%s>",
      	                1.0e-6*DBL_MAX, nline, buf);
        editor->highlightCurrentBlock();
	OptimMessageBox::warning(this, "OptiM", (QString("Parsing Error: ")+ QString(buferr)).toUtf8().data(), QMessageBox::Ok); 
      return 1;
  }
  
  ep = std::shared_ptr<Element>(Element::makeElement(ep->name()));
  
  if (dynamic_cast<GCavity*>(ep.get()) ) {


    //    ep->setParameters(np, vdat, &ext_dat[ep->N], CtSt_.NStep);
    ep->setParameters(np, dat, &ext_dat[ep->N], CtSt_.NStep);

    if(ext_dat[ep->N].n==0) {
      replaceExistingLine(editor, nline-1);
      editor->highlightCurrentBlock();
      sprintf(buf,"File reference %d not found.",ep->N);
      OptimMessageBox::warning(this, "Conversion Error", buf,QMessageBox::Ok);
      return 1;
    }

    if(ep->tilt() <=0.00001 ){
      replaceExistingLine(editor, nline-1);
      OptimMessageBox::warning(this, "OptiM", "Wavelength must be positive", QMessageBox::Ok);
      return 1;
    }

    if ( fabs( ext_dat[ep->N].x[ext_dat[ep->N].n-1] -
               ext_dat[ep->N].x[0]-ep->length())>0.0001 )   {
      replaceExistingLine(editor, nline-1);
      editor->highlightCurrentBlock();
      sprintf(buf,"Cavity length of %g is not equal to its value of %g from file %d",
	           ext_dat[ep->N].x[ext_dat[ep->N].n-1]-
		   ext_dat[ep->N].x[0], ep->length(), ep->N);
      OptimMessageBox::warning(this, "OptiM", (QString("Conversion Error: ")+ QString(buf)).toUtf8().data(), QMessageBox::Ok);  
      return 1;
    }

    return 0;
 }

  //-------------------------------------------------------------------------------
  // NOTE: at this point, the matrices for the 'X' type elements are NOT YET initialized. 
  // This means that the element cannot hold a copy of the matrix; rather it
  // holds a pointer to it. For now we initialize with a blank matrix.
  //-------------------------------------------------------------------------------
  
  if (dynamic_cast<XferMatrix*>(ep.get()) ) {
    RMatrix* rm = 0;
    // ep->setParameters(np, vdat, rm );
    ep->setParameters(np, dat, rm );
    return 0;
  }

  //  ep->setParameters(np, vdat);  // default
  ep->setParameters(np, dat);  // default

  if (dynamic_cast<PCavity*>(ep.get()) ){
    if(ep->N <= 1.0) ep->N=1.0;
    if(ep->tilt() <= 0.00001){
      editor->highlightCurrentBlock();
      replaceExistingLine(editor, nline-1);
      OptimMessageBox::warning(this, "OptiM", "RF Wavelength must be positive", QMessageBox::Ok);
      return 1;
   } 
  }


  return 0;
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#define NBB_NAME 32

int OptimMainWindow::analyze(bool Reprint, int NmbTurn)
{

 auto& IncludeMode      = appstate.IncludeMode;
 auto& IncludeFileName  = appstate.IncludeFileName;
 auto& InclLineNum      = appstate.InclLineNum;
   
 double dat[7];
 double result;
 //char   buf[LSTR+1];
 //char   buf1[LSTR+1];
 char   buf[LSTR];
 char   buf1[LSTR]; 
 char   buf2[NAME_LENGTH];
 char*  bufpt;
 char*  bufpttmp;
 //char   str_res[LSTR+1];
 char   str_res[LSTR];
 int    nline = 0;
 int    DoWhileFirstLine = 0;
 int    NmbWhileCycles   = 0;
 int    i, j, k, nmtr;

 char nmNp[NBB_NAME];
 char nmemx[NBB_NAME];
 char nmemy[NBB_NAME];
 char nmsigmas[NBB_NAME];
 char nmScolis[NBB_NAME];
 char nmnslice[NBB_NAME];

 
 if (!LatticeCh_) return 1;
 OptimEditor* editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());     
 if (!editor) return 1;

 editor->clearHighlightedBlocks();

 // CurLine  = editor->GetLineFromPos(-1);
 QTextCursor cursor =  editor->textCursor();

 int CurLine  = cursor.blockNumber();
 int nmblines = editor->document()->blockCount()-1;
 
 if(nmblines <13 ){ 
    OptimMessageBox::warning(this,"Conversion Error", "File is not an OptiM file.", QMessageBox::Ok);
    return 1;
 }
 
 // Parse math header 
 
 IncludeMode = false;
 calc_.zeroCalc();
 sprintf(buf, "$_turn=%d", NmbTurn);
 calc_.calcLine(buf, &result, "%12.9lg", str_res, buf1);
 
 while(true){
   nline = getLineCalc(editor, buf, LSTR, nline);

   if(nline<0){
     OptimMessageBox::warning(this,"Conversion Error", "Missing \"OptiM\" delineator.", QMessageBox::Ok);
     return 1;
   }
   if(strcmpr("OptiM",buf)) break;
   if(strcmpr("do{",buf)){
     if(IncludeMode){
       OptimMessageBox::warning(this,"Syntax Error", "do{...}while loops are not allowed in include files", QMessageBox::Ok);
       return 1;
     }
     if(DoWhileFirstLine){
       OptimMessageBox::warning(this, "Syntax Error", "Nested while loops are not allowed", QMessageBox::Ok);
       return 1;
     }
     DoWhileFirstLine=nline;
     continue;
   }
   if(strcmpr("}while",buf)){
     if(IncludeMode){
       OptimMessageBox::warning(this, "Syntax Error", "do{...}while loops are not allowed in include files", QMessageBox::Ok);
       return 1;
     }
     if(!DoWhileFirstLine){
       OptimMessageBox::warning(this, "Syntax Error", "}while encountered before do{", QMessageBox::Ok);
       return 1;
     }
     i=calc_.calcLine(buf+6, &result, "%12.9lg", str_res, buf1);
     if(i>0){
       replaceLine(editor, nline-1, buf);
       editor->highlightCurrentBlock();
       OptimMessageBox::warning(this, "Calculator Error", buf1, QMessageBox::Ok);
       return 1;
     }
     if((result - 1.0e-10) > 0) {
       if(NmbWhileCycles++ >  MAX_NMB_WHILE_CYCLES){
         OptimMessageBox::warning(this,"Syntax Error", "Loop iterations limit [10000] exceeded.", QMessageBox::Ok);
         return 1;
       }
       nline=DoWhileFirstLine;
     }
     else DoWhileFirstLine=0;
     continue;
   }
   bufpt=buf; 
   
   while(*bufpt){ 
     if(*bufpt==';' || *bufpt=='#'){*bufpt='\0'; break;}
     bufpt++;
   }

   i=calc_.calcLine(buf, &result, "%12.9lg", str_res, buf1);

   if(i>0){
     if(!IncludeMode){
       replaceLine(editor, nline-1, buf);
       editor->highlightCurrentBlock();
       OptimMessageBox::warning(this, "Calculator Error", buf1, QMessageBox::Ok);
       return 1;
     }
     else {
       sprintf(str_res,"%s in file %s at line %d", buf1, IncludeFileName, InclLineNum);
       OptimMessageBox::warning(this, "Calculator Error ", str_res, QMessageBox::Ok);
       return 1;
     }
   }
   if(Reprint && CtSt_.RewriteBuf){
     strcat(buf,";  =>"); strcat(buf,str_res);
     replaceLine(editor, nline-1, buf);
   }
 } // while (true)


 // Analysis of OptiM file
 
 lineOptiM = nline;
 nline     = getLineCmt(editor, buf, LSTR, nline); 
 if(nline==-1)return 1;

 decodeLine(buf,dat,2);
 Ein=dat[0];  ms=dat[1];  Hr=sqrt(2.*ms*Ein+Ein*Ein)/C_DERV1;

 if(Reprint && CtSt_.RewriteBuf){
   sprintf(buf,"Energy[MeV]=%-8.8lg   Mass[MeV]=%-8.8lg ",Ein,ms);
   replaceLine(editor, nline-1, buf);
 }

 if((Ein<=0.)&&(ms<=0.)){ 
   OptimMessageBox::warning(this, "Conversion Error", "Energy and/or mass cannot be zero or negative", QMessageBox::Ok);
   return 1;
 }
 nline=getLineCmt(editor, buf, LSTR, nline);  if(nline==-1)return 1;
 if(nline==-1)return 1;
 decodeLine(buf,dat,3);  ex_ =dat[0];  ey_ =dat[1];  dpp_ =dat[2];
 if( Reprint && CtSt_.RewriteBuf){
   sprintf(buf,"Emittance: ex[cm]=%g  ey[cm]=%g  DP/P=%g ",ex_,ey_,dpp_);
   replaceLine(editor, nline-1, buf);
 }

  nline=getLineCmt(editor, buf, LSTR, nline);  if(nline==-1)return 1;
  if(nline==-1)return 1;
  decodeLine(buf,dat,2);  BetaXin=dat[0];  BetaYin=dat[1];
  if( Reprint && CtSt_.RewriteBuf){
    sprintf(buf,"Initial:\tBetaX[cm]=%g \tBetaY[cm]=%g ",BetaXin,BetaYin);
    replaceLine(editor, nline-1, buf);
  }
  if((BetaXin<=0.)&&(BetaYin<=0.)){ 
    OptimMessageBox::warning(this,  "Conversion Error", "Beta function cannot be zero or negative", QMessageBox::Ok);
    return 1;
  }

  nline=getLineCmt(editor, buf, LSTR, nline);   if(nline==-1)return 1;
  if(nline==-1)return 1;
  decodeLine(buf,dat,4);  AlfaXin=dat[0];  AlfaYin=dat[1]; QXin=dat[2];  QYin=dat[3];
  if(Reprint&&CtSt_.RewriteBuf){
    sprintf(buf,"\tAlphaX=%g \tAlphaY=%g  Qx=%g  Qy=%g",AlfaXin,AlfaYin,QXin,QYin);
    replaceLine(editor, nline-1, buf);
  }

  nline=getLineCmt(editor, buf, LSTR, nline);   if(nline==-1)return 1;
  if(nline==-1)return 1;
  decodeLine(buf,dat,2);  DispXin=dat[0];  DispYin=dat[1];
  if(Reprint&&CtSt_.RewriteBuf){
    sprintf(buf,"\tDispersX[cm]=%g \tDispersY[cm]=%g ",DispXin,DispYin);
    replaceLine(editor, nline-1, buf);
  }

  nline=getLineCmt(editor, buf, LSTR, nline);    if(nline==-1)return 1;
  if(nline==-1)return 1;
  decodeLine(buf,dat,2);  DispPrimeXin=dat[0];  DispPrimeYin=dat[1];
  if(Reprint&&CtSt_.RewriteBuf){
    sprintf(buf,"\tDsp_PrimeX=%g \tDspPrimeY=%g ",DispPrimeXin,DispPrimeYin);
    replaceLine(editor, nline-1, buf);
  }

  nline=getLineCmt(editor, buf, LSTR, nline);    if(nline==-1)return 1;
  if(nline==-1)return 1;
  decodeLine(buf,dat,4);
  xo0_ = dat[0];  yo0_ = dat[1];   zo0_ =dat[2];  so0_ = dat[3];

  if(Reprint&&CtSt_.RewriteBuf){
    sprintf(buf,"\tX[cm]=%-8.3lf \tY[cm]=%-8.3lf \tZ[cm]=%-8.3lf \tS[cm]=%-8.3lf ",xo0_, yo0_, zo0_, so0_);
    replaceLine(editor, nline-1, buf);
  }

  nline=getLineCmt(editor, buf, LSTR, nline);   if(nline==-1)return 1;
  if(nline==-1)return 1;
  decodeLine(buf,dat,2);
  tetaXo0_ = dat[0];  tetaYo0_ = dat[1];
  if(Reprint&&CtSt_.RewriteBuf){
    sprintf(buf,"\ttetaX[deg]=%-8.8lg \ttetaY[deg]=%-8.8lg ",tetaXo0_, tetaYo0_);
    replaceLine(editor, nline-1, buf);
  }

  // check for references to files
  
  nline = getLineCmt(editor, buf, LSTR, nline);
  if(nline==-1) return 1;

  if(strcmpr("file reference start",buf)) {

    ext_dat.resize(0);

    while(1){
      nline = getLineCmt(editor, buf, LSTR, nline);
      if(nline<0) { 
        OptimMessageBox::warning(this,"Conversion Error", "The delineator \"file reference end\" is missing", QMessageBox::Ok);
        return 1;
      }
    
      if(strcmpr("file reference end",buf)){
        nline=getLineCmt(editor, buf, LSTR, nline);   if(nline==-1)return 1;
        break;
      }
    
      if(getDataFromFile(buf)){
        replaceExistingLine(editor, nline-1);
        return 1;
      }
    }
  }
  
  // Count Elements in the lattice structure
  
  if(!strcmpr("begin lattice",buf)){
    OptimMessageBox::warning(this, "Conversion Error", "The line must be: begin lattice", QMessageBox::Ok);
    replaceExistingLine(editor, nline-1);
    return 1;
  }
  decodeLine(buf,dat,1);  NmbPer=dat[0];  if(NmbPer<1)NmbPer=1;
  if(Reprint&&CtSt_.RewriteBuf){
    sprintf(buf,"begin lattice. Number of periods=%d ",NmbPer);
    replaceLine(editor, nline-1, buf, QColor(Qt::black) );    // *** FIXME ***  
  }
  // Deletes previously allocated memory
  
  beamline_.resize(0); // should we just clear ?? 
  elmdict_.resize(0);

  Npp.resize(0);       
  emxp.resize(0);      
  emyp.resize(0);      
  sigmasp.resize(0);    
  Scolisp.resize(0);   
  nslicep.resize(0);  

  nelm_  = Nbb = 0;
  LineIn = nline;
  i=0;
  do{
    nline = getLineCmt(editor, buf, LSTR, nline);  if(nline==-1)return 1;
    bufpt = buf;
    if(nline<0) return 1;
    if (strcmpr("end lattice", buf)) break;

    //Looks for beam-beam Elements which has to be described in the first line
    if(!(i++)) {
      if(strcmpr("_B_BEAM",buf)) {
        nline--;
        nline=getLineCalc(editor, buf, LSTR, nline);
        if(nline==-1)return 1;
        bufpt = buf+7; // skips _B_BEAM  statement
        while (*bufpt=='(' || isspace(*bufpt)) bufpt++; //skips spaces and "("
        bufpttmp=bufpt;
        while (*bufpt!='0' && *bufpt!='\n' && *bufpt!=',')bufpt++; //Finds variable end
        *bufpt=0;
        bufpt=bufpttmp;
        if(calc_.calcLine(bufpt, &result, "%12.9lg", str_res, buf1)){
          sprintf(str_res,"Error in definition of number of IPs in the beam-beam statament <%s>", buf1);
          OptimMessageBox::warning(this, "Conversion Error", buf, QMessageBox::Ok);
          return 1;
        }
        Nbb = (int)(result+.01);
        if(Nbb>1000 || Nbb<=0){
          sprintf(str_res,"Error: number of IPs in the beam-beam statament =%d but has to be in the range of [1, 999]", Nbb);
          OptimMessageBox::warning(this, "Conversion Error", buf, QMessageBox::Ok);
          return 1;
        }
        nline=getLineCmt(editor, buf, LSTR, nline);
        if(nline==-1)return 1;
        bufpt = buf;
        if (strcmpr("end lattice",buf)) break;
      }
    }
    // Count Elements in the beamline

    //while( getElmName(ElemNameFCh,&bufpt,buf1, buf2) ==0 ){

    while( getElmNameX(bufpt, buf1, buf2) ) { ++ nelm_; }
    //if(buf1[NAME_LENGTH-1]!=0){
    if( (*buf1) && !isValidType(buf1) ) {
      if(!IncludeMode){
        replaceExistingLine(editor, nline-1);
        editor->highlightCurrentBlock();
        //sprintf(str_res,"No Element with name <%s> on line %d : <%s>", nline-1, buf1, buf);
        sprintf(str_res,"No Element with name <%s> on line %d : <%s>", buf2, nline-1, buf);
        OptimMessageBox::warning(this,"Conversion Error", str_res, QMessageBox::Ok);
        return 1;
      }
      else{
        sprintf(str_res,"No Element with name <%s> on line %d <%s> in file %s at line",
                  		buf1, InclLineNum, buf, IncludeFileName);
        OptimMessageBox::warning(this,"Conversion Error", buf, QMessageBox::Ok);
	return 1;
      }
    }
  } while (nline<nmblines);

  // Count Elements in the Element list
  
  nline=getLineCmt(editor, buf, LSTR, nline);
  if(nline==-1)return 1;
  if(!strcmpr("begin list",buf)){
    OptimMessageBox::warning(this,"Conversion Error", "The line must be: begin list", QMessageBox::Ok);
    replaceExistingLine(editor, nline-1);
    return 1;
  }
  
  if( Reprint && CtSt_.RewriteBuf) {
    sprintf(buf,"begin list");
    replaceLine(editor, nline-1, buf);
  }

  nelmlist_ = 0;             // Number of different Elements (and matrices) in the Element list
  LineLIn   = nline;         // Number of First line for list of Elements in TEdit.

  do{
    nline = getLineCmt(editor, buf, LSTR, nline);   
    if (nline == -1) return 1;
    if(strcmpr("end list",buf)) break;

    ++nelmlist_;
    bufpt = buf;
    //getElmName(ElemNameFCh, &bufpt, buf1, buf2);
    getElmNameX(bufpt, buf1, buf2);
    if( toupper(*buf1)=='X') {
      for(i=0; i<6; ++i){
         nline = getLineCmt(editor, buf, LSTR, nline);
         if(nline==-1) return 1;
      }
    }
  } while (nline<nmblines);

  if(!strcmpr("end list",buf)){
    OptimMessageBox::warning(this, "Parser Error", "\"end list\" statement not found.", QMessageBox::Ok);
    replaceExistingLine(editor, nline-1);
    return 1;
  }

  LineLFin = nline-2;

  // allocate memory
  
  nelm_     += 2*Nbb; // beamline 
  nelmlist_ += 3*Nbb; // elm dictionary
  
  beamline_.resize(nelm_);
  elmdict_.resize(nelmlist_);  // Array which Length_ is equal to  nelmlist_

  // ***FIXME***
  //if(!(beamline_ && elmdict_ )) {
  //  OptimMessageBox::warning(this,"Conversion Error", "Can not get enough memory for the task", QMessageBox::Ok);
  // }

  // Get beam-beam parameters if the beam-beam statement is present
  
  if(Nbb){
    nline = LineIn;       // First line for list of Element names in TEdit.
    nline = getLineCalc(editor, buf, LSTR, nline);
    bufpt = bufpttmp;   // Skips _B_BEAM statement, ( and spaces
    while (*bufpt!='0' && *bufpt!='\n' && *bufpt!=',')bufpt++; //Finds end of Nbb expression
    if(*bufpt==',') bufpt++;
    else {
      editor->highlightCurrentBlock();
      OptimMessageBox::warning(this,"Conversion Error", "No comma in _B_BEAM statement after number of bunches", QMessageBox::Ok);
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    // Gets array name for number of particles in each bunch
     
    bufpt = GetVariableNameSp(nmNp, bufpt); // gets array name for N_part
    if(calc_.findValue(nmNp, &result,"%-15.10le", buf1)!=-2){
      editor->highlightCurrentBlock();
      OptimMessageBox::warning(this,"Conversion Error", "Number of particles in _B_BEAM statement is not determined by array", QMessageBox::Ok);
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    if(int(result+0.01)<Nbb){
      OptimMessageBox::warning(this,"Conversion Error", "Array size for number of particles in _B_BEAM statement is smaller than the number of collision points", QMessageBox::Ok);
      editor->highlightCurrentBlock();
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    
    // Gets array name for X emmitance of each bunch
    bufpt = GetVariableNameSp(nmemx, bufpt); // gets array name for X emmitance
    if(calc_.findValue(nmemx, &result,"%-15.10le", buf1)!=-2){
      editor->highlightCurrentBlock();
      OptimMessageBox::warning(this,"Conversion Error", "Hor. emittance in _B_BEAM statement is not determined by array", QMessageBox::Ok);
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    if(int(result+.01)<Nbb){
      editor->highlightCurrentBlock();
      OptimMessageBox::warning(this,"Conversion Error", "Array size for hor. emittance in _B_BEAM statement is smaller than the number of collision points", QMessageBox::Ok);
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    
    // Gets array name for Y emmitance of each bunch
    bufpt = GetVariableNameSp(nmemy, bufpt); // gets array name for Y emmitance
    if(calc_.findValue(nmemy, &result,"%-15.10le", buf1)!=-2){
      editor->highlightCurrentBlock();
      OptimMessageBox::warning(this,"Conversion Error", "Vert. emittance in _B_BEAM statement is not determined by array", QMessageBox::Ok);
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    if(int(result+.01)<Nbb){
      OptimMessageBox::warning(this,"Conversion Error", "Array size for vert. emittance in _B_BEAM statement is smaller than the number of collision points", QMessageBox::Ok);
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    // Gets array name for sigma_s of each bunch
    bufpt = GetVariableNameSp(nmsigmas, bufpt);
    if(calc_.findValue(nmsigmas, &result,"%-15.10le", buf1)!=-2){
      editor->highlightCurrentBlock();
      OptimMessageBox::warning(this, "Conversion Error", "Bunch lengths in _B_BEAM statement is not determined by array", QMessageBox::Ok);
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    
    if(int(result+.01)<Nbb){
      editor->highlightCurrentBlock();
      OptimMessageBox::warning(this,"Conversion Error", "Array size for bunch lengths in _B_BEAM statement is smaller than the number of collision points", QMessageBox::Ok);
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    
    // Gets array name for number of slices in each bunch
    bufpt=GetVariableNameSp(nmnslice, bufpt);
    if(calc_.findValue(nmnslice, &result,"%-15.10le", buf1)!=-2){
      editor->highlightCurrentBlock();
      OptimMessageBox::warning(this,"Conversion Error", "Number of slices in _B_BEAM statement is not determined by array", QMessageBox::Ok);
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    
    if(int(result+.01)<Nbb){
      OptimMessageBox::warning(this, "Conversion Error", "Array size for number of slices in _B_BEAM statement is smaller than the number of collision points", QMessageBox::Ok);
      editor->highlightCurrentBlock();
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    
    // Gets array name for long. coordinates of each collision point
    
    bufpt=GetVariableNameSp(nmScolis, bufpt);
    if(calc_.findValue(nmScolis, &result,"%-15.10le", buf1)!=-2){
      editor->highlightCurrentBlock();
      OptimMessageBox::warning(this,"Conversion Error", "Long. IP pos. in _B_BEAM statement is not determined by array", QMessageBox::Ok);
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    
    if(int(result+.01)<Nbb){
      editor->highlightCurrentBlock();
      OptimMessageBox::warning(this, "Conversion Error", "Array size for long. IP pos. in _B_BEAM statement is smaller than the number of collision points", QMessageBox::Ok);
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    
    while (isspace(*bufpt))bufpt++;  // skip spaces
    if(*bufpt!=')'){
      editor->highlightCurrentBlock();
      OptimMessageBox::warning(this,"Conversion Error", "Closing \" ) \" are not found in _B_BEAM statement", QMessageBox::Ok);
      replaceExistingLine(editor, nline-1);
      return 1;
    }
    // Set parameters for each bunch
    
    Npp.resize(Nbb);      
    emxp.resize(Nbb);     
    emyp.resize(Nbb);     
    sigmasp.resize(Nbb);  
    nslicep.resize(Nbb);  
    Scolisp.resize(Nbb);  
    
    for(i=0; i<Nbb; ++i){
      calc_.FindValueInArray(nmNp,     i, &Npp[i]);
      calc_.FindValueInArray(nmemx,    i, &emxp[i]);
      calc_.FindValueInArray(nmemy,    i, &emyp[i]);
      calc_.FindValueInArray(nmsigmas, i, &sigmasp[i]);
      calc_.FindValueInArray(nmnslice, i, &Scolisp[i]);
      nslicep[i]=(int)(Scolisp[i]+.0001);
      calc_.FindValueInArray(nmScolis, i, &Scolisp[i]);
      if((emxp[i]<=0.) || (emyp[i]<=0.) || (sigmasp[i]<=0.) || (Scolisp[i]<-1.e-7) || (nslicep[i]<=0) || (nslicep[i]>=32) ){
        OptimMessageBox::warning(this,"Conversion Error", "Incorrect value of bunch parameter in the beam-beam statement", QMessageBox::Ok);
        return 1;
      }
      if(i>0)if(Scolisp[i]<=Scolisp[i-1]){
	  OptimMessageBox::warning(this,"Conversion Error", "Longitdinal bunch positions in the beam-beam statement do not grow monotonically", QMessageBox::Ok);
        return 1;
      }
    }
  }
  
  // Analyze Elements in the Element list
 
  int ie= 0;  // counter for the number of Element in the list of Elements

  nline = LineLIn;  // First line for list of Elements in TEdit.
  j=nmtr=0;

  do{
    nline = getLineCmt(editor, buf, LSTR, nline);
    if(nline==-1) return 1;
    if(strcmpr("end list", buf)) break;
    bufpt = buf;
    //if( getElmName(ElemNameFCh, &bufpt, buf1, buf2) == 1) {
    getElmNameX(bufpt, buf1, buf2 );
    if( (*buf1) && !isValidType(buf1) ) {
      if(!IncludeMode){
        sprintf(str_res,"Invalid Element type <%s> detected on line %d", buf1, nline-1);
        editor->highlightCurrentBlock();
        OptimMessageBox::warning(this,"Conversion Error", str_res, QMessageBox::Ok);
        replaceExistingLine(editor, nline-1);
        return 1;
      }
        else {
        sprintf(str_res,"No Element labeled <%s> on line %d <%s> in file %s",
            			buf1, InclLineNum, buf, IncludeFileName);
        OptimMessageBox::warning(this,"Conversion Error", str_res, QMessageBox::Ok);
        return 1;
      }
    }

    elmdict_[ie] = std::make_shared<Element>();
    elmdict_[ie]->name(buf1);

    if (analyzeElement(editor, nline, buf, nmtr, elmdict_[ie] ) ) return 1;
    print_elm(elmdict_[ie].get(), buf);

    if(Reprint && CtSt_.RewriteBuf) replaceLine(editor, nline-1, buf);

    if( elmdict_[ie]->etype() =='X'){

      RMatrix tm;
      for(int m=0; m<6; ++m){
         nline = getLineCmt(editor, buf, LSTR, nline);
	 if(nline<0)return 1;
         bufpt = buf;
         for(int n=0; n<6; ++n) {
	   decodeNumber(bufpt, tm[m][n]);
	 }
	 if(!bufpt) break;

         if(Reprint && CtSt_.RewriteBuf) {
            sprintf(buf,"%g\t%g\t%g\t%g\t%g\t%g ",
		   tm[m][0], tm[m][1], tm[m][2], tm[m][3], tm[m][4], tm[m][5]);
            replaceLine(editor, nline-1, buf);
         }
     }
      
      std::dynamic_pointer_cast<XferMatrix>(elmdict_[ie])->setMatrix(tm); 
   }

   ++ie;

  } while (nline<nmblines);

  // Construct beamline and compute its length. Insert beam-beam Elements if any have been specified 
   
  nline = LineIn;
  if(Nbb) nline++;
  i        = 0;
  Length_  = 0.;
  int nbb  = 0;  // index of current number beam-beam element

  do {

    nline = getLineCmt(editor, buf, LSTR, nline);  if(nline==-1) return 1;

    bufpt = buf;

    if(nline<0) return 0;
    if(strcmpr("end lattice",buf)) break;

    // Count Elements
 
    //while( getElmName(ElemNameFCh, &bufpt, buf1, FullElmName[i]) == 0 ) {

    char fullname[128];  
    while(  getElmNameX( bufpt, buf1, fullname) ) {

      for(j=0; j < nelmlist_-3*Nbb; ++j) { // the dict contains a max of nelmlist_ + 3*Nb since each collision point will become 2 el + 1 bb kick 
	if(!strcmp(buf1, elmdict_[j]->name())) break;
      }

      if (j  >=  nelmlist_-3*Nbb ) { 
      	if(!IncludeMode){
	  replaceExistingLine(editor, nline-1);
	  editor->highlightCurrentBlock();
	  sprintf(buf,"No Element labeled  %s in the Element list.",buf1);
	  OptimMessageBox::warning(this,"Conversion Error", buf, QMessageBox::Ok);
	  return 1;
        }
	  sprintf(buf,"No Element labeled %s in the Element list. File: %s, line: %d.",
                     			buf1, IncludeFileName, InclLineNum);
	  OptimMessageBox::warning(this, "Conversion Error", buf, QMessageBox::Ok);
	  return 1;
      }

      if(Nbb)  {
	if(nbb<Nbb)  {
	  if(elmdict_[j]->length() > 0.0 ) {
	    if( (Length_ + elmdict_[j]->length()) > Scolisp[nbb] ) { //  Scolisp = position of bb-kick nbb

	      // what follows is a rather convoluted mess. The outcome is that element j is split in two if a beam-beam kick position
	      // falls within it. A 0-length bb kick is added at the point where the elemen was split.        
	      
	      // set parameters for the upstream and dowstream elements.  

              elmdict_[ie  ] = std::shared_ptr<Element>(elmdict_[j]->clone());
              elmdict_[ie  ]->fullName((QString(fullname ) + ".1"  ).toUtf8().data());    
	      elmdict_[ie+2] = std::shared_ptr<Element>(elmdict_[j]->clone());
              elmdict_[ie+2]->fullName((QString(fullname ) + ".2"  ).toUtf8().data());    

	
              elmdict_[ie]->length(Scolisp[nbb] - Length_);
              elmdict_[ie+2]->length(elmdict_[j]->length() - elmdict_[ie]->length());

	      beamline_[i++]     = elmdict_[ie++]; // upstream half element

	
              sprintf(fullname,"_bb.%-d",nbb+1);  // note that i has been incremented ! 
  
              // .......................................................................................
	
	      std::shared_ptr<BBeam> bbk( dynamic_cast<BBeam*>( Element::makeElement(  fullname ))); // this is UGLY. Fix factory.
              double dat[] = { 0.0, Npp[nbb], emxp[nbb], emyp[nbb], sigmasp[nbb],  0.0, static_cast<double>(nslicep[nbb]) };
	      bbk->setParameters(sizeof(dat)/sizeof(dat[0]), dat );
              elmdict_[ie] = bbk;
	
	      beamline_[i++]  = elmdict_[ie++];   // bb kick
              beamline_[i++]  = elmdict_[ie++];   // downstream half element

	      //Length_ +=  fabs(elmdict_[j]->length());
	      Length_ +=  elmdict_[j]->length();

	      nbb++;
              continue;
            }
	  }
        }
     }

    beamline_[i]  =  std::shared_ptr<Element>(elmdict_[j]->clone());
    beamline_[i]->fullName(fullname);
      
    //Length_       +=  fabs(beamline_[i++]->length()); // we add absolute values. Some lengths may be negative 
    Length_       +=  beamline_[i++]->length();

    if (i > nelm_) break;
    } //  while(  getElmNameX( bufpt, buf1, fullname) )  
  } while (nline<nmblines);

  if(Nbb != nbb){
    OptimMessageBox::warning(this,"Conversion Error", "S coordinate for at least one beam-beam Element is more than the machine length",
    		QMessageBox::Ok);
    return 1;
  }

  beamline_.updateEdges();  //  add info to dipole edges (e.g. upstream or dwnstream of a bend)  
  
   if(CtSt_.CompAtExcitedOrb){

    if( CtSt_.IsRingCh ) {

      if(getTrajParamFromFile(false, false, v_anlz)) return 1; // to get dpp
       closeTraject(v_anlz); 

       if(NstTool.FullClosure) {
	 if (!NstTool.Closure4D) { getTrajParamFromFile(false, false, v_anlz); } // to get all 6D traj parameters
          if (closeTrajectFull(v_anlz)) {
               OptimMessageBox::warning(this,
			      "Close Error", "Cannot close trajectory. Make sure that Twiss parameters can be closed.\n"
                                             "If OK reduce the convergence coefficient.", QMessageBox::Ok);
                return 1;
         }
       }
    }
    else { // this a line
      if(getTrajParamFromFile(false, false, v_anlz)) return 1; // to get dpp
    }
  

    if(analyze2(v_anlz)) return 1; // analyze2 element lengths are different and inferred from the trajectory.  
                                   // NOTE: to get an exact trajectory, the elements may need to be further subdivided   
     double L0 = Length_;
   
     for(i=0, Length_=0.0; i<nelm_; i++){
        auto ep      =  beamline_[i];
        Length_      +=  ep->length();
     }
     DeltaL_        =  Length_-L0;
  }

   //if(Reprint&&CtSt.RewriteBuf){ edclt->GetLine(buf, LSTR, CurLine);
   //				   ReplaceLine(edclt, CurLine, buf);  }
 
   analyzed_ = true;
   return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void OptimMainWindow::cmdAnalyzeHeader ()
{                                      
 auto& IncludeMode      = appstate.IncludeMode;
 auto& IncludeFileName  = appstate.IncludeFileName;
 auto& InclLineNum      = appstate.InclLineNum;

 double result;
 char buf[LSTR+1]; 
 char buf1[LSTR+1];
 char buf2[LSTR+1];
 char str_res[LSTR+1]; ;
 char *bufpt1, *bufpt2;
 
 if (!LatticeCh_) return; 

 OptimEditor*   editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());     
 if (!editor ) return;

 editor->clearHighlightedBlocks();
 
  // Analysis of math header
 
 IncludeMode = false;
 calc_.zeroCalc();

 // ---------------------------------------------------------------------------------
 // savevars directive 

 QTextDocument* doc = editor->document();
 QTextCursor cursor = doc->find("OptiM");
 if (cursor.isNull()) { std::cerr << "OptiM marker not found." <<std::endl; return; }
 int optimmarker =  cursor.block().blockNumber();
   
 cursor.setPosition(QTextCursor::Start);
 
 QString savevars;
 while ( !(cursor = doc->find("#!",cursor)).isNull()) {
   while ( cursor.block().text().startsWith("#!") ) {
     savevars += cursor.block().text().remove("#!");
     cursor.movePosition(QTextCursor::NextBlock);
   }
   if (cursor.block().blockNumber() >= optimmarker) break;   
 }

 std::string  names;
 std::string  fname;
 char mode;
 std::vector<std::string> varlist; // list of variables to save
 compat::cmatch cm;

 static compat::regex rxsavevars( R"(^\s*(savevars|SAVEVARS)(,\s*?|\s*?).*)");
 bool savevars_is_specified = compat::regex_match( savevars.toUtf8().data(), cm, rxsavevars);  

 if (savevars_is_specified) { 
   static compat::regex rxnames( R"((NAMES|names)\s*=\s*\{(.*?)\})");  
   bool names_attr = compat::regex_search( savevars.toUtf8().data(), cm, rxnames);  
   names = names_attr ? cm[2] : std::string();

   static compat::regex rxvar( R"(\$[a-zA-Z:_0-9]+)");  
   char const* p = names.c_str();
   while (compat::regex_search( p, cm, rxvar) ) {   
     varlist.push_back(cm[0].str());
     p += cm.position() + cm.length();
   }

   static compat::regex rxfile( R"((FILE|file)\s*=\s*\"(.+?)\")");  
   bool file_attr = compat::regex_search( savevars.toUtf8().data(), cm, rxfile);  
   fname = file_attr ? cm[2].str() : std::string("variables.txt");
 
   static compat::regex rxmode( R"((MODE|mode)\s*=\s*([WwAa]))");
   bool mode_attr =  compat::regex_search( savevars.toUtf8().data(), cm, rxmode);  
   mode = mode_attr ? cm[2].str()[0] :'W'; 
 }
  
 //------------------------------------------------------------------------------------


 int DoWhileFirstLine= 0; 
 int NmbWhileCycles  = 0;
 int nline = 0;

 while(true){
   nline = getLineCalc(editor, buf, LSTR, nline);
   
   if(nline<0){ OptimMessageBox::warning(this, "Conversion Error", "The \"OptiM\" delimiter line is missing", QMessageBox::Ok);
		break;
   }
   if(strcmpr("OptiM",buf)) break;

   if(strcmpr("do{",buf)){
     if(IncludeMode){
       OptimMessageBox::warning(this, "Syntax Error", "do{...}while  loops not allowed in include files", QMessageBox::Ok);
       break;
     }
     if(DoWhileFirstLine){
       OptimMessageBox::warning(this, "Syntax Error", "Nested while loops are not allowed", QMessageBox::Ok);
       break;
     }
     DoWhileFirstLine=nline;
     continue;
   }

   if ( strcmpr("}while",buf) ){

     if(IncludeMode){
       OptimMessageBox::warning(this, "Syntax Error", "do{...} while loop is not allowed in include files", QMessageBox::Ok);
       break;
     }
     if(!DoWhileFirstLine){
       OptimMessageBox::warning(this, "Syntax Error", "}while encountered before do{", QMessageBox::Ok);
       break;
     }
     int i = calc_.calcLine(buf+6, &result, "%12.9lg", str_res, buf1);
     if (i>0) {
         replaceLine(editor, nline-1, buf);
         editor->moveCursor(QTextCursor::Start); 
         for (int i=0; i<nline-1; ++i) {editor->moveCursor(QTextCursor::QTextCursor::Down); } 
         editor->highlightCurrentBlock(nline-1); 
	 OptimMessageBox::warning(this, "Calculator Error[1]", buf1, QMessageBox::Ok);
	 break;
       }
       if( (result-1.e-10)>0 ) {
       if( NmbWhileCycles++ > MAX_NMB_WHILE_CYCLES ){
         OptimMessageBox::warning(this, "Syntax Error", "Loop iterations exceeded 10000 limit", QMessageBox::Ok);
         break;
       }
       nline = DoWhileFirstLine;
     }
     else DoWhileFirstLine = 0;
     continue;
   }

   bufpt1 = buf;    
   bufpt2 = buf2;

   while(*bufpt1){
     if(*bufpt1==';' || *bufpt1=='#') break;
        *bufpt2++ = *bufpt1++;
     }
     *bufpt2 = 0;
     int i = calc_.calcLine(buf2, &result, "%12.9lg", str_res, buf1);
     if(i>0){
       if(!IncludeMode){
         replaceLine(editor, nline-1, buf);
	 for (int i=0; i<nline-1; ++i) {editor->moveCursor(QTextCursor::QTextCursor::Down); } 
         editor->highlightCurrentBlock(nline-1); 
         OptimMessageBox::warning(this, "Calculator Error[2]",buf1, QMessageBox::Ok);
	 break;
       }
       else{
         sprintf(buf2,"%s in file %s at line %d", buf1, IncludeFileName, InclLineNum);
         OptimMessageBox::warning(this, "Calculator Error [3]", buf2, QMessageBox::Ok);
	 break;
       }
     }
     if(i<0) {
       strcat(buf2,";");
     }
     else {
       strcat(buf2,";  =>");
       strcat(buf2, str_res);
     }
     if(*bufpt1==';' || *bufpt1=='#'){
       while(*bufpt1){
         if(*bufpt1=='#')break;
           bufpt1++;
         }
       }
       if(*bufpt1=='#'){strcat(buf2, "  ");
       strcat(buf2, bufpt1);}
       if(!IncludeMode) replaceLine(editor, nline-1, buf2);
     }
     IncludeMode=false;

     if (savevars_is_specified) calc_.dumpVariables(varlist, fname, mode);

     return;
 } 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int OptimMainWindow::getLineCalc(QPlainTextEdit* editor, char* buf, int /*lenstr*/, int  nline )
{

  auto& IncludeMode      = appstate.IncludeMode;
  auto& IncludeFileName  = appstate.IncludeFileName;
  auto& InclLineNum      = appstate.InclLineNum;

  OptimEditor* leditor = qobject_cast<OptimEditor*>(editor);

  if (leditor) {
   strcpy( MainFileDir, leditor->fileInfo().absolutePath().toUtf8().data() );
  }     
  else {
    strcpy( MainFileDir, QDir::currentPath().toUtf8().data());
  }
    

  // std::regex rgx_include("^#include"); // regex is broken for g++ < 4.9 

  static FILE* fp;

  char buf1[256];
  
  int nmblines  = editor->document()->lineCount();
  bool iscomment = false;
  
  do {
      
    if (IncludeMode) { // if in include mode, fetch a line from the included file  
      if (!fgets(buf, LSTR,fp) ) {
         fclose(fp);
         IncludeMode = false;
         buf[0]      = 0;
      }
    }
 
    else { // otherwise, get the next line from the editor buffer 
      
      strcpy( buf, editor->document()->findBlockByLineNumber(nline++).text().toUtf8().data());
      if( strcmpr("#include",buf) )  {
	if( !getFileNameOpt(&buf[8], MainFileDir, IncludeFileName) ) { return -1; }
         fp = fopen(IncludeFileName,"r");
        
        if( !fp ) {
           sprintf(buf1, "Cannot open file <%s> in #include directive", IncludeFileName);
             OptimMessageBox::warning(this, "OptiM",
				  buf1, QMessageBox::Ok);
             return -1;
        }
        else {
           IncludeMode = true;
      	   if( !fgets(buf, LSTR,fp) ) {
             fclose(fp);
             IncludeMode=false;
           }
        }
      }
    } //else 

  }  while( (iscomment = Utility::checkComment(buf))  &&  (nline <= nmblines) );
  
  if((nline-1) > nmblines ){
       OptimMessageBox::warning(this, "Conversion Error", "Premature end of file encountered." );
       return -1;
  }
  
  return nline;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::replaceLine(QPlainTextEdit* editor, int nline, char *buf, QColor const& color)
{

 auto& IncludeMode      = appstate.IncludeMode;
 auto& IncludeFileName  = appstate.IncludeFileName;
 auto& InclLineNum      = appstate.InclLineNum;

 char buf1[LSTR+1];

  if (nline<0) nline=0;
  
  if (IncludeMode) { 
    //strcpy( buf1, editor->getLine(nline).toUtf8().data() );
    strcpy( buf1, editor->document()->findBlockByLineNumber(nline).text().toUtf8().data() );

  }

  // use this to get colored output - useful for debugging
  QTextCharFormat format;
  //format.setForeground(QColor(Qt::black)); // format.setForeground(QColor(Qt::green));
  format.setForeground(color);
  QTextCursor cursor = editor->textCursor();

  cursor.setPosition( 0 , QTextCursor::MoveAnchor);  
  cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, nline);
  cursor.select(QTextCursor::BlockUnderCursor);
  cursor.removeSelectedText();
  // ------------------------------------------------------------------------------------------
  // it seems necessary to insert a new line when a block (not the same as a line) is removed.
  // block n ends just *before* the n-th newline separator, which means that block n starts with
  // the n-1 th newline separator.    
  // line n=0 is a special case. It contains no initial newline.   
  //-------------------------------------------------------------------------------------------
  if ( nline > 0 ) cursor.insertText("\n");
     
  if(IncludeMode)  { 
    cursor.insertText(buf1);
  }
  else { 
    cursor.setCharFormat(format); 
    cursor.insertText(buf);
   }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::replaceExistingLine(QPlainTextEdit* editor, int nline)
{
   char buf[LSTR+1];

   if(nline<0) nline = 0;

   QTextCursor cursor = editor->textCursor(); 
   cursor.movePosition(QTextCursor::Start);
   cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, nline);
   strcpy(buf, cursor.block().text().toUtf8().data()); 
   cursor.select( QTextCursor::BlockUnderCursor); 
   cursor.removeSelectedText();
   cursor.insertText("\n");
   cursor.insertText(buf);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdAnalyzeLine()  
{

  auto& IncludeMode      = appstate.IncludeMode;
  auto& IncludeFileName  = appstate.IncludeFileName;
  auto& InclLineNum      = appstate.InclLineNum;
  
  char  buf[LSTR+1];
  char  buf1[LSTR+1];
  char  buf2[LSTR+1];
  char  str_res[LSTR+1];

  int DoWhileFirstLine = 0;
  int NmbWhileCycles   = 0;
 
  if (!LatticeCh_) return; 
  OptimEditor*   editor = qobject_cast<OptimEditor*>(LatticeCh_->widget());     
  if (!editor ) return;
 
  if(analyzed_) {
    if(analyze(false, 1))return;
  } 
  else {
    if(analyze(true, 1))return;
  }
  
  int     nline   = 0;
  double  result;
  int     CurLine = editor->textCursor().blockNumber();

  if(CurLine < lineOptiM-1){
    // Analysis in math header
    IncludeMode=false;
    calc_.zeroCalc();
    while(nline<=CurLine){
      nline = getLineCalc(editor, buf, LSTR, nline);
      if (nline >= lineOptiM)  break;
      if ( strcmpr("do{",buf)) {
        DoWhileFirstLine = nline;
        continue;
      }
      if (strcmpr("} while",buf)){
        calc_.calcLine(buf+6, &result, "%12.9lg", str_res, buf1);
        if((result-1.e-10)>0){
          if(NmbWhileCycles++ >  MAX_NMB_WHILE_CYCLES){
            editor->highlightCurrentBlock();
	    OptimMessageBox::warning(this, "Syntax Error", "Iteration limit (10000) exceeded.", QMessageBox::Ok);
            break;
          }
          nline=DoWhileFirstLine;
        }
        else DoWhileFirstLine=0;
        continue;
      }
      char* bufpt1 = buf;
      char* bufpt2 = buf2;
      while(*bufpt1){
        if(*bufpt1==';' || *bufpt1=='#')break;
        *bufpt2++=*bufpt1++;
      }
      *bufpt2 = 0;
      int i = calc_.calcLine(buf2, &result, "%12.9lg", str_res, buf1);
      if(i>0){ 
        replaceLine(editor, nline-1, buf);
        editor->moveCursor(QTextCursor::Start); 
        for (int i=0; i<nline-1; ++i) {editor->moveCursor(QTextCursor::QTextCursor::Down); } 
        editor->highlightCurrentBlock();
        OptimMessageBox::warning(this, "Calculator Error [4] ", buf1, QMessageBox::Ok);
        break;
      }
      if(i<0)strcat(buf2,";");
      else {strcat(buf2,";  =>"); strcat(buf2, str_res);}
      if(*bufpt1==';' || *bufpt1=='#'){
        while(*bufpt1){
          if(*bufpt1=='#')break;
          bufpt1++;
        }
      }
      if(*bufpt1=='#'){strcat(buf2, "  "); strcat(buf2, bufpt1);}
      if(!IncludeMode && ((nline-1)==CurLine)){
        replaceLine(editor, nline-1, buf2);
        break;
      }
    }
    IncludeMode = false;
    return;
   }

  if(CurLine >= LineLIn && CurLine <= LineLFin) {
     nline = getLineCmt(editor, buf, LSTR, CurLine);
     if (nline != CurLine+1) return;
     char* bufpt = buf;
     // if( getElmName(ElemNameFCh, &bufpt, buf1, buf2) == 1 ){
     getElmNameX( bufpt, buf1, buf2); 
     if( (*buf1) && !isValidType(buf1) ) {
         sprintf(str_res,"No Element with name <%s>  on line %d <%s> ", buf1, nline, buf);
         editor->highlightCurrentBlock();
         OptimMessageBox::warning(this, "Conversion Error [5]", str_res, QMessageBox::Ok);
         replaceExistingLine(editor, nline-1);
         return;
     }
     auto ElmL = std::make_shared<Element>();
     ElmL->name(buf1);
     if (analyzeElement(editor, CurLine, buf, 0, ElmL)) return;
     print_elm( ElmL.get(), buf1);
     replaceLine(editor, CurLine, buf1);
     IncludeMode = false;
     return;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  OptimMainWindow::getLineCmt(OptimEditor* editor, char* buf, int L, int nline)
{
   using Utility::getExpression;

   auto& IncludeMode      = appstate.IncludeMode;
   auto& IncludeFileName  = appstate.IncludeFileName;
   auto& InclLineNum      = appstate.InclLineNum;
  
   char b[LSTR+1];
   char* bp1 = 0;
   char* bp2 = 0;
   char* p   = 0;
   char str_result[LSTR+1];
   char name_var[32];
   char modif[32];
   char expr_str[LSTR+1];
   char buf1[256];
   double result;

   nline = getLineCalc(editor, b, L, nline);
   
   if(nline<0) {*buf=0; IncludeMode=0; return -1;}
   bp1=b; bp2=buf;
   
   while((*bp1!=0)&&(*bp1!='\n')){
     if(*bp1!='$' && *bp1!='='){*bp2++=*bp1++; continue;}
     if(*bp1=='$'){
       bp1=getVariableName(bp1, name_var, modif);
       if(calc_.findValue(name_var, &result,"%-17.12le", str_result)>0){
	 OptimMessageBox::warning(this, "Variable not found", name_var, QMessageBox::Ok);
	 *bp2=0;
	 IncludeMode=0;
	 return -1;
       }
       correctNames(str_result, modif);
       p = str_result;
       while(*p) *bp2++ = *p++;
    }
    if(*bp1=='='){
      *bp2++=*bp1++;
      bp1=getExpression(bp1, expr_str);
      if(calc_.calcLine(expr_str, &result, "%-17.12le", str_result, buf1)>0){
        sprintf(&buf1[strlen(buf1)], " on line %d: <%s>", nline-1, b);
        editor->moveCursor(QTextCursor::Start); 
        for (int i=0; i<nline-1; ++i) {editor->moveCursor(QTextCursor::QTextCursor::Down); } 
	editor->highlightCurrentBlock();
	OptimMessageBox::warning(this, "Calculator Error [6] ", buf1, QMessageBox::Ok);
        *bp2=0;
	IncludeMode=0;
	return -1;
      }
      p=str_result;
      while(*p) *bp2++ = *p++;
      *bp2++ = ' ';
    }
  }
  *bp2 = 0;
  return nline; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

