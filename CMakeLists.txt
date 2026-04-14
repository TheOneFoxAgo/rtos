cmake_minimum_required(VERSION 3.10)
project(rtos LANGUAGES C ASM)

option(ENABLE_DEBUG_LOG "Debug log" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME} main.c)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
    os.c
    global.c
    resource.c
    task.c
    events.c
    linux-x86-64.s
)

if (ENABLE_DEBUG_LOG)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()