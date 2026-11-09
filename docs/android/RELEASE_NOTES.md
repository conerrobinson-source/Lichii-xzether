# Lichii for Android 0.5.0

Tested on a Samsung Galaxy S26 (Exynos 2600, Xclipse 960, 12 GB, Android 17) with a custom Mesa RADV driver.

## Highlights

- **The Legend of Zelda: Ocarina of Time**
  - **Faster and much steadier.**
    - The game's anti-aliasing network is skipped by default on Android. It ran as slow software tensor math on phones. A new per-game setting, "Ocarina of Time anti-aliasing" (advanced graphics), brings it back with Game.
    - The two GPU threads wait for each other far less (two upstream fixes for semaphore acquires and releases).
  - **Fewer broken starts.** Some starts showed a black scene with white and purple streaks. An upstream fix for a shader-translation race that depended on compile order should stop them.
  - **More ground detail on Xclipse.** The game's mesh-shader draws were skipped because they stalled the GPU. They are now drawn through compute instead.
  - A crash when loading a save is fixed.
- **Memory.** Fewer games closed for low memory:
  - GPU memory gives way before Samsung's low-memory killer acts on low swap.
  - Render targets drawn below 1x no longer keep a full-size copy as well.
  - On GPUs without ASTC (Xclipse), converted ASTC textures keep their mip levels from 512 px on.
  - NUP game files are decompressed only as far as the game reads.
  - Images that don't fit in memory fail cleanly instead of showing garbage.
  - Shader compiling uses less memory.
- **Smoother for all games.**
  - Evicted textures the GPU drew into are copied back together, behind one wait.
  - Each draw costs less on the GPU thread (upstream).

## Known issues

- Ocarina of Time on a 12 GB phone can still be closed for low memory, most often on the first start while shaders compile, or with other apps open. Use **0.5x** resolution. **16 GB of RAM** is recommended.
- Microphone input is not supported on Android.
- The phone heats up during long sessions and slows down until it cools. Game Booster's performance mode helps.

## For testers

- Install the APK, add your games folder, and set up keys and firmware as usual. None are included.
- Recommended: Handheld mode, NCE CPU backend, 0.5x resolution, and a custom RADV driver on Xclipse GPUs.
- Diagnostic switches can be set in `Android/data/dev.lichii.android/files/env.txt` (see the README).
- Report problems at https://github.com/pavelgajdo/Lichii-Android/issues with your phone, driver and `eden_log.txt`.
