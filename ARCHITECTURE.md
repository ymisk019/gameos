# Yazan Game OS - Technical Architecture

## 1. System Overview
Yazan Game OS is a 32-bit monolithic x86 operating system implemented directly on bare-metal hardware in C++ with minimal assembly bootstrap routines. It possesses zero dependencies on Linux, POSIX, GNU, or any third-party kernel.

## 2. Memory Organization
```
0x00000000 - 0x000003FF : Real Mode Interrupt Vector Table (IVT)
0x00000400 - 0x000004FF : BIOS Data Area (BDA)
0x00007C00 - 0x00007DFF : MBR / Boot Sector Loader (512 bytes)
0x00090000 - 0x0009FFFF : Early Boot Stack (64 KB)
0x00100000 - 0x003FFFFF : Kernel Code, Rodata, Data, BSS (linked at 1MB)
0x00400000 - 0x02400000 : Dynamic Kernel Heap (32 MB managed by buddy/bump allocator)
0xFD000000 - 0xFDFFFFFF : VESA VBE Linear Framebuffer (LFB)
```

## 3. Interrupts & PIC
- **PIC 8259**: Master (0x20, 0x21) and Slave (0xA0, 0xA1) remapped to interrupts 32 through 47.
  - IRQ 0 (Int 32): PIT Timer (100 Hz).
  - IRQ 1 (Int 33): PS/2 Keyboard.
  - IRQ 12 (Int 44): PS/2 Mouse.

## 4. Graphics Subsystem (VESA Linear Framebuffer)
- **Resolution**: 800x600, 32 bits per pixel (True Color ARGB).
- **Double Buffering**: Operations render to a dynamically allocated 32-bit backbuffer in high RAM.
- **Vsync / Blit**: When `flip()` is invoked, the buffer is copied in contiguous 32-bit words to the LFB physical address.
- **Primitives**: High performance line drawing (Bresenham), filled rectangles, circles, gradients, and rounded rectangles.

## 5. Typography Engine
- Bilingual Font Renderer:
  - ASCII 8x8 bitmap rasterizer for English characters.
  - Custom Arabic 8x8 glyph bitmaps covering letters with contextual shaping and Right-To-Left (RTL) ordering.

## 6. Window Manager & Compositor
- Windows are managed in a z-ordered stack of active objects.
- Each window has its own clipping coordinate system, titlebar, minimize and close handlers.
- Mouse dragging tracks offsets relative to the window header.

## 7. Power Management
- ACPI power-off via standard ports (`0x604`, `0xB004`, `0x4004`).
- APM BIOS fallback.
- Hardware reboot via the 8042 Keyboard Controller reset pulse (`0xFE` to port `0x64`).
