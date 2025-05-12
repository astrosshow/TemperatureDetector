/* The array with ASCII codes */
/* Tina, 5/1/2025             */
#pragma once

#include <stdint.h>

void lcd_draw_text(int x, int y, const char* s, uint16_t color);

// ASCII 32–127 (printable characters)
extern const uint8_t font8x8_basic[96][8];

