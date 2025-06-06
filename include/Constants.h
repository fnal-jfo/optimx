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
  extern  double const  PI;      //= 3.1415926535897932385;
  extern  double const  E_NUMB;  //= 2.7182818284590452354;
  extern  double const  ME_MEV;  // = 0.5110034;
  extern  double const  ME_kg;
  extern  double const  MP_MEV;  // = 938.2796;
  extern  double const  MP_kg;   // = 1.67262192369e−27;
  extern  double const  Mmu_MEV; // 
  extern  double const  C_CGS;   //= 2.99792458e10;
  extern  double const  C_DERV1; //= 0.299792458;
  extern  double const  C_DERV2; //= 29979.2458;
  extern  double const  C_DERV3; //= 299.792458;
  //double const  C10  = 299.7925;
  //double const  C01  = 0.2997925;
  extern  double const  KVSR;    //= 2.e-13/3.*4.80298*2.997925*2.997925*2.997925;
  extern  double const  KE;      //= 1.0128841e-19; //2.997925^5 * 137.03882 * 4.80298^2*55/(24*sqrt(3)*1.e-25

  extern  double const E_CGS;
  extern  double const E_SI; 
  extern  double const C_SI;
  extern  double const B_Euler;
  extern  double const EPS;

  extern  double const pi;              
  extern  double const a0;           // [m] Bohr radius 
  extern  double const amu;          // [kg] atomic mass unit;    
  extern  double const clight;       // [m/s] speed of light   
  extern  double const N_avogadro;   // Avogadro's number  
  extern  double const e;            // [C] elementary electric charge [C]   
  extern  double const r_p;          // [m] classical proton   radius  1/(4pi epsilon_0) e^2/(m c^2)   
  extern  double const r_e;          // [m] classical electron radius  1/(4pi epsilon_0) e^2/(m c^2)   
  extern  double const alpha_fs;     // [m] fine structure constexprant
  extern  double const eps0;         // []  permitivity  of vacuum [MKS] 
  extern  double const mu0;          // []  permeability of vacuum [MKS] 

  
}

#endif //  CONSTANTS_H

