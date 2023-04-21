#pragma once
#include <string>
namespace TRay {
namespace Key {
// Common.
const std::string Name = "name";
const std::string Type = "type";
const std::string Value = "value";
const std::string Transform = "transform";
// Transforms
const std::string Transforms = "transforms";
const std::string Sequence = "sequence";
const std::string Position = "position";
const std::string Look = "look";
const std::string Up = "up";
const std::string Theta = "theta";
const std::string Axis = "axis";
const std::string Shape = "shape";
// Colors.
const std::string Colors = "colors";
// Textures.
const std::string Textures = "textures";
const std::string Return = "return";
// Materials.
const std::string Materials = "materials";
const std::string Diffuse = "diffuse";
const std::string Sigma = "sigma";
// Shapes.
const std::string Shapes = "shapes";
const std::string FlipNormal = "flip_normal";
const std::string Radius = "radius";
const std::string Vertex = "vertex";
const std::string Index = "index";
const std::string File = "file";
// Lights.
const std::string Lights = "lights";
const std::string Emit = "emit";
const std::string NSamples = "n_samples";
// Primitives.
const std::string Primitives = "primitives";
const std::string Material = "material";
const std::string Light = "light";
// Accelerator.
const std::string Accelerator = "accelerator";
// Camera.
const std::string Camera = "camera";
const std::string Screen = "screen";
const std::string ShutterTime = "shutter_time";
const std::string LensRadius = "lens_radius";
const std::string FocalDistance = "focal_distance";
const std::string FOV = "fov";
const std::string Film = "film";
const std::string Resolution = "resolution";
const std::string Crop = "crop";
const std::string Filter = "filter";
const std::string FilterRadius = "filter_radius";
// Sampler.
const std::string Sampler = "sampler";
const std::string SamplePerPixel = "sample_per_pixel";
const std::string SampleDimension = "sample_dimension";
const std::string Jitter = "jitter";
// Integrator
const std::string Integrator = "integrator";
const std::string MaxDepth = "max_depth";

}  // namespace Key

namespace Val {
// Common.
// Transforms
const std::string Translate = "translate";
const std::string Rotate = "rotate";
const std::string Scale = "scale";
const std::string LookAt = "look_at";
// Colors.
const std::string RGB = "RGB";
// Textures.
const std::string Constant = "constant";
const std::string Spectrum = "spectrum";
const std::string Float = "float";
// Materials.
const std::string MatteMaterial = "matte";
// Shapes.
const std::string Sphere = "sphere";
const std::string MeshPlain = "mesh_plain";
const std::string MeshObj = "mesh_obj";
// Lights.
const std::string DiffuseArea = "diffuse_area";
// Primitives.
const std::string Geometric = "geometric";
// Accelerator.
const std::string LinearAccel = "linear";
// Camera.
const std::string PerspectiveCamera = "perspective";
const std::string BoxFilter = "box";
const std::string NoFilter = "no";
// Sampler.
const std::string StratifiedSampler = "stratified";
// Integrator
const std::string PathIntegrator = "path";

}  // namespace Val
}  // namespace TRay