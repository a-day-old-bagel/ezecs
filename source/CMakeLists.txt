
# check if the user has defined an ecs config file in some parent scope (one is necessary)
if (NOT EZECS_CONFIG_FILE)
  message(FATAL_ERROR "Ezecs was not given a config file (EZECS_CONFIG_FILE not defined).")
endif()

set( EZECS_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( EZECS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR} )

configure_file( ${EZECS_INPUT_DIR}/ecsDelegate.hpp ${EZECS_OUTPUT_DIR}/ecsDelegate.hpp COPYONLY )
configure_file( ${EZECS_INPUT_DIR}/ecsHelpers.hpp ${EZECS_OUTPUT_DIR}/ecsHelpers.hpp COPYONLY )
configure_file( ${EZECS_INPUT_DIR}/ecsHelpers.cpp ${EZECS_OUTPUT_DIR}/ecsHelpers.cpp COPYONLY )
configure_file( ${EZECS_INPUT_DIR}/ecsKvMap.hpp ${EZECS_OUTPUT_DIR}/ecsKvMap.hpp COPYONLY )
configure_file( ${EZECS_INPUT_DIR}/ecsSystem.hpp ${EZECS_OUTPUT_DIR}/ecsSystem.hpp COPYONLY )
configure_file( ${EZECS_INPUT_DIR}/ezecs.hpp ${EZECS_OUTPUT_DIR}/ezecs.hpp COPYONLY )

add_executable ( generator ecsGenerator.cpp )
set_property( TARGET generator  PROPERTY CXX_STANDARD 11 )
set_property( TARGET generator  PROPERTY CXX_STANDARD_REQUIRED ON )
add_custom_command (
    OUTPUT
      ${EZECS_OUTPUT_DIR}/ecsComponents.generated.hpp
      ${EZECS_OUTPUT_DIR}/ecsComponents.generated.cpp
      ${EZECS_OUTPUT_DIR}/ecsState.generated.hpp
      ${EZECS_OUTPUT_DIR}/ecsState.generated.cpp
    COMMAND generator
      ${EZECS_INPUT_DIR}
      ${EZECS_OUTPUT_DIR}
      ${EZECS_CONFIG_FILE}
    DEPENDS generator ${EZECS_CONFIG_FILE}
)

include_directories ( ${EZECS_OUTPUT_DIR} )

if ( EZECS_BUILD_STATIC )
  add_library( ecs_static STATIC
      ${EZECS_OUTPUT_DIR}/ecsComponents.generated.cpp
      ${EZECS_OUTPUT_DIR}/ecsState.generated.cpp
      ${EZECS_OUTPUT_DIR}/ecsHelpers.cpp
  )
  target_link_libraries( ecs_static ${EZECS_LINK_TO_LIBS})
  target_include_directories( ecs_static PUBLIC
      ${EZECS_OUTPUT_DIR}
  )
  set_property( TARGET ecs_static PROPERTY CXX_STANDARD 11 )
  set_property( TARGET ecs_static PROPERTY CXX_STANDARD_REQUIRED ON )
endif()

# FixMe: The shared library doesn't seem to be working.
if ( EZECS_BUILD_SHARED )
  add_library ( ecs_shared SHARED
      ${EZECS_OUTPUT_DIR}/ecsComponents.generated.cpp
      ${EZECS_OUTPUT_DIR}/ecsState.generated.cpp
      ${EZECS_OUTPUT_DIR}/ecsHelpers.cpp
  )
  target_link_libraries( ecs_shared ${EZECS_LINK_TO_LIBS})
  target_include_directories( ecs_shared PUBLIC
      ${EZECS_OUTPUT_DIR}
  )
  set_property( TARGET ecs_shared PROPERTY CXX_STANDARD 11 )
  set_property( TARGET ecs_shared PROPERTY CXX_STANDARD_REQUIRED ON )
endif()
