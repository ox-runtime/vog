#ifdef _WIN32

#include "platform.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#include <windows.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

namespace vog {

bool is_system_dark_mode() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0,
                      KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD value = 0;
        DWORD size = sizeof(value);
        if (RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr, reinterpret_cast<LPBYTE>(&value), &size) ==
            ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return value == 0;  // 0 = dark mode
        }
        RegCloseKey(hKey);
    }
    return false;
}

void apply_dark_titlebar(GLFWwindow* window, bool dark) {
    HWND hwnd = glfwGetWin32Window(window);
    if (hwnd) {
        BOOL use_dark = dark ? TRUE : FALSE;
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &use_dark, sizeof(use_dark));
    }
}

}  // namespace vog

#endif  // _WIN32
