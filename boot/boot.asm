; Yazan Game OS - Direct BIOS Boot Sector & VBE Mode Switch
[BITS 16]
[ORG 0x7C00]

jmp start
nop

; OEM Parameter Block / Disk Information
os_name: db "YAZAN_OS"

start:
    ; Setup segment registers
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; Print early boot message
    mov si, msg_booting
    call print_str

    ; 1. Query and Set VESA VBE Mode (800x600x32bpp)
    ; Mode 0x118 is standard 800x600x24/32bpp, or use 0x4118 (with LFB bit 14 set)
    mov ax, 0x4F02
    mov bx, 0x4118 ; 800x600x32 Linear Frame Buffer mode
    int 0x10
    cmp ax, 0x004F
    je .vbe_ok

    ; Fallback to 800x600 16-bit (0x4115) or 640x480 (0x4112)
    mov ax, 0x4F02
    mov bx, 0x4115
    int 0x10

.vbe_ok:
    ; 2. Enable A20 Line via Fast A20 Gate (port 0x92)
    in al, 0x92
    or al, 2
    out 0x92, al

    ; 3. Disable Interrupts for PM transition
    cli

    ; 4. Load Global Descriptor Table (GDT)
    lgdt [gdt_descriptor]

    ; 5. Set Protected Mode bit in CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; 6. Far Jump to 32-bit Protected Mode Segment
    jmp 0x08:pm_entry

print_str:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

msg_booting: db "Booting Yazan Game OS...", 13, 10, 0

; GDT Table for 32-bit Protected Mode
align 8
gdt_start:
    ; Null Descriptor
    dd 0x00000000
    dd 0x00000000

    ; Code Segment Descriptor (Base=0, Limit=4GB, Access=0x9A, Flags=0xCF)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

    ; Data Segment Descriptor (Base=0, Limit=4GB, Access=0x92, Flags=0xCF)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; ---------------------------------------------
; 32-bit Protected Mode Entry Point
; ---------------------------------------------
[BITS 32]
pm_entry:
    ; Reload segment registers with Data Selector (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x00090000

    ; Jump to Kernel entry point loaded at 0x100000 (1MB) or directly
    mov eax, 0x2BADB002 ; Multiboot-style magic
    mov ebx, 0x00007E00 ; Info block pointer

    ; Call kernel
    extern kernel_main
    ; In monolithic boot image, kernel starts at next sector
    jmp kernel_stage2

kernel_stage2:
    hlt
    jmp kernel_stage2

; Pad boot sector to 510 bytes + 0x55AA signature
times 510 - ($ - $$) db 0
dw 0xAA55
