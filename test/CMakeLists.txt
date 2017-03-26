
if ( EZECS_CONFIG_FILE )
  message( WARNING "EZECS_CONFIG_FILE path is being overwritten by the ezecs test case! \
           Turn off option EZECS_BUILD_TEST if this is a problem.")
endif()

set( EZECS_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ecsConfig.hpp )
add_subdirectory( ../source ${CMAKE_CURRENT_BINARY_DIR}/../source )

add_executable( ezecsTest main.cpp )
target_link_libraries( ezecsTest ecs_static )
