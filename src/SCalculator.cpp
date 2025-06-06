//  =================================================================
//
//  SCalculator.cpp
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
#include <SCalculator.h>
#include <Constants.h>
#include <Globals.h>
#include <Utility.h>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <random>

using Constants::PI;
using Constants::E_NUMB;
using Constants::ME_MEV;
using Constants::MP_MEV;
using Constants::C_CGS;
using Constants::Mmu_MEV;

using Utility::strcmpr;
using Utility::gauss;

namespace {
  std::normal_distribution<double>       normal(0.0,1.0);  // normal distribution mean=0.0, sigma=1.0
  std::uniform_real_distribution<double> uniform(0.0,1.0);  // uniform distribution mean=0.0, sigma=1.0
}


char const* SCalc::FuncList[] = { "sin","cos","tan","asin","acos","atan","exp","log","sqrt","abs","theta","int","fact","hro","sign","gauss","" };

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

SCalc::SCalc()
{
  for (int i=0; i<TBLSZ; ++i) { table[i]=0; }

  data_str_err.value      = 0.0;
  data_str_err.str_value  = "";
  data_str_err.str        = "";

  Start();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void SCalc::Start()
{
  insert("$pi")->value    = PI;
  insert("$PI")->value    = PI;
  insert("$e")->value     = E_NUMB;
  insert("$E")->value     = E_NUMB;
  insert("$Me")->value    = ME_MEV;
  insert("$Mp")->value    = MP_MEV;
  insert("$Mmu")->value   = Mmu_MEV;
  insert("$c")->value     = C_CGS;
  insert("$_turn")->value = 1;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

SCalc::~SCalc()
{
  Variable *nn;
  for (int i=0; i<TBLSZ; i++){
    Variable* n = table[i];
    while (n) {
      Variable* nn = (n->next);
      if ( n != table[i] ) delete n;
      n = nn;
     }
   }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void SCalc::zeroCalc()
{
#if GCC_VERSION >= 70000
  vardict_.clear();
#else  
  Variable *nn;
  for (int i=0; i<TBLSZ; i++){
    Variable* n = table[i];
    while (n) {
      Variable* nn = (n->next);
      if ( n != table[i] ) delete n;
      n = nn;
     }
    table[i] = 0;
  }
#endif
  Start();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int SCalc::GetNextValue(std::string& var_name, double& result, char* format,  std::string& result_str)
{

  while( !name_get_addr ){
    if(++i_get_addr == TBLSZ) return 1;
    name_get_addr   = table[i_get_addr];
  }
  var_name = name_get_addr->str;

  if(name_get_addr->str_value != "") {
    result = 1.0;
    result_str    = name_get_addr->str_value;
    name_get_addr = name_get_addr->next;
    return -1;
  }

  if( name_get_addr->array.size() ){
    result = name_get_addr->array.size();
    char buf[256];
    sprintf(buf, "%ld", name_get_addr->array.size());
    result_str = buf;
    return -2;
   }

  result = name_get_addr->value; 
  char buf[256];
  sprintf(buf, format, name_get_addr->value);
  result_str = buf;
  name_get_addr = name_get_addr->next;

   return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int SCalc::findValue(char* var_name, double* result,
				 char const* format, char* result_str)
{
  num_of_err  = 0;
  Variable* n = look(var_name, 0);
  if(num_of_err)return num_of_err;
  if(n->array_size()){
    *result=n->array_size();
    sprintf(result_str, format, *result);
    return -2;
   }
   else{
     if(n->str_value != "" ){ strcpy(result_str,n->str_value.c_str()); return -1;}
    *result = n->value;
    sprintf(result_str, format, n->value);
   }
   return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int SCalc::FindValueInArray(char* var_name, int k, double* result)
{
  Variable *n;
  num_of_err=0;
  n=look(var_name, 0);
  if(num_of_err)return num_of_err;
  if(n->array_size()){
    *result=n->array[k];
    return n->array_size();
   }
   *result=0;
   return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int SCalc::calcLine(char const* buf, double* result,
			  char const* format, char* result_str, char* error)
{
  Variable* n;
  char *p, bdat[256];
  int i;
  // initialization
  *error     = 0;
  pterr      = error;
  num_of_err = 0;
  func_numb  = 0;
  b = buf;
  while (*b!='\n' && isspace(*b))b++;  // skip spaces but '\n'
	// Creates array and sets its values
   if(strcmpr("ARRAY", buf)){
     b=b+5;
     get_token();

     if(num_of_err)return num_of_err;

     if( (cur_tok != NAME) || (*cur_ArrayPointer==0) ){
       strcpy(error,"Error in array initialization");
       return 1;
     }

     i=atoi(cur_ArrayPointer);
     if( i<1 ){strcpy(error,"Error in array initialization"); return 1;}
     n = insert(cur_name);
     if(VariableExists) {
       strcpy(error,"Cannot initialize the array. The variable ");
       strcat(error, cur_name);
       strcat(error, " already exists.");
       return 1;
     }
     n->array.resize(i);

     for(i=0; i < n->array.size(); ++i) n->array[i] = 0;

     get_token();
     if(num_of_err) return num_of_err;
     if( cur_tok == ASSIGN){
       while((*b!='{') && (*b!=0))b++;
       if(*b != '{'){
	 strcpy(error,"Error in array initialization");
         return 1;
       }
       b++;
       i=0;
       while(i<n->array.size() ){
   	 while (*b!='\n' && isspace(*b))b++;  // skip spaces but '\n'
         if(*b==0){strcpy(error,"Error in array initialization"); return 1;};
         p=bdat;
   	 while ((*b!=',')&&(*b!='}')&&(*b!=0))*p++=*b++;
         *p=0;
         n->array[i++]=atof(bdat);
         if(*b=='}')return(-2);
         if(*b==0){strcpy(error,"Error in array initialization"); return 1;};
         b++;
       }

       strcpy(error,"Error in array initialization");
         return 1;
       }
       return -2;
   }

   // string analysis

   get_token(); if(cur_tok == END) return 0;

   if(num_of_err)return num_of_err;
   if(cur_tok == NAME){
      get_token();
      if(num_of_err)return num_of_err;
      if(cur_tok == '='){
        while (*b!='\n' && isspace(*b))b++;  // skip spaces but '\n'
        if(*b++ == '\"'){  // Starts string analysis or goes to expression
	    p=result_str;
	    while(*b){
	      if(*b == '\"'){ 
                *p = 0;
	        *result =1.;
	        n = insert(cur_name);
	        n->value=1.;
	        n->str_value = result_str;
	        return -1;
             }
	     *p++ = *b++;
            }
	    *p=0;
            strcpy(error,"no \" at the end of text string");
	    return 1;
        }
      }
   }

   // expression analysis

   b = buf;
   while (*b){
     get_token();
     if(num_of_err)return num_of_err;
     if(cur_tok == END)break;
     if(cur_tok == PRINT)continue;
     *result=expr();
     sprintf(result_str, format, *result);
   }
   return num_of_err;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


SCalc::token_value SCalc::get_token()
{
  char *p;
  int i;
  while (*b!='\n' && isspace(*b))b++;  // skip spaces but '\n'
  if(!*b) return cur_tok = END;

  switch (*b) {
    case ';':
    case '\n':
      b++;
      return cur_tok = PRINT;

    case '*':
    case '/':
    case '+':
    case '-':
    case '^':
    case '(':
    case ')':
    case '=':
      return cur_tok=(token_value)*b++;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':
      p =bf;
      while(isdigit(*b)||(tolower(*b)=='e')||(*b=='.')){
	*p++=*b++;
	if(tolower(*(b-1))=='e'&&((*b=='+')||(*b=='-')))*p++=*b++;}
	*p=0;
	number_value=atof(bf);
	return cur_tok=NUMBER;
     case '$':
       p=cur_name;
       *p++ = *b++;
       while( isalnum(*b) || (*b=='_') || (*b==':') ) *p++ = *b++;
       *p=0;
       *cur_ArrayPointer=0;

       // checks if array and gets the array number

       while (*b!='\n' && isspace(*b))b++;  // skip spaces but '\n'
         if(*b=='['){
           b++;
 	   while (*b!='\n' && isspace(*b))b++;  // skip spaces but '\n'
           p=cur_ArrayPointer;
           i=0;
           while(*b!=']' && *b!='\n' && *b!=0){
             *p++ = *b++;
             if(i++ == (ARRAY_POINTER_LEN-2))break;
           }
	   *p=0;
           if(*b!=']'){
	     strcpy(pterr,"No ] in the reference to array Element");
	     num_of_err++;
	     return cur_tok=NAME;
            }
            b++;
          }

	return cur_tok=NAME;

       default:
	  if(isalpha(*b)){
	    p=bf;
	    while(isalnum(*b)) *p++ = *b++;
	    *p=0;
	    for(i=0; *FuncList[i]!=0; i++)
	    if(strcmp(FuncList[i],bf)==0){
	      cur_func[func_numb++]=(func_value)i;
	      return cur_tok=FUNCTION;
            }
	    sprintf(pterr, "function %s is undefined.", bf);
	    num_of_err++;
	    return cur_tok=PRINT;
	  }
	  sprintf(pterr, "bad token:<%c>",*b);
	  b++;
	  num_of_err++;
	  return cur_tok=PRINT;
	}
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double SCalc::prim() // primary 
{ 
  Variable*   n;
  double      e, iresult, *dref;
  char        bb[128];
  int         i;
  char        save_bf[128];
  char        save_cur_name[25];
  char const* bt;
  token_value save_cur_tok;
  int         save_func_numb;
  
  switch(cur_tok) {

    case NUMBER:
      if(get_token()==NUMBER){
        strcpy(pterr,"Missing operand between numeric values");
	num_of_err++;
      }
      return number_value;
    case NAME:
      if(get_token()==NAME){
        strcpy(pterr,"Missing operand between variables");
        num_of_err++;
	return 0.;
      }
      if(cur_tok == ASSIGN){
	if(num_of_err)return 0.;
	n = insert(cur_name);
        if(*cur_ArrayPointer){
          if(n->array.size() <= 0){ // FIXME cannot be < 0
            strcpy(pterr,"Array index used with a scalar variable");
	    num_of_err++; return 0.;
	  }
          //
          bt = b;
          strcpy(save_bf, cur_ArrayPointer);
          strcpy(save_cur_name, cur_name);
          save_func_numb=func_numb;
          save_cur_tok = cur_tok;
   	  if(calcLine(save_bf, &iresult,"%le", bb, pterr)) {
	     num_of_err++;
	     return 0;
	  }
          b = bt;
          strcpy(cur_ArrayPointer, save_bf);
          strcpy(cur_name, save_cur_name);
          func_numb = save_func_numb;
          cur_tok   = save_cur_tok;
          //
      	  i = ((int)(fabs(iresult)+1.e-10));
          if((iresult+1.e-10)<0) i=-i;
      	  if((i<0) || ( i >= n->array.size() )){ 
             strcpy(pterr,"Array index out of range");
	     num_of_err++;
	     return 0.;
	  }
          dref = &(n->array[i]);
        }
	else {
          if(n->array.size() > 0){
            strcpy(pterr,"Cannot assign scalar to ARRAY"); num_of_err++; return 0.;}
            dref=&(n->value);
          }
	  get_token();
	  if(num_of_err) return 0.;
	  *dref = expr();
	  if(cur_tok == RP) {
	    strcpy(pterr,"excessive ')'");
	    num_of_err++;
	    return *dref;
	  }
	  return *dref;
      }
      if(*cur_ArrayPointer){
        n = look(cur_name, 0);
        //
        bt = b;
        strcpy(save_bf, cur_ArrayPointer);
        strcpy(save_cur_name, cur_name);
        save_func_numb=func_numb;
        save_cur_tok=cur_tok;
        if(calcLine(save_bf, &iresult,"%le", bb, pterr)){num_of_err++; return 0;}
        b=bt;
        strcpy(cur_ArrayPointer, save_bf);
        strcpy(cur_name, save_cur_name);
        func_numb = save_func_numb;
        cur_tok   =  save_cur_tok;
        //
      	i = ((int)(fabs(iresult)+1.e-10));
        if((iresult+1.e-10)<0) i=-i;
      	if((i<0) || (i >=n->array.size())){
	  strcpy(pterr,"Array index outside range");
	  num_of_err++;
          return 0.;
	}
        return n->array[i];
      }
      return look(cur_name, 0)->value;

    case MINUS:
      get_token();
      return -prim();
    case LPx:
      get_token();
      if(num_of_err) return 0.;
      e = expr();
      if(num_of_err) return 0.;
      if(cur_tok != RP) {strcpy(pterr,"missing ')'"); num_of_err++; return 0.;}
      get_token();
      return e;
    case FUNCTION:
      get_token();
      if(cur_tok != LPx) {
	 strcpy(pterr,"missing '(' after function name");
	 num_of_err++;
	 return 0.;
      }
      get_token();
      if(num_of_err) return 0.;
      e = expr();
      if(num_of_err) return 0.;
      if(cur_tok != RP) {
	 strcpy(pterr,"missing ')'");
	 num_of_err++; return 0.;}
	 get_token();
	 return Function(e,cur_func[--func_numb]);
    case END:
      return 1;
    default:
      strcpy(pterr,"must be a primary");
      num_of_err++;
      return 0.;
   }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double SCalc::Function(double x, func_value cur_func)
{
  int i,j,m;

  switch(cur_func){
    case absx:
      return fabs(x);
    case sinx:
      return sin(x);
    case cosx:
      return cos(x);
    case tanx:
      return tan(x);
    case intx:
      return (double)((int)x);
    case thetax:
      return (x>0.0) ? 1.0 : 0.0; 
    case signx:
      return (x>=0.0) ? 1.0 :-1.0;
    case hrox:
      return x/C_CGS*1e11;
    case gaussx:
      return x*::normal(appstate.generator); // x*gauss();
    case factx:
      j = (int)(x+1.e-9);
      if(( j < 1 ) || ( j > 20 )){
        strcpy(pterr,"argument out of range of [1, 20] in fact(...)");
        num_of_err++;
        return 0.;
      }
      for(i=1, m=1; i<=j; i++) m=m*i;
      return double(m);
    case asinx:
      if(fabs(x)>1.){
	strcpy(pterr,"argument out of range in asin(...)");
	num_of_err++;
	return 0.;
      }
      return asin(x);
    case acosx:
      if(fabs(x)>1.){
	strcpy(pterr,"argument out of range in acos(...)");
	num_of_err++;
	return 0.0;
      }
      return acos(x);
    case atanx:
      return atan(x);
    case expx:
      return exp(x);
    case logx:
      if(x<0.){strcpy(pterr,"argument out of range in log(...)");
	num_of_err++;
	return 0.;
      }
      return log(x);
    case sqrtx:
      if(x<0.){
	strcpy(pterr,"argument out of range in sqrt(...)");
	num_of_err++;
	return 0.;
      }
      return sqrt(x);
   default:
     break;
  }
  return x;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double SCalc::power()
{
  double d;
  int i,j;
  double left=prim();
  if(num_of_err) return 0.;
  for(;;){
    switch(cur_tok){
      case POWER:
	get_token();
	if(num_of_err) return 0.;
	d = prim();
   	if(num_of_err) return 0.;
        i=(int)(d+1e-10);
        if((i>=0.) && (i<12.) && (fabs(i-d)<1.e-9) ){
          d=1.;
	  j=0;
          while(j<i) { d=d*left; j++;}
          left=d;
           break;
        }
	if(left<0){
	  strcpy(pterr,"negative argument in computing power");
	  num_of_err++;
        }
	else {
          if(left!=0)left=exp(log(left)*d);
        }
	break;
      default:
	return left;
    }
  }
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double SCalc::term()
{
  double d;                
  double left = power();
   if(num_of_err)return 0.;

   for(;;){
      switch(cur_tok){
        case MUL:
	  get_token();
	  if(num_of_err) return 0.;
	  left *= power();
	  if(num_of_err) return 0.;
	  break;
        case DIV:
	  get_token();
	  if(num_of_err) return 0.;
	  d = power();
	  if(num_of_err) return 0.;
	  if(d==0) {
	    strcpy(pterr,"Division by zero");
	    num_of_err++;
            return 0.;
	  }
	  else left /=d;
	  break;
	default:
	  return left;
      }
    }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double SCalc::expr()
{       // additions and subtractions

   double left = term();
   if(num_of_err)return 0.;

   for(;;){
      switch(cur_tok){
        case PLUS:
	  get_token();
	  if(num_of_err)return 0.;
	  left +=term();
	  if(num_of_err)return 0.;
	  break;
       case MINUS:
	  get_token();
	  if(num_of_err)return 0.;
	  left -=term();
	  if(num_of_err)return 0.;
	  break;
	default:
	  return left;
     }
  }
}

//..............................................................................
#if GCC_VERSION >= 70000
//..............................................................................

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Variable* SCalc::look(char const* p, int ins)
{

  VariableExists =  ( vardict_.find(p) != vardict_.end() ); 
  
  if (VariableExists ) return &vardict_[p];

  if(ins == 0){
     sprintf(pterr, "Variable <%s> is undefined", p); 
     num_of_err++;
     return &data_str_err;
  }

  vardict_.try_emplace(std::string(p), std::string(p), static_cast<Variable*>(0), 1.0, std::string(""), 0);     
  return &vardict_[p]; 
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void  SCalc::dumpVariables(std::vector<std::string> const& varlist, std::string const& fname, char mode) const
{
  time_t rawtime;

  time (&rawtime);
  tm*    timeinfo = localtime (&rawtime);


  std::ofstream ofs(fname, std::ios_base::out | ((mode == 'A') ? std::ios_base::app : std::ios_base::trunc) );
 
  if ( !ofs.good() ) throw std::runtime_error("Error: Failed to open file: " + fname);    

  ofs << "--------------------------"<< std::endl;
  ofs <<  asctime(timeinfo);
  ofs << "Variables"<< std::endl;
  ofs << "--------------------------\n"<< std::endl;
  
  if (varlist.size() == 0 ) {  
     for (auto const& item: vardict_) {
         ofs << item.second << std::endl;
     }
  }
  else {
    for (auto const& item: varlist) {
      Variable* var = const_cast<SCalc *>(this)->look( item.c_str(), 0);  
      if (VariableExists)  {
	ofs << *var << std::endl;
      }
      else {
	 ofs << item.c_str() <<" is undefined." << std::endl;
      }
    }
  }
}

//.........................................................................
#else   // GCC_VERSION  < 70000 
// ........................................................................

// look for a variable by name. If not found, create a new one if ins !=0 
Variable* SCalc::look(char const* p, int ins)
{
  VariableExists = false;

  int ii = 0;
  char const *pp = p;

  // hash function ?  ii = hash(p)
  while(*pp) ii = (ii<<1) ^ (*pp++);
  if (ii<0) ii=-ii;
  ii %= TBLSZ;

  Variable* n = table[ii];   // table : hash table

  while(n!=0){            // search (possible collision)
    if( p == n->str ) {VariableExists=true; return n;}
    n=(Variable*)(n->next);
  }
  
  if(ins == 0){
     sprintf(pterr, "Variable <%s> is undefined", p); 
     num_of_err++;
     return &data_str_err;
  }

  Variable *nn = new Variable;
  nn->str = p;
  nn->value     = 1.0;
  nn->str_value = "";
  nn->array.resize(0);
  if (table[ii]) nn->next = table[ii]; else nn->next = 0;
  table[ii]=nn;
  return nn;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  SCalc::dumpVariables(std::vector<std::string> const& varlist, std::string const& fname, char mode) const
{

  time_t rawtime;
  time (&rawtime);
  tm*    timeinfo = localtime (&rawtime);

  std::ofstream ofs(fname, std::ios_base::out | ((mode == 'A') ? std::ios_base::app : std::ios_base::trunc) );
  mode = toupper(mode); 

  ofs << "--------------------------"<< std::endl;
  ofs <<  asctime(timeinfo);
  ofs << "Variables"<< std::endl;
  ofs << "--------------------------\n"<< std::endl;

  if (varlist.size() == 0 ) {  
    for (int i=0; i< TBLSZ; ++i) {
      Variable* var = table[i];
      if  (!var) continue;  
      ofs << (*var) << std::endl;
      // follow the nodes in the linked list, if any
       var = var->next;
       while (var) {
         ofs << (*var) << std::endl;
         var = var->next;
       }
     }
  }
  else {
    for (auto const& item: varlist) {
      Variable* var = const_cast<SCalc *>(this)->look( item.c_str(), 0);  
      if (var)  {
	ofs << *var << std::endl;
      }
      else {
	 ofs << item.c_str() <<" is undefined." << std::endl;
      }
    }
  }
}
//.................................................................................
#endif
//..................................................................................
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::ostream& operator<<(std::ostream& os, Variable const& var)
{

    if (var.array_size() >0 ) { 
      for (int i=0; i<var.array_size(); ++i) {     
	os << std::setw(20) << var.str<<"["<<i<<"] = " <<  var.array[i] <<std::endl;
      }
      return os;   
    }
    
    if ( var.str_value != "" ) { 
       os << std::setw(16)  <<  var.str;
       os << "    " <<  var.str_value <<std::endl;
    }
    else { 
      os << std::setw(16)  <<  var.str;
      os << std::setw(16)  <<  var.value << std::endl;
    }
 
    return os;   
}  
