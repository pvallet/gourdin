cmake_minimum_required(VERSION 3.1)


project(gourdin)

set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

set (CMAKE_CXX_STANDARD 11)

add_definitions(-D_USE_MATH_DEFINES)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(SFML 2 REQUIRED system window graphics)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(TinyXML REQUIRED)
find_package(FLANN REQUIRED)

# link_directories(${OPENGL_LIBRARY_DIRS} ${GLEW_LIBRARY_DIRS})
include_directories(
  ${SFML_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${TinyXML_INCLUDE_DIRS}
  ${FLANN_INCLUDE_DIRS})


file(GLOB_RECURSE source_files src/*)

add_executable(out ${source_files})

target_link_libraries(out
  ${SFML_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${TinyXML_LIBRARIES}
  ${FLANN_LIBRARIES})
