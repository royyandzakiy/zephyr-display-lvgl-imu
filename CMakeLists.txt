cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(zephyr-display-lvgl)

target_include_directories(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

FILE(GLOB app_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
target_sources(app PRIVATE ${app_sources})