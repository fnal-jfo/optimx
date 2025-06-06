//  =================================================================
//
//  SCalculator.h
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

/* Calculator parsing rules

Program:
	expr_list END			// END is the end of line
expr_list:
	expression PRINT  	// PRINT  is '\n' or ';'
	expression PRINT expr_list
expression:
	expression+term
	expression-term
	term
term:
	term/power
	term*power
power
	power^primary
	primary
function:
	function(expression)
primary:
	NUMBER      			// number in float or exponential format
	NAME                 // variable name
	FUNCTION
	NAME=expression
	-primary
	(expression)
*/

/*----------------------------------------------------------------------

Class SCalculator 
	SCalculator();	       

	virtual ~SCalculator();

	int CalcLine(char* buf, double* result, char* format,
			        char* str_result, char* strerr);
		Performs calculations  of determined by one line. A few operators can be
		in the line. Keeps values of all assignments performed in the course of
		computations. PERL type input languige.
		Examples:
			$a=sin($pi*0.25+0.05)*(0.2*0.2+0.5*0.5*sqrt(5)); $b=$a+20;
			$text12="print text here"
			2*3+5
			$var1=$a+$b
		Parameters:
			*buf  -	input line, '\n', and '\0' denote the end of line,
			;' denotes the end of operator, variables begin with $ like
						in PERL script
			*result -  result of the calculations
			str_result  - result of the calculations presented by a text string
							  determined by format.
			sterr   -   keeps explanation of the last encounted error
			return	-  number of errors.
         				0, -1 and -2 - means no errors,
                      0 - numerical variable
                     -1 - text variable
                     -2 - Array is created and assigned
		
                 Predefined names
		
                 	$pi = $PI = 3.1415926535897932385;
			$e = $E = 2.7182818284590452354;
			$Me = 0.511006;
			$Mp = 938.;
			$c = 2.997925e-10;

	int FindValue(char* name_var, double* result,
		      char* format, char* str_result);
		Extracts a value of the variable which were previously stored
		Parameters:
			name_var	- name of the variable
			result	- value of the variable
			str_result  - result of the calculations presented by a text string
							  determined by format.
			return  -  1 - no such name,
         			  0 - number,
                    -1 - text string (result=1)
                    -2 array (then *result has array number).

        int FindValueInArray(char* name_var, int n, double* result);
          Finds value of n-th Element in Array name_var.
          Returns the array length, or zero if not array

	inline void ZeroCounter();  // zeros the counter for extraction of values
				       of all variables with function GetNextValue( )

	int GetNextValue(char* name_var, double* result, char* format,
					               char* str_result);
			return  -  0 - number,  
                                  -1 - text string (result=1), 
                                   1 - when the last number was extracted
-------------------------------------------------------------------------
*/

#ifndef SCALCULATOR_H
#define SCALCULATOR_H

#include <cctype>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>
#include <unordered_map>
#include <ostream>
#include <iostream>
#include <platform.h>

struct Variable {
  std::string          str;        // variable name
  Variable*            next;       // points to next Variable (linked list)
  double               value;      // numerical value
  std::string          str_value;  // string value

  std::vector<double>  array;
			     // numerical array value
  int array_size() const { return array.size(); }

  Variable() {}
  Variable(std::string const& s, Variable* p, double val, std::string const& sv, int n)
: str(s), next(p), value(val), str_value(sv), array(n)
  {}
};

std::ostream& operator<<(std::ostream& os, Variable const& var);

class SCalc {

  enum token_value { NAME,         NUMBER, 	 END, 	 FUNCTION,
	             PLUS =  '+',  MINUS = '-', MUL    = '*', DIV = '/',
		     POWER = '^',  PRINT = ';', ASSIGN = '=', LPx = '(',  RP=')'};

  enum func_value{  sinx, cosx,   tanx, asinx, acosx, atanx, expx, logx,   sqrtx,
	            absx, thetax, intx, factx, hrox,  signx, gaussx };

  static unsigned int const TBLSZ = 1024;             // hash table size (increased from 23 to 1024 at Oct.29.2005)
  static unsigned int const ARRAY_POINTER_LEN = 256;

 public:

  static char const* FuncList[]; // built-in functions 

	SCalc();
	virtual ~SCalc();

        int            calcLine(char  const*,   double*, char const*, char*, char*);
	int           findValue(char* var_name, double*  result, char const* format, char* str_result);
        int        GetNextValue(std::string& var_name, double& result, char* format,  std::string& result_str);
	inline void ZeroCounter(){i_get_addr=0; name_get_addr=table[0];};
	void           zeroCalc();
	int    FindValueInArray(char* name_var, int n, double* result);
        void   dumpVariables(std::vector<std::string> const& varlist = std::vector<std::string>(),
			     std::string const& fname="", char mode='w') const;

 protected:

	// vardict_ will replace custom hash table when C++17 is supported by all compilers.
        // for now, it is unused when C+*17 is not supported. Thenew version of look() requires "emplace" which is supported by C++17 and later.   
	std::unordered_map<std::string,Variable> vardict_; // variables dict
                                                          	
	Variable*    table[TBLSZ];
	token_value  cur_tok;
	func_value   cur_func[12];
	int          func_numb;
	double       number_value;
	char         bf[256];
	char         cur_name[25];
	char         cur_ArrayPointer[ARRAY_POINTER_LEN];
	char*        pterr;
	char  const* b;
        bool         VariableExists;
	int          num_of_err;
	int          i_get_addr;

	Variable*    name_get_addr;
	Variable     data_str_err;

 protected:
	void Start();

	Variable*             look(char const* p, int ins=0);
	inline Variable*    insert(char const* s) { return look(s,1);}
	token_value      get_token();

	double Function(double e, func_value cur_func);

	double prim();
	double term();
	double expr();
	double power();
};

#endif //SCALCULATOR_H
