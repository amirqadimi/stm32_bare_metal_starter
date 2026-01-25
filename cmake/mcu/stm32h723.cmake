set(MCU_DIR "${CMAKE_SOURCE_DIR}/mcu/stm32h7xx")

target_sources(${PROJECT_NAME} PRIVATE
	"${MCU_DIR}/startup_stm32h723xx.s"	
	"${MCU_DIR}/system_stm32h7xx.c"
	"${CMAKE_SOURCE_DIR}/src/led_h723.c"
)

set(MCU_LIB_CMSIS 
	"${CMAKE_SOURCE_DIR}/drivers/CMSIS/Device/ST/STM32H7xx/Include"
)

set(MCU_COMPILE_DEFINITIONS STM32H723xx)

set(COMPILER_FLAGS 
	"-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -fdata-sections -ffunction-sections"
)

set(CMAKE_ASM_FLAGS			"${COMPILER_FLAGS}" CACHE INTERNAL "")
set(CMAKE_C_FLAGS			"${COMPILER_FLAGS}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS			"${COMPILER_FLAGS}" CACHE INTERNAL "")

set(MCU_LINKER_FILE "${MCU_DIR}/STM32H723XG_FLASH.ld")

set(CMAKE_EXE_LINKER_FLAGS 
	"-Wl,--gc-sections -T ${MCU_LINKER_FILE}" CACHE INTERNAL ""
)