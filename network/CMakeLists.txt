
add_library( ezecs_network STATIC
  client.hpp client.cpp
  netInterface.hpp netInterface.cpp
  server.hpp server.cpp
  constants.hpp
  )
target_link_libraries( ezecs_network
  ezecs_extern_interface
  )
target_include_directories( ezecs_network PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  )
