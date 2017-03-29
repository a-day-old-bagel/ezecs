
# What you see here is all you need to do in CMake to configure and use ezecs:
# 1. Set the config file path
# 2. Use 'add_subdirectory' on the ezecs directory
#    (this file is a little unusual because it adds the ezecs/source directory instead)
# 3. At this point you'll have access to the ecs_static and/or ecs_shared targets
#    (depending on the cmake options you used). Link to them as usual.
# 
# Both targets automatically include the correct source directory which has the
# generated files in it, so DO NOT 'include_directories' on the ezecs/source directory,
# since the generated files will NOT be found there.
#
# For example, if your project included ezecs as a sumbodule, 
# your CMakeLists.txt might include the following lines:
#
# set( EZECS_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/myEcsConfig.hpp )
# add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/ezecs )
# add_executable( myTarget mySource.cpp )
# target_link_libraries( myTarget ecs_static )

set( EZECS_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ecsConfig.hpp )
add_subdirectory( ../source ${CMAKE_CURRENT_BINARY_DIR}/../source )
add_executable( ezecsTest main.cpp )
target_link_libraries( ezecsTest ecs_static )
