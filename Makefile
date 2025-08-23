# Toolchain
CROSS   ?= arm-none-eabi
CC      := $(CROSS)-gcc
OBJCOPY := $(CROSS)-objcopy
GDB     := $(CROSS)-gdb

# MCU flags (Cortex-M7 r1p1, single-precision FPU on H723)
CPUFLAGS := -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard

CFLAGS  := $(CPUFLAGS) -O2 -ffunction-sections -fdata-sections -Wall -Wextra -std=c11
LDFLAGS := $(CPUFLAGS) -Wl,--gc-sections -Wl,-Tlinker.ld -nostartfiles

SRCS := startup.c main.c
OBJS := $(SRCS:.c=.o)

TARGET := blink-h723

all: $(TARGET).bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS) linker.ld
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

size: $(TARGET).elf
	$(CROSS)-size $<

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).bin

flash: $(TARGET).elf
	openocd -f interface/stlink.cfg -f target/stm32h7x.cfg \
		-c "init" \
		-c "reset halt" \
		-c "program $(TARGET).elf verify" \
		-c "reset run" \
		-c "shutdown"

gdb:
	$(GDB) $(TARGET).elf
