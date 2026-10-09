#ifndef FONT_H
#define FONT_H

#include "types.h"

enum Language {
    LANG_ARABIC,
    LANG_ENGLISH
};

class FontRenderer {
public:
    static void init();
    static void draw_char(int x, int y, char c, uint32_t color, int scale = 1);
    static void draw_string(int x, int y, const char* str, uint32_t color, int scale = 1);
    static void draw_arabic_char(int x, int y, uint16_t glyph_index, uint32_t color, int scale = 1);
    static void draw_string_utf8(int x, int y, const char* utf8_str, uint32_t color, int scale = 1, bool rtl = false);
    static void draw_bilingual_text(int x, int y, const char* ar_text, const char* en_text, uint32_t color, int scale = 1);
    static int get_string_width(const char* str, int scale = 1);
    static int get_utf8_width(const char* str, int scale = 1);
    
    static Language current_language;
    static void set_language(Language lang);
    static Language get_language();
    static void toggle_language();
    static const char* tr(const char* ar, const char* en);
};

#endif // FONT_H
