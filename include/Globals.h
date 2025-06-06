//  =================================================================
//
//  Globals.h
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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <PlotPreferences.h>
#include <QString>
#include <random>
#include <sqlite/connection.hpp>

class SCalc;

struct GlobalState {

  static bool IncludeMode;
  static char IncludeFileName[1024];
  static int  InclLineNum;
  static SCalc* calc;
  static uint   seed;
  static bool   userseed;
  // 32-bit mersennes twister uniform pseudo-random generator
  static std::mt19937 generator;
};

extern GlobalState appstate;

//............................................................

struct DefaultFont {
  QString    specs;
  QByteArray family;
  uint       point_size;
  uint       weight;
  bool       italics;
};
  
struct OptimPreferences
{
    PlotPreferences     plot_preferences;   
    DefaultFont         editor_default_font;
    uint                lattice_editor_tabsp;
    uint                text_output_tabsp;
    uint                editor_hi_fore_r;
    uint                editor_hi_fore_g;
    uint                editor_hi_fore_b;
    uint                editor_hi_back_r;
    uint                editor_hi_back_g;
    uint                editor_hi_back_b;
    uint                editor_curwidth;
    bool                editor_showlines;
    bool                editor_curline;

    bool                ignore_autorepeat;
    bool                fringe_effects_on;
    double              neg_phase_adv_threshold;
    uint                rng_seed;
    bool                use_set_rng_seed;
    bool                parallel_tracking;
    std::string         moments_db; 
    sqlite::connection* con; 
};

namespace Globals
{
    OptimPreferences& preferences();
}

#endif // GLOBALS_H
