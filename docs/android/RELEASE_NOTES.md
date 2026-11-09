# Lichii for Android 0.4.0

Tested on a Samsung Galaxy S26 (Exynos 2600, Xclipse 960, 12 GB, Android 17) with a custom Mesa RADV driver.

## Highlights

- **The Legend of Zelda: Ocarina of Time**
  - The latest game update boots. It stopped at a black screen while opening a microphone stream. Lichii for Android now tells games there is no microphone, as before.
  - Cutscene audio stays in sync with the video.
  - Fewer graphical issues: fixes for white flashes from stale occlusion counts, the red line in the tomb, Hyrule Field grass, and flickering water.
  - Built-in FPS Unlock: the game logic keeps real speed when the frame rate changes.
- **Memory**
  - **New per-game setting "Limit memory during scene loads"** (Advanced graphics, Auto / Off / On). 0.3.0 applied this limit to every game, which slowed down games that load textures all the time. Auto now applies it to Ocarina of Time only.
  - **GPU memory follows the phone's real RAM.** Some drivers report a 32 GB GPU heap on a 12 GB phone. The GPU now gets at most a third of the RAM, and the texture cache gives memory back when Android runs low. This should mean fewer games closed for low memory.
  - The log now lists the GPU memory heaps the driver reports, the RAM and the free memory, to help with memory reports.
- **The latest Lichii (Zortos) changes for Android**, about 130 in all. Among them:
  - Mesh shader draws on GPUs without mesh shader support.
  - Tensor operations on GPUs with cooperative matrix support.
  - Faster ASTC texture decoding.
  - Quicker GPU thread wake-ups.
  - Shader and rendering fixes for Mario Kart World, Welcome Tour movies, Mario Tennis Fever button icons and other Switch 2 games.

  PC-only changes (Windows, desktop UI, updater, NVIDIA DLSS) are left out or have no effect on Android.

## Known issues

- Ocarina of Time at **1x** on a 12 GB phone uses more memory than the phone has. Android then swaps constantly and the frame rate drops. Use **0.5x** resolution in the game's settings. **16 GB of RAM** is still recommended.
- Microphone input is not supported on Android.
- The phone heats up during long sessions and slows down until it cools.

## For testers

- Install the APK, add your games folder, and set up keys and firmware as usual. None are included.
- Recommended: Handheld mode, NCE CPU backend, 0.5x resolution, and a custom RADV driver on Xclipse GPUs.
- Diagnostic switches can be set in `Android/data/dev.lichii.android/files/env.txt` (see the README).
- Report problems at https://github.com/pavelgajdo/Lichii-Android/issues with your phone, driver and `eden_log.txt`.
