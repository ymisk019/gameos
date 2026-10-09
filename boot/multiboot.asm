; Yazan Game OS - Multiboot Header & Early Boot Assembly
[BITS 32]

MBOOT_PAGE_ALIGN    equ 1 << 0
MBOOT_MEM_INFO      equ 1 << 1
MBOOT_VIDEO_MODE    equ 1 << 2
MBOOT_HEADER_MAGIC  equ 0x1BADB002
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO | MBOOT_VIDEO_MODE
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

section .multiboot
align 4
    dd MBOOT_HEADER_MAGIC
    dd MBOOT_HEADER_FLAGS
    dd MBOOT_CHECKSUM

    ; Multiboot address fields (unused if ELF)
    dd 0, 0, 0, 0, 0

    ; Video mode request (800x600x32 Linear Framebuffer)
    dd 0    ; 0 = Linear graphics mode, 1 = EGA text
    dd 800  ; Width
    dd 600  ; Height
    dd 32   ; BPP

section .bss
align 16
stack_bottom:
    resb 65536 ; 64 KB Kernel Stack
stack_top:

section .text
global _start
extern kernel_main

_start:
    ; Disable interrupts
    cli

    ; Initialize kernel stack pointer
    mov esp, stack_top

    ; Push Multiboot parameters (magic in EAX, info struct in EBX)
    push ebx
    push eax

    ; Call C++ Kernel Main
    call kernel_main

.halt:
    cli
    hlt
    jmp .halt
