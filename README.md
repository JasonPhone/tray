# TRay

TRay for "yet another Ray Tracer".

Served as my bachelor's undergraduate graduation project, now as a playground for leaning rendering.

# Build and Run

TRay is OK with clang 15.0.7. Some builtin function may fail MSVC.

`cd build` and `build ..` should be ok. The required libs should be libglfw.a only, for supporting ImGUI and some other features based on OpenGL.

I dislike build systems, so any related problems please work it out by yourself. I am trying to make it OK on most platforms, by writing (for me) most portable codes.

# Project Structure

```text
+---.vscode
+---apps
|   +---sample_images
|   +---test
|   |   \---bin
|   \---TRay
|       \---bin
+---build
+---extern
|   +---file_dialog
|   +---glad
|   +---GLFW
|   +---ImGui
|   +---KHR
|   \---nlohmann
+---include
|   +---accelerators
|   +---cameras
|   +---core
|   +---filters
|   +---gui
|   +---integrators
|   +---lights
|   +---loaders
|   +---materials
|   +---samplers
|   +---shapes
|   \---textures
+---lib
\---src
    +---accelerators
    +---cameras
    +---core
    +---filters
    +---gui
    +---integrators
    +---lights
    +---loaders
    +---materials
    +---samplers
    \---shapes
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
  - [ ] Parallelism
  - [x] Statistics (Counter only)
  - [ ] Memory Pool ([This?](https://github.com/microsoft/mimalloc))
  - [ ] Better UI

## GUI

- [ ] Basic Framework
  - [x] Image Preview
  - [x] Render Control
    - [x] Start/Pause Rendering
  - [x] Scene Description File I/O
    - [x] Scene File Load/Reload
    - [x] File Dialog with [tiny file dialogs](https://sourceforge.net/projects/tinyfiledialogs/)

## Others

Design: Film + Filter = Sensor?

## TODO

https://www.pbr-book.org/3ed-2018/Utilities/Parallelism#ParallelForLoops