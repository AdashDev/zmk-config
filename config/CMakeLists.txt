# CMakeLists.txt for custom ZMK modules
# This file enables the custom cat display module

if(CONFIG_ZMK_DISPLAY)
    target_sources(app PRIVATE cat_display.c)
    target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()