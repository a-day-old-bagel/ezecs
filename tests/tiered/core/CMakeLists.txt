
# See test/basic/CMakeLists.txt for general instructions.
set( EZECS_CONFIG_FILE ${EZECS_CONFIG_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/ecsConfig.hpp PARENT_SCOPE )

add_library( ${TEST_TARGET_NAME}_core STATIC
  coreSystems.cpp coreSystems.hpp
  )
target_link_libraries( ${TEST_TARGET_NAME}_core
  ${TEST_TARGET_NAME}_ecs
  )
target_include_directories( ${TEST_TARGET_NAME}_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  )
