# Find CrashRpt
# ~~~~~~~~~~~~
# Copyright (c) 2017 Petr Ohlidal
#
# CMake module to search for CrashRpt library
#
# This module will set the following variables:
#
# * CrashRpt_FOUND         - True if MoFileReader is found
# * CrashRpt_INCLUDE_DIRS  - The include directory
# * CrashRpt_LIBRARIES     - The libraries to link against
#
# In addition the following imported targets are defined:
#
# * CrashRpt::CrashRpt
#

# Find the directories
find_path( CrashRpt_INCLUDE_DIR MoFileReader.h )
find_library( CrashRpt_LIBRARY
  NAMES CrashRpt1403
  PATH_SUFFIXES Debug Release RelWithDebInfo
)

# Export the paths
set(CrashRpt_INCLUDE_DIRS ${CrashRpt_INCLUDE_DIR})
set(CrashRpt_LIBRARIES ${CrashRpt_LIBRARY})

# Register the package
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( CrashRpt
  FOUND_VAR CrashRpt_FOUND
  REQUIRED_VARS CrashRpt_INCLUDE_DIRS CrashRpt_LIBRARIES
)

if( CrashRpt_FOUND )
  add_library( CrashRpt::CrashRpt INTERFACE IMPORTED )
  set_target_properties( CrashRpt::CrashRpt PROPERTIES
    INTERFACE_LINK_LIBRARIES "${CrashRpt_LIBRARIES}" 
    INTERFACE_INCLUDE_DIRECTORIES "${CrashRpt_INCLUDE_DIRS}"
  )
endif()

mark_as_advanced( CrashRpt_INCLUDE_DIR CrashRpt_LIBRARY )
