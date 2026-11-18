//  =================================================================
//
//  Constants.h
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

#ifndef CONSTANTS_H
#define CONSTANTS_H


namespace Constants {
   inline constexpr double  PI      = 3.1415926535897932385;
   inline constexpr double  E_NUMB  = 2.7182818284590452354;
   inline constexpr double  ME_MEV  = 0.510998910;
   inline constexpr double  ME_kg   = 9.10938356e-31;
   inline constexpr double  MP_kg   = 1.67262192369e-27;  // Proton mass in kg
   inline constexpr double  MP_MEV  = 938.272046;
   inline constexpr double  Mmu_MEV = 105.6583715;
   inline constexpr double  C_CGS   = 2.99792458e10;
   inline constexpr double  C_SI    = 2.99792458e8;
   inline constexpr double  C_DERV1 = 0.299792458;
   inline constexpr double  C_DERV2 = 29979.2458;
   inline constexpr double  C_DERV3 = 299.792458;
  // inline constexpr double  C10  = 299.7925;  // c*10  ?
  // inline constexpr double  C01  = 0.2997925; // c/100 ? 
   inline constexpr double  KVSR    = 2.e-13/3.*4.80298*2.997925*2.997925*2.997925;
   inline constexpr double  KE      = 1.0128841e-19; //2.997925^5 * 137.03882 * 4.80298^2*55/(24*sqrt(3)*1.e-25
   inline constexpr double  EPS     = 1.0e-5; 
   inline constexpr double  B_Euler = -0.922784335;
   inline constexpr double  E_SI    = 1.6021766208e-19; 
   inline constexpr double  E_CGS   = 4.80320451e-10; //  esu 


  inline constexpr double pi         = 3.1415926535897932385;
  inline constexpr double a0         = 5.29177210903e-11;         // [m] Bohr radius 
  inline constexpr double clight     = 2.99792458e8;              // [m/s] speed of light   
  inline constexpr double amu        = 1.66053906660e-27;         // [kg]  atomic mass unit
  inline constexpr double N_avogadro = 6.022140857e23;            // Avogadro's number  
  inline constexpr double e          = 1.602176e-19;              // [C] elementary electric charge [C]   
  inline constexpr double r_p        = 1.5346978992706655e-18;    // [m] classical proton   radius  1/(4pi epsilon_0) e^2/(m c^2)   
  inline constexpr double r_e        = 2.8179403227e-15;          // [m] classical electron radius  1/(4pi epsilon_0) e^2/(m c^2)   
  inline constexpr double alpha_fs   = 0.007297352566354976;      // [m] fine structure inline constexprant
  inline constexpr double mu0        = 4*pi*1.0e-7;               // []  permeability of vacuum [MKS] 
  inline constexpr double eps0       = 1.0/(mu0*clight*clight);   // []  permittivity  of vacuum [MKS] 
  
}

#endif //  INLINE CONSTANTS_H

