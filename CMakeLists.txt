
cmake_minimum_required( VERSION 3.9.2 FATAL_ERROR )
cmake_policy( VERSION 3.9.2...3.17 )

set( CMAKE_DISABLE_SOURCE_CHANGES ON )
set( CMAKE_DISABLE_IN_SOURCE_BUILD ON )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

project( ezecs )

# If ezecs is used by multiple targets with different respective configs, build config-independent code only once.
if ( NOT TARGET ezecs_extern_interface )
  set( TARGET_PREFIX ezecs_extern )
  # SLIKENET
  include( realtimeutils/helpers.cmake )
  option( RAKNET_ENABLE_DLL "Generate the DLL project if true." TRUE )
  option( RAKNET_ENABLE_STATIC "Generate the static library project if true." FALSE )
  option( RAKNET_GENERATE_INCLUDE_ONLY_DIR "Setup a include/RakNet/ directory in which all the headers are copied." FALSE )
  option( RAKNET_ENABLE_SAMPLES "Generate RakNet sample projects if true." FALSE )
  add_subdirectory( slikenet )
  extern( include ${CMAKE_CURRENT_SOURCE_DIR}/slikenet/Source )
  extern( link RakNetDLL )
  # REALTIMEUTILS
  add_subdirectory( realtimeutils )
  extern( link realtimeutils )
  # DISCORD
  option( DISCORD_BUILD_EXAMPLES "Build Examples"	ON )
  add_subdirectory( discordcppsdk )
  extern( link discord )
  # CREATE ezecs_extern_interface
  extern( finish )
  if ( hasParent )
    set( EZECS_EXTERN_BUILT TRUE PARENT_SCOPE )
  endif ()
  # Networking code should also only be built once and shared, like the external interface.
  add_subdirectory( network )
else ()
  if ( hasParent )
    set( EZECS_EXTERN_BUILT FALSE PARENT_SCOPE )
  endif ()
endif ()

option( EZECS_SET_PATH_ONLY "Only set the variables 'EZECS_SOURCE_DIR' and 'EZECS_BINARY_DIR' - don't build." OFF )

if ( EZECS_SET_PATH_ONLY )
  set( EZECS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Path to ezecs source" )
  set( EZECS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated CACHE STRING "Path to ezecs build" FORCE )
  message( STATUS "Setting EZECS_SOURCE_DIR to ${EZECS_SOURCE_DIR} and EZECS_BINARY_DIR to ${EZECS_BINARY_DIR}" )

else ( EZECS_SET_PATH_ONLY )

  if ( EZECS_CONFIG_FILE )
    option( EZECS_BUILD_TEST "builds using the test case (exclusively)" OFF )
  else ()
    message( STATUS "Provide a list of files to EZECS_CONFIG_FILE to build your ECS instead of just the tests" )
    option( EZECS_BUILD_TEST "builds using the test case (exclusively)" ON )
  endif ()

  if ( EZECS_BUILD_TEST )
    message( STATUS "Ezecs is building tests ONLY." )
    if ( EZECS_CONFIG_FILE )
      message( WARNING "EZECS_CONFIG_FILE path is being overwritten by the ezecs test case! \
             Turn off option EZECS_BUILD_TEST if this is a problem." )
    endif ()
    set( EZECS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Path to ezecs source" )
    set( EZECS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "Path to ezecs build" )
    add_subdirectory( tests )
  else ( EZECS_BUILD_TEST )
    add_subdirectory( source )
  endif ( EZECS_BUILD_TEST )
endif ( EZECS_SET_PATH_ONLY )
