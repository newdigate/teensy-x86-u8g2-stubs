This file is compiled from imgui with the following cmake file
(at the root of the imgui folder)

```cmake
cmake_minimum_required(VERSION 3.10)
project(imgui C CXX)
set(CMAKE_CXX_STANDARD 11)

include(opengl.cmake.in)
include_directories(.)
add_library(imgui
        imgui.cpp
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        backends/imgui_impl_glfw.cpp
        backends/imgui_impl_opengl3.cpp
)
```