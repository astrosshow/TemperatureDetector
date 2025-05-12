#include "gd32vf103.h"
#include "gd32vf103_gpio.h"
#include <stdio.h>
#include "ds18b20.h"
#include "font8x8.h"
#include "lcd_framebuffer.h"
#include "gd32vf103_usart.h"

// UART Protocal for printf
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        usart_data_transmit(USART0, (uint8_t)ptr[i]);
        while (usart_flag_get(USART0, USART_FLAG_TBE) == RESET);
    }
    return len; 
}

int main(void) {
    rcu_periph_clock_enable(RCU_GPIOB);
    ds18b20_init(); // Initialize temperature convertor 
    for (volatile int i = 0; i < 2000000; i++);
    lcd_init();     // Initialize LCD

    // Initialize PA3 as output for LED
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    // Set framebuffer for LCD
    lcd_fb_setaddr(g_fb);
    lcd_fb_enable();

    char temp_str[32];

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART0);

    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9); 
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10); 

    // UART Protocal configuration 
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
    
    while (1) {
        // Read temperature
        float temp = ds18b20_read_temp();

        printf("DEBUG: temp = %.2f\n", temp);

        // Format temperature string
        snprintf(temp_str, sizeof(temp_str), "Temp: %.2f C", temp);

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