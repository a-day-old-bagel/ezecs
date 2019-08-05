
# See test/basic/CMakeLists.txt for general instructions.
# This test covers the use of tierd configuration files, for cases where you have a core part of the engine that
# requires some set of components and then optional higher-level, or valence, parts of the engine or game that require
# an additional set of components to compliment the core components.

set( TEST_TARGET_NAME ezecsTestTiered )

set( EZECS_CONFIG_FILE ecsConfig.hpp )
set( EZECS_TARGET_PREFIX ${TEST_TARGET_NAME}_ )
set( EZECS_LINK_TO_LIBS ) # empty in this case
add_subdirectory( ${PROJECT_SOURCE_DIR}/source ${PROJECT_BINARY_DIR}/source )

add_executable( ${TEST_TARGET_NAME} main.cpp )
target_link_libraries( ${TEST_TARGET_NAME} ${TEST_TARGET_NAME}_ecs )
set_property( TARGET ${TEST_TARGET_NAME} PROPERTY CXX_STANDARD 11 )
set_property( TARGET ${TEST_TARGET_NAME} PROPERTY CXX_STANDARD_REQUIRED ON )
