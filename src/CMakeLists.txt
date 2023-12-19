cmake_minimum_required(VERSION 3.5)
project(teensy_x86_u8g2_stubs.c C CXX)
set(teensy_x86_u8g2_stubs_VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)

set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/teensy_x86_u8g2_stubs/)
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/teensy_x86_u8g2_stubs )

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})
include_directories(csrc)
add_compile_definitions(U8X8_NO_HW_SPI U8X8_NO_HW_I2C)

set(SOURCE_FILES
	csrc/mui_u8g2.c
 	csrc/mui_u8g2.h csrc/mui.c
	csrc/mui.h csrc/u8g2_arc.c
	csrc/u8g2_bitmap.c
	csrc/u8g2_box.c
	csrc/u8g2_buffer.c
	csrc/u8g2_button.c
	csrc/u8g2_circle.c
	csrc/u8g2_cleardisplay.c
	csrc/u8g2_d_memory.c
	csrc/u8g2_d_setup.c
	csrc/u8g2_font.c
	csrc/u8g2_fonts.c
	csrc/u8g2_hvline.c
	csrc/u8g2_input_value.c
	csrc/u8g2_intersection.c
	csrc/u8g2_kerning.c
	csrc/u8g2_line.c
	csrc/u8g2_ll_hvline.c
	csrc/u8g2_message.c
	csrc/u8g2_polygon.c
	csrc/u8g2_selection_list.c
	csrc/u8g2_setup.c
	csrc/u8g2.h csrc/u8log_u8g2.c
	csrc/u8log_u8x8.c
	csrc/u8log.c
	csrc/u8x8_8x8.c
	csrc/u8x8_byte.c
	csrc/u8x8_cad.c
	csrc/u8x8_capture.c
	csrc/u8x8_d_a2printer.c
	csrc/u8x8_d_gp1247ai.c
	csrc/u8x8_d_gp1287ai.c
	csrc/u8x8_d_gp1294ai.c
	csrc/u8x8_d_gu800.c
	csrc/u8x8_d_hd44102.c
	csrc/u8x8_d_il3820_296x128.c
	csrc/u8x8_d_ist3020.c
	csrc/u8x8_d_ist3088.c
	csrc/u8x8_d_ist7920.c
	csrc/u8x8_d_ks0108.c
	csrc/u8x8_d_lc7981.c
	csrc/u8x8_d_ld7032_60x32.c
	csrc/u8x8_d_ls013b7dh03.c
	csrc/u8x8_d_max7219.c
	csrc/u8x8_d_pcd8544_84x48.c
	csrc/u8x8_d_pcf8812.c
	csrc/u8x8_d_pcf8814_hx1230.c
	csrc/u8x8_d_s1d15e06.c
	csrc/u8x8_d_s1d15300.c
	csrc/u8x8_d_s1d15721.c
	csrc/u8x8_d_sbn1661.c
	csrc/u8x8_d_sed1330.c
	csrc/u8x8_d_sh1106_64x32.c
	csrc/u8x8_d_sh1106_72x40.c
	csrc/u8x8_d_sh1107.c
	csrc/u8x8_d_sh1108.c
	csrc/u8x8_d_sh1122.c
	csrc/u8x8_d_ssd1305.c
	csrc/u8x8_d_ssd1306_48x64.c
	csrc/u8x8_d_ssd1306_64x32.c
	csrc/u8x8_d_ssd1306_64x48.c
	csrc/u8x8_d_ssd1306_72x40.c
	csrc/u8x8_d_ssd1306_96x16.c
	csrc/u8x8_d_ssd1306_96x40.c
	csrc/u8x8_d_ssd1306_128x32.c
	csrc/u8x8_d_ssd1306_128x64_noname.c
	csrc/u8x8_d_ssd1306_2040x16.c
	csrc/u8x8_d_ssd1309.c
	csrc/u8x8_d_ssd1316.c
	csrc/u8x8_d_ssd1317.c
	csrc/u8x8_d_ssd1318.c
	csrc/u8x8_d_ssd1320.c
	csrc/u8x8_d_ssd1322.c
	csrc/u8x8_d_ssd1325.c
	csrc/u8x8_d_ssd1326.c
	csrc/u8x8_d_ssd1327.c
	csrc/u8x8_d_ssd1329.c
	csrc/u8x8_d_ssd1362.c
	csrc/u8x8_d_ssd1606_172x72.c
	csrc/u8x8_d_ssd1607_200x200.c
	csrc/u8x8_d_st7511.c
	csrc/u8x8_d_st7528.c
	csrc/u8x8_d_st7539.c
	csrc/u8x8_d_st7565.c
	csrc/u8x8_d_st7567.c
	csrc/u8x8_d_st7571.c
	csrc/u8x8_d_st7586s_erc240160.c
	csrc/u8x8_d_st7586s_jlx320160.c
	csrc/u8x8_d_st7586s_jlx384160.c
	csrc/u8x8_d_st7586s_s028hn118a.c
	csrc/u8x8_d_st7586s_ymc240160.c
	csrc/u8x8_d_st7588.c
	csrc/u8x8_d_st7920.c
	csrc/u8x8_d_st75160.c
	csrc/u8x8_d_st75256.c
	csrc/u8x8_d_st75320.c
	csrc/u8x8_d_stdio.c
	csrc/u8x8_d_t6963.c
	csrc/u8x8_d_uc1601.c
	csrc/u8x8_d_uc1604.c
	csrc/u8x8_d_uc1608.c
	csrc/u8x8_d_uc1609.c
	csrc/u8x8_d_uc1610.c
	csrc/u8x8_d_uc1611.c
	csrc/u8x8_d_uc1617.c
	csrc/u8x8_d_uc1628.c
	csrc/u8x8_d_uc1638.c
	csrc/u8x8_d_uc1701_dogs102.c
	csrc/u8x8_d_uc1701_mini12864.c
	csrc/u8x8_debounce.c
	csrc/u8x8_display.c
	csrc/u8x8_fonts.c
	csrc/u8x8_gpio.c
	csrc/u8x8_input_value.c
	csrc/u8x8_message.c
	csrc/u8x8_selection_list.c
	csrc/u8x8_setup.c
	csrc/u8x8_string.c
	csrc/u8x8_u8toa.c
	csrc/u8x8_u16toa.c
	cppsrc/U8g2lib.cpp 
	cppsrc/U8x8lib.cpp
)

