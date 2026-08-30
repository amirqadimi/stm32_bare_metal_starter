cmake_minimum_required(VERSION 3.28.3)

set(PROJECT_NAME "firmware")

project(${PROJECT_NAME})

enable_language(C ASM)

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
)

include(${CMAKE_SOURCE_DIR}/cmake/mcu-type.cmake)
include(${MCU_CONFIG_FILE})

message(STATUS "MCU type is \"${MCU_TYPE}\". Compile definition is \"${MCU_COMPILE_DEFINITIONS}\"")

set(INCLUDES_LIB_CMSIS
	${MCU_LIB_CMSIS}
	${CMAKE_SOURCE_DIR}/drivers/CMSIS/Include
)

add_library(mcu_config INTERFACE)

target_include_directories(mcu_config INTERFACE
	${INCLUDES_LIB_CMSIS}
	${CMAKE_SOURCE_DIR}/include
)

target_compile_definitions(mcu_config
	INTERFACE
	${MCU_COMPILE_DEFINITIONS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	mcu_config
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
	COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
	COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
)
