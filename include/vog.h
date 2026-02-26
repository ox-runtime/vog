#pragma once

// vog — GLFW + Dear ImGui windowing library, v0.3.0
//
// Single include for everything. ImGui is included automatically so callers
// only need: #include "vog.h"

#include <atomic>
#include <functional>
#include <optional>
#include <string>
#include <thread>

#include "IconsFontAwesome6.h"
#include "imgui.h"

#define NOMINMAX

struct GLFWwindow;

// ---------------------------------------------------------------------------
// vog — window management and theming
// ---------------------------------------------------------------------------
namespace vog {

struct ThemeVars {
    // Window padding
    std::optional<ImVec2> window_padding;

    // Main application font size in pixels
    std::optional<float> font_size;
};

// Semantic color tokens for the UI theme.
// All fields are optional: set only the ones you want to customize.
// Unset fields fall back to the default system theme (dark or light) when
// passed to WindowConfig::theme or SetTheme().
struct ThemeColors {
    // Backgrounds
    std::optional<ImVec4> bg;        // app / window background
    std::optional<ImVec4> surface;   // elevated surface: panels, popups, child windows
    std::optional<ImVec4> titlebar;  // titlebar and menubar background

    // Interactive element states
    std::optional<ImVec4> element;  // default state of input elements

    // Borders
    std::optional<ImVec4> border;         // primary borders and dividers
    std::optional<ImVec4> border_subtle;  // inner / light borders (table cell lines, etc.)
    std::optional<ImVec4> border_shadow;  // drop-shadow tint (transparent in most themes)

    // Text
    std::optional<ImVec4> text;        // primary readable text
    std::optional<ImVec4> text_muted;  // secondary, disabled, or hint text

    // Accent (brand / interactive color)
    std::optional<ImVec4> accent;     // focus rings, nav highlight, slider grab
    std::optional<ImVec4> selection;  // text-selection highlight (translucent accent)

    // Status
    std::optional<ImVec4> positive;  // success, ok, checkmarks
    std::optional<ImVec4> warning;   // caution, drag-drop target
    std::optional<ImVec4> danger;    // errors, critical status, delete actions

    // Overlays
    std::optional<ImVec4> dim;      // modal / full-screen dim scrim
    std::optional<ImVec4> nav_dim;  // Ctrl+Tab nav-windowing background dim

   private:
    static bool is_dark_color(const ImVec4& color) {
        float luminance = 0.2126f * color.x + 0.7152f * color.y + 0.0722f * color.z;
        bool isDark = luminance < 0.5f;
        return isDark;
    }

    static ImVec4 adjust_brightness(const ImVec4& color, const ImVec4& bg, float amount) {
        amount *= is_dark_color(bg) ? -1 : 1;
        return ImVec4(std::min(color.x + amount, 1.0f), std::min(color.y + amount, 1.0f),
                      std::min(color.z + amount, 1.0f), color.w);
    }

   public:
    static ImVec4 get_hover_color(const ImVec4& color, const ImVec4& bg) { return adjust_brightness(color, bg, 0.05f); }
    static ImVec4 get_active_color(const ImVec4& color, const ImVec4& bg) {
        return adjust_brightness(color, bg, -0.1f);
    }

    ImVec4 get_hover_color(const ImVec4& color) { return ThemeColors::get_hover_color(color, bg.value()); }
    ImVec4 get_active_color(const ImVec4& color) { return ThemeColors::get_active_color(color, bg.value()); }
};

struct Theme {
    ThemeColors colors;
    ThemeVars vars;
};

// ---------------------------------------------------------------------------
// Window
// ---------------------------------------------------------------------------

struct WindowConfig {
    // Window title
    std::string title = "App";

    // Initial window size
    int width = 1280;
    int height = 720;

    // Optional partial theme override. Set only the fields you want to
    // customize; unset fields fall back to the default system theme.
    std::optional<Theme> theme;

    // creates a default full-size ImGui window (with no decorations) to host the UI.
    bool createDefaultImGuiWindow = true;
};

// Self-contained windowing + ImGui lifecycle.
//
// Call Start() to open the window and begin rendering. The render_frame
// callback is invoked once per frame between ImGui::NewFrame() and
// ImGui::Render(); put all ImGui widget calls there.
//
// Threading:
//   macOS  — Start() runs the render loop on the calling thread and blocks
//             until the window is closed or Stop() is called.  The calling
//             thread must be the main thread.
//   Others — Start() spawns a background thread and returns immediately.
class Window {
   public:
    Window() = default;
    ~Window();  // calls Stop() if still running

    // Open the window and start rendering. Returns false on init failure.
    bool Start(const WindowConfig& config, std::function<void()> render_frame);

    // Signal the render loop to exit and join the thread (if any).
    void Stop();

    // Block until the window is closed. On macOS, Start() already blocks so
    // this is a no-op. On Windows/Linux, Start() returns immediately and
    // Wait() joins the background render thread — use this instead of a
    // manual platform-specific loop.
    void Wait();

