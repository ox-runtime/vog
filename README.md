# vog

**vog** is a batteries-included C++ windowing library built on [GLFW](https://www.glfw.org/) and [Dear ImGui](https://github.com/ocornut/imgui). It handles window-creation, DPI-aware font scaling, system dark mode detection, native titlebar appearance, and other platform-dependent details. It also bundles Font Awesome 6 icons and a couple of useful ImGui widgets suitable for applications.

This allows you to focus entirely on drawing the contents of your window. Supports Windows, Linux and macOS.

Primarily built for [ox-simulator](https://github.com/ox-runtime/ox-simulator), but feel free to use it if you find it useful!

<img  height="300" alt="vog dark on Windows" src="https://github.com/user-attachments/assets/7e73fb8f-61fc-4290-be25-e836fe332f1d" />
<img  height="300" alt="vog light on Windows" src="https://github.com/user-attachments/assets/0bfac840-e28a-4366-8e52-e1d78b8331fe" />

## Usage

Fetch the library in CMake, then link against it:

```cmake
include(FetchContent)

FetchContent_Declare(vog
    GIT_REPOSITORY https://github.com/ox-runtime/vog.git
    GIT_TAG v1.0.0  # Replace with a specific tag or branch if needed
)
FetchContent_MakeAvailable(vog)

target_link_libraries(your_target PRIVATE vog)
```

Open a window by calling `Window::Start` with a config and a frame callback. Put all ImGui calls in the callback; do not call `NewFrame` or `Render` yourself.

```cpp
#include "vog.h"

vog::Window window;

vog::WindowConfig cfg;
cfg.title  = "My Window";
cfg.width  = 1280;
cfg.height = 720;

window.Start(cfg, []() {
    ImGui::Begin("Hello");
    ImGui::TextColored(vog::GetThemeColors().accent, "accent-colored text");
    ImGui::End();
});

// Block until the window is closed
window.Wait();
```

## Theme colors

`GetThemeColors()` returns a reference to the active [ThemeColors](include/vog.h) instance. A dark/light theme is chosen automatically based on the OS dark/light mode setting when the window opens, but you can set a custom theme as well (see below).

**Note:** Dark mode detection on Linux needs more work. Please feel free to [contribute](src/platform_linux.cpp)!

### Custom theme colors

Supply a fully populated `ThemeColors` to `SetThemeColors()` at any point after `Window::Start()` has been called:

```cpp
vog::ThemeColors my_theme;
my_theme.bg             = ImVec4(0.05f, 0.05f, 0.10f, 1.0f);  // RGBA
my_theme.surface        = ImVec4(0.09f, 0.09f, 0.16f, 1.0f);
my_theme.text           = ImVec4(0.92f, 0.93f, 1.00f, 1.0f);
my_theme.accent         = ImVec4(0.56f, 0.40f, 0.96f, 1.0f);
// ... fill the remaining fields ...

vog::SetThemeColors(my_theme);
```

`SetThemeColors()` overwrites the color table and immediately re-applies all ImGui style settings, so the change takes effect on the very next frame.

## Font Awesome icons

Font Awesome 6 Solid is bundled. `vog.h` includes [IconsFontAwesome6.h](third_party/IconsFontAwesome6.h) automatically, so `ICON_FA_*` macros are available as soon as you include `vog.h`:

```cpp
if (ImGui::Button(ICON_FA_COPY " Copy")) { ... }
```

## Custom widgets

`vog::widgets` contains a small set of controls built on top of ImGui:

| Widget | Description |
|---|---|
| `vog::widgets::ToggleButton(label, &value)` | iOS-style on/off toggle switch |
| `vog::widgets::Combo(label, &index, items, count)` | `ImGui::Combo` with better popup padding |
| `vog::widgets::ShowItemTooltip(text)` | Tooltip anchored below the previous item |
| `vog::widgets::Button(text, bgColor)` | Button with optional background color and automatic text color for good contrast |

## Building the example

The demo window showcases controls, status indicators, an animated plot, the theme switcher, and platform info. It is off by default; enable it with `VOG_BUILD_EXAMPLES`:

```bash
# Configure
cmake -B build -DVOG_BUILD_EXAMPLES=ON

# Build
cmake --build build --config Release

# Run
./build/bin/vog_demo        # Linux / macOS
build\bin\Release\vog_demo  # Windows
```

The demo is self-contained and has no dependencies beyond vog itself.
