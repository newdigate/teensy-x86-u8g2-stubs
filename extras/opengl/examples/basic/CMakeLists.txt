cmake_minimum_required(VERSION 3.10)
project(teensy_u8g2_opengl_examples_basic C CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

find_package(teensy_x86_twowire_stubs)
include_directories(${teensy_x86_twowire_stubs_INCLUDE_DIR})

find_package(teensy_x86_u8g2_stubs)
include_directories(${teensy_x86_u8g2_stubs_INCLUDE_DIR})

find_package(teensy_x86_keypad_stubs)
include_directories(${teensy_x86_keypad_stubs_INCLUDE_DIR})

find_package(teensy_x86_mpr121_stubs)
include_directories(${teensy_x86_mpr121_stubs_INCLUDE_DIR})

find_package(teensy_x86_tlc5947_stubs)
include_directories(${teensy_x86_tlc5947_stubs_INCLUDE_DIR})

add_subdirectory(../../imgui imgui)

include_directories(../../src)
include_directories(../../imgui)
include_directories(../../../xr1)
include_directories(../../src/include)

add_definitions(-fms-extensions)
add_definitions(-DArduino=146)

add_executable(basic ../../src/libs/stbi/stb_image_aug.c ../../src/libs/stbi/stbi_DDS_aug_c.h graphicstest.cpp)
#target_include_directories(basic ${STB_INCLUDE_DIRS})

target_link_libraries(basic ${teensy_x86_u8g2_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_u8g2_stubs_opengl_LIBS})
target_link_libraries(basic ${teensy_x86_twowire_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_keypad_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_mpr121_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_tlc5947_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_stubs_LIBS})
target_link_libraries(basic assimp)
target_link_libraries(basic -L/usr/local/Cellar/assimp/5.3.1/lib/)

include(../../opengl.cmake.in)
target_link_libraries(basic ${LIBS})
target_link_libraries(basic imgui)

if  (APPLE)
        target_link_libraries(basic
                "-framework CoreServices"
                "-framework CoreAudio"
                "-framework Foundation"
                "-framework AudioUnit")
endif()