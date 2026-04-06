cmake_minimum_required(VERSION 3.10)
project(rtos LANGUAGES C ASM)

add_library(${PROJECT_NAME} STATIC
    os.c
    global.c
    task.c
    events.c
    linux-x86-64.s
)

add_executable(test main.c)

target_link_libraries(test PRIVATE ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR})
