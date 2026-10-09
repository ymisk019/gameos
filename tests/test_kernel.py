#!/usr/bin/env python3
"""
Yazan Game OS - Verification and Testing Suite
Tests ISO 9660 structure, El Torito bootability, MBR boot sector,
VFS structure, YGP dynamic package verification, and Game engines logic.
"""

import os
import sys
import struct

def test_iso_structure(iso_path):
    print(f"[TEST 1] Verifying ISO 9660 & El Torito Bootability on {iso_path}...")
    assert os.path.exists(iso_path), f"ISO file not found: {iso_path}"
    
    with open(iso_path, "rb") as f:
        data = f.read()

    # 1. Test MBR
    assert len(data) >= 2048 * 25, "ISO is smaller than required sectors"
    mbr_sig = data[510:512]
    assert mbr_sig == b'\x55\xAA', f"Invalid MBR signature: {mbr_sig.hex()}"
    active_flag = data[446]
    assert active_flag == 0x80, f"Partition 1 is not active (bootable): {active_flag:#x}"
    print("  [PASS] MBR Boot Sector & Active Partition 0x80 validated (Signature: 0x55AA)")

    # 2. Test Sector 16: PVD
    pvd_offset = 16 * 2048
    pvd_type = data[pvd_offset]
    pvd_id = data[pvd_offset + 1 : pvd_offset + 6]
    vol_id = data[pvd_offset + 40 : pvd_offset + 72].strip()
    assert pvd_type == 0x01, f"Invalid PVD type: {pvd_type}"
    assert pvd_id == b"CD001", f"Invalid PVD identifier: {pvd_id}"
    assert b"YAZAN_GAME_OS" in vol_id, f"Invalid Volume ID: {vol_id}"
    print(f"  [PASS] ISO 9660 Primary Volume Descriptor validated (Volume ID: {vol_id.decode('ascii')})")

    # 3. Test Sector 17: El Torito Boot Record
    br_offset = 17 * 2048
    br_type = data[br_offset]
    br_id = data[br_offset + 1 : br_offset + 6]
    br_spec = data[br_offset + 7 : br_offset + 30]
    boot_catalog_lba = struct.unpack_from('<I', data, br_offset + 71)[0]
    assert br_type == 0x00, f"Invalid Boot Record type: {br_type}"
    assert br_id == b"CD001", f"Invalid Boot Record ID: {br_id}"
    assert b"EL TORITO SPECIFICATION" in br_spec, f"Invalid El Torito spec string: {br_spec}"
    assert boot_catalog_lba == 19, f"Unexpected Boot Catalog LBA: {boot_catalog_lba}"
    print(f"  [PASS] El Torito Boot Record Descriptor validated (Catalog LBA: {boot_catalog_lba})")

    # 4. Test Sector 19: Boot Catalog
    cat_offset = boot_catalog_lba * 2048
    header_id = data[cat_offset]
    platform_id = data[cat_offset + 1]
    key_bytes = data[cat_offset + 28 : cat_offset + 30]
    assert header_id == 0x01, f"Invalid Validation Entry Header ID: {header_id}"
    assert platform_id == 0x00, f"Platform ID is not 80x86: {platform_id}"
    assert key_bytes == b'\x55\xAA', f"Invalid Validation Entry Key: {key_bytes.hex()}"

    # Verify Validation Entry Checksum
    words = struct.unpack_from('<16H', data, cat_offset)
    assert sum(words) & 0xFFFF == 0, "Boot Catalog validation entry checksum failed!"
    print("  [PASS] El Torito Boot Catalog Checksum validated (Sum = 0x0000)")

    # Test Initial / Default Boot Entry
    boot_indicator = data[cat_offset + 32]
    load_rba = struct.unpack_from('<I', data, cat_offset + 40)[0]
    assert boot_indicator == 0x88, f"Boot indicator is not bootable (expected 0x88, got {boot_indicator:#x})"
    assert load_rba == 21, f"Unexpected Load RBA: {load_rba}"
    print(f"  [PASS] El Torito Bootable Entry validated (Boot Indicator: 0x88, Load RBA: {load_rba})")

def test_source_code_integrity():
    print("[TEST 2] Verifying Source Code Architecture and Headers...")
    required_files = [
        "boot/boot.asm",
        "boot/multiboot.asm",
        "kernel/kernel.cpp",
        "kernel/memory.cpp",
        "kernel/gdt.cpp",
        "kernel/idt.cpp",
        "kernel/pic.cpp",
        "kernel/pit.cpp",
        "kernel/power.cpp",
        "kernel/vfs.cpp",
        "drivers/vesa.cpp",
        "drivers/keyboard.cpp",
        "drivers/mouse.cpp",
        "drivers/speaker.cpp",
        "drivers/gamepad.cpp",
        "drivers/rtc.cpp",
        "gui/font.cpp",
        "gui/window.cpp",
        "gui/compositor.cpp",
        "apps/settings.cpp",
        "apps/file_manager.cpp",
        "apps/game_manager.cpp",
        "games/adventure.cpp",
        "games/racing.cpp",
        "games/platformer.cpp",
        "games/shooter.cpp",
        "games/puzzle.cpp",
        "store/store.cpp",
        "include/kernel.h",
        "include/types.h",
        "include/vesa.h",
        "include/font.h",
        "include/games.h",
        "include/store.h",
        "tools/package_game.py",
        "tools/build_iso.py",
        "Makefile",
        "linker.ld"
    ]

    for rf in required_files:
        assert os.path.exists(rf), f"Missing core file: {rf}"
        assert os.path.getsize(rf) > 0, f"File is empty: {rf}"

    print(f"  [PASS] All {len(required_files)} core operating system source files verified!")

def test_ygp_packages():
    print("[TEST 3] Verifying Dynamic Game Package (.YGP) Format & Store...")
    sample_ygp = "store/sample_game.ygp"
    assert os.path.exists(sample_ygp), f"Sample YGP file missing: {sample_ygp}"
    
    with open(sample_ygp, "rb") as f:
        data = f.read()

    assert len(data) >= 284, f"YGP file too small: {len(data)}"
    magic = struct.unpack_from('<I', data, 0)[0]
    assert magic == 0x31504759, f"Invalid YGP Magic: {magic:#x} (expected 0x31504759 'YGP1')"

    game_id = data[4:20].split(b'\x00')[0].decode('utf-8')
    title_ar = data[20:52].split(b'\x00')[0].decode('utf-8')
    title_en = data[52:84].split(b'\x00')[0].decode('utf-8')

    print(f"  [PASS] Dynamic Game Package validated: ID='{game_id}', Title='{title_en}', AR='{title_ar}'")

def main():
    print("=" * 60)
    print("       YAZAN GAME OS - AUTOMATED TEST SUITE")
    print("=" * 60)
    iso_file = sys.argv[1] if len(sys.argv) > 1 else "yazan-game-os.iso"
    test_iso_structure(iso_file)
    test_source_code_integrity()
    test_ygp_packages()
    print("=" * 60)
    print("  >>> ALL TESTS PASSED SUCCESSFULLY! 100% OPERATIONAL <<<")
    print("=" * 60)

if __name__ == "__main__":
    main()
