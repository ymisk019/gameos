# Yazan Game OS - Dynamic Store & YGP Package Specification

## 1. Overview
Yazan Game OS features an extensible package format: **YGP (Yazan Game Package)**.
This allows creators to write and distribute games that run dynamically on the OS without re-compiling the operating system or modifying the bootable ISO file.

## 2. Package Binary Header Format
Every `.ygp` file begins with a 284-byte packed binary header:
```c
struct YGPHeader {
    uint32_t magic;         // 0x31504759 ("YGP1")
    char id[16];           // Unique game slug (e.g., "retro_snake")
    char title_ar[32];     // Arabic Display Title (UTF-8)
    char title_en[32];     // English Display Title (ASCII/UTF-8)
    char author[32];       // Developer name
    char version[16];      // Version string (e.g. "1.0.0")
    char desc_ar[64];      // Arabic Description
    char desc_en[64];      // English Description
    uint32_t icon_color;   // RGB color for desktop icon / card
    uint32_t code_size;    // Size of bytecode payload in bytes
    uint32_t data_size;    // Size of additional sprite/asset payload
} __attribute__((packed));
```

## 3. Creating a Package
Use the provided `tools/package_game.py` tool:
```bash
python3 tools/package_game.py output.ygp my_game "My Game" "لعبتي"
```
Place the resulting `.ygp` file into `/store/` on your virtual disk or VFS image. When Yazan Game OS boots, open the **Game Store (متجر الألعاب)** and click **Scan /store for Packages** to register and play the game immediately!
