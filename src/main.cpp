//  =================================================================
//
//  main.cpp
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

#include <OptimApp.h>
#include <BeamMoments.h>
#include <QScreen>
#include <OptimMainWindow.h>
#include <GlobalEventFilter.h>
#include <Globals.h>
#undef emit
#include <sqlite3.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <cstring>
#include <sqlite/connection.hpp>
#include <fmt/core.h>

namespace {

int auto_db_extensions( sqlite3* db, char const **pzErrMsg, struct sqlite3_api_routines const* pThunk)
{
  auto optimx_logger = spdlog::get("optimx_logger");
  
 

  int status = 0;
  SPDLOG_LOGGER_DEBUG( optimx_logger, "Enabling sqlite extensions dynamic loading".);
  sqlite3_db_config(db ,SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION,1, &status);
  char* errmsg = 0;

  sqlite3_load_extension(db, "./liboptimx_sqlite_extensions", "sqlite3_extension_init", &errmsg);
  if (errmsg) {
    SPDLOG_LOGGER_ERROR( optimx_logger, errmsg );
    sqlite3_free(errmsg);
  }
  sqlite3_load_extension(db, "./liboptimx_sqlite_carray",     "sqlite3_carray_init", &errmsg);
  if (errmsg) {
    SPDLOG_LOGGER_ERROR( optimx_logger, errmsg );
    sqlite3_free(errmsg);
  }

  BeamMoments::initDBCustomFunctions(db);

  return SQLITE_OK;
}
}// namespace


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int main(int argc, char **argv)
{
   // ..................................................................................................
   // spdlog logger setup
   //
   // Logging levels in order ( TRACE is the less critical, most verbose level)
   // TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL, OFF }
   //
   // see comments below to turn logging ON/OFF  
   // ...................................................................................................
 
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);
    console_sink->set_pattern(" [%H:%M:%S] [OptiMX] [%s:%#] [%^%l%$] %v");

    
    //auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("optimx.log", true);
    //file_sink->set_level(spdlog::level::trace);
    //file_sink->set_pattern(" [%H:%M:%S.%e] [OptiMX] [%s:%#]  %v");
    
    //spdlog::sinks_init_list sink_list = { file_sink, console_sink };
    spdlog::sinks_init_list sink_list = { console_sink };
	auto optimx_logger = std::make_shared<spdlog::logger>("optimx_logger", sink_list.begin(), sink_list.end());

    spdlog::register_logger(optimx_logger);    // so that logger can be accessed using get function  
    spdlog::set_default_logger(optimx_logger); // optimx_logger can also be accessed as default logger 

    // ***** FOR NOW *****
    // use this to control logging level e.g. to turn all logging OFF -- eventually this should be controlled from the command line 
    optimx_logger->set_level(spdlog::level::trace);
    optimx_logger->set_level(spdlog::level::off);

    SPDLOG_LOGGER_WARN(optimx_logger, "this should appear in both console and file");
    optimx_logger->info("this message should not appear in the console, only in the file");
    
    SPDLOG_LOGGER_INFO( optimx_logger, "this message should not appear in the console, only in the file");

    //....................................................................................................

    int status = sqlite3_threadsafe();  
    if ( !status ) {
      SPDLOG_LOGGER_WARN(optimx_logger, "The sqlite library was compiled in serialized mode and is not threadsafe (SQLITE_THREADSAFE = 0).");
    }
    else {  
      int ierr = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
      if (ierr) {
	SPDLOG_LOGGER_WARN(optimx_logger, fmt::format("Attempt to enable the sqlite library multithreaded mode failed with error code {:d}",ierr));   
      }
    }

    status = sqlite3_auto_extension( reinterpret_cast<void(*)(void)>( auto_db_extensions) ); //  void(*xEntryPoint)(void) );

    // Open a db connection now. This prevents in-memory or temp db be deleted unless application exits.
    
    std::unique_ptr<sqlite::connection> connection_guard( Globals::preferences().con = new sqlite::connection(Globals::preferences().moments_db));

    Q_INIT_RESOURCE(optimx);

    OptimApp app(argc, argv);
  
    GlobalEventFilter filter(&app);
  
    OptimMainWindow* mainWin = new OptimMainWindow();

    QRect       dtsize   = QGuiApplication::primaryScreen()->availableGeometry();


    mainWin->setGeometry ( 20, 20, 3*dtsize.width()/4, (3*dtsize.height())/4 );
    mainWin->ensurePolished(); 
      
    mainWin->commandLine(argc,argv);
    mainWin->show();

    return app.exec();


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
