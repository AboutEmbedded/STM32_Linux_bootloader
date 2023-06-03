cmake_minimum_required(VERSION 3.0.0)
project(HexLoader VERSION 0.0.1)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/main.c
    ${PROJECT_SOURCE_DIR}/src/parser.c
    ${PROJECT_SOURCE_DIR}/src/uart_RPi.c
)

# Include dirs definition
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/inc
)
