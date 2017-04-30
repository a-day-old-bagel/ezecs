
cmake_minimum_required( VERSION 3.3 )
set( CMAKE_DISABLE_SOURCE_CHANGES ON )
set( CMAKE_DISABLE_IN_SOURCE_BUILD ON )
project( ezecs )

# FixMe: The shared library doesn't seem to be working.
option( EZECS_BUILD_STATIC "build static library" ON )
option( EZECS_BUILD_SHARED "build shared library" OFF )
option( EZECS_BUILD_TEST "builds using the test case (exclusively)" OFF )

if ( EZECS_BUILD_TEST )
  message( WARNING "Ezecs is building with the test case configuration, as per option EZECS_BUILD_TEST" )
  if ( EZECS_CONFIG_FILE )
    message( WARNING "EZECS_CONFIG_FILE path is being overwritten by the ezecs test case! \
           Turn off option EZECS_BUILD_TEST if this is a problem." )
  endif()
  add_subdirectory( test )
else ( EZECS_BUILD_TEST )
  add_subdirectory( source )
endif ( EZECS_BUILD_TEST )