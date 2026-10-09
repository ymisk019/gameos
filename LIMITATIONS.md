# Yazan Game OS - System Requirements, Limitations & Known Constraints

## 1. System Requirements (المتطلبات الدنيا والموصى بها)
- **Architecture**: x86 (IA-32 compatible) or x86_64.
- **CPU**: Intel Pentium II or higher, AMD K6 or higher, or any modern Intel Core / AMD Ryzen / Atom processor.
- **RAM**: Minimum 64 MB. Recommended: 128 MB or more (Kernel baseline consumes < 8 MB).
- **Firmware**: Legacy BIOS or UEFI with Compatibility Support Module (CSM) enabled.
- **Display**: VESA VBE 2.0+ compatible graphics adapter supporting 800x600 resolution (32-bit or 24-bit color). Supported out-of-the-box by Intel HD Graphics, Nvidia, AMD Radeon, QEMU `std-vga`, VirtualBox VBoxVGA/VBoxSVGA, and VMware SVGA II.
- **Audio**: PC Speaker (standard PIT Channel 2 at ports 0x42/0x61).
- **Input**: PS/2 Keyboard and PS/2 Mouse (or USB HID with BIOS Legacy Emulation enabled).

---

## 2. Technical Limitations & Known Constraints (القيود التقنية)

1. **UEFI Native Mode vs Legacy BIOS (CSM)**:
   - *Limitation*: Yazan Game OS boots using the standard x86 BIOS El Torito / MBR bootstrap sequence.
   - *Resolution / Workaround*: On modern PCs shipping with Pure UEFI, enable **Legacy Boot / CSM (Compatibility Support Module)** in BIOS/UEFI settings, or disable "Secure Boot". A native 64-bit UEFI GOP loader can be chained in future releases.

2. **Sound Hardware Architecture**:
   - *Limitation*: The primary native audio synthesizer uses the motherboard PC Speaker (PIT timer frequency synthesis) rather than HD Audio (Intel HDA) or AC97 DMA buffers.
   - *Advantage*: This ensures 100% universal sound reproduction across all hardware without needing vendor-specific PCI audio drivers (Realtek, Conexant, Creative, Intel).

3. **Storage Persistence**:
   - *Limitation*: When booting directly from CD-ROM / Read-Only ISO, file system modifications (such as saving new `.ygp` files) are stored in volatile RAM disk (VFS).
   - *Resolution*: When booting from a USB flash drive written as a writable FAT32 partition or using persistent ramdisk images, changes persist across sessions.

4. **USB Hotplugging**:
   - *Limitation*: PS/2 mouse and keyboard drivers assume initialized controller ports. USB mice/keyboards require the motherboard's BIOS Legacy USB Emulation (which is enabled by default on 99% of PCs).

5. **Graphics Scaling**:
   - *Limitation*: Default display mode is 800x600x32bpp. Monitors with wide aspect ratios (16:9 or 21:9) will display in 4:3 pillarbox mode unless hardware display scaling is engaged.

---

## 3. Error Handling and Diagnostics (معالجة الأخطاء)
- **VBE Mode Not Found**: If video mode 0x4118 (800x600x32) fails, the bootloader automatically falls back to mode 0x4115 (16-bit) or 0x4112 (640x480).
- **Triple Fault Recovery**: System reboot handler pulses the 8042 controller; if masked, it triggers a clean CPU triple fault via empty IDT to force a reliable hardware restart.
- **Out of Memory (OOM)**: The heap allocator returns `NULL` and prevents buffer overrun.
