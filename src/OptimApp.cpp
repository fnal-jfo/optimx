//  =================================================================
//
//  OptimApp.cpp
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

#include <Globals.h>
#include <Element.h>
#include <OptimApp.h>
#include <OptimMainWindow.h>
#include <OptimEditor.h>
#include <OptimExceptions.h>
#include <OptimTextEditor.h>
#include <QEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSettings>
#include <QByteArray>
#include <OptimMessages.h>
#include <cstring>
#include <chrono>

namespace {
  // custom deleter for "wait cursor guard" 
void restore(int* p){
  QGuiApplication::restoreOverrideCursor();
  delete p;
};
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimApp::OptimApp (int& argc, char** argv) 
  : QApplication(argc,argv),
    guimode_(true),
    optimDirName_(), 
    filename_(), 
    helpfilename_("")
{
   QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
   setOrganizationName("FRA/Fermilab");
   setOrganizationDomain("fnal.gov");
   setApplicationName("OptiMX");
   setWindowIcon(QIcon(":/bitmaps/OptimApp.ico"));
   restorePreferences(); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::unique_ptr<int, OptimApp::restore_t> OptimApp::cursorWait()
{
  // the std::unique_ptr<> custom destructor restores the the default cursor
  std::unique_ptr<int,restore_t> cursor_guard(new(int), ::restore); 
  QGuiApplication::setOverrideCursor(Qt::WaitCursor);
  return cursor_guard;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimApp::restorePreferences()
{
  
  QSettings settings;  

  OptimPreferences& p =  Globals::preferences();
  
  settings.beginGroup("general_preferences");
  p.ignore_autorepeat       = settings.value("ignore_autorepeat",       p.ignore_autorepeat ).toBool();  
  p.fringe_effects_on       = settings.value("fringe_effects_on",       p.fringe_effects_on ).toBool();
  p.neg_phase_adv_threshold = settings.value("neg_phase_adv_threshold", p.neg_phase_adv_threshold).toDouble();
  p.rng_seed                = settings.value("rng_seed",                p.rng_seed).toUInt();
  p.use_set_rng_seed        = settings.value("use_set_rng_seed",        p.use_set_rng_seed).toBool();
  p.parallel_tracking       = settings.value("parallel_tracking",       p.parallel_tracking).toBool();
  settings.endGroup();

  Element::fringe_on = Globals::preferences().fringe_effects_on; // !!! FIX ME !!! element constructor (e.g. quad)
                                                                 // should set this from glolbal appstate 
  
  settings.beginGroup("plot_preferences");
  p.plot_preferences.resolution   = settings.value("resolution",    p.plot_preferences.resolution).toInt();
  p.plot_preferences.vsize        = settings.value("vsize",         p.plot_preferences.vsize ).toDouble();
  p.plot_preferences.hsize        = settings.value("hsize",         p.plot_preferences.hsize ).toDouble();
  p.plot_preferences.lwidth       = settings.value("lwidth",        p.plot_preferences.lwidth).toDouble();
  p.plot_preferences.alwidth      = settings.value("alwidth",       p.plot_preferences.alwidth).toInt();
  p.plot_preferences.fontsize     = settings.value("fontsize",      p.plot_preferences.fontsize ).toInt();
  p.plot_preferences.compact      = settings.value("compact",       p.plot_preferences.compact ).toInt();
  p.plot_preferences.timestamp    = settings.value("timestamp",     p.plot_preferences.timestamp).toBool();
  p.plot_preferences.title        = settings.value("title",         p.plot_preferences.title).toBool();
  p.plot_preferences.coloredframe = settings.value("coloredframe",  p.plot_preferences.coloredframe).toBool();
  settings.endGroup();

  settings.beginGroup("editor_default_font");

  p.editor_default_font.specs      =  settings.value("specs",                       p.editor_default_font.specs).toString();
  p.lattice_editor_tabsp           =  settings.value("lattice_editor_tabs",         p.lattice_editor_tabsp).toUInt();    
  p.text_output_tabsp              =  settings.value("text_editor_tabs",            p.text_output_tabsp).toUInt();    
  settings.endGroup();

  settings.beginGroup("editor_features");
  p.editor_curwidth  = settings.value( "cursor_width", p.editor_curwidth).toUInt();
  p.editor_showlines = settings.value( "show_lines",   p.editor_showlines).toBool();
  p.editor_curline   = settings.value( "show_curline", p.editor_curline).toBool();
  settings.endGroup();
 
  settings.beginGroup("editor_highlighting");
  p.editor_hi_fore_r =  settings.value("foreground_red",     p.editor_hi_fore_r).toUInt();
  p.editor_hi_fore_g =  settings.value("foreground_green",   p.editor_hi_fore_g).toUInt();
  p.editor_hi_fore_b =  settings.value("foreground_blue",    p.editor_hi_fore_b).toUInt();
  p.editor_hi_back_r =  settings.value("background_red",     p.editor_hi_back_r).toUInt();
  p.editor_hi_back_g =  settings.value("background_green",   p.editor_hi_back_g).toUInt();
  p.editor_hi_back_b =  settings.value("background_blue",    p.editor_hi_back_b).toUInt();
  settings.endGroup();
  
  QFont font;
  font.fromString(  p.editor_default_font.specs);

  OptimEditor::setDefaultFont(font);
  OptimTextEditor::setDefaultFont(font);

  settings.beginGroup("window_style_preference");

  QString stylekey = settings.value("style").toString().remove(QChar('&'));
  if (stylekey != "") {
    setStyle(stylekey);
    setProperty("GUIstyle", stylekey);
  }
  settings.endGroup();

  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OptimApp::savePreferences()
{

  QSettings settings( organizationName(), applicationName()); 

  OptimPreferences& p =  Globals::preferences();

  settings.beginGroup("general_preferences");
  settings.setValue("ignore_autorepeat",       p.ignore_autorepeat);
  settings.setValue("fringe_effects_on",       p.fringe_effects_on );  
  settings.setValue("neg_phase_adv_threshold", p.neg_phase_adv_threshold);
  settings.setValue("rng_seed",                p.rng_seed);
  settings.setValue("use_set_rng_seed",        p.use_set_rng_seed);
  settings.setValue("parallel_tracking",       p.parallel_tracking);
  settings.endGroup();

  settings.beginGroup("plot_preferences");
  settings.setValue("resolution" ,  p.plot_preferences.resolution );
  settings.setValue("vsize" ,       p.plot_preferences.vsize      );  // vsize [in]
  settings.setValue("hsize" ,       p.plot_preferences.hsize      );  // hsize [in]
  settings.setValue("lwidth",       p.plot_preferences.lwidth     );  // pixels
  settings.setValue("alwidth" ,     p.plot_preferences.alwidth    );  // pixels
  settings.setValue("fontsize",     p.plot_preferences.fontsize   );  // points
  settings.setValue("compact",      p.plot_preferences.compact    );  // points
  settings.setValue("timestamp",    p.plot_preferences.timestamp  );  // points
  settings.setValue("title",        p.plot_preferences.title  );      // points
  settings.setValue("coloredframe", p.plot_preferences.coloredframe); // points
  settings.endGroup();

  settings.beginGroup("editor_default_font");
  settings.setValue("specs",                p.editor_default_font.specs);
  settings.setValue("lattice_editor_tabs",  p.lattice_editor_tabsp);    
  settings.setValue("text_editor_tabs",     p.text_output_tabsp);    
  settings.endGroup();

  settings.beginGroup("editor_features");
  settings.setValue( "cursor_width", p.editor_curwidth);
  settings.setValue( "show_lines",   p.editor_showlines);
  settings.setValue( "show_curline", p.editor_curline);
  settings.endGroup();
 
  settings.beginGroup("editor_highlighting");
  settings.setValue("foreground_red",     p.editor_hi_fore_r);
  settings.setValue("foreground_green",   p.editor_hi_fore_g);
  settings.setValue("foreground_blue",    p.editor_hi_fore_b);
  settings.setValue("background_red",     p.editor_hi_back_r);
  settings.setValue("background_green",   p.editor_hi_back_g);
  settings.setValue("background_blue",    p.editor_hi_back_b);
  settings.endGroup();

  settings.beginGroup("window_style_preference");
  if ( property("GUIstyle").isValid() )  {
    settings.setValue("style", property("GUIstyle").toString().remove('&') );
  }
  else { 
    settings.remove("style");
  }
  settings.endGroup();
}
  
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OptimApp::~OptimApp()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool OptimApp::notify(QObject * receiver, QEvent * event)
{

  bool accept = false;
  
  char module[128];
    
  try { return QApplication::notify(receiver, event); }
  
  catch (OptimInterruptException& e)
  { 
    return true; 
  }    

  catch (std::runtime_error& e)
  {
    strcpy( module, e.what());
    try {
      std::rethrow_if_nested(e);
      OptimMessageBox::warning(0, "OptiMX", e.what(), QMessageBox::Ok );
      return true;
    }
    catch ( ...)
   {
    std::cout << "An unknown exception occurred." << std::endl;
    OptimMessageBox::warning(0, "OptimX", "An unknown exception has occurred.", QMessageBox::Ok );
    accept = true;
   }

  }

  catch (std::exception& e)
  {  
    try {
      std::rethrow_if_nested(e);
      OptimMessageBox::warning(0, "OptiMX", e.what(), QMessageBox::Ok );
      return true;
    }
    catch ( ...)
    {
     std::cout << "An unknown exception occurred." << std::endl;
     OptimMessageBox::warning(0, "OptimX", "An unknown exception has occurred.", QMessageBox::Ok );
     accept = true;
    }
  }
  
  catch ( ...)
  {
    std::cout << "An unknown exception occurred." << std::endl;
    OptimMessageBox::warning(0, "OptimX", "An unknown exception has occurred.", QMessageBox::Ok );
    accept = true;
  }

  return accept;
  
}
