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

  inline constexpr double  DELTAV               = 1.e-6;
  inline constexpr double  FUDGE                = 6.0;     // 
  inline constexpr double  NUMBER_THRESHOLD     = 1.e10;
  inline constexpr double  RADIX                = 2.0;     // matrinv.cpp
  inline constexpr double  STEP_MIN             = 1.e-5;
  inline constexpr double  STEP_MULT            = 0.001;
  inline constexpr double  TINY                 = 1.0e-20; // matrinv.cpp 
  inline constexpr double  V_ACCURACY           = 1.e-8;
  inline constexpr int     ARG_LEN              = 10;
  inline constexpr int     ARRAY_POINTER_LEN    = 256;
  inline constexpr int     DATA_LEN             = 9;
  inline constexpr int     FDATA_LEN            = 32; 
  inline constexpr int     LBUF                 = 256;
  inline constexpr int     LEN_BUF              = 1200;
  inline constexpr int     LMADBUF              = 1000;
  inline constexpr int     LSTR                 = 1024;
  inline constexpr int     LSTRL                = 512;
  inline constexpr int     MAD_TYPE_LEN         = 19;
  inline constexpr int     MAXDAT               = 20;
  inline constexpr int     MAXFILES             = 10;
  inline constexpr int     MAX_NMB_WHILE_CYCLES = 10000;
  inline constexpr int     NAME_LENGTH          = 32;
  inline constexpr int     NBB_NAME             = 32;
  inline constexpr int     NELEM                = 8;
  inline constexpr int     NGROUP               = 51;
  inline constexpr int     NLINES               = 4;
  inline constexpr int     NLINES_ELM           = 9;
  inline constexpr int     NPOINT               = 21;
  inline constexpr int     N_ELM_FIT            = 50;       //Max no of elements in a fitting group
  inline constexpr int     N_NAMES              = 20000;
  inline constexpr int     N_PARAM_NAME         = 9;        // for import to DIMAD
  inline constexpr int     N_PARAM_NAME_TOTAL   = 17;
  inline constexpr int     N_UNIT_NAME          = 7;
  inline constexpr int     TBLSZ                = 1024;    // calc hash table size (increased from 23 to 1024 at Oct.29.2005)
#ifdef __linux
  inline constexpr int     O_BINARY             = 0;  // already defined under windows
#endif
}

#endif //  PROGRAMCONSTANTS_H

