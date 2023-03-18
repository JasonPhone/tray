#include "core/geometry/Ray.h"

namespace TRay {
Point3f Ray::operator()(Float t) const { return ori + t * dir; }
}  // namespace TRay