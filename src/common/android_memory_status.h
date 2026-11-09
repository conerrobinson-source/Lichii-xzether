// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <charconv>
#include <fstream>
#include <string>
#include <string_view>

#include <fmt/format.h>

#include "common/common_types.h"

namespace Common {

/// "rss=A MiB swap=B MiB | avail=C MiB swap_free=D MiB" from /proc/self/status and /proc/meminfo:
/// where the process stands against the low-memory killer.
inline std::string AndroidMemoryStatus() {
    // Line by line: /proc/self/status starts with "Name:\t<process>", which no number parse
    // survives.
    const auto field = [](const char* path, std::string_view name) -> u64 {
        std::ifstream file{path};
        std::string line;
        while (std::getline(file, line)) {
            if (!line.starts_with(name) || line.size() <= name.size() || line[name.size()] != ':') {
                continue;
            }
            u64 kib{};
            const auto* begin = line.data() + name.size() + 1;
            const auto* end = line.data() + line.size();
            while (begin < end && (*begin == ' ' || *begin == '\t')) {
                ++begin;
            }
            std::from_chars(begin, end, kib);
            return kib >> 10;
        }
        return 0;
    };
    return fmt::format("rss={} MiB swap={} MiB | avail={} MiB swap_free={} MiB",
                       field("/proc/self/status", "VmRSS"), field("/proc/self/status", "VmSwap"),
                       field("/proc/meminfo", "MemAvailable"), field("/proc/meminfo", "SwapFree"));
}

} // namespace Common
