 cmake_minimum_required(VERSION 2.4.6)


# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE RelWithDebInfo)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})


#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


set(CMAKE_CXX_FLAGS "-fopenmp")

link_directories(
  $ENV{OBVIOUSLY_ROOT}/build/release/obvision
  $ENV{OBVIOUSLY_ROOT}/build/release/obgraphic
  $ENV{OBVIOUSLY_ROOT}/build/release/obcore
	${PROJECT_SOURCE_DIR}/lib
	)
include_directories(
	$ENV{OBVIOUSLY_ROOT}
	${PROJECT_SOURCE_DIR}/include
	)


add_executable(servo_rotate src/servo_rotate.cpp)

target_link_libraries(servo_rotate m dxl obcore)
