set(HEADER_DIR "${CMAKE_SOURCE_DIR}/include")

add_library(stb_image
  SHARED
  ${HEADER_DIR}/stb_image.h
  stb_image.cpp
)

add_library(tiny_obj_loader
  SHARED
  ${HEADER_DIR}/tiny_obj_loader.h
  tiny_obj_loader.cpp
)
