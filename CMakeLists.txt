cmake_minimum_required(VERSION 3.28.3)

project(stm32-firmware)
enable_language(C CXX ASM)

# Set the following variable to `ON` to print all cmake variables
set(CMAKE_DUMP_VARIABLES            OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS   ON)

set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)
set(CMAKE_CXX_STANDARD              14)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            ON)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
		src/main.c
		src/system_stm32f4xx.c
		startup/startup_stm32f407vgtx.s
		
		drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
		drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
		drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
		drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
)

set(INCLUDES_LIB_CMSIS
    drivers/CMSIS/Device/ST/STM32F4xx/Include
    drivers/CMSIS/Include
    drivers/STM32F4xx_HAL_Driver/Inc
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

option(CMAKE_BUILD_TYPE "Debug mode" On)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
)