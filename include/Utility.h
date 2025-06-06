//  =================================================================
//
//  Utility.h
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

#ifndef UTILITY_H
#define UTILITY_H

#include <complex>
#include <functional>
#include <vector>

struct Element;
class QString;

  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

namespace Utility {

  extern void    fPrint(FILE* fp, char const* buf1);
  
  extern int     factorial(int n); 

  extern void    correctNames(char *strng, char *modif);
  
  extern QString strippedName(QString const& fullFileName);

  extern bool    strcmpr(char const* buf1, char const* buf2); 

  extern std::string trim(std::string const& str);

  extern void    correctNames(char *strng, char *modif);

  extern char*   getExpression(char* buf, char* expr_str);

  extern char*   getVariableName(char *buf, char *name, char *modif);

  extern bool    isValidType( char const* name);
  
  extern int     getElmName(char const* ellist, char** buf, char *buf1, char* fullname);

  extern char*   getElmNameX(char*& buf, char* const shortname, char* const fullname);

  //  extern void    CopyElement(Element *elm2, char*name2, Element *elm1, char const* prefix, char const* name1); //deprecated

  extern int     GetUserNameLocal( char* buf,  long unsigned int* len);

  extern bool    checkComment(char* buf);

  extern char*   readLineCmt(FILE* fp, char* buf);
  extern char*   readLineCmt(FILE* fp, char* buf, char* MnFileDir, char* buferr);

  extern char*   readLineCalc( FILE* fp, char* const buf, char* MnFileDir, char* buferr);
  
  extern int     decodeNumber(char *& buf, double& d);

  extern int     decodeLine(char const* buf, double* d, int n);
  extern int  decodeLineNew(char const* buf, std::vector<double>& d, int n);

  extern  char*  decodeLineCalc(char* bufin, char* buf, char* buferr);

  extern  int    decodeExtLine(char const* buf, double* dat, int n);

  extern void    copyname(char* buf1, char const* buf, char const* fullname);
  
  extern char*   getLineFromBuffer(char* peb, int lim, char* buf);

  extern int     split(char* buf,int n, char **varg);

  extern int     splits(char* buf,int n, char **varg);

  extern char*   getFileNameOpt(char* buf, char* MainFileDir, char* IncludeFileName);

  extern char*   cutName(char* buf);

  extern char*   fileextn(char* name, char* extn);

  extern bool    filterName(char const* name, char const* filter, bool MatchCase); 
 
  extern double  gauss(unsigned int seed=0);

  extern bool    strcmpr(char const* buf1, char const* buf2);

  extern bool    strcmpre(char const* buf1, char const* buf2);

  extern char*   strcmpf(char const* buf1, char* buf2); 

  extern char*   strcmpp(char const* buf1, char* buf2);

  extern char*   skipspaces(char* buf);

  extern void    cpname(char* dest, char* src);

  extern char*   copystr(char* dest, char* src);
  
  extern void    Filter7(double* y, int n);

  extern void    Filter5(double* y, int n);

  extern double  Integrand(double x, double y);

  extern double  Landau(double x);

  extern double  LandauRng( std::function<double()> const& random);

  extern char* GetVariableNameSp(char* name, char* buf);

  extern char* getExpression(char* buf, char* expr_str);

  extern char* splitText(char const* buf);

} // namespace Utility




#endif //  UTILITY_H


