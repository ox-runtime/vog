#include "imgui.h"
#include "platform.h"
#include "vog.h"

namespace vog {

static ThemeColors g_theme_colors;

// ---- Built-in palettes ----

static void fill_dark_palette(ThemeColors& c) {
    c.bg = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);        // #141414
    c.surface = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);   // #0D0D0D
    c.titlebar = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);  // #262626

    c.element = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);  // #404040

    c.border = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);         // #404040
    c.border_subtle = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);  // #0D0D0D
    c.border_shadow = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);     // #00000000

    c.text = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);        // #DBDBDB
    c.text_muted = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);  // #8C8C8C

    c.accent = ImVec4(0.26f, 0.62f, 0.95f, 1.0f);      // #429DF2
    c.selection = ImVec4(0.26f, 0.62f, 0.95f, 0.30f);  // #429DF24D

    c.positive = ImVec4(0.40f, 0.74f, 0.40f, 1.0f);  // #66BD66
    c.warning = ImVec4(0.96f, 0.78f, 0.36f, 1.0f);   // #F5C75C
    c.danger = ImVec4(0.85f, 0.30f, 0.30f, 1.0f);    // #D94D4D

    c.dim = ImVec4(0.0f, 0.0f, 0.0f, 0.35f);      // #00000059
    c.nav_dim = ImVec4(0.8f, 0.8f, 0.8f, 0.20f);  // #CCCCCC33
}

static void fill_light_palette(ThemeColors& c) {
    c.bg = ImVec4(0.95f, 0.95f, 0.96f, 1.0f);        // #F2F2F5
    c.surface = ImVec4(0.98f, 0.98f, 0.99f, 1.0f);   // #FAFAFC
    c.titlebar = ImVec4(0.92f, 0.92f, 0.94f, 1.0f);  // #EBEBF0

    c.element = ImVec4(0.92f, 0.92f, 0.94f, 1.0f);  // #EBEBF0

    c.border = ImVec4(0.75f, 0.75f, 0.78f, 1.0f);         // #BFBFCC
    c.border_subtle = ImVec4(0.88f, 0.88f, 0.90f, 1.0f);  // #E0E0E6
    c.border_shadow = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);     // #00000000

    c.text = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);        // #26262E
    c.text_muted = ImVec4(0.45f, 0.45f, 0.48f, 1.0f);  // #73737A

    c.accent = ImVec4(0.28f, 0.58f, 0.92f, 1.0f);      // #4994EB
    c.selection = ImVec4(0.20f, 0.50f, 0.88f, 0.35f);  // #3380E059

    c.positive = ImVec4(0.28f, 0.68f, 0.35f, 1.0f);  // #47AD59
    c.warning = ImVec4(0.90f, 0.65f, 0.20f, 1.0f);   // #E5A533
    c.danger = ImVec4(0.78f, 0.18f, 0.18f, 1.0f);    // #C72E2E

    c.dim = ImVec4(0.0f, 0.0f, 0.0f, 0.35f);      // #00000059
    c.nav_dim = ImVec4(0.2f, 0.2f, 0.2f, 0.20f);  // #33333333
}

// ---- Style application ----

