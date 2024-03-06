cmake_minimum_required(VERSION 3.5)
project(teensy_x86_u8g2_stubs_opengl CXX)
set(teensy_x86_u8g2_stubs_opengl_VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

find_package(teensy_x86_u8g2_stubs)
include_directories(${teensy_x86_u8g2_stubs_INCLUDE_DIR})

find_package(teensy_x86_keypad_stubs)
include_directories(${teensy_x86_keypad_stubs_INCLUDE_DIR})

find_package(teensy_x86_tlc5947_stubs)
include_directories(${teensy_x86_tlc5947_stubs_INCLUDE_DIR})

include(../opengl.cmake.in)

set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/teensy_x86_u8g2_stubs_opengl/)
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/teensy_x86_u8g2_stubs_opengl )

set(SOURCE_FILES
        libs/stbi/stb_image_aug.c
        ../imgui/imgui.cpp
        ../imgui/imgui_demo.cpp
        ../imgui/imgui_draw.cpp
        ../imgui/imgui_tables.cpp
        ../imgui/imgui_widgets.cpp
        ../imgui/imgui_impl_glfw.cpp
        ../imgui/imgui_impl_opengl3.cpp)

set(HEADER_FILES
        u8g2_opengl.h
        u8g2_opengl_main.h
        u8g2_opengl_serial_log.h
        ../../xr1/XR1Model.h
        ../imgui/imgui_impl_opengl3.h
        ../imgui/imgui.h
        ../imgui/imgui_internal.h
        ../imgui/imstb_textedit.h
        ../imgui/imconfig.h
        ../imgui/imgui_impl_glfw.h)

set(OTHER_HEADER_FILES
        include/animation.h
        include/animator.h
        include/animdata.h
        include/assimp_glm_helpers.h
        include/bone.h
        include/camera.h
        include/entity.h
        include/filesystem.h
        include/mesh.h
        include/model.h
        include/model_animation.h
        include/shader.h
        include/shader_c.h
        include/shader_m.h
        include/shader_s.h
        include/shader_t.h
        include/stb_image.h
        include/stbi_DDS_aug.h
        include/stb_image_aug.h)

include_directories(../imgui)
include_directories(../../xr1)

add_library(teensy_x86_u8g2_stubs_opengl STATIC ${SOURCE_FILES} ${HEADER_FILES})

set_target_properties(teensy_x86_u8g2_stubs_opengl PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

include(CMakePackageConfigHelpers)
configure_package_config_file(../cmake/teensy_x86_u8g2_stubs_opengl.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/teensy_x86_u8g2_stubs_openglConfig.cmake
        INSTALL_DESTINATION ${LIB_INSTALL_DIR}/teensy_x86_u8g2_stubs_opengl/cmake
        PATH_VARS)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/teensy_x86_u8g2_stubs_openglConfigVersion.cmake
        VERSION ${teensy_x86_u8g2_stubs_opengl_VERSION}
        COMPATIBILITY SameMajorVersion )

install(TARGETS teensy_x86_u8g2_stubs_opengl DESTINATION
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
        PUBLIC_HEADER DESTINATION "include/teensy_x86_u8g2_stubs_opengl"
        )

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/teensy_x86_u8g2_stubs_openglConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/teensy_x86_u8g2_stubs_openglConfigVersion.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/uninstall.cmake
        DESTINATION "lib/cmake/teensy_x86_u8g2_stubs_opengl" )

install(FILES ${OTHER_HEADER_FILES}
        DESTINATION "/usr/local/include/teensy_x86_u8g2_stubs_opengl/include" )
##
# copied from: https://gist.github.com/royvandam/3033428
# Add uninstall target
# Requirements: Copy the uninstall.cmake file to the appropriate CMAKE_MODULE_PATH.
add_custom_target(uninstall
        "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/uninstall.cmake")