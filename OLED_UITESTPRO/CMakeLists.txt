cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME OLED_UITESTPRO)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

file(GLOB_RECURSE UI_U8G2_SOURCE "UI_u8g2/*c")
# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
        ${UI_U8G2_SOURCE}
        Hardware/oled/OLED.c
        Hardware/ws2812/ws2812.c
        Hardware/rtctimer/RTCtimer.c


        astra/app/SerialMonitor_app.cpp
        astra/app/Setting_app.cpp
        astra/app/Clock_app.cpp
        astra/ui/element/menu.cpp
        astra/ui/element/widget.cpp
        astra/ui/element/camera.cpp
        astra/ui/element/selector.cpp

        astra/ui/launcher.cpp
        astra/astra_logo.cpp
        astra/astra_rocket.cpp
        hal/hal.cpp
        hal/Myhal.cpp


)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        UI_u8g2
        Hardware/oled
        Hardware/ws2812
        Hardware/rtctimer
        hal
        astra
        astra/app
        astra/config
        astra/ui
        astra/ui/element



)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
add_custom_command(TARGET OLED_UITESTPRO POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:OLED_UITESTPRO> ${CMAKE_BINARY_DIR}/OLED_UITESTPRO.hex
        COMMENT "生成HEX文件: ${CMAKE_BINARY_DIR}/OLED_UITESTPRO.hex"
)