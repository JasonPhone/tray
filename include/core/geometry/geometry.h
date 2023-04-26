/**
 * @file geometry.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Geometry and maths basis for TRay.
 * @version 0.1
 * @date 2023-03-11
 */

/**
 * In 3D spaces, the origin and three basis vectors are called "frame".
 * They define a coordinate system.
 *
 * Vectors express the direction and distance,
 * while points need the origin to express the position.
 * So vector and points are different, in spite of the
 * same presentation.
 *
 * Coordinate frame and points, vectors rely on each other
 * to be meaningful, so we specify a standard "world space" frame
 * which has (0, 0, 0) as origin and (1, 0, 0), (0, 1, 0) and (0, 0, 1)
 * as basis vectors.
 * All other frames are relative to world space.
 *
 * The code impl could be optimized using a 3-tuple and inheritance
 * to reduce the similar computation. But we want to make the difference clear.
 */

// ------------------
//   Handness: left
// ------------------
#pragma once
#include "core/TRay.h"
#include "core/geometry/Bound.h"
#include "core/geometry/Interaction.h"
#include "core/geometry/Normal.h"
#include "core/geometry/Point.h"
#include "core/geometry/Ray.h"
#include "core/geometry/Shape.h"
#include "core/geometry/Transform.h"
#include "core/geometry/Vector.h"
#include "core/geometry/precision.h"

namespace TRay {
}  // namespace TRay