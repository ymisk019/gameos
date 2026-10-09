# Yazan Game OS Makefile
# Target: x86 32-bit Bare Metal

CXX = g++
AS = nasm
LD = ld

CXXFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -fno-pie -fno-stack-protector -nostdlib -Iinclude
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

OBJS = boot/multiboot.o \
       kernel/kernel.o \
       kernel/types.o \
       kernel/memory.o \
       kernel/gdt.o \
       kernel/idt.o \
       kernel/pic.o \
       kernel/pit.o \
       kernel/power.o \
       kernel/vfs.o \
       drivers/vesa.o \
       drivers/keyboard.o \
       drivers/mouse.o \
       drivers/speaker.o \
       drivers/gamepad.o \
       drivers/rtc.o \
       gui/font.o \
       gui/window.o \
       gui/compositor.o \
       apps/settings.o \
       apps/file_manager.o \
       apps/game_manager.o \
       games/adventure.o \
       games/racing.o \
       games/platformer.o \
       games/shooter.o \
       games/puzzle.o \
       store/store.o

ISO_DIR = isodir
ISO_NAME = yazan-game-os.iso

all: $(ISO_NAME)

boot/multiboot.o: boot/multiboot.asm
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(ISO_NAME): kernel.bin
	mkdir -p $(ISO_DIR)/boot/grub
	cp kernel.bin $(ISO_DIR)/boot/kernel.bin
	echo 'set timeout=0' > $(ISO_DIR)/boot/grub/grub.cfg
	echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo 'menuentry "Yazan Game OS" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '    multiboot /boot/kernel.bin' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '    boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)

python-build:
	python3 tools/build_iso.py

run: $(ISO_NAME)
	qemu-system-i386 -cdrom $(ISO_NAME) -soundhw pcspk -m 256M

clean:
	rm -rf $(OBJS) kernel.bin $(ISO_NAME) $(ISO_DIR)
