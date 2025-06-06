//  =================================================================
//
//  ProgramConstants.h
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
#ifndef PROGRAMCONSTANTS_H
#define PROGRAMCONSTANTS_H

namespace ProgramConstants {

#if 0
  double const FUDGE = 6.0; // 
  double const RADIX = 2.0; // matrinv.cpp
../src/Constants.cpp:#define NRANSImatrinv.cpp
  double const TINY  = 1.0e-20; // matrinv.cpp 
../src/ExtrnPlt.cpp:#define MAXDAT 20
../src/Import.cpp:#define N_NAMES 20000
../src/Import.cpp:#define LBUF    256
../src/Import.cpp:#define NLINES  4
../src/Import.cpp:#define NELEM   8
../src/Import.cpp:#define LSTRL   512
../src/Import.cpp:#define ARG_LEN 10
../src/Import.cpp:#define NLINES_ELM 9
../src/Import.cpp:#define N_PARAM_NAME_TOTAL 17
../src/Import.cpp:#define N_UNIT_NAME 7
../src/Import.cpp:#define LEN_BUF 1200
../src/Import.cpp:#define LMADBUF 1000
../src/Import.cpp:#define MAD_TYPE_LEN 19
../src/OptimCLTxt.cpp:#define LSTR 1024
../src/OptimCLTxt.cpp:#define O_BINARY 0
../src/OptimCommandLine.cpp:#define LSTR 1024
../src/OptimFileMenu.cpp:#define ARG_LEN 10
../src/OptimFileMenu.cpp:#define DATA_LEN 9
../src/OptimFits.cpp:#define LSTR      1024
../src/OptimFits.cpp:#define NPOINT    21
../src/OptimFits.cpp:#define NGROUP    51
../src/OptimFits.cpp:#define STEP_MULT 0.001
../src/OptimFits.cpp:#define STEP_MIN  1.e-5
../src/OptimMainWindow.cpp:#define MAX_NMB_WHILE_CYCLES 10000
../src/OptimMainWindow.cpp:#define NAME_LENGTH 20 
../src/OptimMainWindow.cpp:#define ARG_LEN 10
../src/OptimMainWindow.cpp:#define DATA_LEN 9
../src/OptimMainWindow.cpp:#define NBB_NAME 32
../src/OptimPlot.cpp:#define FDATA_LEN 32 
../src/OptimTracker.cpp:#define LSTR 1024
../src/SpaceCharge.cpp:#define LSTR 1024
../src/Utility.cpp:#define LSTR 1024
../src/Utility.cpp:#define NUMBER_THRESHOLD 1.e10
../src/View4D.cpp:#define LSTR 1024
../src/View4D.cpp:   #define MAXDAT 20
../src/View4D.cpp:#define DELTAV 1.e-6
../src/View4D.cpp:#define V_ACCURACY 1.e-8
../src/eigval4d.cpp:#define RADIX 2.0
../src/matrinv.cpp:#define NRANSI
../src/matrinv.cpp:#define TINY 1.0e-20;
../include/OptimMainWindow.h:#define MAXFILES 10
../include/OptimMdiArea.h:#define OPTIMMDIAREA_H
../include/SCalculator.h:#define TBLSZ 1024      // size of the hash table (increased from 23 to 1024 at Oct.29.2005)
../include/SCalculator.h:#define ARRAY_POINTER_LEN 256
../include/Structs.h:#define N_PARAM_NAME 9      // for import to DIMAD
../include/Structs.h:#define NAME_LENGTH 20
../include/Structs.h:#define N_ELM_FIT 50 //Nunber of Elements in a fitting group
../include/Structs.h:#define STEP_MIN 1.e-5
     ../src/Constants.cpp:#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))eigval4d.cpp
../src/Constants.cpp:#define SWAP(g,h) {y=(g);(g)=(h);(h)=y;}eigval4d.cpp
#endif
#endif //  PROGRAMCONSTANTS_H

