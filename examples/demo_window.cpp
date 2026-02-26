// vog demo window — exercises key library features across platforms.
//
// Build:   set VOG_BUILD_EXAMPLES=ON in cmake
// Run:     bin/vog_demo

#include <cmath>
#include <cstdio>
#include <string>

#include "vog.h"

// ---------------------------------------------------------------------------
// Demo state
// ---------------------------------------------------------------------------
struct DemoState {
    // Controls
    bool toggle_a = true;
    bool toggle_b = false;
    bool checkbox = true;
    int combo_index = 0;
    float slider_f = 0.42f;
    int slider_i = 7;
    char input_buf[256] = "Hello, vog!";

    // Custom theme
    bool use_custom_theme = false;

    // Status simulation
    float sim_time = 0.0f;
    bool sim_running = false;
};

static const char* COMBO_ITEMS[] = {"Option A", "Option B", "Option C", "Option D"};
static constexpr int COMBO_COUNT = 4;

// A bright custom theme to demonstrate SetThemeColors().
static vog::ThemeColors make_custom_theme() {
    vog::ThemeColors c{};
    c.bg = ImVec4(0.06f, 0.07f, 0.14f, 1.0f);
    c.surface = ImVec4(0.10f, 0.11f, 0.20f, 1.0f);
    c.titlebar = ImVec4(0.08f, 0.09f, 0.17f, 1.0f);
    c.element = ImVec4(0.18f, 0.20f, 0.35f, 1.0f);
    c.border = ImVec4(0.28f, 0.31f, 0.52f, 1.0f);
    c.border_subtle = ImVec4(0.14f, 0.15f, 0.27f, 1.0f);
    c.border_shadow = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c.text = ImVec4(0.92f, 0.93f, 1.00f, 1.0f);
    c.text_muted = ImVec4(0.55f, 0.58f, 0.80f, 1.0f);
    c.accent = ImVec4(0.56f, 0.40f, 0.96f, 1.0f);
    c.selection = ImVec4(0.56f, 0.40f, 0.96f, 0.35f);
    c.positive = ImVec4(0.35f, 0.85f, 0.60f, 1.0f);
    c.warning = ImVec4(0.98f, 0.76f, 0.26f, 1.0f);
    c.danger = ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
    c.dim = ImVec4(0.0f, 0.0f, 0.0f, 0.45f);
    c.nav_dim = ImVec4(0.5f, 0.5f, 0.8f, 0.15f);
    return c;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static void ColorSwatch(const char* label, ImVec4& col) {
    ImGui::ColorEdit4(label, (float*)&col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    ImGui::SameLine();
    ImGui::TextUnformatted(label);
}

// ---------------------------------------------------------------------------
// Render function — called every frame
// ---------------------------------------------------------------------------
static void RenderFrame(DemoState& s) {
    s.sim_time += ImGui::GetIO().DeltaTime;

    // Full-viewport host window (no decorations)
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGuiWindowFlags host_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    ImGui::Begin("##host", nullptr, host_flags);
    ImGui::PopStyleVar();

    const vog::ThemeColors& tc = vog::GetThemeColors();

    // ---- Tab bar ----
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 10));
    if (ImGui::BeginTabBar("MainTabs")) {
        // ================================================================
        // Tab: Controls
        // ================================================================
        if (ImGui::BeginTabItem("Controls")) {
            ImGui::Spacing();

            // -- vog custom widgets --
            ImGui::SeparatorText("Custom widgets");
            vog::widgets::ToggleButton("Feature A", &s.toggle_a);
            vog::widgets::ShowItemTooltip("Enable or disable feature A");
            ImGui::SameLine(220);
            vog::widgets::ToggleButton("Feature B", &s.toggle_b);
            vog::widgets::ShowItemTooltip("Enable or disable feature B");

            ImGui::Spacing();
            int ci = s.combo_index;
            if (vog::widgets::Combo("Selection", &ci, COMBO_ITEMS, COMBO_COUNT)) s.combo_index = ci;
            ImGui::SameLine();
            vog::widgets::ShowItemTooltip("Pick an option from the list");

            ImGui::Spacing();
            ImGui::SeparatorText("Standard ImGui controls");

            ImGui::Checkbox("Checkbox", &s.checkbox);
            ImGui::SliderFloat("Float slider", &s.slider_f, 0.0f, 1.0f);
            ImGui::SliderInt("Int slider", &s.slider_i, 0, 20);
            ImGui::InputText("Text input", s.input_buf, sizeof(s.input_buf));

            ImGui::Spacing();
            ImGui::SeparatorText("Buttons");
            if (vog::widgets::Button("Normal")) {
            }
            ImGui::SameLine();
            if (vog::widgets::Button("Accent", ImVec4(0.95f, 0.95f, 1.f, 1.f), tc.accent)) {
            }

            ImGui::SameLine();
            if (vog::widgets::Button("Danger", ImVec4(1.f, 0.95f, 0.95f, 1.f), tc.danger)) {
            }

            ImGui::EndTabItem();
        }

        // ================================================================
        // Tab: Status & Data
        // ================================================================
        if (ImGui::BeginTabItem("Status & Data")) {
            ImGui::Spacing();

            // Status badges
            ImGui::SeparatorText("Status indicators");
            ImGui::PushStyleColor(ImGuiCol_Text, tc.positive);
            ImGui::BulletText("All systems operational");
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, tc.warning);
            ImGui::BulletText("Latency elevated (32 ms)");
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, tc.danger);
            ImGui::BulletText("Service B unreachable");
            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::SeparatorText("Animated plot");

            // Build sine wave data
            static float plot_data[90];
            for (int i = 0; i < 90; ++i) {
                float t = s.sim_time + i * 0.07f;
                plot_data[i] = std::sin(t) * 0.5f + std::sin(t * 2.3f) * 0.25f;
            }
            ImGui::PlotLines("##wave", plot_data, 90, 0, nullptr, -1.0f, 1.0f,
                             ImVec2(ImGui::GetContentRegionAvail().x - 12, 60));

            ImGui::Spacing();
            ImGui::SeparatorText("Table");

            if (ImGui::BeginTable(
                    "demo_table", 3,
                    ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame)) {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Value");
                ImGui::TableSetupColumn("Status");
                ImGui::TableHeadersRow();

                const char* rows[][3] = {
                    {"Framerate", "60 fps", "OK"},
                    {"Memory", "128 MB", "OK"},
                    {"GPU temp", "74 C", "Warn"},
                    {"Disk I/O", "N/A", "Error"},
                };
                const ImVec4 row_colors[] = {tc.text, tc.text, tc.warning, tc.danger};

                for (int r = 0; r < 4; ++r) {
                    ImGui::TableNextRow();
                    for (int c = 0; c < 3; ++c) {
                        ImGui::TableSetColumnIndex(c);
                        if (c == 2) ImGui::PushStyleColor(ImGuiCol_Text, row_colors[r]);
                        ImGui::TextUnformatted(rows[r][c]);
                        if (c == 2) ImGui::PopStyleColor();
                    }
                }
                ImGui::EndTable();
            }

            ImGui::EndTabItem();
        }

        // ================================================================
        // Tab: Theme
        // ================================================================
        if (ImGui::BeginTabItem("Theme")) {
            ImGui::Spacing();

            if (ImGui::Button("Use custom \"space\" theme")) {
                s.use_custom_theme = true;
                vog::SetThemeColors(make_custom_theme());
            }
            if (s.use_custom_theme) {
                ImGui::PushStyleColor(ImGuiCol_Text, tc.text_muted);
                ImGui::TextUnformatted("  Custom 'space' theme active. Restart demo to reset.");
                ImGui::PopStyleColor();
            }

            ImGui::Spacing();
            ImGui::SeparatorText("Current palette");
            ImGui::TextUnformatted("Click a color to edit it.");
            ImGui::Spacing();

            vog::ThemeColors editable_theme = vog::GetThemeColors();
            ImVec4* color_ptrs[] = {
                &editable_theme.bg,         &editable_theme.surface, &editable_theme.titlebar,
                &editable_theme.element,    &editable_theme.border,  &editable_theme.text,
                &editable_theme.text_muted, &editable_theme.accent,  &editable_theme.selection,
                &editable_theme.positive,   &editable_theme.warning, &editable_theme.danger,
            };

            const char* names[] = {"bg",         "surface", "titlebar",  "element",  "border",  "text",
                                   "text_muted", "accent",  "selection", "positive", "warning", "danger"};

            int col = 0;
            for (int i = 0; i < 12; ++i) {
                if (col > 0 && col % 2 == 0) ImGui::NewLine();
                if (col % 2 == 1) ImGui::SameLine(220);
                ColorSwatch(names[i], *color_ptrs[i]);
                ++col;
            }

            vog::SetThemeColors(editable_theme);

            ImGui::EndTabItem();
        }

        // ================================================================
        // Tab: Info
        // ================================================================
        if (ImGui::BeginTabItem("Info")) {
            ImGui::Spacing();
            ImGui::SeparatorText("Platform");

#if defined(_WIN32)
            ImGui::BulletText("OS: Windows");
#elif defined(__APPLE__)
            ImGui::BulletText("OS: macOS");
#elif defined(__linux__)
            ImGui::BulletText("OS: Linux");
#else
            ImGui::BulletText("OS: Unknown");
#endif

            ImGuiIO& fio = ImGui::GetIO();
            char fps_buf[64];
            std::snprintf(fps_buf, sizeof(fps_buf), "%.1f fps  (%.2f ms/frame)", fio.Framerate,
                          1000.0f / (fio.Framerate > 0 ? fio.Framerate : 1));
            ImGui::BulletText("%s", fps_buf);
            ImGui::BulletText("Display: %.0f x %.0f", fio.DisplaySize.x, fio.DisplaySize.y);

            ImGui::Spacing();
            ImGui::SeparatorText("vog");
            ImGui::BulletText("Version: %d.%d.%d", VOG_VERSION_MAJOR, VOG_VERSION_MINOR, VOG_VERSION_PATCH);
            ImGui::BulletText("dear imgui: %s", ImGui::GetVersion());

            ImGui::Spacing();
            ImGui::SeparatorText("Collapsible section");
            if (ImGui::TreeNode("More details")) {
                ImGui::TextWrapped(
                    "This demo exercises vog's window management, custom widgets "
                    "(ToggleButton, Combo, ShowItemTooltip), theme system, and "
                    "standard ImGui integration. It should look correct on "
                    "Windows, Linux, and macOS.");
                ImGui::TreePop();
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::PopStyleVar();  // WindowPadding

    ImGui::End();
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------
int main() {
    DemoState state;

    vog::Window window;
    vog::WindowConfig cfg;
    cfg.title = "vog demo";
    cfg.width = 900;
    cfg.height = 560;

    if (!window.Start(cfg, [&state]() { RenderFrame(state); })) {
        return 1;
    }

    // Block until the window is closed
    window.Wait();

    return 0;
}
