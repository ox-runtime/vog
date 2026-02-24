// Internal header — not part of the public API.
#pragma once

struct GLFWwindow;

namespace vog {

// Returns true when the OS is in dark mode.
// Implemented per-platform in platform_*.cpp / .mm.
bool is_system_dark_mode();

// Apply dark/light titlebar decoration to match the OS theme.
// Implemented per-platform in platform_*.cpp / .mm.
void apply_dark_titlebar(GLFWwindow* window, bool dark);

}  // namespace vog
