// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <string>

#include "common/logging.h"
#include "core/core.h"
#include "core/hle/kernel/k_process.h"
#include "core/hle/kernel/k_thread.h"
#include "core/hle/kernel/kernel.h"
#include "core/hle/kernel/svc.h"
#include "core/hle/kernel/svc/svc_nx2_oot_poll.h"

namespace Kernel::Svc {
namespace {

// Ocarina of Time (main build E0F3CCFA...), from zortos293/Lichii#641: the resource request
// thread (main+0xF7A0, state 3) walks its pending requests (a name lookup through the query
// engine at main+0x58EEA0, a mutex and status checks per request) and, if any is not ready yet,
// starts the walk again at once with no wait. It only ever polls, so sleeping before the next walk
// keeps every outcome and just lowers the rate; the game's own idle wait in the same loop is 16 ms.
constexpr u64 OotProgramId = 0x0400486024DEC000ULL;
constexpr u32 PollSvc = 0xF3;
constexpr s64 DefaultBackoffUs = 500;
constexpr s64 MaxBackoffUs = 16000;

// EDEN_NX2_OOT_POLL_BACKOFF_US (read once): microseconds the resource request thread sleeps
// before rescanning requests that are not ready. Unset or 1 = 500 us; 0 = no sleep (as without
// the patch, plus one SVC per rescan); otherwise that many us, at most 16000.
s64 BackoffUs() {
    static const s64 us = [] {
        const char* value = std::getenv("EDEN_NX2_OOT_POLL_BACKOFF_US");
        if (value == nullptr || *value == '\0') {
            return DefaultBackoffUs;
        }
        const s64 parsed = std::strtoll(value, nullptr, 10);
        return parsed <= 0 ? s64{0}
               : parsed == 1 ? DefaultBackoffUs
                             : (std::min)(parsed, MaxBackoffUs);
    }();
    return us;
}

} // Anonymous namespace

bool CallNx2OotPollBackoff(Core::System& system, u32 imm) {
    if (imm != PollSvc) {
        return false;
    }
    auto& process = GetCurrentProcess(system.Kernel());
    if (!process.Is64Bit() || process.GetProgramId() != OotProgramId) {
        return false;
    }
    const s64 us = BackoffUs();
    static std::atomic<u64> calls{};
    const u64 n = calls.fetch_add(1, std::memory_order_relaxed) + 1;
    if (n == 1 || n % 100000 == 0) {
        LOG_INFO(Kernel_SVC, "OoT resource poll backoff: {} rescans, {} us each", n,
                 us);
    }
    if (us > 0) {
        SleepThread(system, us * 1000);
    }
    return true;
}

} // namespace Kernel::Svc
