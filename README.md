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
- [ ] Color, Radiometry
  - [ ] Interface
  - [ ] Try only basic RGB
- [ ] Camera
  - [ ] Interface
  - [ ] Camera Model
  - [ ] Perspective Camera and Thin Lens
- [ ] **Sampling**
  - [ ] Interface
  - [ ] Stratified
  - [ ] Halton
  - [ ] (0, 2)-Sequence
  - [ ] Maximized Minimal Distance
  - [ ] Sobol'
  - [ ] Cascaded Sobol'
  - [ ] Matbuilder
  - [ ] Film and Filter
- [ ] Reflection, Material
  - [ ] Interface
  - [ ] Disney BSDF
- [ ] Texture
  - [ ] Interface
  - [ ] Checkerboard
- [ ] Light
  - [ ] Interface
  - [ ] Point Light
  - [ ] Distant(Directional) Light
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