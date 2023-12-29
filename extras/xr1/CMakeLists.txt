cmake_minimum_required(VERSION 3.5)
project(xr1_machine_model CXX)
set(xr1_machine_model_VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

find_package(teensy_x86_u8g2_stubs)
include_directories(${teensy_x86_u8g2_stubs_INCLUDE_DIR})

find_package(teensy_x86_keypad_stubs)
include_directories(${teensy_x86_keypad_stubs_INCLUDE_DIR})

find_package(teensy_x86_tlc5947_stubs)
include_directories(${teensy_x86_tlc5947_stubs_INCLUDE_DIR})

set(HEADER_FILES
        XR1Model.h.h)

include_directories(../../xr1)

add_library(teensy_x86_u8g2_stubs_opengl INTERFACE ${HEADER_FILES})
