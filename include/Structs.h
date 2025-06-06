//  =================================================================
//
//  Structs.h
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

#ifndef STRUCTS_H
#define STRUCTS_H

#define N_PARAM_NAME 9      // for import to DIMAD
#define NAME_LENGTH 32

//#define N_ELM_FIT 50 //Nunber of Elements in a fitting group
//#define STEP_MIN 1.e-5

#include <complex>
#include <iostream>
#include <array>
#include <vector>
#include <initializer_list>

#include <RMatrixFwd.h>


enum SubSpace { Horizontal=0, Vertical=2, Longitudinal=4}; 

//- Structures for Lattice calculations ----------------------------------------

struct resolve { 
  
   char name[NAME_LENGTH]; 
   double L; 
   double B; 
   double G; 
   double S; 
   double T;
   double beta1; 
   double beta2; 
   double Hin; 
   double Hout; 
   double Kin; 
   double Kout; 
   double GAP;
};


struct dimad {

  char tip; 
  char name[NAME_LENGTH]; 
  double v[N_PARAM_NAME]; 

};


struct ExtData {
   
  ExtData() { name[0] = 0; n=0; } 
  char name[NAME_LENGTH]; 
  int n; 
  std::vector<double> x;
  std::vector<double> y;  
  std::vector<double> v;
};



struct BeamSize {
  double a;
  double b;
  double alpha;

};
    
struct BunchParam{ 
  double  I;
  double  L;
  double  Dp;
  double  alpha;
  double  sOffest;
};


#endif // STRUCTS_H
