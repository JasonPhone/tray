cmake_minimum_required(VERSION 3.5.0)

project(TRay)

# Options
# -------
option(TRAY_FLOAT_AS_DOUBLE "Use double instead of float" ON)

if(TRAY_FLOAT_AS_DOUBLE)
  add_definitions(-D TRAY_FLOAT_AS_DOUBLE)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)

# set(CMAKE_BUILD_RPATH ${CMAKE_SOURCE_DIR}/bin/)
# set(CMAKE_INSTALL_RPATH ${CMAKE_SOURCE_DIR}/bin/)
include_directories(
  ./extern
  ./include
  ./src
)

add_subdirectory(./extern/ImGui)
add_subdirectory(./src)
add_subdirectory(./apps)