set(HEADER_FILES
	csrc/u8x8.h
	csrc/u8g2.h
	cppsrc/MUIU8g2.h 
	cppsrc/U8g2lib.h
	cppsrc/U8x8lib.h
)

add_definitions(-DARDUINO=146)
add_library(teensy_x86_u8g2_stubs SHARED STATIC ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(teensy_x86_u8g2_stubs PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

include(CMakePackageConfigHelpers)
configure_package_config_file(../cmake/teensy_x86_u8g2_stubs.cmake.in
		${CMAKE_CURRENT_BINARY_DIR}/teensy_x86_u8g2_stubsConfig.cmake
		INSTALL_DESTINATION ${LIB_INSTALL_DIR}/teensy_x86_u8g2_stubs/cmake
		PATH_VARS)

write_basic_package_version_file(
		${CMAKE_CURRENT_BINARY_DIR}/teensy_x86_u8g2_stubsConfigVersion.cmake
		VERSION ${teensy_x86_u8g2_stubs_VERSION}
		COMPATIBILITY SameMajorVersion )

install(TARGETS teensy_x86_u8g2_stubs DESTINATION
		LIBRARY DESTINATION ${LIB_INSTALL_DIR}
		ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
		PUBLIC_HEADER DESTINATION "include/teensy_x86_u8g2_stubs"
		)

install(FILES
		${CMAKE_CURRENT_BINARY_DIR}/teensy_x86_u8g2_stubsConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/teensy_x86_u8g2_stubsConfigVersion.cmake
		../cmake/uninstall.cmake
		DESTINATION "lib/cmake/teensy_x86_u8g2_stubs" )

##
# copied from: https://gist.github.com/royvandam/3033428
# Add uninstall target
# Requirements: Copy the uninstall.cmake file to the appropriate CMAKE_MODULE_PATH.
#

add_custom_target(uninstall
		"${CMAKE_COMMAND}" -P "../cmake/uninstall.cmake"
		)