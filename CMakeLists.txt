cmake_minimum_required(VERSION 3.15...3.25)
if(APPLE)
    project(vog VERSION 1.1.11 LANGUAGES CXX OBJC OBJCXX)
else()
    project(vog VERSION 1.1.11 LANGUAGES CXX)
endif()

option(VOG_BUILD_EXAMPLES "Build vog example applications" OFF)
option(VOG_WAYLAND "Build with Wayland support (requires wayland-scanner and wayland-protocols dev packages)" OFF)

include(FetchContent)

# GLFW
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
if(UNIX AND NOT APPLE)
    if(VOG_WAYLAND)
        set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
    else()
        set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    endif()
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(glfw)

if(UNIX AND NOT APPLE)
    set_target_properties(glfw PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Dear ImGui
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.7)
FetchContent_MakeAvailable(imgui)

add_compile_definitions(
    VOG_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    VOG_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    VOG_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

set(VOG_SOURCES
    src/window.cpp
    src/theme.hpp
    src/platform_windows.cpp
    src/platform_linux.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

if(APPLE)
    list(APPEND VOG_SOURCES src/platform_macos.mm)
    set_source_files_properties(src/platform_macos.mm PROPERTIES
        LANGUAGE OBJCXX)
endif()

add_library(vog STATIC ${VOG_SOURCES})

if(UNIX AND NOT APPLE)
    set_target_properties(vog PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(APPLE)
    target_compile_definitions(vog PRIVATE GL_SILENCE_DEPRECATION)
endif()

target_include_directories(vog PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_include_directories(vog PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(OpenGL REQUIRED)
target_link_libraries(vog PUBLIC glfw OpenGL::GL)

if(WIN32)
    target_link_libraries(vog PUBLIC dwmapi)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
    target_link_libraries(vog PUBLIC
        ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
elseif(UNIX)
    find_package(X11)
    if(X11_FOUND)
        target_link_libraries(vog PUBLIC ${X11_LIBRARIES})
    endif()
endif()

target_compile_features(vog PUBLIC cxx_std_17)

if(VOG_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
