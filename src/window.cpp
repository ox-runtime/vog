#include <GLFW/glfw3.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#include "fonts.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "platform.h"
#include "theme.hpp"
#include "vog.h"

namespace vog {

Theme Window::theme_;

static void glfw_error_callback(int error, const char* description) {
    std::cerr << "vog: GLFW error " << error << ": " << description << std::endl;
}

// ---- Window public API ----

Window::~Window() { Stop(); }

bool Window::Start(const WindowConfig& config, std::function<void()> render_frame) {
    if (running_.load()) {
        std::cerr << "vog::Window::Start: already running" << std::endl;
        return false;
    }
    this->config_ = &config;

    should_stop_.store(false);

#if defined(__APPLE__)
    // macOS requires the render loop on the main thread.
    // Start() blocks until the window closes or Stop() is called.
    running_.store(true);
    RenderLoop(std::move(render_frame));
    running_.store(false);
    return true;
#else
    running_.store(true);
    try {
        render_thread_ = std::thread(&Window::RenderLoop, this, std::move(render_frame));
        return true;
    } catch (const std::exception& e) {
        std::cerr << "vog::Window::Start: " << e.what() << std::endl;
        running_.store(false);
        return false;
    }
#endif
}

void Window::Stop() {
    if (!running_.load()) return;
    should_stop_.store(true);
    if (render_thread_.joinable()) render_thread_.join();
    running_.store(false);
}

void Window::Wait() {
    // On macOS the render loop ran on the calling thread inside Start(), so
    // render_thread_ was never started — joinable() is false and this is a
    // no-op.  On Windows/Linux Start() spawned a background thread; joining
    // it here blocks until the window closes naturally.
    if (render_thread_.joinable()) render_thread_.join();
}

// ---- Private implementation ----

bool Window::InitializeGraphics() {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        std::cerr << "vog: failed to initialize GLFW" << std::endl;
        return false;
    }

#if defined(__APPLE__)
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    window_ = glfwCreateWindow(config_->width, config_->height, config_->title, nullptr, nullptr);
    if (!window_) {
        std::cerr << "vog: failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    apply_dark_titlebar(window_, is_system_dark_mode());

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // SetTheme resolves any nullopt fields from system defaults, so passing
    // an empty Theme (or a partially-filled user Theme) always works correctly.
    SetTheme(config_->theme.value_or(Theme{}));

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
        std::cerr << "vog: failed to initialize ImGui OpenGL3 backend" << std::endl;
        ImGui::DestroyContext();
        glfwDestroyWindow(window_);
        window_ = nullptr;
        glfwTerminate();
        return false;
    }

    const GLubyte* ver = glGetString(GL_VERSION);
    const GLubyte* rend = glGetString(GL_RENDERER);
    std::cout << "vog: OpenGL " << (ver ? reinterpret_cast<const char*>(ver) : "?") << " | "
              << (rend ? reinterpret_cast<const char*>(rend) : "?") << std::endl;

    return true;
}

void Window::CleanupGraphics() {
    if (window_) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window_);
        window_ = nullptr;
        glfwTerminate();
    }
}

void Window::RenderFrameNow() {
    if (!active_render_frame_ || !window_) return;

    int display_w, display_h;
    glfwGetFramebufferSize(window_, &display_w, &display_h);
    if (display_w == 0 || display_h == 0) return;

    ThemeVars theme_vars = GetTheme().vars;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (config_->createDefaultImGuiWindow) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGuiWindowFlags host_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                      ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, theme_vars.window_padding.value());
        ImGui::Begin("##host", nullptr, host_flags);
        ImGui::PopStyleVar();
    }

    active_render_frame_();  // render the user's UI

    if (config_->createDefaultImGuiWindow) {
        ImGui::End();
    }

    ImGui::Render();

    const ThemeColors& tc = GetTheme().colors;
    glViewport(0, 0, display_w, display_h);
    glClearColor(tc.bg.value().x, tc.bg.value().y, tc.bg.value().z, tc.bg.value().w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window_);
}

inline float getContentScale(GLFWwindow* window) {
    float xs, ys;
    glfwGetWindowContentScale(window, &xs, &ys);
    return (xs + ys) * 0.5f;
}

void Window::RenderLoop(std::function<void()> render_frame) {
    if (!InitializeGraphics()) {
        running_.store(false);
        return;
    }

    // Store for use in resize/refresh callbacks
    active_render_frame_ = render_frame;

    // Re-render immediately during window resize/refresh for live layout updates.
    // On Windows the OS enters a modal resize loop inside glfwPollEvents(), and
    // these callbacks fire on the same thread, so it is safe to call RenderFrameNow().
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* w, int, int) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        if (self) self->RenderFrameNow();
    });

    ImGuiIO& io = ImGui::GetIO();

    last_content_scale_ = getContentScale(window_);
    last_global_font_size_ = -1.0f;  // force font atlas build on first loop iteration

    while (!should_stop_.load() && !glfwWindowShouldClose(window_)) {
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        // Rebuild the font atlas if the content scale changed (window moved
        // to a display with a different DPI), or theme vars changed the font size
        float current_content_scale = getContentScale(window_);
        bool content_scale_changed = std::abs(current_content_scale - last_content_scale_) > 0.01f;

        float current_global_font_size = GetTheme().vars.font_size.value();
        bool theme_font_size_changed = std::abs(current_global_font_size - last_global_font_size_) > 0.01f;

        if (content_scale_changed || theme_font_size_changed) {
            last_content_scale_ = current_content_scale;
            last_global_font_size_ = current_global_font_size;
            io.Fonts->Clear();

            setup_fonts();
            ImGui_ImplOpenGL3_CreateFontsTexture();
        }

        RenderFrameNow();
    }

    CleanupGraphics();
    running_.store(false);
}

}  // namespace vog
