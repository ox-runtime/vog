#ifdef __APPLE__

#import <Cocoa/Cocoa.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "platform.h"

namespace vog {

bool is_system_dark_mode() {
    if (@available(macOS 10.14, *)) {
        NSString* mode = [[NSUserDefaults standardUserDefaults] stringForKey:@"AppleInterfaceStyle"];
        return [mode isEqualToString:@"Dark"];
    }
    return false;
}

void apply_dark_titlebar(GLFWwindow* window, bool dark) {
    NSWindow* nsWindow = glfwGetCocoaWindow(window);
    if (nsWindow) {
        if (@available(macOS 10.14, *)) {
            nsWindow.appearance = [NSAppearance appearanceNamed:dark ? NSAppearanceNameDarkAqua
                                                                     : NSAppearanceNameAqua];
        }
    }
}

}  // namespace vog

#endif  // __APPLE__
