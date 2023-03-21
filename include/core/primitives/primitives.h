#pragma once
#include "core/TRay.h"
#include "core/primitives/Primitive.h"
#include "core/primitives/GeometricPrimitive.h"
#include "core/primitives/TransformedPrimitive.h"

/**
 * Primitives are the abstraction of geometry, shading and transform.
 * They are created to bound shading info with geometric info, or to
 * reduce the memory cost of instancing only one geometry object at multiple
 * positions.
 *
 */

namespace TRay {}  // namespace TRay