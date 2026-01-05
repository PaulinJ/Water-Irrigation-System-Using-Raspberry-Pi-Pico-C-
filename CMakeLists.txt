# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# CMake configuration for Raspberry Pi Pico project using I2C and LCD

cmake_minimum_required(VERSION 3.13)

# Set C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Detect user home directory for Pico SDK extension
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

# VS Code Pico SDK integration (do not edit)
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# Specify board type
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK
include(pico_sdk_import.cmake)

# Define project
project(ITCAA_Group_Project C CXX ASM)

# Initialize the SDK
pico_sdk_init()

# Add executable and source files
add_executable(ITCAA_Group_Project
    ITCAA_Group_Project.c
    i2c-display-lib.h
)

# Set program metadata
pico_set_program_name(ITCAA_Group_Project "ITCAA_Group_Project")
pico_set_program_version(ITCAA_Group_Project "0.1")

# Enable/disable USB/UART output
pico_enable_stdio_uart(ITCAA_Group_Project 0)
pico_enable_stdio_usb(ITCAA_Group_Project 0)

# Link required libraries
target_link_libraries(ITCAA_Group_Project
    pico_stdlib
    hardware_i2c
    hardware_adc
    hardware_pwm
    pico_multicore
)

# Include directories
target_include_directories(ITCAA_Group_Project PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/lib  # Adjust if your LCD headers are in a subfolder
)

# Generate UF2 and other outputs
pico_add_extra_outputs(ITCAA_Group_Project)
