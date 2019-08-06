
cmake_minimum_required( VERSION 3.3 )
set( CMAKE_DISABLE_SOURCE_CHANGES ON )
set( CMAKE_DISABLE_IN_SOURCE_BUILD ON )
project( ezecs )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

option( EZECS_SET_PATH_ONLY "Only set the variable 'EZECS_SOURCE_DIR' - don't build." OFF )
option( EZECS_BUILD_TEST "builds using the test case (exclusively)" ON )

if (NOT TARGET realtimeutils)
  add_subdirectory( realtimeutils )
endif()

if( EZECS_SET_PATH_ONLY )
  set( EZECS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Path to use for EZECS" )
else( EZECS_SET_PATH_ONLY )
  if ( EZECS_BUILD_TEST )
    message( WARNING "Ezecs is building with the test case configuration ONLY, as per option EZECS_BUILD_TEST" )
    if ( EZECS_CONFIG_FILE )
      message( WARNING "EZECS_CONFIG_FILE path is being overwritten by the ezecs test case! \
             Turn off option EZECS_BUILD_TEST if this is a problem." )
    endif()
    set( EZECS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Path to use for EZECS" )
    add_subdirectory( tests )
  else ( EZECS_BUILD_TEST )
    add_subdirectory( source )
  endif ( EZECS_BUILD_TEST )
endif( EZECS_SET_PATH_ONLY )
