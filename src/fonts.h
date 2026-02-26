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
    return "C:/Windows/Fonts/arial.ttf";
#elif defined(__APPLE__)
    return "/System/Library/Fonts/Helvetica.ttc";
#elif defined(__linux__)
    const char* paths[] = {
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
    };
    for (const char* p : paths) {
        if (std::filesystem::exists(p)) return p;
    }
    return "";
#else
    return "";
#endif
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
