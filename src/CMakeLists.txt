set(HEADER_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(EXTERN_DIR "${CMAKE_SOURCE_DIR}/extern")

add_library(TRay_geometry
  STATIC
  ${SOURCE_DIR}/core/math/Matrix.cpp
  ${SOURCE_DIR}/core/geometry/Transform.cpp
  ${SOURCE_DIR}/core/geometry/Interaction.cpp
  ${SOURCE_DIR}/core/geometry/Ray.cpp
  ${SOURCE_DIR}/core/geometry/Bound.cpp
  ${SOURCE_DIR}/core/geometry/Quaternion.cpp
)
add_library(TRay_shape
  STATIC
  ${SOURCE_DIR}/core/geometry/Interaction.cpp
  ${SOURCE_DIR}/core/geometry/Ray.cpp

  ${SOURCE_DIR}/shapes/Sphere.cpp
  ${SOURCE_DIR}/shapes/TriangleMesh.cpp
)
add_library(TRay_primitive
  STATIC
  ${SOURCE_DIR}/core/primitives/GeometricPrimitive.cpp
  ${SOURCE_DIR}/core/primitives/Aggregate.cpp
  ${SOURCE_DIR}/accelerators/LinearAccel.cpp
)
add_library(TRay_camera
  STATIC
  ${SOURCE_DIR}/filters/BoxFilter.cpp
  ${SOURCE_DIR}/core/Camera.cpp
  ${SOURCE_DIR}/core/geometry/Bound.cpp
  ${SOURCE_DIR}/core/Film.cpp
  ${SOURCE_DIR}/cameras/PerspectiveCamera.cpp
)
add_library(TRay_sampler
  STATIC
  ${SOURCE_DIR}/core/Sampler.cpp
  ${SOURCE_DIR}/core/math/lowdiscrepancy.cpp

  ${SOURCE_DIR}/samplers/StratifiedSampler.cpp
  ${SOURCE_DIR}/samplers/HaltonSampler.cpp
)
add_library(TRay_material
  STATIC
  ${SOURCE_DIR}/core/math/sampling.cpp
  ${SOURCE_DIR}/core/reflection/BxDF.cpp
  ${SOURCE_DIR}/core/reflection/Fresnel.cpp
  ${SOURCE_DIR}/core/reflection/FresnelSpecular.cpp
  ${SOURCE_DIR}/core/reflection/Lambertian.cpp
  ${SOURCE_DIR}/core/reflection/BSDF.cpp
  ${SOURCE_DIR}/core/reflection/OrenNayar.cpp

  ${SOURCE_DIR}/materials/MatteMaterial.cpp
  ${SOURCE_DIR}/materials/PlasticMaterial.cpp
  ${SOURCE_DIR}/materials/MirrorMaterial.cpp
)
add_library(TRay_texture
  STATIC
  ${SOURCE_DIR}/core/Texture.cpp
)
add_library(TRay_light
  STATIC
  ${SOURCE_DIR}/core/Light.cpp
  ${SOURCE_DIR}/lights/DistantLight.cpp
  ${SOURCE_DIR}/lights/DiffuseAreaLight.cpp
)
add_library(TRay_scene
  STATIC
  ${SOURCE_DIR}/core/Scene.cpp
)
add_library(TRay_integrator
  STATIC
  ${SOURCE_DIR}/core/geometry/Bound.cpp
  ${SOURCE_DIR}/core/Integrator.cpp

  ${SOURCE_DIR}/integrators/WhittedIntegrator.cpp
  ${SOURCE_DIR}/integrators/DirectIntegrator.cpp
  ${SOURCE_DIR}/integrators/PathIntegrator.cpp
)
add_library(TRay_loader
  STATIC
  ${SOURCE_DIR}/filters/BoxFilter.cpp

  ${SOURCE_DIR}/integrators/PathIntegrator.cpp

  ${SOURCE_DIR}/cameras/PerspectiveCamera.cpp

  ${SOURCE_DIR}/shapes/Sphere.cpp
  ${SOURCE_DIR}/shapes/TriangleMesh.cpp

  ${SOURCE_DIR}/loaders/meshloading.cpp
  ${SOURCE_DIR}/loaders/SceneLoader.cpp
)