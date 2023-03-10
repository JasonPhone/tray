set(HEADER_DIR "${CMAKE_SOURCE_DIR}/include")
set(EXTERN_DIR "${CMAKE_SOURCE_DIR}/extern")

add_library(stb_image
  STATIC
  ${EXTERN_DIR}/stb_image.h
  stb_image.cpp
)

add_library(tiny_obj_loader
  STATIC
  ${EXTERN_DIR}/tiny_obj_loader.h
  tiny_obj_loader.cpp
)
