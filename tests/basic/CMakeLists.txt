
# What you see here is all you need to do in CMake to configure and use ezecs:
# 1. Set the config file paths (more than one config file can be given - follow normal CMake list practices)
# 2. Set the ezecs targets' naming prefix to something unique
# 3. Specify the libs that ezecs will need to link to to build whatever you put in
#    your config file (consider what you had to #include in your config file)
# 4. Use 'add_subdirectory' on the ezecs directory
#    (this test file is a little unusual because it adds the ezecs/source directory instead)
# 5. At this point you'll have access to the ${YOUR_PREFIX}_ecs target. Link to it as usual.
# 
# The generated target automatically includes the correct build directory which has the
# generated files in it, so DO NOT 'include_directories' on the ezecs/source directory,
# since the generated files will NOT be found there (it's a bad practice to modify the source tree during the
# build process, and including part of the build tree is less bad).
#
# For example, your CMakeLists.txt might include the following lines:
#
# set( EZECS_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/myEcsConfig.hpp )
# set( EZECS_TARGET_PREFIX ${MY_PREFIX} )
# set( EZECS_LINK_TO_LIBS ${MY_LIBS} )
# add_subdirectory( ${WHEREVER_YOU_PUT_THE_EZECS_SOURCE_FOLDER}/ezecs ${CMAKE_CURRENT_BINARY_DIR}/ezecs )
# add_executable( myTarget mySource.cpp )
# target_link_libraries( myTarget ${MY_PREFIX}_ecs )

set( TEST_TARGET_NAME ezecsTestBasic )

set( EZECS_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ecsConfig.hpp )
set( EZECS_TARGET_PREFIX ${TEST_TARGET_NAME} )
set( EZECS_LINK_TO_LIBS ) # empty in this case
add_subdirectory( ${EZECS_SOURCE_DIR}/source ${CMAKE_CURRENT_BINARY_DIR}/generated ) # Don't normally add "/source"

add_executable( ${TEST_TARGET_NAME} main.cpp )
target_link_libraries( ${TEST_TARGET_NAME} ${TEST_TARGET_NAME}_ecs )
set_property( TARGET ${TEST_TARGET_NAME} PROPERTY CXX_STANDARD 11 )
set_property( TARGET ${TEST_TARGET_NAME} PROPERTY CXX_STANDARD_REQUIRED ON )
