set(MCU_DIR "${CMAKE_SOURCE_DIR}/mcu/stm32f4xx")

target_sources(${PROJECT_NAME} PRIVATE
	"${MCU_DIR}/startup_stm32f407xx.s"	
	"${MCU_DIR}/system_stm32f4xx.c"
)

set(MCU_LIB_CMSIS 
	"${CMAKE_SOURCE_DIR}/drivers/CMSIS/Device/ST/STM32F4xx/Include"
)

set(MCU_COMPILE_DEFINITIONS STM32F407xx)

set(COMPILER_FLAGS 
	"-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fdata-sections -ffunction-sections"
)

set(CMAKE_ASM_FLAGS			"${COMPILER_FLAGS}" CACHE INTERNAL "")
set(CMAKE_C_FLAGS			"${COMPILER_FLAGS}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS			"${COMPILER_FLAGS}" CACHE INTERNAL "")

set(MCU_LINKER_FILE "${MCU_DIR}/STM32F407XX_FLASH.ld")

set(CMAKE_EXE_LINKER_FLAGS 
	"-Wl,--gc-sections -T ${MCU_LINKER_FILE}" CACHE INTERNAL ""
)

