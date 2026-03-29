cmake_minimum_required(VERSION 3.10)
project(rtos)

add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    os.c
    flobal.c
    resource.c
    task.c
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
