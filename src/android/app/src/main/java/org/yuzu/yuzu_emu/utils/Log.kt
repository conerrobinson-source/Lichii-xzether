// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// SPDX-FileCopyrightText: 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

package org.yuzu.yuzu_emu.utils
import org.yuzu.yuzu_emu.NativeLibrary

import android.app.ActivityManager
import android.app.ApplicationExitInfo
import android.content.Context
import android.os.Build

object Log {
    // Tracks whether we should share the old log or the current log
    var gameLaunched = false

    external fun debug(message: String)

    external fun warning(message: String)

    external fun info(message: String)

    external fun error(message: String)

    external fun critical(message: String)

    /**
     * Why Android says the app's last processes ended (a low-memory kill and a native crash both
     * leave the previous log without a final line).
     */
    fun logLastExits(context: Context) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.R) {
            return
        }
        try {
            val manager = context.getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
            val exits = manager.getHistoricalProcessExitReasons(null, 0, 3)
            if (exits.isEmpty()) {
                info("Previous exit: none recorded")
            }
            val now = System.currentTimeMillis()
            exits.forEach {
                val reason = when (it.reason) {
                    ApplicationExitInfo.REASON_LOW_MEMORY -> "LOW_MEMORY (killed by the system)"
                    ApplicationExitInfo.REASON_CRASH_NATIVE -> "CRASH_NATIVE"
                    ApplicationExitInfo.REASON_CRASH -> "CRASH (Java)"
                    ApplicationExitInfo.REASON_ANR -> "ANR"
                    ApplicationExitInfo.REASON_SIGNALED -> "SIGNALED"
                    ApplicationExitInfo.REASON_EXCESSIVE_RESOURCE_USAGE -> "EXCESSIVE_RESOURCE_USAGE"
                    ApplicationExitInfo.REASON_USER_REQUESTED -> "USER_REQUESTED"
                    ApplicationExitInfo.REASON_USER_STOPPED -> "USER_STOPPED"
                    ApplicationExitInfo.REASON_EXIT_SELF -> "EXIT_SELF"
                    ApplicationExitInfo.REASON_INITIALIZATION_FAILURE -> "INITIALIZATION_FAILURE"
                    ApplicationExitInfo.REASON_DEPENDENCY_DIED -> "DEPENDENCY_DIED"
                    ApplicationExitInfo.REASON_OTHER -> "OTHER"
                    else -> "reason ${it.reason}"
                }
                info(
                    "Previous exit: $reason status=${it.status} importance=${it.importance} " +
                        "pss=${it.pss / 1024} MiB rss=${it.rss / 1024} MiB " +
                        "${(now - it.timestamp) / 1000} s ago, process ${it.processName}: " +
                        "${it.description ?: "no description"}"
                )
            }
        } catch (e: Exception) {
            info("Previous exit: unavailable (${e.message})")
        }
    }

    fun logDeviceInfo() {
        info("Device Manufacturer - ${Build.MANUFACTURER}")
        info("Device Model - ${Build.MODEL}")
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.R) {
            info("SoC Manufacturer - ${Build.SOC_MANUFACTURER}")
            info("SoC Model - ${Build.SOC_MODEL}")
            NativeLibrary.getCpuSummary().split('\n').forEach {
                info("CPU Info - $it")
            }
        }
        NativeLibrary.getVulkanDriverVersion().split('\n').forEach {
            info("Vulkan Driver: - $it")
        }
        NativeLibrary.getVulkanApiVersion().split('\n').forEach {
            info("Vulkan API Version: - $it")
        }
        info("Total System Memory - ${MemoryUtil.getDeviceRAM()}")
    }
}
