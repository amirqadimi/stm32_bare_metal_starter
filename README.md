# stm32-bare-metal-starter

A bare-metal STM32 starting point: CMSIS headers only, no HAL, no CubeMX
generated code. Clocks, GPIO, SPI and I2C are brought up by writing registers
directly.

The point of the repo is the *structure*: a build that targets several MCUs
from one source tree, with the application written against small platform
interfaces and each MCU supplying its own implementation. The firmware itself
is deliberately trivial: `src/main.c` blinks the user LED.

## What is here

| Interface | Header | Purpose |
|---|---|---|
| platform | `include/platform.h` | `init_platform()`: clocks, then every peripheral |
| delay | `include/delay.h` | SysTick millisecond tick, `delay_ms()` / `get_millis()` |
| toggle_led | `include/toggle_led.h` | `led_on()` / `led_off()` on the user LED |
| spi | `include/spi.h` | blocking full-duplex master transfer |
| i2c | `include/i2c.h` | blocking 7-bit master read and write |

`main.c` only uses delay and toggle_led. SPI and I2C are there as worked
examples of real peripheral drivers (GPIO alternate function, kernel clock
selection, prescalers and error-flag handling) for you to copy when adding
UART, timers or anything else.

Pin assignments live in each `platform/<mcu>/` file, listed at the top of
`init_<mcu>.c`. They are the pins the author's boards happened to use; change
them to match yours.

## Targets

| MCU | Preset | Build dir | Probe |
|---|---|---|---|
| STM32H7B0VBT6 | `debug-h7b0` / `release-h7b0` | `build-h7b0` | J-Link, SWD |
| STM32H723ZG | `debug-h723` / `release-h723` | `build-h723` | ST-LINK, OpenOCD |

Adding another MCU means four things: an entry in `MCU_SUPPORTED_TYPES`
(`cmake/mcu-type.cmake`), a `cmake/mcu/<mcu>.cmake`, a `platform/<mcu>/`
implementing the interfaces in `include/`, and presets in `CMakePresets.json`.

The two existing ports are worth reading side by side. The H7B0 runs at
280 MHz from a 25 MHz crystal with an HSI fallback, the H723 at 180 MHz off the
HSI, and each has its own voltage scaling and flash-latency rules.

## Build

Needs `arm-none-eabi-gcc`, CMake and Ninja. All three ship with STM32CubeCLT.

`<configure-preset>` is one of:

| Value | Target | Build type |
|---|---|---|
| `debug-h7b0` | STM32H7B0VBT6 | `Debug` |
| `release-h7b0` | STM32H7B0VBT6 | `Release` |
| `debug-h723` | STM32H723ZG | `Debug` |
| `release-h723` | STM32H723ZG | `Release` |

`<build-preset>` is always the configure preset prefixed with `build-`, so
`debug-h7b0` pairs with `build-debug-h7b0`. Both presets of a target share one
build directory (`build-h7b0`, `build-h723`), so switching build type
reconfigures in place. `cmake --list-presets` and `--list-presets=build` print
the live lists.

Note the per-MCU config in `cmake/mcu/` hardcodes `-g3 -Og` into the compiler
flags, and `CMAKE_BUILD_TYPE` only appends to that. `Release` therefore still
carries full debug symbols and is barely smaller, so debug either preset.

```sh
cmake --preset <configure-preset>
cmake --build --preset <build-preset>
```

For example, a debug build for the H7B0:

```sh
cmake --preset debug-h7b0
cmake --build --preset build-debug-h7b0
```

The output ELF is `<build dir>/firmware`.

The VS Code tasks `configure-h7b0` / `build-h7b0` (and the `h723` pair) run the
same two commands.

## Flash and run with J-Link

Most probes do not supply target power, so power the board separately.

```sh
printf 'loadfile build-h7b0/firmware\nr\ng\nexit\n' \
  | JLinkExe -device STM32H7B0VB -if SWD -speed 4000 -autoconnect 1 -NoGui 1
```

Sanity checks before blaming the firmware:

```sh
printf 'exit\n' | JLinkExe -NoGui 1     # banner shows VTref, ~3.3V when SWD is connected
printf 'ShowEmuList\nexit\n' | JLinkExe -NoGui 1
```

`VTref = 0.000V` means the probe sees no target: the board is unpowered or the
SWD cable is not seated.

## Flash and run with OpenOCD

```sh
openocd -f interface/stlink.cfg -f target/stm32h7x.cfg \
        -c "program build-h723/firmware verify reset exit"
```

## Debug

VS Code: `Debug STM32H7B0 (J-Link)` or `Debug STM32H723`, both in
`.vscode/launch.json`, each with a `preLaunchTask` that rebuilds first.
Requires the Cortex-Debug extension.

Command line:

```sh
JLinkGDBServerCL -device STM32H7B0VB -if SWD -speed 4000   # macOS binary name
arm-none-eabi-gdb build-h7b0/firmware -ex "target remote :2331"
```

On Windows the server is `JLinkGDBServerCL.exe`, on Linux
`JLinkGDBServerCLExe`. `launch.json` already picks the right one per platform.

## Layout

```
src/          application code (main)
include/      platform interfaces (platform, delay, spi, i2c, toggle_led)
platform/     per-MCU implementations of those interfaces
mcu/          CMSIS startup, system_*.c and linker scripts
cmake/mcu/    per-MCU build config (sources, defines, flags, linker script)
drivers/CMSIS ARM CMSIS core + ST device headers
```
