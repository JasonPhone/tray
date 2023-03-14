/**
 * @file tray.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Global includes, defines, consts, inlines and types.
 * @version 0.1
 * @date 2023-03-11
 */
#pragma once
#include <iostream>
#include <cmath>
#include <utility>
#include <assert.h>

#ifdef TRAY_FLOAT_AS_DOUBLE
using Float = double;
#else
using Float = float;
#endif

// Logging and assert warp
#define ASSERT(x) assert((x))
// For compiling speed.
#ifndef NDEBUG
#include "spdlog/spdlog.h"
#define SINFO(msg) spdlog::info(msg)
#define FINFO(fmt, msg) spdlog::info(fmt, msg)
#define SWARN(msg) spdlog::WARN(msg)
#define FWARN(fmt, msg) spdlog::WARN(fmt, msg)
#define SERROR(msg) spdlog::error(msg)
#define FERROR(fmt, msg) spdlog::error(fmt, msg)
#define SCRITICAL(msg) spdlog::critical(msg)
#define FCRITICAL(fmt, msg) spdlog::critical(fmt, msg)
#else
#define SINFO(msg)
#define FINFO(fmt, msg)
#define SWARN(msg)
#define FWARN(fmt, msg)
#define SERROR(msg)
#define FERROR(fmt, msg)
#define SCRITICAL(msg)
#define FCRITICAL(fmt, msg)
#endif

namespace TRay {
// geometry.h
template <typename T>
class Vector3;
template <typename T>
class Vector2;
template <typename T>
class Point3;
template <typename T>
class Point2;
template <typename T>
class Normal3;
// Math constants.
static constexpr Float FLOAT_MAX = std::numeric_limits<Float>::max();
static constexpr Float FLOAT_INF = std::numeric_limits<Float>::infinity();
}  // namespace TRay