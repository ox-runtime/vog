#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "fonts.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "platform.h"
#include "theme.hpp"
#include "vog.h"

namespace vog {

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
    should_stop_.store(false);

#if defined(__APPLE__)
    // macOS requires the render loop on the main thread.
    // Start() blocks until the window closes or Stop() is called.
    running_.store(true);
    RenderLoop(config, std::move(render_frame));
    running_.store(false);
    return true;
#else
    running_.store(true);
    try {
        render_thread_ = std::thread(&Window::RenderLoop, this, config, std::move(render_frame));
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

bool Window::InitializeGraphics(const WindowConfig& config) {
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

    window_ = glfwCreateWindow(config.width, config.height, config.title, nullptr, nullptr);
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

    setup_fonts(io, window_);
    setup_theme();

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

void Window::RenderLoop(const WindowConfig& config, std::function<void()> render_frame) {
    if (!InitializeGraphics(config)) {
        running_.store(false);
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    while (!should_stop_.load() && !glfwWindowShouldClose(window_)) {
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        int display_w, display_h;
        glfwGetFramebufferSize(window_, &display_w, &display_h);
        float xscale, yscale;
        glfwGetWindowContentScale(window_, &xscale, &yscale);
        io.FontGlobalScale = 1.0f / ((xscale + yscale) * 0.5f);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        render_frame();

        ImGui::Render();

        const ThemeColors& tc = GetThemeColors();
        glViewport(0, 0, display_w, display_h);
        glClearColor(tc.bg.x, tc.bg.y, tc.bg.z, tc.bg.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window_);
    }

    CleanupGraphics();
    running_.store(false);
}

}  // namespace vog
