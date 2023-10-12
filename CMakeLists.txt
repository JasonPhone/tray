cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 20)

project(TRay)

# #######################################
# Options.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Float or double.
option(TRAY_FLOAT_AS_DOUBLE "Use double instead of float" ON)

if(TRAY_FLOAT_AS_DOUBLE)
  add_definitions(-D TRAY_FLOAT_AS_DOUBLE)
endif()

# #######################################
# Code checks.
INCLUDE(CheckCXXSourceCompiles)

# INCLUDE (CheckCXXSourceRuns)
# Aligned memory allocation check.
CHECK_CXX_SOURCE_COMPILES(
  " 
  struct alignas(32) Foo { char x; };
  int main() { } 
  "
  HAVE_ALIGNAS)

IF(HAVE_ALIGNAS)
  ADD_DEFINITIONS(-D TRAY_HAVE_ALIGNAS)
ENDIF()

CHECK_CXX_SOURCE_COMPILES(
  "
  #include <malloc.h>
  int main() { void * ptr = _aligned_malloc(1024, 32); }
  "
  HAVE__ALIGNED_MALLOC)
CHECK_CXX_SOURCE_COMPILES(
  "
  #include <malloc.h>
  int main() { void *ptr = memalign(32, 1024); }
  "
  HAVE_MEMALIGN)

IF(HAVE__ALIGNED_MALLOC)
  ADD_DEFINITIONS(-D TRAY_HAVE__ALIGNED_MALLOC)
ELSEIF(HAVE_MEMALIGN)
  ADD_DEFINITIONS(-D TRAY_HAVE_MEMALIGN)
ELSE()
  MESSAGE(SEND_ERROR "No way to allocate aligned memory.")
ENDIF()

# #######################################
# Compiler configure.
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic")
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
add_subdirectory(./extern/file_dialog)
add_subdirectory(./src)
add_subdirectory(./apps)