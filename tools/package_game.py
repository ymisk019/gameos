#!/usr/bin/env python3
"""
Yazan Game OS - Package Builder for Dynamic Games (.ygp)
Allows packaging any game to be installed and run dynamically on Yazan Game OS
without recompiling the operating system or re-generating the ISO.
"""

import sys
import struct
import os

YGP_MAGIC = 0x31504759  # "YGP1"

def create_ygp(game_id, title_ar, title_en, author, version, desc_ar, desc_en, icon_color, code_bytes=b'', data_bytes=b''):
    header = struct.pack(
        '<I16s32s32s32s16s64s64sIII',
        YGP_MAGIC,
        game_id.encode('utf-8')[:15].ljust(16, b'\x00'),
        title_ar.encode('utf-8')[:31].ljust(32, b'\x00'),
        title_en.encode('utf-8')[:31].ljust(32, b'\x00'),
        author.encode('utf-8')[:31].ljust(32, b'\x00'),
        version.encode('utf-8')[:15].ljust(16, b'\x00'),
        desc_ar.encode('utf-8')[:63].ljust(64, b'\x00'),
        desc_en.encode('utf-8')[:63].ljust(64, b'\x00'),
        icon_color,
        len(code_bytes),
        len(data_bytes)
    )
    return header + code_bytes + data_bytes

def main():
    if len(sys.argv) < 3:
        print("Usage: python3 package_game.py <output.ygp> <game_id> [title_en] [title_ar]")
        # Create a sample package
        out_path = "store/sample_game.ygp"
        pkg = create_ygp(
            game_id="cyber_pong",
            title_ar="بونغ السايبر",
            title_en="Cyber Pong",
            author="Yazan Community",
            version="1.2.0",
            desc_ar="لعبة بونغ كلاسيكية ثنائية الأبعاد تعمل عبر حزمة YGP",
            desc_en="Classic 2D retro pong game running via dynamic YGP package",
            icon_color=0x003498DB,
            code_bytes=b'\x01\x02\x03\x04' * 16,
            data_bytes=b''
        )
        with open(out_path, "wb") as f:
            f.write(pkg)
        print(f"Created sample package at: {out_path} ({len(pkg)} bytes)")
        return

    out_file = sys.argv[1]
    gid = sys.argv[2]
    ten = sys.argv[3] if len(sys.argv) > 3 else gid
    tar = sys.argv[4] if len(sys.argv) > 4 else gid

    pkg = create_ygp(
        game_id=gid,
        title_ar=tar,
        title_en=ten,
        author="Community Dev",
        version="1.0.0",
        desc_ar="حزمة لعبة إضافية",
        desc_en="Additional game package",
        icon_color=0x00E67E22,
        code_bytes=b'\x90' * 32
    )

    with open(out_file, "wb") as f:
        f.write(pkg)
    print(f"Successfully packaged {out_file} ({len(pkg)} bytes)")

if __name__ == "__main__":
    main()
