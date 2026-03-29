cmake_minimum_required(VERSION 3.10)
project(rtos)

add_library(${PROJECT_NAME} STATIC
    os.c
    global.c
    resource.c
    task.c
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