    bool IsRunning() const { return running_.load(); }

    // Escape hatch: the raw GLFW window pointer (nullptr if not running).
    GLFWwindow* GetNativeWindow() const { return window_; }

    static Theme& GetTheme() { return theme_; }
    static void SetTheme(const Theme& theme);

   private:
    bool InitializeGraphics();
    void CleanupGraphics();
    void RenderLoop(std::function<void()> render_frame);
    void setup_fonts();

    void RenderFrameNow();

    GLFWwindow* window_ = nullptr;
    WindowConfig config_;
    std::atomic<bool> running_{false};
    std::atomic<bool> should_stop_{false};
    std::thread render_thread_;
    std::function<void()> active_render_frame_;
    float last_content_scale_ = 1.0f;
    float last_global_font_size_ = -1.0f;
    static Theme theme_;
};

// ---------------------------------------------------------------------------
// vog::widgets — custom widget helpers (inline, built on top of ImGui)
// ---------------------------------------------------------------------------
namespace widgets {

// Fixed-position tooltip shown below the last item after a short delay.
inline void ShowItemTooltip(const char* text) {
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay)) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
        ImVec2 item_br = ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y + 4.0f);
        ImGui::SetNextWindowPos(item_br, ImGuiCond_Always);
        if (ImGui::BeginTooltip()) {
            ImGui::TextUnformatted(text);
            ImGui::EndTooltip();
        }
        ImGui::PopStyleVar();
    }
}

// ImGui::Combo with vertical padding added to the dropdown popup.
inline bool Combo(const char* label, int* current_item, const char* const items[], int items_count) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, 6.0f));
    bool ret = ImGui::Combo(label, current_item, items, items_count);
    ImGui::PopStyleVar();
    return ret;
}

// On/off toggle switch. Returns true when the value changes.
// labelOnRight=false draws the label to the left of the track.
inline bool ToggleButton(const char* label, bool* v, bool labelOnRight = true) {
    ImDrawList* dl = ImGui::GetWindowDrawList();

    if (!labelOnRight) {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine();
    }

    ImVec2 pos = ImGui::GetCursorScreenPos();

    const float frame_h = ImGui::GetFrameHeight();
    const float h = frame_h * 0.9f;
    const float w = h * 1.85f;
    const float r = h * 0.5f;
    const float knob_r = h * 0.40f;

    // Adjust vertical position to center the toggle button
    pos.y += (frame_h - h) * 0.5f;

    ImGui::InvisibleButton(label, {w, h});

    bool changed = false;
    if (ImGui::IsItemClicked()) {
        *v = !*v;
        changed = true;
    }

    const bool hovered = ImGui::IsItemHovered();
    ThemeColors& tc = Window::GetTheme().colors;

    ImVec4 col_on = hovered ? tc.get_hover_color(tc.accent.value()) : tc.accent.value();
    ImVec4 col_off = hovered ? tc.element.value() : tc.get_active_color(tc.element.value());
    ImVec4 b = tc.border.value();

    ImU32 col_bg = ImGui::GetColorU32(*v ? col_on : col_off);
    ImU32 col_border = ImGui::GetColorU32(*v ? ImVec4(0.0f, 0.0f, 0.0f, 0.0f) : tc.border.value());
    ImU32 col_knob = ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f));
    ImU32 col_knob_border = ImGui::GetColorU32(*v ? ImVec4(b.x, b.y, b.z, 0.55f) : ImVec4(b.x, b.y, b.z, 0.8f));

    ImVec2 p_max = {pos.x + w, pos.y + h};
    dl->AddRectFilled(pos, p_max, col_bg, r);
    dl->AddRect(pos, p_max, col_border, r, 0, 1.f);

    float knob_x = *v ? (pos.x + w - r) : (pos.x + r);
    ImVec2 knob_center = {knob_x, pos.y + h * 0.5f};
    dl->AddCircleFilled(knob_center, knob_r, col_knob, 32);
    dl->AddCircle(knob_center, knob_r, col_knob_border, 32, 1.5f);

    if (labelOnRight) {
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
    }

    return changed;
}

// Widget for a button with dynamic text coloring based on background color
inline bool Button(const char* label, const ImVec4& textColor, const ImVec4& bgColor) {
    // Calculate hover and active colors by modifying the background color
    ImVec4 hoverColor = ThemeColors::get_hover_color(bgColor, bgColor);
    ImVec4 activeColor = ThemeColors::get_active_color(bgColor, bgColor);

    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
    bool clicked = ImGui::Button(label);
    ImGui::PopStyleColor(4);
    return clicked;
}

inline bool Button(const char* label, const ImVec4& textColor) {
    const ThemeColors& tc = Window::GetTheme().colors;
    return Button(label, textColor, tc.element.value());
}

inline bool Button(const char* label) {
    const ThemeColors& tc = Window::GetTheme().colors;
    return Button(label, tc.text.value(), tc.element.value());
}

}  // namespace widgets
}  // namespace vog
