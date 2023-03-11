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
#include "spdlog/spdlog.h"

#ifdef TRAY_FLOAT_AS_DOUBLE
using Float = double;
#else
using Float = float;
#endif
// Logging and assert warp
#define ASSERT(x) assert((x))
#define SINFO(msg) spdlog::info(msg)
#define FINFO(fmt, msg) spdlog::info(fmt, msg)
#define SWARN(msg) spdlog::WARN(msg)
#define FWARN(fmt, msg) spdlog::WARN(fmt, msg)
#define SERROR(msg) spdlog::error(msg)
#define FERROR(fmt, msg) spdlog::error(fmt, msg)
#define SCRITICAL(msg) spdlog::critical(msg)
#define FCRITICAL(fmt, msg) spdlog::critical(fmt, msg)

namespace TRay {
}  // namespace TRay