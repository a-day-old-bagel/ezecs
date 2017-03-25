
# check if the user has defined an ecs config file in some parent scope (this is necessary)
if (NOT EZECS_CONFIG_FILE)
  message(FATAL_ERROR "Ezecs was not given a config file (EZECS_CONFIG_FILE not defined).")
endif()

add_executable ( generator ecsGenerator.cpp )
add_custom_command (
    OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/test.generated.hpp
      ${CMAKE_CURRENT_BINARY_DIR}/test.generated.cpp
    COMMAND generator
      ${EZECS_CONFIG_FILE}
      ${CMAKE_CURRENT_BINARY_DIR}/test.generated.hpp
      ${CMAKE_CURRENT_BINARY_DIR}/test.generated.cpp
    DEPENDS generator
)
include_directories ( ${CMAKE_CURRENT_BINARY_DIR} )
add_library ( ecs SHARED
    ${CMAKE_CURRENT_BINARY_DIR}/test.generated.cpp
)
target_include_directories( ecs PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)