void apply_theme_styling() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    ThemeColors& c = g_theme_colors;

    const ImVec4 nav_highlight{c.text.x, c.text.y, c.text.z, 0.7f};
    const ImVec4 row_bg_alt{c.text.x, c.text.y, c.text.z, 0.04f};

    auto element_hover = c.get_hover_color(c.element);
    auto element_active = c.get_active_color(c.element);
    auto accent_active = c.get_active_color(c.element);

    colors[ImGuiCol_WindowBg] = c.bg;
    colors[ImGuiCol_ChildBg] = c.surface;
    colors[ImGuiCol_PopupBg] = c.surface;
    colors[ImGuiCol_Border] = c.border;
    colors[ImGuiCol_BorderShadow] = c.border_shadow;

    colors[ImGuiCol_FrameBg] = c.element;
    colors[ImGuiCol_FrameBgHovered] = element_hover;
    colors[ImGuiCol_FrameBgActive] = element_active;

    colors[ImGuiCol_TitleBg] = c.titlebar;
    colors[ImGuiCol_TitleBgActive] = c.surface;
    colors[ImGuiCol_TitleBgCollapsed] = c.titlebar;
    colors[ImGuiCol_MenuBarBg] = c.titlebar;

    colors[ImGuiCol_ScrollbarBg] = c.surface;
    colors[ImGuiCol_ScrollbarGrab] = c.element;
    colors[ImGuiCol_ScrollbarGrabHovered] = element_hover;
    colors[ImGuiCol_ScrollbarGrabActive] = element_active;

    colors[ImGuiCol_CheckMark] = c.positive;
    colors[ImGuiCol_SliderGrab] = c.accent;
    colors[ImGuiCol_SliderGrabActive] = accent_active;

    colors[ImGuiCol_Button] = c.element;
    colors[ImGuiCol_ButtonHovered] = element_hover;
    colors[ImGuiCol_ButtonActive] = element_active;

    colors[ImGuiCol_Header] = c.element;
    colors[ImGuiCol_HeaderHovered] = element_hover;
    colors[ImGuiCol_HeaderActive] = element_active;

    colors[ImGuiCol_Separator] = c.border;
    colors[ImGuiCol_SeparatorHovered] = element_active;
    colors[ImGuiCol_SeparatorActive] = c.accent;

    colors[ImGuiCol_ResizeGrip] = c.element;
    colors[ImGuiCol_ResizeGripHovered] = element_hover;
    colors[ImGuiCol_ResizeGripActive] = element_active;

    colors[ImGuiCol_Tab] = c.element;
    colors[ImGuiCol_TabHovered] = element_hover;
    colors[ImGuiCol_TabActive] = element_active;
    colors[ImGuiCol_TabUnfocused] = c.element;
    colors[ImGuiCol_TabUnfocusedActive] = element_hover;

    colors[ImGuiCol_PlotLines] = c.accent;
    colors[ImGuiCol_PlotLinesHovered] = c.warning;
    colors[ImGuiCol_PlotHistogram] = c.accent;
    colors[ImGuiCol_PlotHistogramHovered] = c.positive;

    colors[ImGuiCol_TableHeaderBg] = c.element;
    colors[ImGuiCol_TableBorderStrong] = c.border;
    colors[ImGuiCol_TableBorderLight] = c.border_subtle;
    colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_TableRowBgAlt] = row_bg_alt;

    colors[ImGuiCol_Text] = c.text;
    colors[ImGuiCol_TextDisabled] = c.text_muted;
    colors[ImGuiCol_TextSelectedBg] = c.selection;

    colors[ImGuiCol_DragDropTarget] = c.warning;

    colors[ImGuiCol_NavHighlight] = c.accent;
    colors[ImGuiCol_NavWindowingHighlight] = nav_highlight;
    colors[ImGuiCol_NavWindowingDimBg] = c.nav_dim;
    colors[ImGuiCol_ModalWindowDimBg] = c.dim;

    // Rounding
    style.WindowRounding = 4.0f;
    style.ChildRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.PopupRounding = 3.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;

    // Spacing
    style.WindowPadding = ImVec2(0.0f, 0.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(10.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style.IndentSpacing = 18.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 10.0f;

    // Borders
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.TabBorderSize = 0.0f;

    // Quality
    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
}

void setup_theme() {
    if (is_system_dark_mode()) {
        fill_dark_palette(g_theme_colors);
    } else {
        fill_light_palette(g_theme_colors);
    }
    apply_theme_styling();
}

ThemeColors& GetThemeColors() { return g_theme_colors; }

void SetThemeColors(const ThemeColors& colors) {
    g_theme_colors = colors;
    apply_theme_styling();
}

}  // namespace vog
