cmake_minimum_required(VERSION 3.28.3)

set(PROJECT_NAME "stm32-firmware")

project(${PROJECT_NAME})
enable_language(C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS   ON)

set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
		src/main.c
		src/system_stm32f4xx.c
		startup/startup_stm32f407xx.s
)

set(INCLUDES_LIB_CMSIS
    drivers/CMSIS/Device/ST/STM32F4xx/Include
    drivers/CMSIS/Include
)

add_library(lib INTERFACE)

target_include_directories(lib INTERFACE
    ${INCLUDES_LIB_CMSIS}
    src
)

target_compile_definitions(lib INTERFACE
    STM32F407xx
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    lib
)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
)