#include "gd32vf103.h"
#include "gd32vf103_gpio.h"
#include <stdio.h>
#include "ds18b20.h"
#include "font8x8.h"
#include "lcd_framebuffer.h"

int main(void) {
    rcu_periph_clock_enable(RCU_GPIOB);
    ds18b20_init(); // Initialize temperature convertor 
    ds18b20_set_alarm(30, 10); // Set alarm: high=30°C, low=10°C
    for (volatile int i = 0; i < 2000000; i++);
    lcd_init();     // Initialize LCD

    // Initialize PA3 as output for LED
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    // Set framebuffer for LCD
    lcd_fb_setaddr(g_fb);
    lcd_fb_enable();

    char temp_str[32];

    while (1) {
        // Read temperature
        float temp = ds18b20_read_temp();

        // Format temperature string or error message
        if (temp == -1000.0f) {
            snprintf(temp_str, sizeof(temp_str), "Sensor Error");
        } else {
            snprintf(temp_str, sizeof(temp_str), "Temp: %.2f C", temp);
        }

        // Clear framebuffer
        for (int i = 0; i < LCD_FRAMEBUFFER_PIXELS; i++)
            g_fb[i] = 0x0000; // Black

        // Draw temperature text
        lcd_draw_text(10, 30, temp_str, 0xFFFF); // White text

        // Blink LED to indicate a reading
        gpio_bit_reset(GPIOA, GPIO_PIN_3); // LED ON
        for (volatile int i = 0; i < 500000; i++);
        gpio_bit_set(GPIOA, GPIO_PIN_3);   // LED OFF

        // Delay
        for (volatile int i = 0; i < 2000000; i++);
    }
}