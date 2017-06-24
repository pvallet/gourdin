cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)

# Debug command
# cmake .. -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles"

project(gourdin)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release
      RelWithDebInfo MinSizeRel."
      FORCE)
endif()

# handling warnings
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-switch -Wno-sign-compare -pedantic")
endif()

set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

set (CMAKE_CXX_STANDARD 11)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

set(GLM_ROOT_DIR   external/glm)
set(GLEW_LOCATION  external/glew)
set(SFML_ROOT      external/SFML)

find_package(OpenGL)
find_package(GLM)
find_package(GLEW)
find_package(SFML 2.4.1 COMPONENTS system window graphics)
find_package(SDL2)

# If they are still not found, abort
if ( NOT(OPENGL_FOUND AND GLM_FOUND AND GLEW_FOUND AND SFML_FOUND AND SDL2_FOUND))

  message( FATAL_ERROR "Some of the dependencies are not found, CMake will exit." )

endif()

set(INCLUDE_DIRS
  ${OPENGL_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${SFML_INCLUDE_DIR}
  ${SDL2_INCLUDE_DIR}
)

# Find sources recursively
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.h")
file(GLOB_RECURSE DEBUG_SOURCE_FILES "src/*.cc")
file(GLOB_RECURSE DEBUG_HEADER_FILES "src/*.hpp")

# Add the folders with headers to the include path
foreach (_headerFile ${HEADER_FILES})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()

# Only add debug classes if we are in debug mode
if ( CMAKE_BUILD_TYPE MATCHES Debug )
  foreach (_headerFile ${DEBUG_HEADER_FILES})
      get_filename_component(_dir ${_headerFile} PATH)
      list (APPEND INCLUDE_DIRS ${_dir})
  endforeach()
endif()

list(REMOVE_DUPLICATES INCLUDE_DIRS)

# Add external libraries added manually to the build workflow
# We don't include the header folders but the source ones, to keep
# the architecture, for example to be able to include GL/glew.h
file(GLOB_RECURSE EXTERNAL_SOURCE_FILES "external_manual/*.cpp")
list(APPEND SOURCE_FILES ${EXTERNAL_SOURCE_FILES})

if ( CMAKE_BUILD_TYPE MATCHES Debug )
  list(APPEND SOURCE_FILES ${DEBUG_SOURCE_FILES})
endif()

foreach (_sourceFile ${EXTERNAL_SOURCE_FILES})
    get_filename_component(_dir ${_sourceFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()

include_directories(${INCLUDE_DIRS})

add_executable(out ${SOURCE_FILES})

target_link_libraries(out
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${SFML_LIBRARIES}
  ${SDL2_LIBRARIES}
)

# If we compile the dependencies ourselves, we need to copy the dlls into the
# target directory

if (WIN32)
  if (SFML_ROOT)
  add_custom_command(TARGET out POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/${SFML_ROOT}/lib/sfml-graphics-2.dll"
    "${CMAKE_SOURCE_DIR}/${SFML_ROOT}/lib/sfml-system-2.dll"
    "${CMAKE_SOURCE_DIR}/${SFML_ROOT}/lib/sfml-window-2.dll"
    $<TARGET_FILE_DIR:out>
  )
  endif()

  if (GLEW_LOCATION)
  add_custom_command(TARGET out POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/${GLEW_LOCATION}/bin/glew32.dll"
    $<TARGET_FILE_DIR:out>
  )
  endif()
endif()
