//  =================================================================
//
//  UtilityCalc.cpp
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

#include <Utility.h>
#include <SCalculator.h>
#include <Globals.h>

using Utility::checkComment;
using Utility::getFileNameOpt;
using Utility::correctNames;
using Utility::getVariableName;
using Utility::strcmpr;
using Utility::trim;

#define LSTR 1024

namespace Utility {
  
#if 1
char* readLineCmt(FILE* fp, char* buf)
{
  char* ret;
  do{ret=fgets(buf, 255, fp);
  if(ret==0)return ret; }
  while(*buf=='#');
  while((*buf!='\n') && (*buf!='\000'))buf++;
 *buf='\000';
 return ret;
}

#endif

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char* readLineCmt(FILE* fp, char* buf, char* MnFileDir, char* buferr)
{
  char b[LSTR+1];
   if(readLineCalc(fp, b, MnFileDir, buferr) ==0 ) return 0;
   return decodeLineCalc(b, buf, buferr);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char* getExpression(char* buf, char* expr_str)
{
  // Extract an expression from the line (buf), returns pointer to the end of expression
  // 
  
  char const* p;

  char *bp1 = buf;
  char *bp2 = expr_str;

  while(true){
    while( (*bp1==' ') || (*bp1=='\t')) bp1++; // skip spaces
    if ( (*bp1=='\n') || (*bp1==0)) {*bp2=0; return bp1;}
    *bp2++=' ';
    if (isalpha(*bp1) || (*bp1=='_') || (*bp1==':')) { // a word starting with an alpha character (letter) marks the end of an expression 
                                                       // marks the expression end if this worrd is not a function name.
      int i=0;
      for( i=0; *SCalc::FuncList[i] != 0; ++i) {
	if(strcmpr( SCalc::FuncList[i], buf)){
	   char const* p = SCalc::FuncList[i];
           while(*p){*bp2++=*p++; *bp1++;}
           break;
         }
      }
      if(*SCalc::FuncList[i]==0){*bp2=0; return bp1;}

    }
    while ((*bp1 !=' '  ) && (*bp1!='\t') &&
	   (*bp1 != '\n') && (*bp1!=0   )) *bp2++=*bp1++;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char* decodeLineCalc(char* bufin, char* buf, char* buferr)
{
  auto& IncludeMode      = appstate.IncludeMode;
  auto& IncludeFileName  = appstate.IncludeFileName;
  auto& InclLineNum      = appstate.InclLineNum;
  auto& calc_            = *appstate.calc;

  char* bp1;
  char* bp2;
  char *p;
  char str_result[LSTR+1];
  char name_var[32];
  char modif[32];
  char expr_str[LSTR+1];
  double result;

  bp1 = bufin;
  bp2 = buf;

  while(( *bp1 != 0) && (*bp1 !='\n')) {
    if(*bp1 != '$' && *bp1 != '=') {*bp2++ = *bp1++; continue;}
    if(*bp1 == '$'){
      bp1 = getVariableName(bp1, name_var, modif);
      if(calc_.findValue( name_var, &result,"%-17.12le", str_result)>0) {
        sprintf(buferr, "Variable not found <%s>", name_var);
        *bp2        = 0;
	IncludeMode = false;
	return 0;
      }

      correctNames(str_result, modif);
      p = str_result;
      while(*p) *bp2++ = *p++;
    }
    if (*bp1 == '='){
      *bp2++ = *bp1++;
       bp1   = getExpression(bp1, expr_str);
       if (calc_.calcLine(expr_str, &result, "%-17.12le", str_result, buferr)>0) {
         sprintf(&buferr[strlen(buferr)], " at the line <%s>", bufin);
        *bp2=0;
	 IncludeMode = false;
	 return 0;
       }
       p = str_result;
       while(*p) *bp2++ = *p++;
       *bp2++ = ' ';
    }
  }
  *bp2 = 0;

  return buf;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  


char* readLineCalc( FILE* fp, char* const buf, char* MnFileDir, char* buferr)
{
  // if successful, returns pointer to string,
  // returns 0 and *buferr=0 at the end of file
  // returns 0 and buferr points to error string if error

  auto& IncludeMode      = appstate.IncludeMode;
  auto& IncludeFileName  = appstate.IncludeFileName;
  auto& InclLineNum      = appstate.InclLineNum;
  auto& calc_            = *appstate.calc;
  
 char *p;
 static FILE *fp2;
 *buferr = 0;

 // read line from file to b
 do {
   if(IncludeMode){
     if( fgets(buf, LSTR,fp2) == 0) {
       fclose(fp2);
       IncludeMode = false;
       buf[0] = 0;
     }
     p=buf;

     while(*p!=0) {if(*p=='\n')*p=0; ++p;} // removing \n
     InclLineNum++;
   }
   else {
     if(fgets(buf, LSTR, fp) == 0){ buf[0] = 0; return 0;}
     p=buf;
     while(*p!=0){if(*p=='\n')*p=0; ++p;} // remove \n

     if(strcmpr("#include",buf)) {
       if( getFileNameOpt(&buf[8], MnFileDir, IncludeFileName) == 0){
         sprintf(buferr,"Cannot get file name in #include in line=<%s>", buf);
         return 0;
	 
       }
       std::string str(IncludeFileName);
       str = trim(str);		       
       fp2 = fopen(str.c_str(),"r");
       if ( fp2==0) {
         sprintf(buferr, "Cannot open file <%s> in #include directive", IncludeFileName);
         return 0;
       }
       else {
         IncludeMode=true;
         if (fgets(buf, LSTR,fp2) == 0){
           fclose(fp2);
           IncludeMode=false;
           *buf='\n';
         }
         p = buf;
         while(*p!=0) { if(*p=='\n')*p=0; ++p; }
         InclLineNum = 1;
       }
    }
  }
 } // do 

 while( checkComment(buf) );
 return buf;
}

} // namespace
