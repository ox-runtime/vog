// Internal header — not part of the public API.
#pragma once

#include "imgui.h"
#include "platform.h"
#include "vog.h"

namespace vog {

// ---- Built-in palettes ----

static ThemeColors get_default_dark_palette() {
    ThemeColors c;

    c.bg = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);      // #141414
    c.panel0 = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);  // #0D0D0D
    c.panel1 = ImVec4(0.11f, 0.11f, 0.11f, 1.0f);  // #1C1C1C
    c.panel2 = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);  // #262626
    c.panel3 = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);  // #2E2E2E

    c.element = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);  // #404040
    c.border = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);   // #383838

    c.text = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);        // #DBDBDB
    c.text_muted = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);  // #8C8C8C

    c.accent = ImVec4(0.1f, 0.51f, 0.90f, 1.0f);       // #1A84E6
    c.selection = ImVec4(0.26f, 0.62f, 0.95f, 0.30f);  // #429DF24D

    c.positive = ImVec4(0.40f, 0.74f, 0.40f, 1.0f);  // #66BD66
    c.warning = ImVec4(0.96f, 0.78f, 0.36f, 1.0f);   // #F5C75C
    c.danger = ImVec4(0.85f, 0.30f, 0.30f, 1.0f);    // #D94D4D
    c.dim = ImVec4(0.0f, 0.0f, 0.0f, 0.35f);         // #00000059

    return c;
}

static ThemeColors get_default_light_palette() {
    ThemeColors c;

    c.bg = ImVec4(0.95f, 0.95f, 0.96f, 1.0f);      // #F2F2F5
    c.panel0 = ImVec4(0.98f, 0.98f, 0.99f, 1.0f);  // #FAFAFC
    c.panel1 = ImVec4(0.91f, 0.91f, 0.93f, 1.0f);  // #E8E8ED
    c.panel2 = ImVec4(0.86f, 0.86f, 0.88f, 1.0f);  // #DBDBE0
    c.panel3 = ImVec4(0.88f, 0.88f, 0.90f, 1.0f);  // #E0E0E6

    c.element = ImVec4(0.92f, 0.92f, 0.94f, 1.0f);  // #EBEBF0
    c.border = ImVec4(0.75f, 0.75f, 0.78f, 1.0f);   // #BFBFCC

    c.text = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);        // #26262E
    c.text_muted = ImVec4(0.45f, 0.45f, 0.48f, 1.0f);  // #73737A

    c.accent = ImVec4(0.03f, 0.50f, 0.94f, 1.0f);      // #0882F2
    c.selection = ImVec4(0.20f, 0.50f, 0.88f, 0.35f);  // #3380E059

    c.positive = ImVec4(0.21f, 0.56f, 0.27f, 1.0f);  // #379046
    c.warning = ImVec4(0.80f, 0.51f, 0.0f, 1.0f);    // #CE8400
    c.danger = ImVec4(0.78f, 0.18f, 0.18f, 1.0f);    // #C72E2E
    c.dim = ImVec4(0.0f, 0.0f, 0.0f, 0.35f);         // #00000059

    return c;
}

static ThemeColors get_default_theme_colors() {
    return is_system_dark_mode() ? get_default_dark_palette() : get_default_light_palette();
}

// ---- Style application ----

