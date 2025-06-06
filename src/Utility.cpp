//  =================================================================
//
//  Utility.cpp
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
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <map>
#include <regex>
#include <string>


#include <Globals.h>
#include <Utility.h>
#include <Element.h>
#include <Constants.h>
#include <Structs.h>
#include <Twiss.h>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cfloat>
#include <memory>
#include <vector>
#if defined(_MSC_VER)
#include <io.h>
#else
#include <unistd.h>
#endif
#include <QFileInfo>
#include <QDir>

#ifdef USE_MSWINDOWS
#include <windows.h>
#endif

#define LSTR 1024
#define NUMBER_THRESHOLD 1.e10
#define NAME_LENGTH 32 

// Utility routines 

using Constants::B_Euler;
using Constants::EPS;
using Constants::PI;
using Utility::readLineCalc;
using Utility::readLineCmt;

static const double pi = PI; 

namespace Utility {


void fPrint(FILE* fp, char const* buf1)
{
   // print the content of buf1 to file fp and strip all blank space   

  char  buf[512];
  char* bp = buf;
 
  while((*buf1 != 0) && (*buf1 != ' '))*bp++ = *buf1++;
  *bp = 0;
  fprintf(fp,"%s ", buf);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

  
int factorial ( int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QString strippedName(QString const &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


 char* getVariableName(char *buf, char *name, char *modif)
{
 char *bp1 = buf;
 char *bp2 = name;

 *bp2++ = *bp1++;
  
 while(isalnum(*bp1) || (*bp1=='_') || (*bp1==':') ) *bp2++ = *bp1++;
 
 *bp2=0;
 bp2=modif;
 
 if(*bp1=='&') {
   *bp2++ = *bp1++;
   while (isalnum(*bp1) || (*bp1=='_') || (*bp1==':') ) *bp2++ = *bp1++;
 }
 *bp2=0;
 return bp1;
}






//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

  bool isValidType( char const* name)
  {

  static char validtypes[] = "ABCDEFGHIJKLMOQRSTUVWXZ";
  char* vp = validtypes;
  
    // check if name is that of a valid Element type

    while ( *vp && (toupper(*vp) != toupper(*name)) ) ++vp;

    return (*vp);
  }  
    
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


  char* getElmNameX( char*& buf, char* const shortname, char* const fullname )
  {

  // buf     input:  list of Elements names  
  //         output: same list, starting at the character that follows the Element whose name is returned.       
  // fullname    return : full Element name
  // shortname   return : name before first '&' character (if present)
  //
  // return value : pointer to buffer if buffer no empty and a valid name was found
  //              : no Element was found or the Element is of invalid type
  //-----------------------------------------------------------------------------------------  

  *shortname = *fullname = 0;
  char* fnp = fullname;

  // skip white space

  while( isspace(*buf) ) ++buf;
  if( (*buf == 0) ) { return 0;}   // we reached the end of the buffer

  // extract next Element name from buffer  
  
  while (  (*buf) && (!isspace(*buf)) && (*buf != ';') ) {
     *(fnp++) = *(buf++);
     if ((fnp-fullname) > (NAME_LENGTH-1) ) { 	 // names longer NAME_LENGTH-1 characters are truncated 
       while (  (*buf) && (!isspace(*buf)) && (*buf != ';') ) ++buf;
       break;
     }
  }

  *fnp = 0;  // terminate the fullname string
  
  // look for '&' in name.  
  size_t span = strcspn(fullname,  "&");
  strncpy( shortname, fullname, span); 
  shortname[span] = 0;
   
  // strip the '&' (concatenation) operator from the name   
  if (fullname[span]) memmove(&fullname[span], &fullname[span+1], strlen(fullname) - span);
  
  return buf; 
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int getElmName(char const* ellist, char **buf, char *buf1, char *fullname)
{
       
  // NOTE: THE INTERFACE OF THIS FUNCTION HAS BEEN MODIFIED I.E. IT DIFFERS FROM
  //       THAT OF THE CORRESPONDING FUNCTION IN THE OLD WINDOWS CODE.
  //       *** THE SECOND ARGUMENT (buf) TYPE HAS CHANGED ****
  // ---------------------------------------------------------------------------   
  // ellist   = string containing a list of valid Elements one-letter types  ( input)
  // *buf     = points to lattice string spec. (input/output) pointer moves beyond the Element whose name is returned.       
  // buf1     =  
  // fullname = fullname of the last Element found       (output)
  //
  // RETURNS:  0 if an Element is found, 1 if no Element was found. 
  // ----------------------------------------------------------------------------


  int i=0, k=0;
  
  while( (**buf==' ' || **buf=='\t') &&  (**buf != 0 ) && (**buf != '\n') &&( **buf !=';') ) (*buf)++;
  
  if( (**buf == 0) || (**buf == '\n') || (**buf == ';')) { return 1;}  //empty string;

  while( **buf !=' ' && **buf!='\t' && **buf != 0 && **buf!='\n' && **buf!=';' && i < (NAME_LENGTH-2)){
    *fullname = **buf;
    if(**buf=='&')k++;  else fullname++;
    if(!k){*buf1++ = **buf; i++;}
    (*buf)++;
  }
  *fullname = 0; 
  while(i < (NAME_LENGTH-1)){*buf1++ = 0; i++;}
  *buf1=0;
  while(*ellist != '\000' && *ellist != toupper(*(buf1-NAME_LENGTH+1)) ) ellist++;
  
  if(*ellist == 0) {*buf1 = 0; return 1;}; //no such Element
  
  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int GetUserNameLocal( char* buf,  long unsigned int* bufsize) 
{
  #if defined(_WIN32) ||  defined(_WIN64)
  TCHAR wtext[255];
  std::mbstowcs(wtext, buf, strlen(buf)+1);
  TCHAR* wbuf = wtext; 
  return ::GetUserName( wbuf, bufsize);
  #endif
  #ifdef __linux
  return getlogin_r(buf, *bufsize);
  #endif
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char* GetVariableNameSp(char* name, char* buf)
{
  char* b = buf;
  char* p = name;
  
  while (*b!='\n' && *b!=0 && isspace(*b))b++;  // skip spaces but '\n'
 
  if(*b=='\n' || *b==0)return 0;
  if(*b == '$')*p++ = *b++;
  else return 0;
  
  while( isalnum(*b) || (*b=='_') || (*b==':') ) *p++ = *b++;
  *p=0;
  while (*b!='\n' && *b!=0 && (isspace(*b) || *b==','))b++;// skip spaces and commas but '\n'
  return b;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool checkComment(char* buf) {

 // returns true if buf contains only whitespace, is empty or if the first 
 // non-whitespace character is '#', false otherwise
   
 while(*buf ){
   
   if(*buf=='#' || *buf=='\0') return true;
   if(*buf==' ' || *buf=='\t') { ++buf; continue; }
      return false;
   }
   return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void copyname(char* buf1, char const * buf, char const* fullname) {
 
  while((*buf != 0) && (*buf != ' ')) {
     *buf1++ = *buf++;
     fullname++;
  }
 
  *buf1 = 0;
  
  if(*fullname!=0) {
    strcat(buf1,"&"); 
    strcat(buf1,fullname);
  }
  strcat(buf1," ");
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int decodeNumber(char* &buf, double& d) 
{

 // buf  =  ascii representation  [input]  
 // d    =  corresponding double  [output]
 // return  0 : all ok, double has been read
 //         1 : no real number to be decoded. 
  
  char  buf1[LSTR];
  
  char* bp = buf1;

  d = 0.0;
 
  while(    ((*buf == ' ') || (*buf == '\t')) 
	&& ((*buf != 0)   || (*buf != '\n')) )  { 
   ++buf; 
  }
	
  if((*buf==0)||(*buf=='\n')) return 1;
	
  while(*buf!=' ' && *buf !='\t' && *buf!='\000' 
	         && *buf!='\n') {
    *bp++ = *buf++; 
   }
  
   *bp = 0;

   d = atof(buf1);
  
   return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int decodeLineNew(char const* buf, std::vector<double>& d, int n)
{
  using std::regex_match;
  using std::regex;

  d.resize(0);
  //for (int i=0; i<n; ++i) { d[i] = 0.0; }
  
  std::map<std::string,double> args_map;

  // buf string format:  <element_label> <attribute_label> = <floating_point >   

  static regex     re_label(R"raw(^\s*([^\s]+)\s+(.*))raw");
  static regex re_attribute(R"raw(\s*([a-zA-Z\[\]_\/\$0-9]+)?\s*=\s*([-+]?(?:[0-9]*[.])?[0-9]+(?:[eE][-+]?\d+)?).*)raw"); // floating point

  
  std::cmatch cm;
  bool ismatch_label = regex_match(buf, cm, re_label);

  if (!ismatch_label) {
    return 0; // no element label found 
  }
 
  const char* p = std::get<0>(*(--cm.end()));  // p points to unparsed part of the string 
 
  // look for additional arguments
  int i = 0;
  while (bool ismatch_attribute = regex_match(p, cm, re_attribute)) {
    auto itb = cm.begin();
    for(auto it = ++itb; it< cm.end(); ++it) {
       auto s = it->str();
      ++it;
      double val =  atof(it->str().c_str());
      args_map[s] = val;
      //d[i++] = val;
      d.push_back(val);
      if( fabs(val) > 1.0e-6 * DBL_MAX ) {return -1;}
    }
    p = std::get<1>(*(--cm.end()));  
  }  
   
  return args_map.size();
} 

//.............................................................................

int decodeLine(char const* buf, double* d, int n)
{
   // d: array of doubles

  
   char bufs[LSTR];
   double r;
    
   for(int i=0; i<n; ++i) {*(d+i) = 0.0;} // init all arguments to 0.0
   
   int i=0;
   if(buf[0]==0) { return i; }            // 0 arguments 

   char* bs = bufs;
   char const*  b = buf;

   while (*b != 0) {
    while((*b != '=') && (*b != 0))b++;
    if(*b==0) {return i;}
    b++;
    while (*b == ' ') {b++;}
    while (*b != ' ' && *b != '\t' && *b != '\000') {*bs++ = *b++;}
    *bs=0;
    r = atof(bufs);
    if( fabs(r) > 1.0e-6 * DBL_MAX ) {return -1;}
    *d++ = r;
     bs  = bufs;
    if( n == ++i) {return i;}
   }
   return i;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int decodeExtLine(char const* buf, double* dat, int n)
{

 std::vector<char> tmpbuf(strlen(buf)+1); 
 char*  tmpbufp = &tmpbuf[0];
   
 strcpy(tmpbufp, buf);

 for (int i=0; i<n; ++i) { dat[i]=0.0;}
 
 int i=0;
 double xp;
 for (; i<n; ++i) {
   if (  decodeNumber(tmpbufp, xp) != 0) break;
   dat[i] = xp;
}

 return i;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char const* filterShort(char const* buf, char const* filter)
{

  //  buf : null terminated string
  //
  //  if the string matches the filter, returns a pointer to the position immediately
  //  after the end of match within buf, otherwise, returns 0;
  //  % is a single character wildcard (i.e. '%' matches any single character)

  //  char const* pt = buf;
  //  while( (*pt) && (*pt != ' ')) { ++pt;}
  //  *pt = 0;  
  
  while( (*buf) && (*filter) ) {
    
     if(*filter=='%'){filter++; buf++; continue;}

     if(*filter == *buf) { filter++; buf++; }
     else return 0;
  }
  
  return (*filter == 0) ? buf : 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool filterNameOnce(char const* name, char const* filter, bool MatchCase)
{

  char    bf[LSTR];
  char   nam[LSTR];   // copy of name,  converted to uppercase if requested
  char  filt[LSTR];   // copy of filter 
  char* str;

   int n;

   if(MatchCase){
     strcpy(filt, filter);
     strcpy(nam,  name  );  
   }

   else { // nam and filt become upper case versions of name and filter
     str = nam;
     while (*name) {
       *str++  = toupper(*name++);
     }
     *str = 0;

     str  = filt;
     while (*filter) { *str++ = toupper(*filter++); }
     *str=0;

   }

   if(*filt==0) return true;

   char const* pfilt = filt;
   char const* pnam  = nam; 

   while (*pfilt ) {
     if(*pfilt != '*') {
       n = 0;
       while( (*pfilt != '*') && (*pfilt) ) { *(bf+n) = *(pfilt++); n++; }
       *(bf+n) =  0;
       pnam =  filterShort(nam, bf);
       if (pnam == 0 ) { return false;}
       continue;
     }

     else { 

       if ( *(++pfilt) == 0) return true;
       n = 0;

       while((*pfilt != '*') && (*pfilt) ) {*(bf+n) = *(pfilt++); ++n;}

       *(bf+n) = 0;
       
       char const* str = 0;

       do {
	 str = filterShort(pnam, bf);
	 if(str) { pnam = str; break; }
        } while(*(++pnam)!=0);

        if(( str==0) && (*pnam==0)) return false;
     }
   }

   return ( (*pfilt == 0 ) && (*pnam ==0));
   
}  


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void strReplace(std::string& str, const std::string& oldStr, const std::string& newStr){
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos){
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool filterName(char const* name, char const* filter, bool MatchCase) 
{

  // Like FilterNameOnce but can use multiple filters separated by ";"

  char filt[LSTR];
  
 stflt:	
   
  char* str = filt;

  while( (*filter != 0 ) && (*filter != ';') ) {*str++ = *filter++; }  
  *str=0;
			
  if (*filt == 0 )  return false; // takes care of a final ';' if present 

  if( filterNameOnce(name,filt,MatchCase)) return true; 

  if (*filter ==0 ) {
    return false;
  }
  else {
    ++filter; 
    goto stflt;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char* getLineFromBuffer(char* peb, int lim, char* buf)
{
  
  while (*peb>=32 || *peb=='\t') {
    *buf++ = *peb++;
    if( lim-- <=2 ) break;
  }
  
  if(*peb==13 || *peb==10) peb++;
  if(*peb==13 || *peb==10) peb++;
  
  *buf=0;
  
  if(*peb==0) return 0;
   
  return peb;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int split(char* buf, int n, char **varg)
{

  char *pt;
  int i=0;
  
  while (*buf) {
   
   if((*buf==' ') || (*buf=='\t')) {buf++; continue;}
   
   pt=varg[i];
   
   if (*buf == '\"'){
     buf++;
     while (*buf){
       if(*buf=='\"') { buf++; break;}
       *pt++ = *buf++;
     }
   } 
   else {
     
      while(*buf){
        if((*buf==' ')||(*buf=='\t'))break;
	*pt++=*buf++;
      }
   }
   
   *pt=0;
   i++;
   
   if(i==n)break;
  }
  
  varg[i]=0;  

  return i;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int splits(char* buf, int n, char **varg)
{
  char *pt;
  int i = 0;

  while(*buf) {
    
    if((*buf==' ')||(*buf=='\t')) {buf++; continue;}

    pt=varg[i];

    if(*buf == '\"'){
      	buf++;
	while(*buf) {
	  if(*buf=='\"') {buf++; break;}
	  *pt++=*buf++;
	}
    } 
    else {
      while (*buf){
        if ((*buf==' ') || (*buf=='\t') ) break;
        *pt++=*buf++;
      }
    }
    
    *pt=0;
    i++;

    if (i==n) break;
  
  }
  return i;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char* getFileNameOpt(char* buf, char* MainFileDir, char* IncludeFileName)
{

  char  buf1[256];
  char* bin  = buf;
  char* bfin = buf1;
   
  // strip whitespace and quotation marks
  
  while ((*bin==' ' || *bin=='\t') && *bin!='\000') { bin++; }
 
  if (*bin==0) return 0;

  if (*bin=='\"'){
    bin++;
    while(*bin!='\"' && *bin!='\000') {*bfin++=*bin++;}
    if(*bin==0) return 0;
  }
  else {
    while(*bin!=' ' && *bin!='\t' && *bin!='\000')*bfin++=*bin++;
  }
  
  *bfin = 0;

  
  if(buf1[1]==':' || buf1[0]=='\\' || buf1[0] == '/') { // keep whole path is the root path is specified  
     strcpy(IncludeFileName, buf1);
  }
  else {

     QString IncludeFileString = QDir(MainFileDir).filePath(buf1);
     strcpy(IncludeFileName, IncludeFileString.toUtf8().data());
  }
 
  return IncludeFileName;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char* cutName(char* buf)
{
  // trim 

  static char name[256];
  char* pt = &name[0];
  
  int i,j;
  
  
  for(i=j=0; i<255-10; ++i) {
    *pt++ = buf[i];
     ++j;
      
     if(buf[i]==0) break;

     if(j>35) {*pt++ =' '; j=0; }
  }
   
   name[255]=0;

   return name;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char* fileextn(char* name, char* extn)
{

  char * nam = name;
  
  while( *name != '.' && *name != '\000') name++;
  
  if( extn ){
    *name++ ='.';
    while (*extn != '\000') *name++=*extn++;
    
    *name='\000';
  }
  else{
   if( *name++!='\000') {
     if (*name!='\000') *name='~';
   } 
   else { 
     *name++='~'; 
     *name=0;
   }
  }
   return nam;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 1
double gauss(unsigned int seed)
{

  typedef boost::mt19937 random_generator; 
  static random_generator rng;

  if (seed != 0 ) {
    rng.seed(seed);
  }
  
  static boost::normal_distribution<> nd(0.0, 1.0);
  static boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);

  return var_nor();
}
#endif

#if  0
 // .......................................................
 //  Old implementation.
 //
 //  Y = sum X_i   where  the X_i's are uncorrelated uniformly
 //                distributed random variables  [ 0,1] 
 // 
 //  returns Z = Y - <Y>     mean(Y)     =  N/2
 //                          variance(Y) =  N/12
 //
 //  where N is the number of X_i in the sum
 //
 // With 12 terms, the variance is 1.0
 //
 // This is known as the Irwin-Hall distribution. For N->infty,
 // the distribution of the sum  converges to a normal distribution. 
 // .......................................................
double gauss()
{

 double c = 0.0;

 for (int i=0; i<12; ++i) { 
      
   c += double ( rand() ) / RAND_MAX; 
 }
 
 c -= 6.0;

 return(c);

}
#endif
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::string trim(std::string const& str)
{
    if(str.empty())
        return str;

    std::size_t firstScan = str.find_first_not_of(" \n\r\t");
    std::size_t first     = firstScan == std::string::npos ? str.length() : firstScan;
    std::size_t last      = str.find_last_not_of(" \n\r\t");
    return str.substr(first, last-first+1);
}
//------------------------------------------------------------------------------------------
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool strcmpr(char const* buf1, char const* buf2) 
{
  // returns : true if the first characters of buf2 matches all the characters of buf1.
  //         : false otherwise
  //           i.e. buf1 is a substring of buf1 
  // examples : with s1 = "thequick" and s2 = "thequickbrownfox"   strcmpr returns true
  //            with s1 = "thequickbrownfox" and s2 = "thequick"   strcmpr returns false

  while(*buf1 != 0) { 
    if(*buf1++ != *buf2++) return false;
  }
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool strcmpre(char const* buf1, char const* buf2)
{
  // returns: false if str(buf2) != str(buf1)
  //          true  if str(buf2) == str(buf1)
  //
  //  NOTE: *** trailing white space is ignored
		
  while((*buf1 != 0) && (*buf1 != ' '))  if(*buf1++ != *buf2++) return false;

  if((*buf2 != 0) && (*buf2 != ' '))  return false;
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//------------------------------------------------------------------------------
// remove spaces and tabs at the beginning of buf2 and compare it with buf1
// return:  0 if strings do not coincide,
//          address of the end of comparison for buf2

char* strcmpf(char const* buf1, char* buf2) 
{
  // removes white space( spaces and tabs)
  while( isspace(*buf2) ){ if (*buf2) ++buf2; else return 0; }
  // compare

  while(*buf1) if(*buf1++ != *buf2++) return 0;

  return buf2;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char* strcmpp(char const* buf1, char* buf2)
{
  // removes spaces and tabs
  while( isspace(*buf2) ) { if(*buf2) ++buf2; else return 0;}
  // compare
  while(  *buf1) if(*buf1++ != *buf2++) return 0;
  // skip "="
  while ( (*buf2) && (*buf2 != '='))  ++buf2;

  return ( !(*buf2) ) ? 0 : buf2+1;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// skips spaces and tabs
char* skipspaces(char* buf)
{
  while((*buf == ' ')||(*buf == '\t')) buf++;
  return buf;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// copy src into OptiM Element name
//
// dest = optim Element name

  void cpname(char* dest, char* src)
{
   int i=0;
   while( isspace(*src) && (*src != '\n') && (*src != 0)) {
     *dest++=*src++;
     ++i; if (i==9) break; // ***** MAGIC NUMBER ?????
   }
   *dest = 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// skip  spaces and tabs and then copy string up to space, tab, '(' or <NL>
// return the address where copying stopped

char* copystr(char* dest, char* src)
{
  while( (*src == ' ')||(*src == '\t') ) ++src;

  while( (*src != ' ') && (*src != '\t')
	 &&(*src != '\n') && (*src != 0) && (*src != '(') ) {
    *dest++=*src++;
  }

  *dest=0;
  return src;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Seven point filter fitting the second order polynomial

void Filter7(double* y, int n)
{
  
  std::vector<double>  yr(n);

  constexpr double c0 = 1./3., c1=2./7., c2=1./7.,  c3=2./21.;
 
  for(int i=3; i<n-3; ++i){
    yr[i] = c0*y[i] + c1*(y[i+1]+y[i-1]) + c2*(y[i+2]+y[i-2]) - c3*(y[i+3]+y[i-3]);
  }

  for(int i=3; i<n-3; ++i) y[i]=yr[i];
    
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Five point filter fitting the second order polynomial

void Filter5(double* y, int n)
{
  std::vector<double>  yr(n);
  constexpr double c0=17./35., c1=12./35., c2=3./35.;
 
  for(int i=2; i<n-2; ++i){
    yr[i]=c0*y[i] + c1*(y[i+1]+y[i-1]) - c2*(y[i+2]+y[i-2]);
  }
  for( int i=2; i<n-2; ++i) y[i]=yr[i];
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Integrand(double x, double y)
{
  double sig = exp(-1.-x);
  double ys  = y/sig;
  double s  = 0.5*log(1.+ys*ys);
  double p  = atan(ys);

  return exp(sig*s-y*p)*cos(y*(s-1.)+sig*p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Landau(double x)
{

 double sig,S,w0,w1,w,xB,Land;

 if(x <= -4.){
   double e,ex,ds;
   sig = exp(-1.-x);
   S = sqrt(sig/(2.*PI))*exp(-sig);
   Land = S*(1.-5./(24.*sig));
   e = 2.*sig;
   ex = exp(e);
   ds = sqrt(3.*sig*e);
   double F1 = S*(1.+(ex-1.)/(4.*sig*e)+2.*ex*exp(-ds)/(sqrt(PI*ds)*(1.-sqrt(e/(3.*sig)))));
 }
 else if(x >= 100.){
 //	  
   w1 = x*( 1.-(log(x)+B_Euler)/(x+1.) );
   w1 = x*(1.-(log(x)+1.)/(x+1.));
   xB = x - B_Euler;
   w = xB - log(xB);
   do {w0 = w; w = xB - log(w0); } while (fabs(w-w0) > EPS);
      double p2 = PI*PI; 
      double w4 = w*w*w*w;
      double R1 = exp(0.926/w)*(p2/2.-3.75)/w4+17.*p2/(6.*w*w4);
      S = 1./(w1*w1+p2);
      double ww = w1-log(w1)-B_Euler;
      double wx = ww*(1.+1./x);
      double xB = log(x)+B_Euler;
      Land = S-R1-(1.+1./w1)*xB*xB*exp(0.926/ww)/(wx*wx*wx*w1*w1);
      double F1 = S+R1;
   }
   else {
     sig = exp(-1.-x);
     int n = 200;
     double s = 0.0;
     double dt = 12./n;
     for (int i=0;i<n+1;i++){
       double t = -6.+i*dt;
       double f = 1.;
       if(i == 0 || i == n)f=0.5;
       s+= Integrand(x,t)*f;
     }
     Land = 0.5*exp(-sig)*s*dt/PI;
   }
  return Land;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char* splitText(char const* buf)
{

 // split text into 35 character chunks separated by a blank space   
 // no clear what this is for ....  
  
 static char name[512];
 char* pt = name;

 int j = 0;
 for( int i=0; i<500; ++i) {
  *pt++ = buf[i];

  ++j;

  if ( buf[i]==0 )   break;
  if ( buf[i]==' ' ) { j=0;}
  if ( j>35 )        {*pt++=' '; j=0;}
 }

 name[511]=0;

 return name;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void correctNames(char *strng, char *modif)
{

 char  buf[LSTR+1]; 
 char* p;
 
 if( *modif==0 ) return;
 strcpy(buf, strng);

 char* bp1 = buf;
 char* bp2 = strng;

 while(*bp1){
   while(*bp1==' ' || *bp1=='\t')*bp2++ = *bp1++;
   while(isalnum(*bp1) || (*bp1=='_') || (*bp1==':') ) *bp2++ = *bp1++;
   p = modif;
   while(*p) *bp2++ = *p++;
 }
 *bp2=0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#if 0
void correctNames(char *strng, char *modif)
{
  // Adds suffix (modif) to all names in the string  (strng)

  char buf[LSTR+1], *bp1, *bp2, *p;

  if(*modif == 0) return;

  strcpy(buf, strng);
  bp1 = buf;
  bp2 = strng;

  while(*bp1){
   while(*bp1 ==' ' || *bp1=='\t') *bp2++ = *bp1++;
   while(isalnum(*bp1) || (*bp1=='_') || (*bp1==':') ) *bp2++ = *bp1++;
   p =modif;
   while(*p)*bp2++ = *p++;
  }

  *bp2 = 0;
}

#endif
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#if 0
// Extract an expression from the line (buf), returns pointer to the end of expression
char* getExpression(char* buf, char* expr_str)
{
  char *bp1, *bp2;
  char const *p;
  int i;
  
   bp1=buf;
   bp2=expr_str;
   while(1){
     while((*bp1==' ')||(*bp1=='\t'))bp1++; // skip spaces
      if((*bp1=='\n')||(*bp1==0)){*bp2=0; return bp1;}
      *bp2++=' ';
      if(isalpha(*bp1)||(*bp1=='_')||(*bp1==':')){//a word starting from letter (if not function) marks the expression end
      	for(i=0; *FuncList[i] !=0; i++){
          if(strcmpr(FuncList[i], buf)){
            p=FuncList[i];
            while(*p){*bp2++=*p++; bp1++;}
            break;
          }
        }
        if(*FuncList[i]==0){*bp2=0; return bp1;}
      }
      while((*bp1!=' ')&&(*bp1!='\t')&&(*bp1!='\n')&&(*bp1!=0))*bp2++=*bp1++;
   }
}
#endif
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> farey( int n, bool descending = false)
{
  // generate the nth Farey sequence, either ascending or descending

  std::vector<double> vseq;
  

  int a = 0;
  int b = 1;
  int c = 1;
  int d = n;

  if (descending) {
        a = 1;
        c = n-1;
  }
    
  vseq.push_back(double(a)/double(b)); 

  while ( ((c <= n) && (!descending)) ||
	   ((a > 0) && descending)    ) {

    int k = (n + b) / d;
    a = c;
    b = d;
    c = (k*c-a);
    d = (k*d-b);

    vseq.push_back(double(a)/double(b));
  };

  return vseq;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

} // namespace Utility
