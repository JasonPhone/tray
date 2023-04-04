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

## System

- [ ] Scene Description File in json with [nlohman json](https://github.com/nlohmann/json)
- [ ] Image Output with [stb_image](https://github.com/nothings/stb)

## Renderer

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
- [ ] Shape
  - [x] Interface
  - [x] Sphere
  - [ ] Triangle Meshes with tiny_obj_loader
- [ ] Primitive (Shape and shading info), Acceleration
  - [x] Interface
  - [x] Primitives
  - [x] Aggregates
  - [x] Plain linear structure
  - [ ] BVH
- [x] Color, Radiometry
  - [x] Interface
  - [x] Try only basic RGB
- [x] Camera
  - [x] Interface
  - [x] Camera Model
  - [x] Perspective Camera with Thin Lens
- [ ] **Sampling**
  - [x] Interfaces
  - [x] Stratified
  - [ ] Halton
  - [ ] (0, 2)-Sequence
  - [ ] Maximized Minimal Distance
  - [ ] Sobol'
  - [ ] Cascaded Sobol'
  - [ ] Matbuilder
  - [x] Film and Filter
- [ ] Reflection, Material
  - [x] Interface
  - [x] Specular
  - [x] Lambertian
  - [ ] Microfacet Models (basis for many material)
  - [ ] BSDF
- [ ] Texture
  - [x] Interface
  - [ ] Checkerboard
- [ ] Light
  - [x] Interface
  - [ ] Distant (Directional) Light
  - [ ] Area Light
- [ ] Integrator: Just basic
- [ ] Surface Reflection: To implement the lights

## GUI

- [ ] Basic Framework
  - [ ] Image Preview
  - [ ] Renderer Control
    - [ ] Start/Stop Rendering
    - [ ] Sampler Choosing
    - [ ] Sampler Configure
    - [ ] Progress Bar
  - [ ] Scene Drscription File I/O
    - [ ] File Dialog with [tiny file dialogs](https://sourceforge.net/projects/tinyfiledialogs/)
  - [ ] Scene Objects Modify
    - [ ] Transformation
    - [ ] Material
  - [ ] Camera Roaming
- [ ] Chinese Font
- [ ] Window Docking with [imguiDock](https://github.com/BentleyBlanks/imguiDock)


## Input file format

`flip_normal` controls if the surface normal of a geometry object is flipped.