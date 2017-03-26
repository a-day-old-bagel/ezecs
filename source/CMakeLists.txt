
# check if the user has defined an ecs config file in some parent scope (this is necessary)
if (NOT EZECS_CONFIG_FILE)
  message(FATAL_ERROR "Ezecs was not given a config file (EZECS_CONFIG_FILE not defined).")
endif()

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ecsDelegate.hpp ${CMAKE_CURRENT_BINARY_DIR}/ecsDelegate.hpp COPYONLY )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ecsHelpers.hpp ${CMAKE_CURRENT_BINARY_DIR}/ecsHelpers.hpp COPYONLY )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ecsHelpers.cpp ${CMAKE_CURRENT_BINARY_DIR}/ecsHelpers.cpp COPYONLY )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ecsKvMap.hpp ${CMAKE_CURRENT_BINARY_DIR}/ecsKvMap.hpp COPYONLY )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ecsSystem.hpp ${CMAKE_CURRENT_BINARY_DIR}/ecsSystem.hpp COPYONLY )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ezecs.hpp ${CMAKE_CURRENT_BINARY_DIR}/ezecs.hpp COPYONLY )

add_executable ( generator ecsGenerator.cpp )
add_custom_command (
    OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/test.generated.hpp
      ${CMAKE_CURRENT_BINARY_DIR}/test.generated.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/ecsComponents.generated.hpp
      ${CMAKE_CURRENT_BINARY_DIR}/ecsComponents.generated.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/ecsState.generated.hpp
      ${CMAKE_CURRENT_BINARY_DIR}/ecsState.generated.cpp
    COMMAND generator
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
      ${EZECS_CONFIG_FILE}
    DEPENDS generator ${EZECS_CONFIG_FILE}
)

include_directories ( ${CMAKE_CURRENT_BINARY_DIR} )

if ( EZECS_BUILD_STATIC )
  add_library( ecs_static STATIC
      ${CMAKE_CURRENT_BINARY_DIR}/test.generated.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/ecsComponents.generated.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/ecsState.generated.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/ecsHelpers.cpp
  )
  target_include_directories( ecs_static PUBLIC
      ${CMAKE_CURRENT_BINARY_DIR}
  )
endif()

if ( EZECS_BUILD_SHARED )
#  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
  add_library ( ecs_shared SHARED
      ${CMAKE_CURRENT_BINARY_DIR}/test.generated.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/ecsComponents.generated.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/ecsState.generated.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/ecsHelpers.cpp
  )
  target_include_directories( ecs_shared PUBLIC
      ${CMAKE_CURRENT_BINARY_DIR}
  )
endif()
