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

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

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

  set(SDL2_PATH ${CMAKE_SOURCE_DIR}/external/SDL2/SDL2)
  set(SDL2_IMAGE_PATH ${CMAKE_SOURCE_DIR}/external/SDL2/SDL2_image)

  find_package(OpenGL)
  find_package(SDL2)
  find_package(SDL2_image)

  if (WIN32)
    set(GLEW_LOCATION  ${CMAKE_SOURCE_DIR}/external/glew)
    find_package(GLEW)
  endif()

# Include SDL2 libraries for android
else()
  string(TOLOWER ${CMAKE_BUILD_TYPE} ANDROID_BUILD_DIR)
  set(DISTRIBUTION_DIR ${CMAKE_SOURCE_DIR}/android/distribution/android/SDL2/intermediates/ndkBuild)
  set(SDL_LOCATION ${CMAKE_SOURCE_DIR}/external/SDL2)

  set(SDL2_INCLUDE_DIR ${SDL_LOCATION}/SDL2/include)
  set(SDL2_IMAGE_INCLUDE_DIRS ${SDL_LOCATION}/SDL2_image)

  set(SDL2_LIBRARY SDL2)
  set(SDL2_IMAGE_LIBRARIES SDL2_image)

  # For SDL2pp
  set(SDL2_IMAGE_INCLUDE_DIR ${SDL2_IMAGE_INCLUDE_DIRS})
  set(SDL2_IMAGE_LIBRARY ${SDL2_IMAGE_LIBRARIES})

  # set(SDL2MAIN_LIBRARY ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2.so)

  add_library( ${SDL2_LIBRARY} SHARED IMPORTED )
  add_library( ${SDL2_IMAGE_LIBRARIES} SHARED IMPORTED )
  set_target_properties(${SDL2_LIBRARY} PROPERTIES IMPORTED_LOCATION
  ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2.so)
  set_target_properties(${SDL2_IMAGE_LIBRARIES} PROPERTIES IMPORTED_LOCATION
  ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL2_image.so)


  # If SDL2 or SDL2_image was not found, the build would have crashed before
  set(SDL2_FOUND TRUE)
  set(SDL2_IMAGE_FOUND TRUE)
  set(OPENGL_FOUND TRUE) # TODO Check for GLESv3

endif()

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


set(SDL2PP_WITH_IMAGE ON)
add_subdirectory(external/libSDL2pp)

# If they are still not found, abort
if ( NOT(OPENGL_FOUND AND SDL2_FOUND AND SDL2_IMAGE_FOUND))

  message( FATAL_ERROR "Some of the dependencies are not found, CMake will exit." )

endif()


set(INCLUDE_DIRS
  ${OPENGL_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/external/glm
  ${GLEW_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIR}
  ${SDL2_IMAGE_INCLUDE_DIRS}
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
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2PP_LIBRARIES}
  )

else()
  add_executable(${PROJECT_NAME} ${SOURCE_FILES})

  if (WIN32)
    target_link_libraries(${PROJECT_NAME}
      ${OPENGL_LIBRARIES}
      ${GLEW_LIBRARIES}
      ${SDL2_LIBRARY}
      ${SDL2_IMAGE_LIBRARIES}
      ${SDL2PP_LIBRARIES}
    )
  else()
    target_link_libraries(${PROJECT_NAME}
      ${OPENGL_LIBRARIES}
      ${SDL2_LIBRARY}
      ${SDL2_IMAGE_LIBRARIES}
      ${SDL2PP_LIBRARIES}
    )
  endif()
endif()

# Generate file for clang-autocomplete and clang-linter in atom

message(STATUS "Generarating ${CMAKE_SOURCE_DIR}/.clang_complete")
file(WRITE ${CMAKE_SOURCE_DIR}/.clang_complete "")
foreach(dir ${INCLUDE_DIRS})
  file(APPEND ${CMAKE_SOURCE_DIR}/.clang_complete "-I${dir}\n")
endforeach()
string(REPLACE "'" "" CMAKE_CXX_FLAGS_SPLIT ${CMAKE_CXX_FLAGS})
string(REPLACE " " ";" CMAKE_CXX_FLAGS_SPLIT ${CMAKE_CXX_FLAGS_SPLIT})
foreach(flag ${CMAKE_CXX_FLAGS_SPLIT})
  file(APPEND ${CMAKE_SOURCE_DIR}/.clang_complete "${flag}\n")
endforeach()

# At the end we need to copy the dlls to the output directory

if (WIN32)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${GLEW_LOCATION}/bin/glew32.dll"
    "${SDL2_PATH}/bin/SDL2.dll"
    "${SDL2_IMAGE_PATH}/bin/SDL2_image.dll"
    "${SDL2_IMAGE_PATH}/bin/libjpeg-9.dll"
    "${SDL2_IMAGE_PATH}/bin/libpng16-16.dll"
    "${SDL2_IMAGE_PATH}/bin/libtiff-5.dll"
    "${SDL2_IMAGE_PATH}/bin/libwebp-4.dll"
    "${SDL2_IMAGE_PATH}/bin/zlib1.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
  )
endif()
