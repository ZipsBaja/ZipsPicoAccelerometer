cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_BOARD pico)
#set(PICO_BOARD pico_w)
#set(PICO_BOARD pico2)

# If pico2 is selected you have to select PICO_PLATFORM
# set(PICO_PLATFORM rp2350-arm-s)
set(PICO_PLATFORM rp2040)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(Accelerometer C CXX ASM)

pico_sdk_init()

if(PICO_BOARD STREQUAL "pico")
add_compile_definitions(USING_PICO)
elseif(PICO_BOARD STREQUAL "pico_w")
add_compile_definitions(USING_PICO_W)
elseif(PICO_BOARD STREQUAL "pico2")
add_compile_definitions(USING_PICO_2)
endif()

add_subdirectory(lib/mpulib)
add_subdirectory(lib/sdlib/FatFs_SPI build)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp src/sd_conf.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}
    pico_stdlib
    haw_mpu6050
    FatFs_SPI
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    include
    lib/mpulib/include 
    lib/sdlib/FatFs_SPI/include 
    lib/sdlib/FatFs_SPI/driver
    $ENV{PICO_SDK_PATH}/src/rp2_common/hardware_i2c/include
)

pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 1)
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

# Export path to project executable to file. File is used in vscode tasks (task.json and launch.json)
set(ELF_PATH "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_FILE}${CMAKE_PROJECT_NAME}.elf")
file(WRITE "${CMAKE_BINARY_DIR}/elf_path.txt" "${ELF_PATH}\n")
