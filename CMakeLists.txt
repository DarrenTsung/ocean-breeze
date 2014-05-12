cmake_minimum_required (VERSION 2.8)
project (Ocean-breeze)
set(VERSION "0.1.0")

# Required libraries in lib
# add_subdirectory(lib/entityx EXCLUDE_FROM_ALL)
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)

# use GLUT
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})

if (NOT APPLE)
    # HACK: This is NOTFOUND on OS X 10.8
    include_directories(${OPENGL_INCLUDE_DIR})
endif()

# use Eigen
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Eigen REQUIRED)
include_directories(${EIGEN_INCLUDE_DIRS})

# include paths
include_directories(${GLUT_INCLUDE_DIRS} lib lib/glfw/include src)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
file(GLOB_RECURSE terrainator_SOURCES "src/*.cpp")

add_executable(Ocean-breeze WIN32 MACOSX_BUNDLE ${terrainator_SOURCES})

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
target_link_libraries(Ocean-breeze glfw ${GLFW_LIBRARIES} ${GLUT_LIBRARIES})
