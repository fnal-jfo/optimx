//  =================================================================
//
//  Compress.cpp
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
#if GCC_VERSION >= 40900 || _MSC_VER >= 1900 || __clang__
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

 #define NELEM   8
 #define LSTR    512
 

void OptimMainWindow::cmdCompress ()
{

  static const compat::regex begin_lattice_rx(R"(^\s*begin\s*lattice)", compat::regex_constants::ECMAScript|compat::regex_constants::icase);
  static const compat::regex   end_lattice_rx(R"(^\s*end\s*lattice)",   compat::regex_constants::ECMAScript|compat::regex_constants::icase);
  static const compat::regex    begin_list_rx(R"(^\s*begin\s*list)",    compat::regex_constants::ECMAScript|compat::regex_constants::icase);
  static const compat::regex      end_list_rx(R"(^\s*end\s*list)",      compat::regex_constants::ECMAScript|compat::regex_constants::icase);

  char* bufpt;
  char  buf[512];         // file read buffer 
  char  b[LSTR+1];        // used by decodeLineCalc
  char  buf1[40];
  char  buf2[NAME_LENGTH];
  char* pt;
  char  buferr[256];      // error message
  char  str_res[LSTR+1];
  bool  rslt = false;

  int nelms1 = 0; // number of elements in the original     line 
  int nelms2 = 0; // number of elements in the concatenated line 

  int i, j, k, i2, nelmlist;

  double dat[7];
  double L;
  double result;

  char MnFileDir[256];
  MnFileDir[0] = '.';       
  MnFileDir[1] = char(0);

  int ndrifts1  = 0,  ndrifts2  = 0;
  int nsext1    = 0,  nsext2    = 0;
  int nsol1     = 0,  nsol2     = 0;  
  int ncor1     = 0,  ncor2     = 0; 
  int napert1   = 0,  napert2   = 0;
  int ninst1    = 0,  ninst2    = 0;
  int nwkfld1   = 0,  nwkfld2   = 0;
  int nscatter1 = 0,  nscatter2 = 0;

  auto& IncludeMode      = appstate.IncludeMode;
  auto& IncludeFileName  = appstate.IncludeFileName;
  auto& InclLineNum      = appstate.InclLineNum;
   
  //......................
  // element filters 
  //.....................

  auto condensed = [this](char etype) {
    return ( (etype =='O') || 
           ( (etype =='K') && NStCompr.cor ) ||
           ( (etype =='Z') && NStCompr.cor ) ||
           ( (etype =='S') && NStCompr.sext   ) ||
           ( (etype =='I') && NStCompr.inst   ) ||
           ( (etype =='C') && NStCompr.sol    ) ||
           ( (etype =='H') && NStCompr.apert  ) ||
           ( (etype =='Y') && NStCompr.wkfld  ) ||
           ( (etype =='T') && NStCompr.scatter) ||
           ( (etype =='U') && NStCompr.scatter) ||
	   ( (etype =='V') && NStCompr.scatter) );
  };
    
  auto thin = [ this](char etype) {
    return  ( (( etype =='H') && NStCompr.apert)   ||
              (( etype =='Y') && NStCompr.wkfld)   ||
	      (( etype =='T') && NStCompr.scatter) ||
	      (( etype =='U') && NStCompr.scatter) ||
              (( etype =='V') && NStCompr.scatter) );
  };
 
  uint id = 0;
  auto eid = [&id]() {
    // condensed element id
    return  id++;
  };

  // ................................................................................

  CompressDialog dialog(0); 
  if ( dialog.exec() == QDialog::Rejected ) return;
  
  NStCompr = dialog.data_;
  id = NStCompr.offset; 

  QString  filenamefrom = dialog.data_.filenamefrom;
  QString  filenameto   = dialog.data_.filenameto;

  if (filenamefrom == "") return;
  if (filenameto   == "") return;

  struct ElmData {
    std::string name;
    double      L;
  };
  
  std::vector<ElmData>     e1;    // source element list 
  std::vector<ElmData>     e2;    // condensed beamline 
  
  auto del = [](FILE* p) { (p ? fclose(p) : 0);};  
  std::unique_ptr<FILE, decltype(del)>  fp1( fopen(filenamefrom.toUtf8().data(),"r"), del);
  std::unique_ptr<FILE, decltype(del)>  fp2( fopen(filenameto.toUtf8().data(),  "w"), del);

  if( !fp1 ) {
    OptimMessageBox::warning( this, "File Error", ("Error opening file: \n"+ filenamefrom.trimmed()).toUtf8().data(), QMessageBox::Ok);
    return;
  }

  if( !fp2 ){
    OptimMessageBox::warning(this,  "File Error", ("Error opening file: \n"+ filenameto.trimmed()).toUtf8().data(), QMessageBox::Ok);
    return;
  }

  OptimTextEditor*     editor = 0;
  QMdiSubWindow* sw = mdiArea->addSubWindow( editor = new OptimTextEditor()); 
  sw->setWindowTitle("Compression"); 
  sw->setGeometry( mdiArea->geometry().width()/2, 0, mdiArea->geometry().width()/2,  mdiArea->geometry().height()/4);


  // MnFileDir is the directory path of the main lattice file
  strcpy(MnFileDir, QFileInfo(filenamefrom).absoluteDir().absolutePath().toUtf8().data()); 
   
  IncludeMode = false;
  
  calc_.zeroCalc();
  calc_.calcLine("$_turn=1", &result, "%12.9lg", str_res, buf1);

  //......................
  //  Copy the beginning
  //.....................

  fprintf(fp2.get(),"%s %s\n", "# Original :", filenamefrom.toUtf8().data()) ;

  while(true) { // processing header 

    char* res = readLineCalc(fp1.get(), buf, MnFileDir, buferr);
    if(!res) {
      if(*buferr ==0 ) { strcpy(buf, "Error: reached premature end of input file while processing header."); }
      else             { strcpy(buf, buferr); }
      goto rtrn;
    }
    fprintf(fp2.get(),"%s\n", buf);

    if( strcmpr("OptiM", buf)) break;

    pt=buf;

    while( *pt ) {
      if (*pt ==';') {*pt=0; break;} ++pt;
    } // strip in-line comments beyond and including the position of the delimiter ';'

    i = calc_.calcLine(buf, &result, "%12.9lg", str_res, buf1);
    if(i>0) { sprintf(buf,"Calculator Error: %s", buf1); goto rtrn; }

  }	       


  while(true) { // process body
    if( !readLineCalc(fp1.get(), buf, MnFileDir, buferr) ) {

      if (!(*buferr)) {
	strcpy(buf, "Error: premature end of file while processing body ..");
	goto rtrn;
      }
      else {
        strcpy(buf, buferr); goto rtrn;
      }
    }
    if(NStCompr.vrbls) {
       decodeLineCalc(buf, b, buferr);
       fprintf(fp2.get(),"%s\n", b);
    }
    else {
        fprintf(fp2.get(),"%s\n", buf);
    }
    //if(strcmpr("begin lattice",buf)) break;
    if(compat::regex_search(buf, begin_lattice_rx)) break;
    
  }
  // ...........................................................................................
  // skip lattice definition on first pass 
  //.............................................................................................

  while( readLineCmt(fp1.get(), buf, MnFileDir, buferr ) != 0) {
    if(compat::regex_search(buf, end_lattice_rx)) break;
  }
  //..................................................
  // Construct source element edict 
  // Discard elements with types specified in NStCompr 
  //..................................................

if( readLineCmt(fp1.get(), buf, MnFileDir, buferr) == 0) goto rtrn;
  if( !compat::regex_search(buf, begin_list_rx)) goto rtrn;

  while( (readLineCmt(fp1.get(), buf, MnFileDir, buferr)) ) {

    if( compat::regex_search(buf, end_list_rx)) break;

    char* bp = buf;
    if( !getElmNameX(bp, buf1, buf2) ) goto rtrn;
    ++nelms1; 
    char etype = toupper(*buf1);  

    if( condensed(etype) ) {
      
      
      if( thin(etype) ) {
        e1.push_back( ElmData({buf1, 0.0}) );
      }
      else {
	decodeLine(buf,dat,1);
        e1.push_back( ElmData({buf1, dat[0]}) );
      }
    }
  } // while

  if( !compat::regex_search(buf, end_list_rx)) goto rtrn;

  //..................................................................
  // Rewind input and set position at beginning of lattice definition. 
  // Continue writing to the output file
  //....................................................................

  fseek(fp1.get(), 0L, SEEK_SET); // rewind  input file 

  while( fgets(buf, 255, fp1.get())!= 0){
    if ( (rslt =  compat::regex_search(buf, begin_lattice_rx)) ) break;
  }
  if(!rslt) goto rtrn;

  L = 0.0;
  // build a list of condensed elements

  while( readLineCmt(fp1.get(), buf, MnFileDir, buferr) !=0 ) {
     char* bp = buf;
     if ( compat::regex_search(buf, end_lattice_rx)) break;

     // append elements to the condensed beamline e2 ( drifts are merged)  

     while( getElmNameX(bp,buf1, buf2) ) {

      char etype = toupper(*buf1);

      for( i=0; i< e1.size(); ++i) {
	if (e1[i].name == buf1) { 	 
          break;
        }
      }; // find lattice elm in list e1

      if( !condensed(etype) ) { // not a condensed element 
	if (L>0.0) { // true if preceeding element(s) were finite length drifts  
	   char lbl[256];
	   sprintf(lbl, "oD%-d", eid());  // output the condensed drift
           e2.push_back( ElmData({lbl, L}) );
           ++nelms2;
	   L = 0.0;
	}
	e2.push_back( (ElmData({buf1, -1.0})) );
 	L = 0.0;
	++nelms2;
	continue;
      }
      
      // if we get here, etype is a condensed type
      
      if( thin(etype) ) continue; // strip condensed thin element 

      // otherwise, accumulate length
      L += e1[i].L;

      if( (etype =='K') && NStCompr.cor )    ++ncor1;
      if( (etype =='Z') && NStCompr.cor )    ++ncor1;     
      if( (etype =='S') && NStCompr.sext   ) ++nsext1;
      if( (etype =='I') && NStCompr.inst   ) ++ninst1;
      if( (etype =='C') && NStCompr.sol    ) ++nsol1;
      if( (etype =='H') && NStCompr.apert  ) ++napert1;
      if( (etype =='Y') && NStCompr.wkfld  ) ++nwkfld1;
      if( (etype =='T') && NStCompr.scatter) ++nscatter1; 
      if( (etype =='U') && NStCompr.scatter) ++nscatter1;
      if( (etype =='V') && NStCompr.scatter) ++nscatter1;
      
    } // while  (lattice elements within a line)
  }  // while (lattice block lines ) 

  for (int i=0; i<e2.size(); ++i) {
    fprintf(fp2.get(),"%s ", e2[i].name.c_str());
    if ( i && ((i%6 == 0) || (i ==e2.size()-1)) )  fprintf(fp2.get(),"%s", "\n");
  }
  
  fprintf(fp2.get(),"end lattice\n");

  // Print new list of elements

  while(fgets(buf, 255, fp1.get()) != 0){
    fprintf(fp2.get(),"%s", buf);
    if ( (rslt = compat::regex_search(buf, begin_list_rx)) ) break;
  }
  if(!rslt) goto rtrn; 

  while( readLineCalc(fp1.get(), buf, MnFileDir, buferr) != 0) {

    if(NStCompr.vrbls){
      decodeLineCalc(buf, b, buferr);
      strcpy(buf,b);
    }
    if ( (rslt = compat::regex_search(buf, end_list_rx)) ) break;

    char etype = toupper(*buf);

    if  (!condensed(etype) ) {
      fprintf(fp2.get(),"%s\n", buf); // not a candidate for concatenation; just copy.
    }
    // exception for matrix type ... copy the next 6 lines.  
    if( etype == 'X') { // if type 'X' (matrix) copy the 6 lines that follow  
      for(i=0; i<6; i++) {
	readLineCalc(fp1.get(), buf, MnFileDir, buferr); // should the calculator be invoked here ? 
        if (NStCompr.vrbls){
          decodeLineCalc(buf, b, buferr);
	  strcpy(buf,b);
	}
	fprintf(fp2.get(),"%s\n", buf);
      }
    }
  } // while 

  // output the rest of the element list .. (condensed elements) 

  for(int i=0; i<e2.size(); ++i) {
    char etype = toupper(e2[i].name[0]);
    if ( condensed(etype) ) {
      fprintf(fp2.get(),"%s \t L[cm]=%-8.8lg \n", e2[i].name.c_str(), e2[i].L);
    }
  }

  fprintf(fp2.get(),"end list\n");

  //  print results to the screen

  sprintf(buf, "--------------------------------\n");
  editor->insertPlainText(buf);
  sprintf(buf, "Beamline condensation successful.\n");
  editor->insertPlainText(buf);
  sprintf(buf, "--------------------------------\n\n");
  editor->insertPlainText(buf);

  sprintf(buf, "Contiguous drifts have been merged.\n");
  editor->insertPlainText(buf);

  if (NStCompr.sext || NStCompr.sol    ||
      NStCompr.cor  || NStCompr.apert  ||
      NStCompr.inst || NStCompr.wkfld  || NStCompr.scatter ) {   

        sprintf(buf, "In addition, elements of the  type(s) listed below have either been removed (zero length) or merged (finite length)  with neighboring drifts :\n");
        editor->insertPlainText(buf); 

        if( NStCompr.sext)   { sprintf(buf, "** %5d sextupole(s)  **\n", nsext1);     editor->insertPlainText(buf); }
        if( NStCompr.sol)    { sprintf(buf, "** %5d solenoid(s)   **\n",  nsol1);      editor->insertPlainText(buf); }
        if( NStCompr.cor)    { sprintf(buf, "** %5d corrector(s)  **\n", ncor1);      editor->insertPlainText(buf); }
        if( NStCompr.apert)  { sprintf(buf, "** %5d aperture(s)   **\n",  napert1);    editor->insertPlainText(buf); }
        if( NStCompr.inst)   { sprintf(buf, "** %5d instrument(s) **\n", ninst1);     editor->insertPlainText(buf); }
        if( NStCompr.wkfld  ){ sprintf(buf, "** %5d wakefield(s)  **\n",  nwkfld1);    editor->insertPlainText(buf); } 
        if( NStCompr.scatter){ sprintf(buf, "** %5d scatterer(s)  **\n",  nscatter1);  editor->insertPlainText(buf); }
    }

    sprintf(buf, "\n\n The original beamline (%s) contains: \n %d elements.\n",  filenamefrom.toUtf8().data(), nelms1);
    editor->insertPlainText(buf);

    sprintf(buf,"The condensed beamline (%s) contains: \n %d elements.\n", filenameto.toUtf8().data(), nelms2); // i2);
    editor->insertPlainText(buf);
    sw->show();

    goto en;

rtrn:
        char errmsg[512];
        sprintf(errmsg, "%s %s" ,"Error:", buf);
	editor->insertPlainText(buf);
        editor->insertPlainText(".\n");

en:
	sw->show();  
        editor->document()->setModified(false);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdCompressOld() // Old version of cmdCompress (currently disabled)
{

#if  0

  FILE *fp1, *fp2;
  char buf[256], buf1[40], buf2[NAME_LENGTH], *bufpt, bel[3]={'\015','\012','\000'};
  bool rslt;
  int i, j, k, i2, nelmlist, larray, offset;
  double dat[5], L;

  
  OptimTextEditor*     editor = 0;
  QMdiSubWindow* sw = mdiArea->addSubWindow( editor = new OptimTextEditor()); 
  sw->setWindowTitle("Compression"); 
  sw->setGeometry( mdiArea->geometry().width()/2, 0, mdiArea->geometry().width()/2,  mdiArea->geometry().height()/4);

  CompressDialog dialog(0); 
  if ( dialog.exec() == QDialog::Rejected ) return;
  
  NStCompr = dialog.data_;
  offset   = dialog.data_.offset;
  
  QString  filenamefrom = dialog.data_.filenamefrom;
  QString  filenameto   = dialog.data_.filenameto;
  
  std::unique_ptr<Element[]> e1;
  std::unique_ptr<Element[]> e2;
 
  if((fp1=fopen(filenamefrom.toUtf8().data(),"r"))==0){
    OptimMessageBox::warning( this, "Error opening file - ", filenamefrom.trimmed().toUtf8().data(), QMessageBox::Ok);
    goto rtrn;
  }
  if((fp2=fopen(filenameto.toUtf8().data(),"w"))==0){
    OptimMessageBox::warning(this,  "Error opening file - ", filenameto.trimmed().toUtf8().data(), QMessageBox::Ok);
    goto rtrn;
  }

  // Copying of the beginning
  while(fgets(buf, 255, fp1)!=0){
    fprintf(fp2,"%s", buf);
    rslt = strcmpr("begin lattice",buf);
    if(rslt)break;
  }

  if(!rslt) goto rtrn;

  // Count Elements in the lattice structure
  i=0;
  while(readLineCmt(fp1, buf) !=0) {
    if(strcmpr("end lattice",buf))break;
    // Counts Elements
    bufpt=buf;
    while( getElmNameX(bufpt,buf1, buf2) ) { 
      char etype = toupper(*buf1);
      if(  ( etype == 'O')                    ||
  	  (( etype == 'K') && NStCompr.cor )  ||
	  (( etype == 'S') && NStCompr.sext)  ||
          (( etype == 'I') && NStCompr.inst)  ||
	  (( etype == 'C') && NStCompr.sol ) )   {++i;}
      }
    };

    larray=i;

    e1 = std::unique_ptr<Element[]>( new Element [larray-1]);
    e2 = std::unique_ptr<Element[]>( new Element [larray-1]);

    // Search elements in the Elements list and get their length

    if(readLineCmt(fp1, buf)==0)   goto rtrn;
    if(!strcmpr("begin list",buf)) goto rtrn;
 
    i=0;  //Number of different Elements in the Element list(drifts will be counted only)

    while((readLineCmt(fp1, buf))!=0){
      if(strcmpr("end list",buf) || (i>= larray) ) break;
      bufpt = buf;
      if( !getElmNameX(bufpt,buf1, buf2) ) goto rtrn;
      char etype = toupper(*buf1);
      if(    (etype =='O')                    ||
	    (( etype =='K') && NStCompr.cor  ) ||
	    (( etype =='S') && NStCompr.sext ) || 
	    (( etype =='I') && NStCompr.inst ) ||
	    (( etype =='C') && NStCompr.sol) ) {
	strcpy(e1[i].name,buf1);
	decodeLine(buf,dat,1);
	e1[i++].L=dat[0];
      }
    }
    
    if(!strcmpr("end list",buf))goto rtrn;
    nelmlist=i;                                
    fclose(fp1);

    // Continue writing of the output file
    if((fp1=fopen(filenamefrom.toUtf8().data(),"r"))==0) goto rtrn;
      while(fgets(buf, 255, fp1)!=0){rslt=strcmpr("begin lattice",buf); if(rslt)break;
    }
    if(!rslt)goto rtrn;
    i2=0;// number of drifts already written to e2
    k=0; // current number of sequential drifts in e1
    j=0; // number of Elements already printed in line
    L=0;

    while(readLineCmt(fp1, buf)!=0){
      if(strcmpr("end lattice",buf)) break;
      bufpt=buf;
      // Counts Elements
      while( getElmNameX( bufpt,buf1, buf2) ) {
	char etype = toupper(*buf1);
        if(    ( etype == 'O')                   ||
	      (( etype == 'K') && NStCompr.cor)  ||
	      (( etype == 'S') && NStCompr.sext) ||
	      (( etype == 'I') && NStCompr.inst) ||
	      (( etype == 'C') && NStCompr.sol) ) {  // if it is a drift adds length
	  for(i=0; i<nelmlist; ++i) { if(!strcmp(buf1, e1[i].name)) break;}
	  L += e1[i].L; 
	  ++k;
        }
        else  {    // if is not a drift
	  if(k)   {  // if the previous one was a drift
	    i=0; 
            while(i<i2) {if(fabs(e2[i].L -L) < 0.0001)break; i++; }
	    if(i<i2) {
              fPrint(fp2, e2[i].name);
	      if(j++ == NELEM){j=0; fprintf(fp2, "\n"); }
	    }
            else { 
              strcpy(e2[i2].name,"oD");
	      sprintf(&e2[i2].name[2], "%-d", i2+offset);
	      fPrint(fp2, e2[i2].name);
	      e2[i2++].L=L;
	      if(j++ == NELEM){j=0; fprintf(fp2, "\n");}
	    }
            L=0.;  k=0;
            fPrint(fp2, buf1);
            if(j++ == NELEM){j=0; fprintf(fp2, "\n");}
 	  }
	  else{ // if the previous one was not a drift
	    fPrint(fp2, buf1);
	    if(j++ == NELEM){j=0; fprintf(fp2, "\n");}
	  }
        }
      }
    }
    if(k){  // if the previous one was a drift
      i=0; while(i<i2){if(fabs(e2[i].L -L) < 0.0001)break; i++;}
      if(i<i2) {
        fPrint(fp2, e2[i].name);
        if(j++ == NELEM){j=0; fprintf(fp2, "\n");}
      }
      else  {strcpy(e2[i2].name,"oD");
        sprintf(&e2[i2].name[2], "%-d", i2+offset);
	fPrint(fp2, e2[i2].name);
	e2[i2++].L=L;
	if(j++ == NELEM){j=0; fprintf(fp2, "\n");}
      }
    }
    if(j)fprintf(fp2," \n");
    fprintf(fp2,"end lattice\n");
    // Print list of Elements
    while(fgets(buf, 255, fp1)!=0){
      fprintf(fp2,"%s", buf);
      rslt=strcmpr("begin list",buf); if(rslt)break;
    }
    if(!rslt)goto rtrn;
    while(readLineCmt(fp1, buf)!=0){
      rslt=strcmpr("end list",buf); if(rslt)break;
      char etype = toupper(*buf1);
      if(!( (etype =='O') ||
	   (( etype =='K') && NStCompr.cor) ||
	   ((etype =='S') && NStCompr.sext) ||
	   ((etype =='I') && NStCompr.inst) ||
	   ((etype =='C') && NStCompr.sol) )) { fprintf(fp2,"%s\n", buf); } 

      if( etype =='X') { 
        for(i=0; i<6; i++) {
	  readLineCmt(fp1, buf);
	  fprintf(fp2,"%s\n", buf);
        }
      }
    }
    for(i=0; i<i2; i++) fprintf(fp2,"%s \t L[cm]=%-8.8lg \n", e2[i].name, e2[i].L);
    fprintf(fp2,"end list\n");
    //  print results to the screen
    sprintf(buf," The original source file (%s) contains : %d drifts", filenamefrom.toUtf8().data(), nelmlist);
    
    editor->insertPlainText(buf);
    
    if(NStCompr.sext){sprintf(buf,", sextupoles"); 	editor->insertPlainText(buf);}
    if(NStCompr.sol) {sprintf(buf,", solenoids"); 	editor->insertPlainText(buf);}
    if(NStCompr.cor) {sprintf(buf,", and correctors");  editor->insertPlainText(buf);}
    editor->insertPlainText(".\n");
    sprintf(buf,"They have been replaced in the new file (%s) by %d equivalent elements", filenameto.toUtf8().data(), i2);
    editor->insertPlainText(buf);
    editor->insertPlainText(".\n");
    goto en;

rtrn:	
    char errmsg[512];
    sprintf(errmsg, "%s : %s\n", "Error", buf);
    editor->insertPlainText(buf); 
    editor->insertPlainText(bel);
en:	
    if(fp1)fclose(fp1);
    if(fp2)fclose(fp2);
    sw->show();  
    editor->document()->setModified(false);

#endif

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

