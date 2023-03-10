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

- [ ] Geometry, Transformation
  - [ ] Coordinate System
  - [ ] Vector
  - [ ] Point
  - [ ] Normal
  - [ ] Ray
  - [ ] Bounding Box
  - [ ] Transformation
  - [ ] Applying Transformation
  - [ ] Animating Transformation
  - [ ] Interaction
- [ ] Shape
  - [ ] Basic Interface
  - [ ] Sphere
  - [ ] Cylinder
  - [ ] Disk
  - [ ] Triangle Meshes with tiny_obj_loader
  - [ ] Rounding Error
- [ ] Primitive (Shape and shading info), Acceleration
  - [ ] Primitives
  - [ ] Aggregates
  - [ ] BVH
- [ ] Color, Radiometry
  - [ ] Try only basic RGB with that interface
- [ ] Camera
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
  - [ ] Disney BSDF
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

