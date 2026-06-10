CROSS_COMPILE ?= aarch64-linux-gnu-

# Target binary name
TARGET = kernel8

# Directories
SRC_DIR = src
INC_DIR = include

# Find all C files recursively in subdirectories
C_SRCS = $(wildcard $(SRC_DIR)/*.c) \
         $(wildcard $(SRC_DIR)/mmu/*.c) \
         $(wildcard $(SRC_DIR)/sched/*.c) \
         $(wildcard $(SRC_DIR)/drivers/*.c) \
         $(wildcard $(SRC_DIR)/sys/*.c)

# Find all Assembly files recursively in subdirectories
S_SRCS = $(wildcard boot/*.S) \
         $(wildcard $(SRC_DIR)/arch/*.S) \
         $(wildcard $(SRC_DIR)/sys/*.S)

# Combine object files
OBJS = $(C_SRCS:.c=.o) $(S_SRCS:.S=.o)

# Compiler and Linker Flags
# Add -I flags so the compiler can locate headers across your tree
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles \
         -I$(INC_DIR) \
         -Isrc \
         -Isrc/mmu \
         -Isrc/sched \
         -Isrc/drivers \
         -Isrc/sys

# Build rules
all: $(TARGET).img

$(TARGET).elf: $(OBJS)
	$(CROSS_COMPILE)ld -T linker.ld -o $(TARGET).elf $(OBJS)

$(TARGET).img: $(TARGET).elf
	$(CROSS_COMPILE)objcopy $(TARGET).elf -O binary $(TARGET).img

%.o: %.c
	$(CROSS_COMPILE)gcc $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CROSS_COMPILE)gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).img