# Target binary name
TARGET = kernel8

# Directories
SRC_DIR = src
INC_DIR = include

# ====================================================================
# DAY 1 ADDITION: Include subdirectories for MMU, Arch, and Scheduler
# ====================================================================
# Find all C files recursively in subdirectories
C_SRCS = $(wildcard $(SRC_DIR)/*.c) \
         $(wildcard $(SRC_DIR)/mm/*.c) \
         $(wildcard $(SRC_DIR)/kernel/*.c) \
         $(wildcard $(SRC_DIR)/drivers/*.c)

# Find all Assembly files recursively in subdirectories
S_SRCS = $(wildcard $(SRC_DIR)/*.S) \
         $(wildcard $(SRC_DIR)/boot/*.S) \
         $(wildcard $(SRC_DIR)/arch/*.S)

# Combine object files
OBJS = $(C_SRCS:.c=.o) $(S_SRCS:.S=.o)

# Compiler and Linker Flags
# Add -I flags so the compiler can locate headers across your tree
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles \
         -I$(INC_DIR) \
         -I$(INC_DIR)/arch \
         -I$(INC_DIR)/kernel \
         -I$(INC_DIR)/mm \
         -I$(INC_DIR)/drivers

# Build rules
all: $(TARGET).img

$(TARGET).elf: $(OBJS)
	aarch64-none-elf-ld -T linker.ld -o $(TARGET).elf $(OBJS)

$(TARGET).img: $(TARGET).elf
	aarch64-none-elf-objcopy $(TARGET).elf -O binary $(TARGET).img

%.o: %.c
	aarch64-none-elf-gcc $(CFLAGS) -c $< -o $@

%.o: %.S
	aarch64-none-elf-gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).img