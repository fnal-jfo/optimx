//  =================================================================
//
//  Globals.cpp
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

#include <Globals.h>
#include <QSettings>
#include <QColor>

bool         GlobalState::IncludeMode        = false;
char         GlobalState::IncludeFileName[];
int          GlobalState::InclLineNum        = 0;
SCalc*       GlobalState::calc               = 0;

uint         GlobalState::seed                = std::chrono::system_clock::now().time_since_epoch().count();
bool         GlobalState::userseed            = false;
// 32-bit mersennes twister uniform pseudo-random generator
std::mt19937 GlobalState::generator(GlobalState::seed);

GlobalState appstate; 

namespace Globals
{

  
OptimPreferences& preferences()
{

  static  OptimPreferences prefer;

  static  bool first_call = true;
  
  if ( first_call) {
    
     prefer.plot_preferences.resolution    = 300;    // dpi 
     prefer.plot_preferences.vsize         = 8.5;    // vsize [in]
     prefer.plot_preferences.hsize         = 11.0;   // hsize [in]
     prefer.plot_preferences.lwidth        = 2.0;    // pixels
     prefer.plot_preferences.alwidth       = 1;      // pixels
     prefer.plot_preferences.fontsize      = 12;     // points
     prefer.plot_preferences.timestamp     = false;  // bool
     prefer.plot_preferences.compact       = 0;      // int
     prefer.plot_preferences.title         = false;  // bool
     prefer.plot_preferences.coloredframe  = false;  // bool

     
#ifdef USE_MSWINDOWS
     prefer.editor_default_font.specs     =  QString("Consolas,9,-1,5,75,0,0,0,0,0");
#else
     prefer.editor_default_font.specs     =  QString("Liberation Mono,9,-1,5,75,0,0,0,0,0");
#endif
     prefer.lattice_editor_tabsp     =  8;
     prefer.text_output_tabsp        =  8;

     prefer.editor_curwidth           = 2;
     prefer.editor_showlines          = true;
     prefer.editor_curline            = false;
 
     QColor fore("white") ;   
     prefer.editor_hi_fore_r            = fore.red();   
     prefer.editor_hi_fore_g            = fore.green();   
     prefer.editor_hi_fore_b            = fore.blue();   

     QColor back("blue") ;   
     prefer.editor_hi_back_r            = back.red();  
     prefer.editor_hi_back_g            = back.green();  
     prefer.editor_hi_back_b            = back.blue();  
     
     prefer.ignore_autorepeat            = false;
     prefer.fringe_effects_on            = true;
     prefer.neg_phase_adv_threshold      = 1.0e-4;
     prefer.rng_seed                     = GlobalState::seed;
     prefer.use_set_rng_seed             = false;

     first_call = false;

     prefer.moments_db                   = ":memory:"; // "./moments.db";  
     prefer.con                          = 0; 

  }

  
  return prefer;
}

} // namespace 


