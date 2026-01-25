set(MCU_TYPE "" CACHE STRING "MCU type")

set(MCU_SUPPORTED_TYPES 
	"stm32f407"
	"stm32h723"
)

if (NOT MCU_TYPE) 
	message(FATAL_ERROR "MCU type is not specified. Please add -DMCU_TYPE={MCU_TYPE}. Supported MCU types are ${MCU_SUPPORTED_TYPES}")
endif()

list(FIND MCU_SUPPORTED_TYPES ${MCU_TYPE} MCU_TYPE_INDEX)

if (MCU_TYPE_INDEX EQUAL -1)
	message(FATAL_ERROR "MCU type \"${MCU_TYPE}\" is not supported. Supported MCU types are ${MCU_SUPPORTED_TYPES}")
endif()

set(MCU_CONFIG_FILE
	"${CMAKE_SOURCE_DIR}/cmake/mcu/${MCU_TYPE}.cmake"
)

if (NOT EXISTS ${MCU_CONFIG_FILE})
	message(FATAL_ERROR "MCU config file does not exists: \"${MCU_CONFIG_FILE}\"")
endif()