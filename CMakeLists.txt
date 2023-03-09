cmake_minimum_required(VERSION 3.5.0)

project(learn-opengl)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
# set(CMAKE_BUILD_RPATH ${CMAKE_SOURCE_DIR}/bin/)
# set(CMAKE_INSTALL_RPATH ${CMAKE_SOURCE_DIR}/bin/)

set(3DPARTY_FOLDER ./extern)
# Fetch template
# --------------
# include(FetchContent)
# Set(FETCHCONTENT_QUIET FALSE)
# if(NOT TARGET <target>)
#   message(STATUS "============start config <target>============")
#   FetchContent_Declare(
#     <target>
#     GIT_REPOSITORY https://gitclone.com/github.com/...
#     GIT_TAG v0.0.0

#     GIT_PROGRESS TRUE
#     SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/<target>
#   )

#   set(ASSIMP_FOLDER ${3DPARTY_FOLDER}/assimp)
#   set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE INTERNAL "")
#   set(ASSIMP_DOUBLE_PRECISION ON CACHE INTERNAL "")
#   set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE INTERNAL "")
#   set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "")
#   set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "")
#   set(ASSIMP_INSTALL OFF CACHE INTERNAL "")

#   FetchContent_MakeAvailable(<target>)

#   set_target_properties(<target> PROPERTIES DEBUG_POSTFIX "_debug")
#   set_target_properties(<target> PROPERTIES RELEASE_POSTFIX "_release")
# endif()

# OpenGL
# find_package(OpenGL REQUIRED)

include_directories(
  ./extern
  ./include
  ./src
)

add_subdirectory(./src)
add_subdirectory(./apps)
