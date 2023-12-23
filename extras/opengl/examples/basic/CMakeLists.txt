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

include_directories(../../src)

add_definitions(-fms-extensions)
add_definitions(-DArduino=146)

add_executable(basic graphicstest.cpp)

target_link_libraries(basic ${teensy_x86_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_u8g2_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_u8g2_stubs_opengl_LIBS})
target_link_libraries(basic ${teensy_x86_twowire_stubs_LIBS})
target_link_libraries(basic ${teensy_x86_keypad_stubs_LIBS})

include(../../opengl.cmake.in)
target_link_libraries(basic ${LIBS})

if  (APPLE)
        target_link_libraries(basic
                "-framework CoreServices"
                "-framework CoreAudio"
                "-framework Foundation"
                "-framework AudioUnit")
endif()