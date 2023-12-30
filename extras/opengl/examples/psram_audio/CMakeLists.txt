cmake_minimum_required(VERSION 3.10)
project(teensy_u8g2_opengl_examples_psram_audio C CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

find_package(teensy_x86_sd_stubs)
include_directories(${teensy_x86_sd_stubs_INCLUDE_DIR})

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

find_package(teensy_audio_x86_stubs)
include_directories(${teensy_audio_x86_stubs_INCLUDE_DIR})

find_package(teensy_audio_x86_stubs_soundio)
include_directories(${teensy_audio_x86_stubs_soundio_INCLUDE_DIR})

find_package(teensy_sampleflashloader)
include_directories(${teensy_sampleflashloader_INCLUDE_DIR})

find_package(teensy_variable_playback)
include_directories(${teensy_variable_playback_INCLUDE_DIR})

include_directories(/usr/local/include)

add_subdirectory(../../imgui imgui)

include_directories(../../src)
include_directories(../../imgui)
include_directories(../../../xr1)

add_definitions(-fms-extensions)
add_definitions(-DArduino=146)
add_definitions(-DBUILD_FOR_LINUX)
add_executable(psram_audio psram_audio.cpp)

target_link_libraries(psram_audio ${teensy_x86_stubs_LIBS})
target_link_libraries(psram_audio ${teensy_x86_u8g2_stubs_LIBS})
target_link_libraries(psram_audio ${teensy_x86_u8g2_stubs_opengl_LIBS})
target_link_libraries(psram_audio ${teensy_x86_twowire_stubs_LIBS})
target_link_libraries(psram_audio ${teensy_x86_keypad_stubs_LIBS})
target_link_libraries(psram_audio ${teensy_x86_mpr121_stubs_LIBS})
target_link_libraries(psram_audio ${teensy_x86_tlc5947_stubs_LIBS})
target_link_libraries(psram_audio ${teensy_audio_x86_stubs_LIBS})
target_link_libraries(psram_audio ${teensy_audio_x86_stubs_soundio_LIBS})
target_link_libraries(psram_audio ${teensy_sampleflashloader_LIBS})
target_link_libraries(psram_audio ${teensy_variable_playback_LIBS})
target_link_libraries(psram_audio ${teensy_x86_sd_stubs_LIBS})
target_link_libraries(psram_audio "-L/usr/local/lib -lsoundio")

include(../../opengl.cmake.in)
target_link_libraries(psram_audio ${LIBS})
target_link_libraries(psram_audio imgui)

if  (APPLE)
        target_link_libraries(psram_audio
                "-framework CoreServices"
                "-framework CoreAudio"
                "-framework Foundation"
                "-framework AudioUnit")
endif()