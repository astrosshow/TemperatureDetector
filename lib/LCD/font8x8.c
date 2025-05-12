/* Contents of ASCII array  */
/* 5/1/2025                 */

#include "font8x8.h"
#include "font8x8_basic.h"
#include "lcd_framebuffer.h"
#include "lcd.h"

uint16_t g_fb[LCD_WIDTH * LCD_HEIGHT];

void lcd_draw_char(int x, int y, char c, uint16_t color) {
    if (c < 32 || c > 127) return;
    const uint8_t* glyph = font8x8_basic[c - 32];
    for (int row = 0; row < 8; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << col)) {
                int px = x + col;
                int py = y + row;
                if (px >= 0 && px < LCD_WIDTH && py >= 0 && py < LCD_HEIGHT)
                    g_fb[py * LCD_WIDTH + px] = color;
            }
        }
    }
}

void lcd_draw_text(int x, int y, const char* s, uint16_t color) {
    while (*s) {
        lcd_draw_char(x, y, *s++, color);
        x += 8;
    }
}

