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
    const vog::ThemeColors& tc = vog::Window::GetTheme().colors;
    ImGui::TextColored(tc.accent.value(), "Hello vog!");
});

// Block until the window is closed
window.Wait();
```

## Theme

`Window::GetTheme()` returns a reference to the active `Theme`, which has two parts:

- **`colors`** — a `ThemeColors` with semantic color tokens (`bg`, `surface`, `accent`, `text`, etc.)
- **`vars`** — a `ThemeVars` with layout variables (`window_padding`, `font_size`)

A dark or light palette is chosen automatically from the OS setting when the window opens. You can override any subset of fields via `WindowConfig::theme` at startup, or by calling `Window::SetTheme()` at any time after `Start()`.

**Note:** Dark mode detection on Linux needs more work. Please feel free to [contribute](src/platform_linux.cpp)!

### Accessing theme values

All `ThemeColors` and `ThemeVars` fields are `std::optional`. After the window is open they are always populated, so `.value()` is safe:

```cpp
const vog::ThemeColors& tc = vog::Window::GetTheme().colors;
ImGui::TextColored(tc.accent.value(), "Hello!");
```

### Overriding the theme at startup

Set `WindowConfig::theme` to a partially or fully populated `Theme`. Any field left unset falls back to the default system palette — you only need to specify what you want to change:

```cpp
vog::Theme my_theme;
my_theme.colors.accent   = ImVec4(0.56f, 0.40f, 0.96f, 1.0f);  // custom accent
my_theme.vars.font_size  = 16.0f;                               // larger font
// everything else inherits from the dark/light system defaults

vog::WindowConfig cfg;
cfg.theme = my_theme;
```

### Changing the theme at runtime

Call `Window::SetTheme()` at any point after `Start()` has been called. Unset fields again fall back to the system defaults, and the change takes effect on the very next frame:

```cpp
vog::Theme space_theme;
space_theme.colors.bg      = ImVec4(0.06f, 0.07f, 0.14f, 1.0f);
space_theme.colors.accent  = ImVec4(0.56f, 0.40f, 0.96f, 1.0f);
space_theme.colors.text    = ImVec4(0.92f, 0.93f, 1.00f, 1.0f);
// ... set as many or as few fields as needed ...

vog::Window::SetTheme(space_theme);
```

**Note:** `Start()` will block on Mac, since it runs on the main thread. You can call `SetTheme()` from inside the Render callback.

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
| `vog::widgets::Button(text, textColor, bgColor)` | Button with optional text color and background color |

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
