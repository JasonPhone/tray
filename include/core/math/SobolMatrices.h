#pragma once
#include "core/TRay.h"

namespace TRay {
static constexpr int NSobolDimensions = 1024;
static constexpr int SobolMatrixSize = 52;

extern const uint32_t SobolMatrices32[NSobolDimensions * SobolMatrixSize];
extern const uint64_t SobolMatrices64[NSobolDimensions * SobolMatrixSize];

extern const uint64_t VDCSobolMatrices[][SobolMatrixSize];
extern const uint64_t VDCSobolMatricesInv[][SobolMatrixSize];

}  // namespace TRay