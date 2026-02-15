# ⚠️ Experimental Project

This project is **experimental** and currently **under development**. Features, structure, and APIs may change at any time.


# STM32 Bare Metal Starter

A minimal bare metal starter project designed for **easy multi-MCU development** across different STM32 families. Switch between MCU targets with just a single CMake parameter - no complex configuration needed!

This project demonstrates basic LED blinking functionality without any HAL or vendor libraries - just pure register manipulation across multiple STM32 families.

## 🚀 Multi-MCU Made Simple

**One project, multiple targets** - switch MCUs effortlessly:
```bash
# Build for STM32F4 Discovery
cmake --preset Debug -DMCU_TYPE=stm32f407

# Build for STM32H7 Nucleo  
cmake --preset Debug -DMCU_TYPE=stm32h723
```

## Supported MCUs

- **STM32F407** (STM32F4 Discovery Board)
- **STM32H723** (STM32H7 Nucleo Board)

*Adding new MCU support is straightforward - see the "Adding New MCUs" section below.*

## Features

- ✅ **One-command MCU switching** - change targets instantly
- ✅ **Automated MCU configuration** - linker scripts, startup code, and drivers selected automatically
- ✅ CMake-based build system with presets
- ✅ ARM GNU Toolchain integration
- ✅ CMSIS Core Support
- ✅ MCU-specific abstraction layer (LED examples included)
- ✅ Easy to extend for new STM32 families

## Prerequisites

- **ARM GNU Toolchain** (`arm-none-eabi-gcc`)
- **CMake** (version 3.28.3 or higher)
- **Ninja** build system

### Install Dependencies (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install gcc-arm-none-eabi cmake ninja-build
```

## Quick Start

1. **Clone the repository**
```bash
git clone <repository-url>
cd stm32_bare_metal_starter
```

2. **Configure for your target MCU**
```bash
# For STM32F407
cmake --preset Debug -DMCU_TYPE=stm32f407

# For STM32H723  
cmake --preset Debug -DMCU_TYPE=stm32h723
```

3. **Build the project**
```bash
cmake --build build
```

## Adding New MCUs

1. Add MCU support files in `mcu/{mcu_family}/`
2. Create MCU configuration in `cmake/mcu/{mcu_type}.cmake`
3. Add MCU type to `MCU_SUPPORTED_TYPES` in `cmake/mcu-type.cmake`

Perfect starting point for bare metal STM32 development!

## License

This project structure and configuration files are provided as-is. CMSIS files retain their original ST/ARM licenses.