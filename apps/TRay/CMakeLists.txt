cmake_minimum_required(VERSION 3.5.0)

project("TRay"
  LANGUAGES CXX
  DESCRIPTION "This is TRay."
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/)

add_executable(${PROJECT_NAME}
  ${CMAKE_SOURCE_DIR}/src/glad.c
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  "${CMAKE_SOURCE_DIR}/lib/libglfw3.a"
  TRay_geometry
  TRay_shape
  TRay_primitive
  TRay_camera
  TRay_sampler
  TRay_material
  TRay_texture
  TRay_light
  TRay_scene
  TRay_integrator
  TRay_loader
)

# Copy assimp dll into this folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_SOURCE_DIR}/bin/"
  $<TARGET_FILE_DIR:${PROJECT_NAME}>
)