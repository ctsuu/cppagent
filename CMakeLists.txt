# The version number.
set(AGENT_VERSION_MAJOR 1)
set(AGENT_VERSION_MINOR 6)
set(AGENT_VERSION_PATCH 0)
set(AGENT_VERSION_BUILD 7)
set(AGENT_VERSION_RC "")

# This minimum version is to support Visual Studio 2017 and C++ feature checking and FetchContent
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

option(AGENT_ENABLE_UNITTESTS "Enables the agent's unit tests" ON)
set(INSTALL_GTEST OFF FORCE)

project(cppagent LANGUAGES C CXX)

# All of the C++ Agent code, DLib require C++ 14 features to compile.
# We will define these properties by default for each CMake target to be created.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# By default only generate 2 configurations (Debug and Release) for simplicity.
# The user can change this option if required for the additional ones such as 'RelWithDebInfo'.
# The first type will be used as the default build type if the user does not specify one.
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
if(NOT CMAKE_BUILD_TYPE)
  list(GET CMAKE_CONFIGURATION_TYPES 0 _default_build_type)
  message(STATUS "Setting build type to '${_default_build_type}'.")
  set(CMAKE_BUILD_TYPE "${_default_build_type}" CACHE STRING "Choose the type of build." FORCE)
else()
  # Check if CMAKE_BUILD_TYPE is valid otherwise fail.
  set(_uppercase_configuration_types)
  foreach(type ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${type}" _uppercase_type)
    list(APPEND _uppercase_configuration_types ${_uppercase_type})
  endforeach()
  string(TOUPPER "${CMAKE_BUILD_TYPE}" _uppercase_build_type)

  list(FIND _uppercase_configuration_types "${_uppercase_build_type}" _is_valid_type)
  if(${_is_valid_type} EQUAL -1)
    message(FATAL_ERROR "'${CMAKE_BUILD_TYPE}' is not a valid build type for MTConnect Adapter. "
      "Valid build types are '${CMAKE_CONFIGURATION_TYPES}'.")
  endif()
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})

# Add our './cmake' sub-folder to the lists searched when calling functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Setup compiler options for Windows, OSX, Linux (each file guards against usage on an inappropriate
# platform)
if(MSVC)
  # Default winver to Vista and Later
  set(WINVER "0x0600" CACHE STRING "Windows Target Version: 0x0400 - 95 & NT 4.0, 0x0500 - 2000, 0x0501 - XP, 0x0600 - Vista, 0x0601 - 7, 0x0602 - 8")
endif()

include(cmake/msvc_use_static_runtime.cmake)
include(cmake/osx_no_app_or_frameworks.cmake)
include(cmake/ClangFormat.cmake)
include(cmake/ClangTidy.cmake)

# Add our dependency projects
include(cmake/LibXML2.cmake)
include(cmake/DLib.cmake)
include(cmake/Date.cmake)
include(cmake/json.cmake)

# Add our projects
add_subdirectory(agent_lib)
add_subdirectory(agent)
if(AGENT_ENABLE_UNITTESTS)
  enable_testing()
  add_subdirectory(test)
endif()

include(cmake/ide_integration.cmake)

# For Visual Studio generators it is now possible (since V3.6) to set a default startup project.
# We will set this to the agent_test project.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT agent_test)

install(FILES README.md LICENSE.TXT DESTINATION ".")
install(DIRECTORY schemas/ DESTINATION "schemas")
install(DIRECTORY simulator/ DESTINATION "simulator")
install(DIRECTORY styles/ DESTINATION "styles")
install(DIRECTORY tools/ DESTINATION "tools")
install(FILES simulator/agent.cfg DESTINATION "bin")

set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_PACKAGE_VERSION "${AGENT_VERSION_MAJOR}.${AGENT_VERSION_MINOR}.${AGENT_VERSION_PATCH}.${AGENT_VERSION_BUILD}")
set(CPACK_PACKAGE_VERSION_MAJOR ${AGENT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${AGENT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${AGENT_VERSION_PATCH})
set(CPACK_PACKAGE_NAME "cppagent")
set(CPACK_PACKAGE_VENDOR "MTConnect.org")

include(CPack)

