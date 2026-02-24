#ifdef __linux__

#include "platform.h"

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

#include <cstdio>
#include <cstring>

namespace vog {

bool is_system_dark_mode() {
    const char* gtk_theme = std::getenv("GTK_THEME");
    if (gtk_theme && std::string(gtk_theme).find("dark") != std::string::npos) {
        return true;
    }
    FILE* pipe = popen("gsettings get org.gnome.desktop.interface gtk-theme 2>/dev/null", "r");
    if (pipe) {
        char buf[256];
        if (std::fgets(buf, sizeof(buf), pipe)) {
            std::string theme(buf);
            pclose(pipe);
            return theme.find("dark") != std::string::npos || theme.find("Dark") != std::string::npos;
        }
        pclose(pipe);
    }
    return false;
}

void apply_dark_titlebar(GLFWwindow* window, bool dark) {
    Display* display = glfwGetX11Display();
    ::Window x11_window = glfwGetX11Window(window);
    if (display && x11_window) {
        Atom gtk_theme_variant = XInternAtom(display, "_GTK_THEME_VARIANT", False);
        Atom utf8_string = XInternAtom(display, "UTF8_STRING", False);
        const char* variant = dark ? "dark" : "light";
        XChangeProperty(display, x11_window, gtk_theme_variant, utf8_string, 8, PropModeReplace,
                        reinterpret_cast<const unsigned char*>(variant), strlen(variant));
        XFlush(display);
    }
}

}  // namespace vog

#endif  // __linux__
