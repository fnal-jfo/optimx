//  =================================================================
//
//  ImportMadX.cpp
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

#include <OptimMainWindow.h>
#include <OptimMdiArea.h>
#include <OptimTextEditor.h>
#include <Constants.h>   
#include <Element.h>   
#include <ImportMadXDialog.h>   
#include <Utility.h>  
#include <OptimMessages.h>

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QDateTime>

#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cctype>

double const pi = 4*atan(1.0);

//---------------------------------
// all valid madx twiss table keywords
//---------------------------------

/* ---------------------------------
 S      path length 
 BETX   betax 
 ALFX   alphax
 MUX    mux (phase)
 BETY   betay 
 ALFY   alphay 
 MUY    muy 
 X      x
 PX     px
 Y      y
 PY     py 
 T      time 
 PT     momentum (tangential)
 DX     eta_x (dispersion)
 DPX    eta_prime_x
 DY     etay_
 DPY    eta_prime_y
 WX     
 PHIX
 DMUX
 WY
 PHIY
 DMUY
 DDX
 DDPX
 DDY
 DDPY
 R11     matrix element r11
 R12    
 R21
 R22
 ENERGY  
 L       element length
 ANGLE   bend angle 
 K0L     
 K0SL
 K1L
 K1SL
 K2L
 K2SL
 K3L
 K3SL
 K4L
 K4SL
 K5L
 K5SL
 K6L
 K6SL
 K7L
 K7SL
 K8L
 K8SL
 K9L
 K9SL
 K10L
 K10SL
 K11L
 K11SL
 K12L
 K12SL
 K13L
 K13SL
 K14L
 K14SL
 K15L
 K15SL
 K16L
 K16SL
 K17L
 K17SL
 K18L
 K18SL
 K19L
 K19SL
 K20L
 K20SL
 KSI
 HKICK
 VKICK
 TILT
 E1
 E2
 H1
 H2
 HGAP
 FINT
 FINTX
 VOLT
 LAG
 FREQ
 HARMON
 SLOT_ID
 ASSEMBLY_ID
 MECH_SEP
 V_POS
 LRAD
 PARENT       parent element label [ string]
 COMMENTS     [string]
 RE11
 RE12
 RE13
 RE14
 RE15
 RE16
 RE21
 RE22
 RE23
 RE24
 RE25
 RE26
 RE31
 RE32
 RE33
 RE34
 RE35
 RE36
 RE41
 RE42
 RE43
 RE44
 RE45
 RE46
 RE51
 RE52
 RE53
 RE54
 RE55
 RE56
 RE61
 RE62
 RE63
 RE64
 RE65
 RE66
 KMAX
 KMIN
 CALIB
 POLARITY
 ALFA
 BETA11
 BETA12
 BETA13
 BETA21
 BETA22
 BETA23
 BETA31
 BETA32
 BETA33
 ALFA11
 ALFA12
 ALFA13
 ALFA21
 ALFA22
 ALFA23
 ALFA31
 ALFA32
 ALFA33
 GAMA11
 GAMA12
 GAMA13
 GAMA21
 GAMA22
 GAMA23
 GAMA31
 GAMA32
 GAMA33
 BETA11P
 BETA12P
 BETA13P
 BETA21P
 BETA22P
 BETA23P
 BETA31P
 BETA32P
 BETA33P
 ALFA11P
 ALFA12P
 ALFA13P
 ALFA21P
 ALFA22P
 ALFA23P
 ALFA31P
 ALFA32P
 ALFA33P
 GAMA11P
 GAMA12P
 GAMA13P
 GAMA21P
 GAMA22P
 GAMA23P
 GAMA31P
 GAMA32P
 GAMA33P
 DISP1
 DISP2
 DISP3
 DISP4
 DISP1P
 DISP2P
 DISP3P
 DISP4P
 DISP1P2
 DISP2P2
 DISP3P2
 DISP4P2
 DISP1P3
 DISP2P3
 DISP3P3
 DISP4P3
 MU1
 MU2
 MU3
 EIGN11
 EIGN12
 EIGN13
 EIGN14
 EIGN15
 EIGN16
 EIGN21
 EIGN22
 EIGN23
 EIGN24
 EIGN25
 EIGN26
 EIGN31
 EIGN32
 EIGN33
 EIGN34
 EIGN35
 EIGN36
 EIGN41
 EIGN42
 EIGN43
 EIGN44
 EIGN45
 EIGN46
 EIGN51
 EIGN52
 EIGN53
 EIGN54
 EIGN55
 EIGN56
 EIGN61
 EIGN62
 EIGN63
 EIGN64
 EIGN65
 EIGN66
 N1
---------------------------------------------------------
*/

