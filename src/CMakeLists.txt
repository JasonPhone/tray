set(HEADER_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(EXTERN_DIR "${CMAKE_SOURCE_DIR}/extern")

add_library(stb_image
  STATIC
  # ${EXTERN_DIR}/stb_image.h
  stb_image.cpp
)

add_library(tiny_obj_loader
  STATIC
  # ${EXTERN_DIR}/tiny_obj_loader.h
  tiny_obj_loader.cpp
)

add_library(TRay_geometry
  STATIC
  ${SOURCE_DIR}/core/geometry/AnimateTransform.cpp
  ${SOURCE_DIR}/core/geometry/Transform.cpp
  ${SOURCE_DIR}/core/geometry/Interaction.cpp
  ${SOURCE_DIR}/core/geometry/Ray.cpp
  ${SOURCE_DIR}/core/geometry/Bound.cpp
  ${SOURCE_DIR}/core/geometry/Quaternion.cpp
  ${SOURCE_DIR}/core/math/Matrix.cpp
)
add_library(TRay_shapes
  STATIC
  ${SOURCE_DIR}/core/geometry/Interaction.cpp
  ${SOURCE_DIR}/core/geometry/Ray.cpp
  # ${SOURCE_DIR}/core/geometry/Bound.cpp
  ${SOURCE_DIR}/shapes/Sphere.cpp
)