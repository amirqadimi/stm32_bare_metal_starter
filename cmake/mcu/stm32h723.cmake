set(MCU_DIR "${CMAKE_SOURCE_DIR}/mcu/stm32h7xx")
set(PLATFORM_DIR "${CMAKE_SOURCE_DIR}/platform/stm32h723")

target_sources(${PROJECT_NAME} PRIVATE
	"${MCU_DIR}/startup_stm32h723xx.s"	
	"${MCU_DIR}/system_stm32h7xx.c"
	"${PLATFORM_DIR}/init_stm32h723.c"
	"${PLATFORM_DIR}/delay.c"
	"${PLATFORM_DIR}/toggle_led.c"
	"${PLATFORM_DIR}/spi.c"
	"${PLATFORM_DIR}/i2c.c"
)

set(MCU_LIB_CMSIS 
	"${CMAKE_SOURCE_DIR}/drivers/CMSIS/Device/ST/STM32H7xx/Include"
)

set(MCU_COMPILE_DEFINITIONS
	STM32H723xx
	USE_PWR_LDO_SUPPLY
)

set(COMPILER_FLAGS
	"-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -fdata-sections -ffunction-sections -Wall -Wextra -g3 -Og"
)

set(CMAKE_ASM_FLAGS			"${COMPILER_FLAGS}" CACHE INTERNAL "")
set(CMAKE_C_FLAGS			"${COMPILER_FLAGS}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS			"${COMPILER_FLAGS}" CACHE INTERNAL "")

set(MCU_LINKER_FILE "${MCU_DIR}/STM32H723XG_FLASH.ld")

set(CMAKE_EXE_LINKER_FLAGS 
	"-Wl,--gc-sections -T ${MCU_LINKER_FILE}" CACHE INTERNAL ""
)