void apply_theme_styling(Theme& theme) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    ThemeColors& c = theme.colors;

    const ImVec4 nav_highlight{c.text.value().x, c.text.value().y, c.text.value().z, 0.7f};
    const ImVec4 row_bg_alt{c.text.value().x, c.text.value().y, c.text.value().z, 0.04f};
    const ImVec4 nav_dim{c.text.value().x, c.text.value().y, c.text.value().z, 0.20f};
    const ImVec4 border_subtle = c.get_hover_color(c.border.value());

    auto element_hover = c.get_hover_color(c.element.value());
    auto element_active = c.get_active_color(c.element.value());
    auto accent_active = c.get_active_color(c.accent.value());

    colors[ImGuiCol_WindowBg] = c.bg.value();
    colors[ImGuiCol_ChildBg] = c.panel1.value();
    colors[ImGuiCol_PopupBg] = c.panel2.value();
    colors[ImGuiCol_Border] = c.border.value();
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    colors[ImGuiCol_FrameBg] = c.bg.value();
    colors[ImGuiCol_FrameBgHovered] = element_hover;
    colors[ImGuiCol_FrameBgActive] = element_active;

    colors[ImGuiCol_TitleBg] = c.panel3.value();
    colors[ImGuiCol_TitleBgActive] = c.panel1.value();
    colors[ImGuiCol_TitleBgCollapsed] = c.panel3.value();
    colors[ImGuiCol_MenuBarBg] = c.panel3.value();

    colors[ImGuiCol_ScrollbarBg] = c.panel0.value();
    colors[ImGuiCol_ScrollbarGrab] = c.element.value();
    colors[ImGuiCol_ScrollbarGrabHovered] = element_hover;
    colors[ImGuiCol_ScrollbarGrabActive] = element_active;

    colors[ImGuiCol_CheckMark] = c.accent.value();
    colors[ImGuiCol_SliderGrab] = c.accent.value();
    colors[ImGuiCol_SliderGrabActive] = accent_active;

    colors[ImGuiCol_Button] = c.element.value();
    colors[ImGuiCol_ButtonHovered] = element_hover;
    colors[ImGuiCol_ButtonActive] = element_active;

    colors[ImGuiCol_Header] = c.element.value();
    colors[ImGuiCol_HeaderHovered] = element_hover;
    colors[ImGuiCol_HeaderActive] = element_active;

    colors[ImGuiCol_Separator] = c.border.value();
    colors[ImGuiCol_SeparatorHovered] = element_active;
    colors[ImGuiCol_SeparatorActive] = c.accent.value();

    colors[ImGuiCol_ResizeGrip] = c.element.value();
    colors[ImGuiCol_ResizeGripHovered] = element_hover;
    colors[ImGuiCol_ResizeGripActive] = element_active;

    colors[ImGuiCol_Tab] = c.element.value();
    colors[ImGuiCol_TabHovered] = element_hover;
    colors[ImGuiCol_TabActive] = element_active;
    colors[ImGuiCol_TabUnfocused] = c.element.value();
    colors[ImGuiCol_TabUnfocusedActive] = element_hover;

    colors[ImGuiCol_PlotLines] = c.accent.value();
    colors[ImGuiCol_PlotLinesHovered] = c.warning.value();
    colors[ImGuiCol_PlotHistogram] = c.accent.value();
    colors[ImGuiCol_PlotHistogramHovered] = c.positive.value();

    colors[ImGuiCol_TableHeaderBg] = c.element.value();
    colors[ImGuiCol_TableBorderStrong] = c.border.value();
    colors[ImGuiCol_TableBorderLight] = border_subtle;
    colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_TableRowBgAlt] = row_bg_alt;

    colors[ImGuiCol_Text] = c.text.value();
    colors[ImGuiCol_TextDisabled] = c.text_muted.value();
    colors[ImGuiCol_TextSelectedBg] = c.selection.value();

    colors[ImGuiCol_DragDropTarget] = c.warning.value();

    colors[ImGuiCol_NavHighlight] = c.accent.value();
    colors[ImGuiCol_NavWindowingHighlight] = nav_highlight;
    colors[ImGuiCol_NavWindowingDimBg] = nav_dim;
    colors[ImGuiCol_ModalWindowDimBg] = c.dim.value();

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

void Window::SetTheme(const Theme& theme) {
    // Resolve: fill any nullopt fields from system defaults so theme_ is always
    // fully populated and safe to dereference in rendering / widget code.
    ThemeColors defaults = get_default_theme_colors();
    Theme resolved;
    auto& rc = resolved.colors;
    const auto& uc = theme.colors;
    rc.bg = uc.bg.has_value() ? uc.bg : defaults.bg;
    rc.panel0 = uc.panel0.has_value() ? uc.panel0 : defaults.panel0;
    rc.panel1 = uc.panel1.has_value() ? uc.panel1 : defaults.panel1;
    rc.panel2 = uc.panel2.has_value() ? uc.panel2 : defaults.panel2;
    rc.panel3 = uc.panel3.has_value() ? uc.panel3 : defaults.panel3;
    rc.element = uc.element.has_value() ? uc.element : defaults.element;
    rc.border = uc.border.has_value() ? uc.border : defaults.border;
    rc.text = uc.text.has_value() ? uc.text : defaults.text;
    rc.text_muted = uc.text_muted.has_value() ? uc.text_muted : defaults.text_muted;
    rc.accent = uc.accent.has_value() ? uc.accent : defaults.accent;
    rc.selection = uc.selection.has_value() ? uc.selection : defaults.selection;
    rc.positive = uc.positive.has_value() ? uc.positive : defaults.positive;
    rc.warning = uc.warning.has_value() ? uc.warning : defaults.warning;
    rc.danger = uc.danger.has_value() ? uc.danger : defaults.danger;
    rc.dim = uc.dim.has_value() ? uc.dim : defaults.dim;

    auto& rv = resolved.vars;
    const auto& uv = theme.vars;
    rv.window_padding = uv.window_padding.has_value() ? uv.window_padding : ImVec2(10.0f, 10.0f);
    rv.font_size = uv.font_size.has_value() ? uv.font_size : 14.0f;

    theme_ = resolved;
    apply_theme_styling(theme_);
}

}  // namespace vog
