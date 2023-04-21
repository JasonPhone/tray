#pragma once
#include "core/TRay.h"

namespace TRay {

bool load_triangle_mesh(const Transform& obj_to_world, bool flip_normal,
                        std::vector<std::shared_ptr<Shape>>* triangles,
                        const char* filename, const char* basepath = NULL);
}  // namespace TRay