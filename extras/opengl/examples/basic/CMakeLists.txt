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

#find_package(imgui REQUIRED)
#find_path(STB_INCLUDE_DIRS "stb.h")

include_directories(../../src)
include_directories(../../imgui)
include_directories(../../../xr1)

add_definitions(-fms-extensions)
add_definitions(-DArduino=146)

add_executable(basic graphicstest.cpp)
#target_include_directories(basic ${STB_INCLUDE_DIRS})

target_link_libraries(basic ${teensy_x86_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_u8g2_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_u8g2_stubs_opengl_LIBS})
target_link_libraries(basic ${teensy_x86_twowire_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_keypad_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_mpr121_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_tlc5947_stubs_LIBS})
include(../../opengl.cmake.in)
target_link_libraries(basic ${LIBS})
target_link_libraries(basic -L../../../lib imgui)

if  (APPLE)
        target_link_libraries(basic
                "-framework CoreServices"
                "-framework CoreAudio"
                "-framework Foundation"
                "-framework AudioUnit")
endif()