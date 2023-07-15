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

- Scene Description File in json with [nlohman json](https://github.com/nlohmann/json)
- Image Output with [stb_image](https://github.com/nothings/stb)

## Renderer

- Geometry, Transformation
  - Coordinate System
  - Vector
  - Point
  - Normal
  - Ray
  - Bounding Box
  - Transformation
  - Applying Transformation
  - Animating Transformation
  - Interaction
- Shape
  - Interface
  - Sphere
  - Triangle Meshes with tiny_obj_loader
- Primitive (Shape and shading info), Acceleration
  - Interface
  - Primitives
  - Aggregates
  - Plain linear structure
- Color, Radiometry
  - Interface
  - Try only basic RGB
- Camera
  - Interface
  - Camera Model
  - Perspective Camera with Thin Lens
- **Sampling**
  - Interfaces
  - Stratified
  - Halton
  - (0, 2)-Sequence
  - Maximized Minimal Distance
  - Sobol'
  - Film and Filter
  - JPEG and uint_8 Output Support
- Reflection, Material
  - Interface
  - Specular
  - Lambertian
  - Microfacet Models (basis for many material)
  - BSDF
- Texture
  - Interface
  - Checkerboard
- Light
  - Interface
  - Distant (Directional) Light
  - Area Light
- Integrator
  - Whitted
  - Direct Lighting
  - Basic Path Tracing

## GUI

- Basic Framework
  - Image Preview
  - Renderer Control
    - Start/Stop Rendering
  - Scene Drscription File I/O
    - File Dialog with [tiny file dialogs](https://sourceforge.net/projects/tinyfiledialogs/)

## Todo

- Memory Pool
- BVH
- Cascaded Sobol' Sampling
- MatBuilder
- Film + Filter = Sensor?