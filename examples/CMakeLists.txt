cmake_minimum_required(VERSION 3.27)
project(vog_examples)

add_executable(vog_demo demo_window.cpp)

target_link_libraries(vog_demo PRIVATE vog)

# Place the executable next to the source for easy discovery
set_target_properties(vog_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)
