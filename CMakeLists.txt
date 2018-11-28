cmake_minimum_required (VERSION 3.1)
#do not lower cmake due to Threads requirements!
project(mariadbwrap)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)

# This is needed for compatibility with Boost 1.64
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

### Set output directories on a per-configuration base
# Single configuration
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{HOME}/bin )
# Multi configuration
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    if( OUTPUTCONFIG STREQUAL "RELEASE" )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} $ENV{HOME}/bin )
    else()
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/${OUTPUTCONFIG} )
    endif()
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

if (MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

find_package(MySQL REQUIRED)

link_directories(${SQLITE3_LIBRARY_DIR})
include_directories(${MYSQL_INCLUDE_DIR}
                    ${CMAKE_SOURCE_DIR}/jsoncons/include
                    ${CMAKE_SOURCE_DIR})

# List of projects
add_executable(test_mariadb "${CMAKE_SOURCE_DIR}/test/test_mariadb.cpp")

# Add linking dependencies
target_link_libraries(test_mariadb ${MYSQL_LIB})

