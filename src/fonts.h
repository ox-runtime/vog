// Internal header — not part of the public API.
#pragma once

#include <GLFW/glfw3.h>

#include <filesystem>
#include <iostream>
#include <string>

#include "IconsFontAwesome6.h"
#include "fa_solid_900.h"
#include "imgui.h"
#include "vog.h"

namespace vog {

// Returns a platform-appropriate Arial-like font path.
inline std::string GetFontPath() {
#ifdef _WIN32
    const char* paths[] = {
        "C:/Windows/Fonts/arial.ttf",
    };
#elif defined(__APPLE__)
    const char* paths[] = {
        "/System/Library/Fonts/Helvetica.ttc",
    };
#elif defined(__linux__)
    const char* paths[] = {
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
    };
#else
    const char* paths[] = {};
#endif
    for (const char* p : paths) {
        if (std::filesystem::exists(p)) return p;
    }
    return "";
}

// Load the platform font and Font Awesome icons into io.Fonts.
// Must be called after ImGui::CreateContext() and before the first frame.
void Window::setup_fonts() {
    ImGuiIO& io = ImGui::GetIO();
    std::string font_path = GetFontPath();

    float globalFontSize = GetTheme().vars.font_size.value();
    float fontSize = globalFontSize * last_content_scale_;
#ifdef __APPLE__
    fontSize *= (72.0f / 96.0f);  // macOS uses 72 DPI as the default, while Windows and Linux typically use 96 DPI
#endif

    ImFont* default_font = nullptr;
    if (!font_path.empty()) {
        default_font = io.Fonts->AddFontFromFileTTF(font_path.c_str(), fontSize);
    }

    if (last_content_scale_ > 1.001f) {
        std::cout << "vog: high DPI display detected (scale: " << last_content_scale_ << "x)" << std::endl;
    }

    if (default_font) {
        io.FontDefault = default_font;
    } else {
        io.FontDefault = io.Fonts->AddFontDefault();
    }

    // Platform-specific FontGlobalScale rationale:
    //
    // macOS (Retina):
    //   GLFW uses SCALE_FRAMEBUFFER — the framebuffer is `content_scale` times larger
    //   than the logical window.  io.DisplayFramebufferScale = (scale, scale)
    //   and io.DisplaySize is in *logical* points.  Loading the font at
    //   size*scale produces a sharp hi-res atlas, but ImGui's glyph metrics
    //   are `scale` times too large in logical space.  FontGlobalScale = 1/scale
    //   restores the correct logical size without losing sharpness.
    //
    // Windows / Linux (DPI scale):
    //   GLFW uses SCALE_TO_MONITOR — the window itself is enlarged so
    //   io.DisplayFramebufferScale = (1, 1) and io.DisplaySize == physical
    //   pixels.  Loading at size*scale and rendering at size*scale physical
    //   pixels is already the correct DPI-aware size; FontGlobalScale must
    //   remain 1.0 or the font will appear smaller than intended.
#ifdef __APPLE__
    io.FontGlobalScale = 1.0f / last_content_scale_;
#else
    io.FontGlobalScale = 1.0f;
#endif

    // Merge Font Awesome icons
    ImFontConfig config;
    config.MergeMode = true;
    config.FontDataOwnedByAtlas = false;
    config.GlyphMinAdvanceX = fontSize;
    static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    io.Fonts->AddFontFromMemoryCompressedTTF(fa_solid_900_compressed_data, fa_solid_900_compressed_size, fontSize,
                                             &config, icon_ranges);
}

}  // namespace vog
