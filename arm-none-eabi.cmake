set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

# Optimization and Debug flags for Cortex-M4
set(FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS "${FLAGS}" CACHE INTERNAL "")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -T ${CMAKE_SOURCE_DIR}/STM32F407VGTx_FLASH.ld" CACHE INTERNAL "")