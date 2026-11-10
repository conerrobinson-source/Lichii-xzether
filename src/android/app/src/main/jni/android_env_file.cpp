// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// Android has no way to pass environment variables to an app, so the EDEN_* diagnostic switches
// are read from <external files dir>/env.txt, one KEY=VALUE per line ('#' starts a comment).
// Several switches are read by static initializers, so this runs before them.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <android/log.h>

namespace {

std::string PackageName() {
    char name[256]{};
    if (FILE* file = std::fopen("/proc/self/cmdline", "rb")) {
        const size_t n = std::fread(name, 1, sizeof(name) - 1, file);
        name[n] = '\0';
        std::fclose(file);
    }
    // A process of the app may carry a ":service" suffix.
    if (char* colon = std::strchr(name, ':')) {
        *colon = '\0';
    }
    return name;
}

std::string Trim(const std::string& s) {
    const size_t begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return {};
    }
    const size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

// s26-ultra test builds: diagnostics on without env.txt, which the tester can't reach.
// env.txt still overrides each (KEY=0 turns it off). Do not merge to nx2.
// The frame trace and GPU profile stay on: they are what shows where a slow game's frame time
// goes (per-frame GPU time by shader, CPU per guest core), and their cost is a few timestamp
// queries per frame and ~15 log lines a second. Pipeline statistics (driver statistics for every
// pipeline) are off; KEY=1 in env.txt turns them on.
void SetTestBuildDefaults() {
    constexpr const char* Defaults[][2] = {
        {"EDEN_NCE_FAULT_TRACE", "1"},
        {"EDEN_DKB_FRAME_TRACE", "1"},
        {"EDEN_NX2_GPU_PROFILE", "1"},
        // Into the log, flushed per line: the last BEGIN without an END names the shaders the
        // driver crashed on (Ocarina of Time on the Qualcomm driver dies on a shader).
        {"EDEN_NX2_PIPELINE_BUILD_TRACE", "log"},
        // Ocarina of Time (Fritchii #641 ports): let its multi-core threads use idle cores, and
        // sleep 500 us in the resource thread's busy rescan. KEY=0 in env.txt turns either off;
        // EDEN_NX2_OOT_POLL_BACKOFF_US=<us> changes the sleep (0 = none).
        // Every 2 s: what the texture cache, staging buffers, buffer cache and VMA hold. Ocarina
        // of Time is killed for memory on a 16 GB phone; this says where the 3 GiB of device
        // memory is.
        {"EDEN_NX2_MEM_TRACE", "1"},
        {"EDEN_NX2_OOT_WIDE_AFFINITY", "1"},
        {"EDEN_NX2_OOT_POLL_BACKOFF", "1"},
    };
    for (const auto& [key, value] : Defaults) {
        if (std::getenv(key) == nullptr) {
            setenv(key, value, 0);
            __android_log_print(ANDROID_LOG_WARN, "YuzuNative", "test build default: %s=%s", key,
                                value);
        }
    }
}

__attribute__((constructor(101))) void LoadEnvFile() {
    const std::string package = PackageName();
    if (package.empty()) {
        SetTestBuildDefaults();
        return;
    }
    const std::string path = "/storage/emulated/0/Android/data/" + package + "/files/env.txt";
    FILE* file = std::fopen(path.c_str(), "r");
    if (file == nullptr) {
        SetTestBuildDefaults();
        return;
    }
    char buffer[1024];
    while (std::fgets(buffer, sizeof(buffer), file) != nullptr) {
        std::string line = Trim(buffer);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        const size_t eq = line.find('=');
        if (eq == std::string::npos || eq == 0) {
            continue;
        }
        const std::string key = Trim(line.substr(0, eq));
        const std::string value = Trim(line.substr(eq + 1));
        setenv(key.c_str(), value.c_str(), 1);
        __android_log_print(ANDROID_LOG_WARN, "YuzuNative", "env.txt: %s=%s", key.c_str(),
                            value.c_str());
    }
    std::fclose(file);
    SetTestBuildDefaults();
}

} // namespace