void output_header( OptimTextEditor* editor, std::ostream& ofs );
void output_variables(OptimTextEditor* editor, std::ostream& ofs );
void output_initial(OptimTextEditor* editor, std::ostream& ofs );
void output_lattice(std::vector<std::string>& beamline,                          OptimTextEditor* editor, std::ostream& ofs );
void output_elements(std::unordered_map<std::string, std::string>& element_dict, OptimTextEditor* editor, std::ostream& ofs );


namespace MadX {

 double mass;
 double pc;
 double gma;
 double etot;
 double ex  = 1.0;
 double ey  = 1.0;
 double dpp = 0.0; 
 double x_0 = 0.0; 
 double y_0 = 0.0;  
 double z_0 = 0.0; 
 double s_0 = 0.0; 
 double thetax = 0.0; 
 double thetay = 0.0;  

 double bx = 1.0; 
 double by = 1.0; 
 double ax = 0.0; 
 double ay = 0.0;  

 double dx  = 0.0; 
 double dxp = 0.0; 

 double dy  = 0.0; 
 double dyp = 0.0; 

 std::string title;
 std::string sequence;
 std::string origin;
 std::string sdate;
 std::string stime;
  
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void trim( std::string& s)
{
  auto pos1 = s.find_first_not_of("\" ");
  auto pos2 = s.find_last_not_of("\" ");

  if (pos1 == std::string::npos ) return; // an empty string 

  s = s.substr(pos1, pos2-pos1+1); 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void make_element( std::vector<std::string>& beamline, std::unordered_map<std::string, std::string>& element_dict,
		   std::string& elabel, std::string& etype,
		   std::unordered_map<std::string, double>& attributes, double brho)
{
  // ***WARNING***: brho in the arg above is expressed in T-m
  // 1 T-m = 10 kG * 100 cm m = 1000 kG-cm 
  // attributes[] are from MADX and they are expressed in MKS
  // in MADX, the multipole coefficients are normalized w/r to brho. 
  // in optim, the multipole coefficients are integrated field and field derivatives w/o normalization  

  std::stringstream ss;

  double  length   =   attributes["L"]*100;  // length in cm
  double  leps     =   1.0e-4;              //  length in cm of "a thin element" (0 length in MADX) 
  double  b        =  (attributes["ANGLE"]*brho)/attributes["L"]*10.0; // in kG
  double  k0l      =   attributes["K0L"]  * brho * 1000.0;     // k0l is in kG-cm in OptiM and (K0L*brho) is in  T-m in MADX;  1 T-m = 10 kG * 100 cm = 1000 kG-cm
  double  k0sl     =   attributes["K0SL"] * brho * 1000.0;
  double  k1l      =   attributes["K1L"]  * brho * 10.0;       //  k1l is in kG in OptiM;  (K1L*brho) is in T in MADX;  1 T = 10 kG
  double  k1sl     =   attributes["K1SL"] * brho * 10.0;
  double  k2l      =   attributes["K2L"]  * brho * 10.0*10.0;  //  k2l is in kG/cm in OptiM;  (K2L*brho) is in T/m in MADX;  1 T = 10 kG
  double  k2sl     =   attributes["K2SL"] * brho * 10.0*10.0;
  double  lrad     =   attributes["LRAD"];
  double  anglerad =   attributes["ANGLE"];  // bend angle in rad. 
  double  angle    =   anglerad*180.0/pi;    // bend angle in deg 
  double tilt      =   attributes["TILT"] * 180.0/pi;  // in degrees
  double e1        =   attributes["E1"];        // in rd;  optimx angle convention is reverse of MADX ecause +ve rotation direction in OptimX is inverse of MADX  
  double e2        =   attributes["E2"];        // in rd
  double hgap      =   attributes["HGAP"]*100;  // half-gap in cm 
  double fint      =   attributes["FINT"];
  double fintx     =   attributes["FINTX"];
  fintx            =   (fintx == 0.0) ? fint : fintx; // MADX manual says that if FINTX is undefined, FINTX=FINT (undefined FINTX is FINITX=0.0)  
  double ksi       =   attributes["KSI"];
     
  if (etype == "QUADRUPOLE") {
    if ( element_dict.find(elabel) == element_dict.end() )  {

      // K1L and K1SL may both be defined. If so, we find the
      // rotation angle for a normal quadrupole that provides
      // both multipole coefficients in the right proportion.
      // The normal quadrupole strength
      // K1L is then adjusted so that K1L(new) = K1L(old)/cos(tilt). 
      // Only K1L(new) is used thereafter.
      
      if ( (fabs(k1sl) > 1.0e-10) && fabs(k1l) < 1.0e-10) {
        k1l  =  k1sl;
	tilt = -45.0;
      }
      else if ( (fabs(k1sl) > 1.0e-10 && fabs(k1l) > 1.0e-10) ) {
	tilt = 0.5*atan2(k1sl,k1l); 
	k1l   /=  cos(tilt); 
	tilt  *= -(180.0/pi); 
      }
      if (fabs(length) < leps) {
        ss << " L[cm]="    << leps; // in cm 
        ss << " G[kG/cm]=" <<  k1l/leps;  // K1*brho is in [T/m] = [10 kG / 100 cm] 
        ss << " Tilt[deg]=" << tilt;
      }
      else {
        ss << " L[cm]="     << length;     // in cm 
        ss << " G[kG/cm]="  << k1l/length; // in kG/cm 
        ss << " Tilt[deg]=" << tilt;
      }	
      element_dict.insert( { "q"+elabel, ss.str() } );
    }
    beamline.push_back( "q"+elabel); 
  }
  else if (etype == "DRIFT") {
    if ( element_dict.find(elabel) == element_dict.end() )  {
      ss << " L[cm]=" << length;
      element_dict.insert( { "o"+elabel, ss.str() } );
    }
    beamline.push_back( "o"+elabel); 
  }
    else if (etype == "MONITOR") {
    if ( element_dict.find(elabel) == element_dict.end() )  {
      ss << " L[cm]=" << length;
      element_dict.insert( { "o"+elabel, ss.str() } );
    }
    beamline.push_back( "o"+elabel); 
  }
  else if (etype == "HMONITOR") {
    if ( element_dict.find(elabel) == element_dict.end() )  {
      ss << " L[cm]=" << length;
      element_dict.insert( { "o"+elabel, ss.str() } );
    }
    beamline.push_back( "o"+elabel); 
  }
  else if (etype == "VMONITOR") {
    if ( element_dict.find(elabel) == element_dict.end() )  {
      ss << " L[cm]=" << length;
      element_dict.insert( { "o"+elabel, ss.str() } );
    }
    beamline.push_back( "o"+elabel); 
  }
  else if (etype == "YROTATION") {
    if ( element_dict.find(elabel) == element_dict.end() )  {
      ss << " L[cm]=" << length;
      element_dict.insert( { "o"+elabel, ss.str() } );
    }
    beamline.push_back( "o"+elabel); 
  }
  else if (etype == "PLACEHOLDER") {
    if ( element_dict.find(elabel) == element_dict.end() )  {
      ss << " L[cm]=" << length;
      element_dict.insert( { "i"+elabel, ss.str() } );
    }
    beamline.push_back( "i"+elabel); 
  }
  else if (etype == "INSTRUMENT") {
    if ( element_dict.find(elabel) == element_dict.end() )  {
      ss << " L[cm]=" << length;
      element_dict.insert( { "i"+elabel, ss.str() } );
    }
    beamline.push_back( "i"+elabel); 
  }
  else if (etype == "SBEND") {
     std::string mnemonic = "d"; // dipole 
     double g =  k1l/length;
     if ( element_dict.find(elabel) == element_dict.end() )  {
       ss << " L[cm]=" << length;          
       ss << " B[kG]=" << b; 
       if ( fabs(k1l) > 0.0) {
	 ss << " G[kG/cm]=" << g;
	 mnemonic = "b"; // cf bend
       }	   
       ss << " Tilt[deg]=" << tilt; 
       std::string s = ss.str(); 

       double angleu =  e1;
       double angled =  e2;

       double  gap     = 2*hgap;
       double  cn      = cos(angleu);
       double  sn      = sin(angleu);
       double  efflen  = fint*gap*(1.0+sn*sn)/(cn*cn*cn);   // upstream   edge effective length

       ss.str("");
       ss << " B[kG]=" << b;
       ss << " Angle[deg]=" << angleu*180.0/pi;
       ss << " EffLen[cm]=" << efflen;
       ss << " T[deg]=" << tilt;
       auto su = ss.str();

       cn     = cos(angled);
       sn     = sin(angled);
       double efflenx = fintx*gap*(1.0+sn*sn)/(cn*cn*cn);    // downstream edge effective length

       ss.str("");
       ss << " B[kG]=" << b;
       ss << " Angle[deg]=" << angled*180.0/pi;
       ss << " EffLen[cm]=" << efflenx;
       ss << " T[deg]=" << tilt;
       auto sd = ss.str();
       
       element_dict.insert( { "gu"+ elabel, su } );
       element_dict.insert( { mnemonic + elabel, s } );
       element_dict.insert( { "gd"+ elabel, sd } );
     }

     beamline.push_back( "gu"+elabel); // upstream   edge 
     beamline.push_back( mnemonic + elabel); 
     beamline.push_back( "gd"+elabel); // downstream edge
  }
  else if (etype == "RBEND") {

     std::string mnemonic = "d"; // dipole 

     if ( element_dict.find(elabel) == element_dict.end() )  {

       ss << " L[cm]=" << length;           
       ss << " B[kG]=" << b; 
       double g =  k1l/length;
       if ( fabs(g) > 0.0) {
	 ss << " G[kG/cm]=" << g;
	 mnemonic = "b"; // cf bend
       }
       ss << " Tilt[deg]=" << tilt;  
       auto s = ss.str();

       double angleu =  e1 + fabs(anglerad)/2; 
       double angled =  e2 + fabs(anglerad)/2;

       double gap    = 2*hgap;
       double cn     = cos(angleu);
       double sn     = sin(angleu);
       double efflen = fint*gap*(1.0+sn*sn)/(cn*cn*cn);       // upstream edge effective length

       ss.str("");
       ss << " B[kG]=" << b;
       ss << " Angle[deg]=" << angleu*180.0/pi;
       ss << " EffLen[cm]=" << efflen;
       ss << " T[deg]="     << tilt;
       auto su = ss.str();

       cn     = cos(angled);
       sn     = sin(angled);
       double efflenx = fintx*gap*(1.0+sn*sn)/(cn*cn*cn);       // downstream edge effective length
 
       ss.str("");
       ss << " B[kG]=" << b;
       ss << " Angle[deg]=" << angled*180.0/pi;
       ss << " EffLen[cm]=" << efflenx;
       ss << " T[deg]=" << tilt;
       auto sd = ss.str();

       element_dict.insert( { "gu"+ elabel, su } );
       element_dict.insert( { mnemonic + elabel, s  } );
       element_dict.insert( { "gd"+ elabel, sd } );
     }    

     beamline.push_back( "gu"+ elabel); // upstream   edge 
     beamline.push_back( mnemonic + elabel); 
     beamline.push_back( "gd"+ elabel); // downstream edge
  }
  else if (etype == "MULTIPOLE") {
     // we care only about 0th and first order for now ... 
     bool has_k0 = (fabs(k0l)  > 0.0 ) ;        
     bool has_k1 = (fabs(k1l)  > 0.0 ) ;        
     bool has_k0s = (fabs(k0sl)  > 0.0 ) ;        
     bool has_k1s = (fabs(k1sl)  > 0.0 ) ;        
     //if (  has_k0s ) std::cout << elabel << "  found skew 0" << std::endl;  
     //if (  has_k1s ) std::cout << elabel << "  found skew 1" << std::endl;  

     if (has_k0) {  
       if ( element_dict.find(elabel+"_0") == element_dict.end() )  {
         ss  << " m="             <<  0;          
         ss  << " S[kG/cm^(m-1)=" <<  k0l;
         ss  << " Tilt[deg]="     <<  tilt; 
         element_dict.insert( { "m"+elabel+"_0", ss.str() } );
	 ss.str("");
       }
       beamline.push_back( "m"+elabel+"_0"); 
     }
     if (has_k1) {
       if ( element_dict.find(elabel+"_1") == element_dict.end() )  {
         ss  << " m="             <<    1;          
         ss  << " S[kG/cm^(m-1)=" <<  k1l;
         ss  << " Tilt[deg]="     << tilt; 
         element_dict.insert( { "m"+elabel+"_1", ss.str() } );
	 ss.str("");
       }
       beamline.push_back( "m"+elabel+"_1"); 
     }
  }
  else if (etype == "SEXTUPOLE") {
    if ( element_dict.find(elabel) == element_dict.end() )  {
      if ( (fabs(k2sl) > 0.0) && (fabs(k2l) < 1.0e-10) ) {
  	  tilt = -30.0;
      }
      else if ( (fabs(k2sl) > 1.0e-10 && fabs(k2l) > 1.0e-10)) {
	tilt = (1.0/3.0)*atan2(k2sl,k2l); 
	k2l   /=  cos(tilt); 
	tilt  *= -(180.0/pi); 
      }

      if (fabs(length) < leps) {
         ss << " L[cm]="       << leps;          
         ss << " S[kG/cm/cm]=" << k2l/leps;  // [kG/cm**2] 
         ss << " Tilt[deg]="   << tilt; 
      }
      else {
          ss << " L[cm]="        << length;       // in cm 
          ss << " G[kG/cm/cm]="  << k2l/length;  // [kG/cm**2] 
          ss << " Tilt[deg]="    << tilt;
      }
      element_dict.insert( { "s" + elabel, ss.str() } );
    }
    beamline.push_back( "s" + elabel); 
  }
  else if (etype == "KICKER") {
    if ( element_dict.find(elabel) == element_dict.end() )  {
      double hkick = attributes["HKICK"];
      double vkick = attributes["VKICK"];
      //if (fabs(hkick) > 0.0 || fabs(vkick)>0.0) std::cout << elabel << " KICKER has finite kick " << std::endl;  
      ss << " L[cm]="       << length;
      if ( fabs(length) < 1.0e-6 ) length = 1.0e-6;
      double  tmp = sqrt((hkick*hkick)+(vkick*vkick))*(brho/length)*1000.0;           
      ss << " B[kg]="       << tmp;
      double  ltilt = (fabs(hkick) > 1.0e-12 ) ? atan2(vkick,hkick) : pi/2;     
      ss << " Tilt="        << tilt + ltilt*180/pi;          
    }
    element_dict.insert( { "k"+elabel, ss.str() } );
    beamline.push_back( "k"+elabel); 
  }
  else if (etype == "HKICKER") {
    if ( element_dict.find(elabel) == element_dict.end() )  {
      double hkick = attributes["HKICK"];
      //if (fabs(hkick) > 0.0 ) std::cout << elabel << " HKICKER has finite kick " << std::endl;  
      ss << " L[cm]="       << length;          
      if ( fabs(length) < 1.0e-6 ) length = 1.0e-6;
      double  tmp = hkick*(brho/length)*1000.0;           
      ss << " B[kG]="       << tmp;          
      ss << " Tilt="        << 0.0 + tilt;          
    }
   element_dict.insert( { "k"+elabel, ss.str() } );
   beamline.push_back( "k"+elabel); 
  }
  else if (etype == "VKICKER") {
    if ( element_dict.find(elabel) == element_dict.end() )  {
     double vkick = attributes["VKICK"];
     //if (fabs(vkick) > 0.0 ) std::cout << elabel << " VKICKER has finite kick " << std::endl;  
     ss << " L[cm]="       << length;          
     if ( fabs(length) < 1.0e-6 ) length = 1.0e-6;
     double  tmp = vkick*(brho/length)*1000.0;           
     ss << " B[kG]="       << tmp;  // brho: T-m to kG-cm
     ss << " Tilt[deg]="   << 90.0 + tilt; 
     element_dict.insert( { "k"+elabel, ss.str() } );
    }
    beamline.push_back( "k"+elabel); 
  }
  else if (etype == "DIPEDGE") {
    if ( element_dict.find(elabel) == element_dict.end() )  {

     double edgeang    = e1;
     double hgap       = attributes["HGAP"]*100.0; // half-gap   
     double oneoverrho = attributes["H1"]*0.01;  // 1/rho    
     double cn         = cos(edgeang);
     double sn         = sin(edgeang);
     double efflen     = fint*2*hgap*(1.0+sn*sn)/(cn*cn*cn);  // edge effective length

     ss << " B[kG]="       << (brho*1000.0*oneoverrho);  //  brho*1/rho = B;  brho: T-m to kG-cm
     ss << " Angle[deg]="  << edgeang*180/pi;
     ss << " L[cm]="       << efflen;          
     ss << " Tilt[deg]="   << tilt; 
     element_dict.insert( { "g"+elabel, ss.str() } );
    }
    beamline.push_back( "g"+elabel); 
  }
  else if (etype == "SOLENOID") {
    if ( element_dict.find(elabel) == element_dict.end() )  {
      if (fabs(length) < 1.0e-6) { // pseudo-solenoid ( focusing only) 
	 ss << " L[cm]="       << 0.0;          
         ss << " B[kG]="       << ksi*brho*1000.0;   // brho : T-m to kG-cm
         ss << " a[cm]="       << 0.0; 
         element_dict.insert( { "cc"+elabel, ss.str() } );
      }
      else {  
         ss << " L[cm]="       << length;          
         ss << " B[kG]="       << ksi*brho*1000.0/length; // brho : T-m to kG-cm
         ss << " a[cm]="       << 0.0;
         element_dict.insert( { "c"+elabel, ss.str() } );
      }
    }
    (fabs(length) < 1.0e-6) ? beamline.push_back( "cc"+elabel) :  beamline.push_back( "c"+elabel); 
  }
  else if (etype == "MARKER") {
    // we skip markers
    //std::cout << "MARKER: " << elabel << std::endl;
  }
  else {
     if (etype == "MARKER") {std::cout << "MARKER (converted to drift):  " << elabel << std::endl;}
     if ( element_dict.find(elabel) == element_dict.end() )  {
        ss << " L[cm]="       << length;    
        element_dict.insert( { "o"+elabel, ss.str() } );
       }
      beamline.push_back( "o"+elabel); 
 }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::string get_token( std::ifstream& ifs)
{
  char c;  
  std::stringstream ss;

  bool quoting = false;
  
  ss.str("");
   
   while ( true ) {
      ifs.get(c);
      if ( !ifs.good() ) break;
      if ( (c == ' ' ) || (c == '\n') || (c == '\t') || (c == '\r') ) continue;
      ss << c;
      if (c == '\"') quoting = true;
      break;
   }

   while ( true ) {
      ifs.get(c);
      if ( !ifs.good() ) break;

      if ( quoting) {
        if (c == '\"') quoting = false; 
	ss << c;
	continue;
      }

      if ( (c != ' ' ) && (c != '\n') && (c != '\t') && (c != '\r') )  {
	ss << c;
	continue;
      }
            
      break;
   }

   return ss.str();
  }


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimMainWindow::cmdImportMadX()
{ 

  std::unordered_map<std::string, std::string> element_dict;
  std::vector<std::string> beamline;
  
  using namespace MadX;
  
  char fname1[255];
  strcpy(fname1, QFileDialog::getOpenFileName( ).toUtf8().data()); 
  if (!fname1[0]) return; 
  
  auto del = [](FILE* p) { (p ? fclose(p) : 0);};  
  std::unique_ptr<FILE, decltype(del)>  fp1 (fopen(fname1,"r"), del);
  
  if( !fp1 ){
    OptimMessageBox::warning(this,  "Error opening file - ", fname1, QMessageBox::Ok);
    return;
  }

  std::ifstream ifs( fname1 );

  //--------------------------------------------------------------
  // look for particle constants
  //---------------------------------------------------------------

  while ( true ) {

    std::string token =  get_token(ifs);
    if ( !ifs.good() ) break;

    if ( token == "@" ) {
       token = get_token(ifs);
       if (token ==  "MASS" ) {
	 get_token(ifs); // skip format spec
	 token = get_token(ifs);
	 mass = std::stod(token);
       }
       else if ( (token ==  "ENERGY") ) {
	 get_token(ifs); // skip format spec
	 token = get_token(ifs);
	 etot = std::stod(token);
       }
       else if ( (token ==  "PC")     ) {
	 get_token(ifs); // skip format spec
	 token = get_token(ifs);
	 pc = std::stod(token);
       }
       else if ( (token ==  "GAMMA")  ) {
	 get_token(ifs); // skip format spec
	 token = get_token(ifs); 
	 gma = std::stod(token);
       }
       else if ( (token ==  "TITLE")  ) {
	 get_token(ifs); // skip format spec
	 token = get_token(ifs); 
	 trim(token);
	 title = token;
       }
       else if ( (token ==  "ORIGIN")  ) {
	 get_token(ifs); // skip format spec
	 token = get_token(ifs); 
	 trim(token);
	 origin = token;
       }
       else if ( (token ==  "DATE")  ) {
	 get_token(ifs); // skip format spec
	 token = get_token(ifs); 
	 trim(token);
	 sdate = token;
       }
       else if ( (token ==  "TIME")  ) {
	 get_token(ifs); // skip format spec
	 token = get_token(ifs); 
	 trim(token);
	 MadX::stime = token;
       }
       else if ( (token ==  "SEQUENCE")  ) {
	 get_token(ifs); // skip format spec
	 token = get_token(ifs); 
	 trim(token);
	 MadX::sequence = token;
       }
       
      //--------------------------------------------------------------
      // look for the attribute keywords specification (starts with *)
      //---------------------------------------------------------------
    }

    if ( token == "*" )  break;
  }

  double brho = (pc*1.0e9)/(2.99792458e8);  // pc is in GeV/c Brho is in T-m

  static ImportMadXDialog* dialog = 0;
  if (!dialog) dialog = new ImportMadXDialog(0);

  dialog->data_.etot   = etot;     // GeV
  dialog->data_.mass   = mass;     // GeV

  QFileInfo finfo1(fname1);
  dialog->data_.filenameto  = finfo1.absoluteDir().path() + QDir::separator() + finfo1.completeBaseName() +  ".opt";
  dialog->set();

  if ( dialog->exec() == QDialog::Rejected )  { return;} 

  OptimTextEditor* editor = 0;
  QMdiSubWindow*   sw = mdiArea->addSubWindow( editor = new OptimTextEditor() );
  editor->show();

  sw->setWindowTitle("Import from MADX");

  std::ofstream ofs( dialog->data_.filenameto.toUtf8().data());

  //-----------------------------------------------------------
  // count the number of keywords (columns) in the TFS table
  //-----------------------------------------------------------

  std::vector<std::string> keywords;

  // get all the columns names ...  

  int icol = 0;
  while ( ifs.good() ) {
    std::string token =  get_token(ifs);
    ++icol;
    if ( token == "$" ) break;
    keywords.push_back(token);
  }

  int nk = keywords.size();
  
  // skip all the corresponding column format information 

  for  ( int i=0; i<nk; ++i ) {
    std::string token =  get_token(ifs);
  }

  std::unordered_map<std::string, double> attributes;

  // main loop over all elements ...
  
  while ( true ) {

    attributes.clear();

    // The first two attributes must be the element label and the element type
    // They cannot be omitted (i.e. they need to be SELECTED) from the TFS file. 
    // Note that to be very general, we should not assume anything about the
    // data type of the columns. This would require using something like
    // boost::any to store the attribute value.

    std::string elabel = get_token(ifs);
    trim(elabel);

    std::string etype = get_token(ifs);
    if (etype == "\"\"") continue;
    trim(etype);

    if (!ifs.good() ) break;
    
    // we assume that all data are real numbers. This is almost true. The
    // column labeled "PARENT" is an exception. It is not needed by the translator,
    // so we set its "value" to 0.0. This is a kludge. 
    
    for (int i = 2; i < nk; ++i ) { 
      auto s =  get_token(ifs);
      if ( keywords[i] == "PARENT")  {
	attributes.insert( {keywords[i],  0.0} );  // FIXME - this is a kludge. 
      }
      else if ( keywords[i] == "COMMENTS") {
	attributes.insert( {keywords[i],  0.0} );  // FIXME - this is a kludge. 
      }
      else {
	attributes.insert( {keywords[i],  std::stod(s)} );
      }
    }  

    // find the start of the line marker and set the initial lattice functions.
    
    if ( ( elabel.rfind("$START") != std::string::npos) &&
	 ( etype == "MARKER") ) {

       bx  = attributes["BETX"]*100; // MADX [m] to OptiMX [cm] 
       by  = attributes["BETY"]*100; // MADX [m] to OptiMX [cm] 
       ax  = attributes["ALFX"];     // dimensionless
       ay  = attributes["ALFY"];     // dimensionless
       dx  = attributes["DX"]*100;   // MADX [m] to OptiMX [cm] 
       dxp = attributes["DPX"];      // dpx is dimensionless
       dy  = attributes["DY"]*100;   // MADX [m] to OptiMX [cm] 
       dyp = attributes["DPY"];      // dpy is dimensionless
    }
    make_element( beamline, element_dict, elabel, etype, attributes, brho);
     
  }

  output_header( editor, ofs     );
  output_variables( editor, ofs  );
  output_initial( editor, ofs    );
  output_lattice( beamline, editor, ofs    );
  output_elements( element_dict, editor, ofs   );

  editor->insertPlainText("Done..\n");
  editor->document()->setModified(false);

  ofs.close();
  ifs.close();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void output_header( OptimTextEditor* editor, std::ostream& ofs )
{
  
  using namespace MadX;
  editor->insertPlainText("Header.\n");
  auto datetime = QDateTime::currentDateTime();
  ofs << "#-----------------------------------------------------------"  << std::endl; 
  ofs << "# Translated from a MADX TFS file by OptimX" << std::endl;
  ofs << "# " << datetime.toString(Qt::ISODate).toUtf8().data() << std::endl;
  ofs << "#-----------------------------------------------------------"  << std::endl;
  ofs << "# MADX file header information" <<  std::endl;
  ofs << "# TITLE:  " << title <<  std::endl;
  ofs << "# ORIGIN: " << origin << std::endl;
  ofs << "# DATE:   " << sdate  << std::endl;
  ofs << "# TIME:   " << MadX::stime  << std::endl;
  ofs << "#-----------------------------------------------------------"  << std::endl;

 }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void output_variables( OptimTextEditor* editor, std::ostream& ofs  )
{
  editor->insertPlainText("Variables.\n");
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void output_initial( OptimTextEditor* editor, std::ostream& ofs  )
{

  using std::setw;
  using std::left;
  using namespace MadX;

  
  std::stringstream arg0;
  std::stringstream arg1;
  std::stringstream arg2;
  std::stringstream arg3;
  
  editor->insertPlainText("Initial Block.\n");

  using namespace MadX;
  
  // output the initial block;
  
  ofs << "OptiM" << std::endl;
  ofs << "# MADX SEQUENCE: " << sequence << std::endl;

  arg0  << "Energy[MeV]=" << etot*1000 << "-"<< mass*1000;
  arg1  << "  Mass[MeV]=" << mass*1000;
 
  ofs << setw(20) << left << arg0.str() 
      << setw(20) << left << arg1.str() << std::endl; 

  arg0.str(""); arg1.str(""); arg2.str(""); arg3.str(""); 

  arg0  << "Emittances:";
  arg1  << "ex[cm]=" << ex;
  arg2  << "ey[cm]=" << ey;
  arg3  << "DP/P="   << dpp;
  
  ofs << setw(20) << left << arg0.str() 
      << setw(20) << left << arg1.str()
      << setw(20) << left << arg2.str()
      << setw(20) << left << arg3.str() << std::endl; 
  
  arg0.str(""); arg1.str(""); arg2.str(""); arg3.str(""); 

  arg0 << "Initial:";
  arg1 << "BetaX[cm]="  << bx;
  arg2 << "BetaY[cm]= " << by;

  ofs << setw(20) << left << arg0.str() 
      << setw(20) << left << arg1.str() 
      << setw(20) << left << arg2.str() << std::endl; 
  
  arg0.str(""); arg1.str(""); arg2.str(""); arg3.str(""); 

  arg1 << "AlphaX="  << ax;
  arg2 << "AlphaY= " << ay;

  ofs << setw(20) << left << arg0.str() 
      << setw(20) << left << arg1.str() 
      << setw(20) << left << arg2.str() << std::endl; 
  
  arg0.str(""); arg1.str(""); arg2.str(""); arg3.str(""); 

  arg1 << "DispX[cm]=" << dx;
  arg2 << "DispY[cm]=" << dy;

  ofs << setw(20) << left << arg0.str() 
      << setw(20) << left << arg1.str() 
      << setw(20) << left << arg2.str() << std::endl; 
  
  arg0.str(""); arg1.str(""); arg2.str(""); arg3.str(""); 

  arg1 << "DispPX=" << dxp;
  arg2 << "DispPY=" << dyp;

  ofs << setw(20) << left << arg0.str() 
      << setw(20) << left << arg1.str() 
      << setw(20) << left << arg2.str() << std::endl; 

  arg0.str(""); arg1.str(""); arg2.str(""); arg3.str(""); 

  arg0 << "X[cm]=" << x_0;
  arg1 << "Y[cm]=" << y_0;
  arg2 << "Z[cm]=" << z_0;
  arg3 << "S[cm]=" << s_0;

  ofs << setw(20) << left << arg0.str() 
      << setw(20) << left << arg1.str()
      << setw(20) << left << arg2.str()
      << setw(20) << left << arg3.str() << std::endl; 

  arg0.str(""); arg1.str(""); arg2.str(""); arg3.str(""); 

  arg0 << "tetaX[deg]=" << MadX::thetax;
  arg1 << "tetaY[deg]=" << MadX::thetay;

  ofs << setw(20) << left << arg0.str() 
      << setw(20) << left << arg1.str() << std::endl;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void output_lattice( std::vector<std::string>& beamline, OptimTextEditor* editor, std::ostream& ofs  )
{
  editor->insertPlainText("Lattice.\n");

  ofs <<"#-------------------------------------------------------" << std::endl;
  ofs <<"#  Elements: O-free space, D-dipole, Q-quad, S-sextupole" << std::endl;
  ofs <<"#  Elements: B-dipole with gradient." << std::endl;
  ofs <<"begin lattice. Number of periods=1" << std::endl;
  

  int sz = beamline.size();
  int ie = 0;
  for ( auto it  = beamline.begin();
	     it != beamline.end(); ++it) { 
    ofs << *it << " ";
    if ( ( (ie+1)%5 == 0) || ie == sz-1) ofs << std::endl; 
    ++ie;      
  }
  
  ofs <<"end lattice" << std::endl;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void output_elements(std::unordered_map<std::string, std::string>& element_dict, OptimTextEditor* editor, std::ostream& ofs )
{
   using std::setw;
   using std::left;
  
   editor->insertPlainText("Element definitions.\n");

   int maxsz = 0;
   std::list<std::string> labels; // list of element labels for sorting

   for (auto it = element_dict.begin(); it != element_dict.end(); ++it ) {
     auto lbl = it->first;
     maxsz = std::max( maxsz, int(lbl.length()) );
     labels.push_back( it->first );
   }

   labels.sort();
   
   ofs << "#----------------------------------------------------"<< std::endl;
   ofs << "begin list"<< std::endl;

   char etype = 0;
   for (auto it = labels.begin(); it != labels.end(); ++it ) {
    auto item = element_dict[ *it ];
    if (toupper((*it)[0]) != etype ) {
      etype = toupper((*it)[0]);     
      ofs << "#----------------------------------------------------" << std::endl;
      ofs << "# "<< Element::type(etype) << std::endl;
      ofs << "#----------------------------------------------------" << std::endl;
    } 
    ofs << setw(maxsz+4) << left << *it << item << std::endl;
  }
  ofs << "end list"<< std::endl;
}    
