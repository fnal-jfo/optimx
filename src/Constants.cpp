//  =================================================================
//
//  Constants.cpp
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

#include <Constants.h>

namespace Constants {
   const double  PI      = 3.1415926535897932385;
   const double  E_NUMB  = 2.7182818284590452354;
   const double  ME_MEV  = 0.510998910;
   const double  ME_kg   = 9.10938356e-31;
   const double  MP_kg   = 1.67262192369e-27;  // Proton mass in kg
   const double  MP_MEV  = 938.272046;
   const double  Mmu_MEV = 105.6583715;
   const double  C_CGS   = 2.99792458e10;
   const double  C_SI    = 2.99792458e8;
   const double  C_DERV1 = 0.299792458;
   const double  C_DERV2 = 29979.2458;
   const double  C_DERV3 = 299.792458;
  // const double  C10  = 299.7925;  // c*10  ?
  // const double  C01  = 0.2997925; // c/100 ? 
   const double  KVSR    = 2.e-13/3.*4.80298*2.997925*2.997925*2.997925;
   const double  KE      = 1.0128841e-19; //2.997925^5 * 137.03882 * 4.80298^2*55/(24*sqrt(3)*1.e-25
   const double  EPS     = 1.0e-5; 
   const double  B_Euler = -0.922784335;
   const double  E_SI    = 1.6021766208e-19; 
   const double  E_CGS   = 4.80320451e-10; //  esu 


  const double pi         = 3.1415926535897932385;
  const double a0         = 5.29177210903e-11;         // [m] Bohr radius 
  const double clight     = 2.99792458e8;              // [m/s] speed of light   
  const double amu        = 1.66053906660e-27;         // [kg]  atomic mass unit
  const double N_avogadro = 6.022140857e23;            // Avogadro's number  
  const double e          = 1.602176e-19;              // [C] elementary electric charge [C]   
  const double r_p        = 1.5346978992706655e-18;    // [m] classical proton   radius  1/(4pi epsilon_0) e^2/(m c^2)   
  const double r_e        = 2.8179403227e-15;          // [m] classical electron radius  1/(4pi epsilon_0) e^2/(m c^2)   
  const double alpha_fs   = 0.007297352566354976;      // [m] fine structure constant
  const double mu0        = 4*pi*1.0e-7;               // []  permeability of vacuum [MKS] 
  const double eps0       = 1.0/(mu0*clight*clight);   // []  permittivity  of vacuum [MKS] 

}

/****************************

  double                   FUDGE = 6.;     // Cavity.cpp 
  unsigned int           N_NAMES = 20000;  // Import.cpp
  unsigned int              LBUF = 25 ;    // Import.cpp
  unsigned int            NLINES = 4;      // Import.cpp 
  unsigned int             NELEM = 8;      // Import.cpp
  unsigned int             LSTRL = 512;    // Import.cpp 
  unsigned int           ARG_LEN  = 10;    // Import.cpp 
  unsigned int        NLINES_ELM  = 9 ;    // Import.cpp 
  unsigned int N_PARAM_NAME_TOTAL = 17;    // Import.cpp 
  unsigned int        N_UNIT_NAME = 7;     // Import.cpp 
  unsigned int            LEN_BUF = 1200;  // Import.cpp 
  unsigned int            LMADBUF = 1000;  // Import.cpp 
  unsigned int       MAD_TYPE_LEN = 19;    // Import.cpp 

LSTR 1024OptimCLTxt.cpp
O_BINARY 0OptimCLTxt.cpp 
ARG_LEN 10OptimFileMenu.cpp 
DATA_LEN 9OptimFileMenu.cpp 
NPOINT 5/OptimFits.cpp 
NGROUP 20OptimFits.cpp 
MAX_NMB_WHILE_CYCLES 10000 OptimMainWindow.cpp
NAME_LENGTH 20 OptimMainWindow.cpp:
ARG_LEN 10 /OptimMainWindow.cpp
DATA_LEN 9OptimMainWindow.cpp 
NBB_NAME 32OptimMainWindow.cpp 

LSTR 1024OptimTracker.cp 
LSTR 1024SpaceCharge.cpp 
LSTR 1024Utility.cpp 
NUMBER_THRESHOLD 1.e10/Utility.cpp 
LSTR 1024UtilityRoutines.cp 
LSTR 1024View4D.cpp 
DELTAV 1.e-6/View4D.cpp 
V_ACCURASY 1.e-8View4D.cpp 

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))eigval4d.cpp
#define SWAP(g,h) {y=(g);(g)=(h);(h)=y;}eigval4d.cpp
#define RADIX 2.0matrinv.cpp
#define NRANSImatrinv.cpp
#define TINY 1.0e-20;/matrinv.cpp 
*****************************************/


