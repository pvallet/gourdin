cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)

# Debug command
# cmake .. -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles"

set(PROJECT_NAME gourdin)

project(${PROJECT_NAME})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release
      RelWithDebInfo MinSizeRel."
      FORCE)
endif()

if (NOT ANDROID)
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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-switch -pedantic")
  endif()

  set (CMAKE_CXX_STANDARD 11)
  set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

  # Move the built shared libraries to the executable location
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${EXECUTABLE_OUTPUT_PATH})

  find_package(OpenGL)

  # Compiling SDL2 and SDL2_image from source
  set(SDL_STATIC OFF)
  add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL2/SDL2)
  add_subdirectory(external/SDL2/SDL2_image)

  if (WIN32)
    SET(BUILD_UTILS OFF)
    add_subdirectory(external/glew/build/cmake)
  ENDIF()

# Include SDL2 libraries for android
endif()

# SDL2pp needs defining where the SDL2 libraries are
set(SDL2_LIBRARY SDL2)
set(SDL2MAIN_LIBRARY SDL2) # Hack to bypass SDL2Main, which is not needed
set(SDL2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/SDL2/SDL2/include)

set(SDL2_IMAGE_LIBRARY SDL2_image)
set(SDL2_IMAGE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/SDL2/SDL2_image)

set(SDL2PP_WITH_IMAGE ON)
add_subdirectory(external/libSDL2pp)

if (ANDROID)
  string(TOLOWER ${CMAKE_BUILD_TYPE} ANDROID_BUILD_DIR)
  set(DISTRIBUTION_DIR ${CMAKE_SOURCE_DIR}/android/distribution/android/SDL2/intermediates/ndkBuild)
  set(SDL_LOCATION ${CMAKE_SOURCE_DIR}/external/SDL2)

  include_directories( ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} )

  add_library( ${SDL2_LIBRARY} SHARED IMPORTED )
  add_library( ${SDL2_IMAGE_LIBRARY} SHARED IMPORTED )
  set_target_properties(${SDL2_LIBRARY} PROPERTIES IMPORTED_LOCATION
  ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2.so)
  set_target_properties(${SDL2_IMAGE_LIBRARY} PROPERTIES IMPORTED_LOCATION
  ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2_image.so)
endif()

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(INCLUDE_DIRS
  ${OPENGL_INCLUDE_DIRS}
  external/glm
  external/glew/include
  ${SDL2PP_INCLUDE_DIRS}
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
file(GLOB_RECURSE EXTERNAL_SOURCE_FILES "external_manual/*.cpp")
list(APPEND SOURCE_FILES ${EXTERNAL_SOURCE_FILES})

if ( CMAKE_BUILD_TYPE MATCHES Debug )
  list(APPEND SOURCE_FILES ${DEBUG_SOURCE_FILES})
endif()

foreach (_sourceFile ${EXTERNAL_SOURCE_FILES})
    get_filename_component(_dir ${_sourceFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()

# Add font header files
list (APPEND INCLUDE_DIRS "external_manual/fonts/")

include_directories(${INCLUDE_DIRS})

if (ANDROID)
  add_library( main SHARED ${SDL_LOCATION}/SDL2/src/main/android/SDL_android_main.c ${SOURCE_FILES} )

  target_link_libraries(main
    GLESv3
    log
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARY}
    ${SDL2PP_LIBRARIES}
  )

else()
  add_executable(${PROJECT_NAME} ${SOURCE_FILES})

  if (WIN32)
    target_link_libraries(${PROJECT_NAME}
      ${OPENGL_LIBRARIES}
      glew_s
      SDL2
      SDL2_image
      ${SDL2PP_LIBRARIES}
    )
  else()
    target_link_libraries(${PROJECT_NAME}
      ${OPENGL_LIBRARIES}
      SDL2
      SDL2_image
      ${SDL2PP_LIBRARIES}
    )
  endif()
endif()

# Generate file for clang-autocomplete and clang-linter in atom

message(STATUS "Generating ${CMAKE_SOURCE_DIR}/.clang_complete")
file(WRITE ${CMAKE_SOURCE_DIR}/.clang_complete "")
foreach(dir ${INCLUDE_DIRS})
  file(APPEND ${CMAKE_SOURCE_DIR}/.clang_complete "-I${dir}\n")
endforeach()
string(REPLACE "'" "" CMAKE_CXX_FLAGS_SPLIT ${CMAKE_CXX_FLAGS})
string(REPLACE " " ";" CMAKE_CXX_FLAGS_SPLIT ${CMAKE_CXX_FLAGS_SPLIT})
foreach(flag ${CMAKE_CXX_FLAGS_SPLIT})
  file(APPEND ${CMAKE_SOURCE_DIR}/.clang_complete "${flag}\n")
endforeach()
