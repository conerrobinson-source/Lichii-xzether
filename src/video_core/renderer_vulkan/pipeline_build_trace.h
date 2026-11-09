// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdio>
#include <cstdlib>
#include <functional>
#include <mutex>
#include <span>
#include <string>
#include <string_view>
#include <thread>

#include <fmt/format.h>

#include "common/common_types.h"
#include "common/logging.h"

namespace Vulkan {

/// EDEN_NX2_PIPELINE_BUILD_TRACE=<file>: appends a line before and after every driver pipeline
/// build, flushed at once, so the last BEGIN without an END names the shaders a driver crashed on.
/// END lines carry the build's wall time (ms=), which includes time spent waiting on other builds.
/// =log writes the lines to the emulator log instead (with flush_line on, each reaches the file).
inline void TracePipelineBuild(const char* phase, const char* kind, u64 key_hash,
                               std::span<const u64> stage_hashes, bool unoptimized,
                               double elapsed_ms = -1.0) {
    static const char* const path = [] {
        const char* const value = std::getenv("EDEN_NX2_PIPELINE_BUILD_TRACE");
        return value != nullptr && *value != '\0' ? value : nullptr;
    }();
    if (path == nullptr) {
        return;
    }
    static const bool to_log = std::string_view{path} == "log";
    static std::FILE* const file = to_log ? nullptr : std::fopen(path, "a");
    if (!to_log && file == nullptr) {
        return;
    }
    std::string line = fmt::format("{} {} pipeline={:016x} thread={}{}", phase, kind, key_hash,
                                   std::hash<std::thread::id>{}(std::this_thread::get_id()),
                                   unoptimized ? " unoptimized" : "");
    for (size_t stage = 0; stage < stage_hashes.size(); ++stage) {
        if (stage_hashes[stage] != 0) {
            line += fmt::format(" s{}:{:016x}", stage, stage_hashes[stage]);
        }
    }
    if (elapsed_ms >= 0.0) {
        line += fmt::format(" ms={:.1f}", elapsed_ms);
    }
    if (to_log) {
        LOG_INFO(Render_Vulkan, "Pipeline build {}", line);
        return;
    }
    static std::mutex mutex;
    std::scoped_lock lock{mutex};
    std::fputs(line.c_str(), file);
    std::fputc('\n', file);
    std::fflush(file);
}

} // namespace Vulkan
