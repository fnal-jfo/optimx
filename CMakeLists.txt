#  CMakeLists.txt for OptimX
#
#  This file is part of OptiMX, an interactive tool  
#  for beam optics design and analysis. 
#
#  Copyright 2025 Fermi Forward Discovery Group, LLC.
#  This material was produced under U.S. Government contract
#  89243024CSC000002 for Fermi National Accelerator Laboratory (Fermilab),
#  which is operated by Fermi Forward Discovery Group, LLC for the
#  U.S. Department of Energy. The U.S. Government has rights to use,
#  reproduce, and distribute this software.
#
#  NEITHER THE GOVERNMENT NOR FERMI FORWARD DISCOVERY GROUP, LLC
#  MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
#  LIABILITY FOR THE USE OF THIS SOFTWARE.
#
#  If software is modified to produce derivative works, such modified
#  software should be clearly marked, so as not to confuse it with the
#  version available from Fermilab.
#
#  Additionally, this program is free software; you can redistribute
#  it and/or modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version. Accordingly,
#  this program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#  See the GNU General Public License for more details.
#
#  https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
#  https://www.gnu.org/licenses/gpl-3.0.html
#
#  =================================================================
#
#
# cmake -DCMAKE_BUILD_TYPE=Release ..
# cmake -DCMAKE_BUILD_TYPE=Debug ..
#--------------------------------------------

cmake_minimum_required(VERSION 3.7.1) # setting this is required
project(optimx)                       # this sets the project name

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
message("Detected OS Linux")
IF("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
  message("Detected arch 64 bit")
ENDIF()
  include(CMakeLists.linux)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
   message("Detected OS Windows")
   IF("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
   message("Detected arch 64-bit")
   include(CMakeLists.win64)
   ELSE()
   message("Detected arch 32-bit")
   include(CMakeLists.win32)
   ENDIF()
ENDIF()

#---------------------------------------------------------------------
  
