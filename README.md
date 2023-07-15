# TRay

TRay for "yet another Ray Tracer".

# Project Structure

```text
+---.vscode
+---apps
|   +---test
|   |   \---bin
|   \---TRay
|       \---bin
+---bin
+---build
+---extern
|   +---glad
|   +---GLFW
|   +---ImGui
|   |   \---backend
|   \---KHR
+---include
|   +---core
|   \---gui
+---lib
+---others
\---src
    +---core
    \---gui
```

# Features

## IO

- Scene Description File in json with [nlohman json](https://github.com/nlohmann/json)
- Image Output with [stb_image](https://github.com/nothings/stb)

## TODO

- [x] Geometry, Transformation
  - [x] Coordinate System
  - [x] Vector
  - [x] Point
  - [x] Normal
  - [x] Ray
  - [x] Bounding Box
  - [x] Transformation
  - [x] Applying Transformation
  - [x] Animating Transformation
  - [x] Interaction
- [x] Shape
  - [x] Interface
  - [x] Sphere
  - [x] Triangle Meshes with tiny_obj_loader
- [ ] Primitive (Shape and shading info), Acceleration
  - [x] Interface
  - [ ] Primitives
    - [x] Geometric Primitive
    - [ ] Transformed Primitive
    - [ ] ...
  - [ ] Aggregates
    - [x] Plain linear structure
    - [ ] BVH
    - [ ] ...
- [ ] Color, Radiometry
  - [x] Interface
  - [ ] Color Representation
    - [x] RGB Tuple
    - [ ] XYZ
    - [ ] ...
- [ ] Camera
  - [x] Interface
  - [x] Camera Model
  - [ ] Cameras
    - [x] Perspective Camera with Thin Lens
    - [ ] Environment Camera
    - [ ] Realistic Camera
    - [ ] ...
- [ ] Sampling
  - [x] Interfaces
  - [ ] Samplers
    - [x] Stratified
    - [x] Halton
    - [x] (0, 2)-Sequence
    - [x] Maximized Minimal Distance
    - [x] Sobol'
    - [ ] Cascaded Sobol' Sampling
    - [ ] MatBuilder
  - [x] Film and Filter
  - [x] JPEG and uint_8 Output Support
- [ ] Reflection, Material
  - [x] Interface
  - [ ] Reflections
    - [x] Specular
    - [x] Lambertian
    - [ ] Microfacet Models
      - [x] Oren–Nayar Diffuse Reflection
      - [ ] ...
  - [x] BSDF
- [ ] Texture
  - [x] Interface
  - [ ] Textures
    - [x] Checkerboard
    - [ ] ...
  - [ ] Noise
- [ ] Light
  - [x] Interface
  - [ ] Light Sources
    - [x] Distant (Directional) Light
    - [x] Area Light
    - [ ] Infinite Area Light
    - [ ] Point Light
    - [ ] ...
- [ ] Integrator
  - [x] Whitted
  - [x] Direct Lighting
  - [x] Basic Path Tracing
  - [ ] ...
- [ ] System
  - [ ] Memory Pool
  - [ ] Better UI

## GUI

- [ ] Basic Framework
  - [ ] Image Preview
  - [ ] Renderer Control
    - [ ] Start/Stop Rendering
  - [ ] Scene Drscription File I/O
    - [ ] File Dialog with [tiny file dialogs](https://sourceforge.net/projects/tinyfiledialogs/)

## Others

Design: Film + Filter = Sensor?