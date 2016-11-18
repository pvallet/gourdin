cmake_minimum_required(VERSION 3.1)

project(gourdin)

set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# set (CMAKE_CXX_STANDARD 11)

set(extra_flags "-std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${extra_flags}")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(SFML 2 REQUIRED system window graphics)
find_package(OpenGL REQUIRED)
find_package(FLANN REQUIRED)
find_package(GLM REQUIRED)
find_package(GLEW REQUIRED)

set(INCLUDE_DIRS
  ${SFML_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIRS}
  ${FLANN_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS})

# Find sources recursively
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.h")

# Add the folders with headers to the include path
foreach (_headerFile ${HEADER_FILES})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()

list(REMOVE_DUPLICATES INCLUDE_DIRS)

# Add external libraries added manually to the build workflow
# We don't include the header folders but the source ones, to keep
# the architecture, for example to be able to include GL/glew.h
file(GLOB_RECURSE EXTERNAL_SOURCE_FILES "external_manual/*.cpp")
list(APPEND SOURCE_FILES ${EXTERNAL_SOURCE_FILES})

foreach (_headerFile ${EXTERNAL_SOURCE_FILES})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()

if (SFML_FOUND AND OPENGL_FOUND AND FLANN_FOUND AND GLM_FOUND AND GLEW_FOUND)
  include_directories(${INCLUDE_DIRS})

  add_executable(out ${SOURCE_FILES})

  target_link_libraries(out
    ${SFML_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${FLANN_LIBRARIES}
    ${GLEW_LIBRARIES})

else()

  message( FATAL_ERROR "Some of the dependencies are not found, CMake will exit." )

endif()
