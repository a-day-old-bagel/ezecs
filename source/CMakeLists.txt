
# check if the user has defined an ecs config file in some parent scope (one is necessary)
if ( NOT EZECS_CONFIG_FILE )
  message( FATAL_ERROR "Ezecs was not given a config file (EZECS_CONFIG_FILE not defined)." )
endif ()

# check if the user has defined a target name prefix for the ecs targets (optional, but strongly recommended)
if ( NOT EZECS_TARGET_PREFIX )
  message( WARNING "Ezecs was not given target name prefix. Target will be named 'ecs'. \
           If multiple targets use ezecs, target name collisions will occur." )
endif ()

set( EZECS_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( EZECS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR} )

message( STATUS "Ezecs is using ${EZECS_CONFIG_FILE}" )
message( STATUS "to pull stuff from ${EZECS_INPUT_DIR}" )
message( STATUS "and write stuff to ${EZECS_OUTPUT_DIR}" )

configure_file( ${EZECS_INPUT_DIR}/ecsHelpers.hpp ${EZECS_OUTPUT_DIR}/ecsHelpers.hpp COPYONLY )
configure_file( ${EZECS_INPUT_DIR}/ecsHelpers.cpp ${EZECS_OUTPUT_DIR}/ecsHelpers.cpp COPYONLY )
configure_file( ${EZECS_INPUT_DIR}/ecsKvMap.hpp ${EZECS_OUTPUT_DIR}/ecsKvMap.hpp COPYONLY )
configure_file( ${EZECS_INPUT_DIR}/ecsSystem.hpp ${EZECS_OUTPUT_DIR}/ecsSystem.hpp COPYONLY )
configure_file( ${EZECS_INPUT_DIR}/ezecs.hpp ${EZECS_OUTPUT_DIR}/ezecs.hpp COPYONLY )

if ( NOT TARGET ezecs_generator )
  add_executable( ezecs_generator ecsGenerator.cpp )
  set_property( TARGET ezecs_generator PROPERTY CXX_STANDARD 11 )
  set_property( TARGET ezecs_generator PROPERTY CXX_STANDARD_REQUIRED ON )
endif ()

add_custom_command(
  OUTPUT
  ${EZECS_OUTPUT_DIR}/ecsComponents.generated.hpp
  ${EZECS_OUTPUT_DIR}/ecsComponents.generated.cpp
  ${EZECS_OUTPUT_DIR}/ecsState.generated.hpp
  ${EZECS_OUTPUT_DIR}/ecsState.generated.cpp
  COMMAND ezecs_generator
  ${EZECS_INPUT_DIR}
  ${EZECS_OUTPUT_DIR}
  ${EZECS_CONFIG_FILE}
  DEPENDS ezecs_generator ${EZECS_CONFIG_FILE}
)

include_directories( ${EZECS_OUTPUT_DIR} )

add_library( ${EZECS_TARGET_PREFIX}ecs STATIC
  ${EZECS_OUTPUT_DIR}/ecsComponents.generated.cpp
  ${EZECS_OUTPUT_DIR}/ecsState.generated.cpp
  ${EZECS_OUTPUT_DIR}/ecsHelpers.cpp
  )
target_link_libraries( ${EZECS_TARGET_PREFIX}ecs ${EZECS_LINK_TO_LIBS} realtimeutils )
target_include_directories( ${EZECS_TARGET_PREFIX}ecs PUBLIC
  ${EZECS_OUTPUT_DIR}
  )
