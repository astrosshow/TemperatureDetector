#include "ds18b20.h"
#include "gd32vf103_gpio.h"
#include "gd32vf103.h"
#include <stdint.h>

#define DS18B20_PORT GPIOB
#define DS18B20_PIN GPIO_PIN_9

// Pin output function 
static void ds18b20_pin_output(void) {
    gpio_init(DS18B20_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, DS18B20_PIN);
}

// Pin input function 
static void ds18b20_pin_input(void) {
    gpio_init(DS18B20_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, DS18B20_PIN);
}

// Function for writing to scractchpad 
static void ds18b20_write(int value) {
    if (value) {
        gpio_bit_set(DS18B20_PORT, DS18B20_PIN);
    } else {
        gpio_bit_reset(DS18B20_PORT, DS18B20_PIN);
    }
}

// Returns input status of pin 
static int ds18b20_read(void) {
    return gpio_input_bit_get(DS18B20_PORT, DS18B20_PIN);
}

// Delay
static void delay(uint32_t us) {
    for (volatile uint32_t i = 0; i < us * 10; i++) __NOP();
}

void ds18b20_init(void) {
    rcu_periph_clock_enable(RCU_GPIOB);
    ds18b20_pin_output();
    ds18b20_write(1);
}

static int ds18b20_reset(void) {
    ds18b20_pin_output();
    ds18b20_write(0);
    delay(400);
    ds18b20_write(1);
    ds18b20_pin_input();
    int presence = !ds18b20_read();
    delay(410);
    return presence;
}

static void ds18b20_write_bit(int bit) {
    ds18b20_pin_output();
    ds18b20_write(0);
    delay(bit ? 6 : 60);
    ds18b20_write(1);
    if (bit) delay(64);
}

static int ds18b20_read_bit(void) {
    int bit;
    ds18b20_pin_output();
    ds18b20_write(0);
    delay(6);
    ds18b20_write(1);
    ds18b20_pin_input();
    delay(9);
    bit = ds18b20_read();
    delay(55);
    return bit;
}

static void ds18b20_write_byte(int byte) {
    for (int i = 0; i < 8; i++) {
        ds18b20_write_bit((byte >> i) & 0x01);
    }
}

static uint8_t ds18b20_read_byte(void) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++)
        if (ds18b20_read_bit())
            byte |= (1 << i);
    return byte;
}

void ds18b20_set_alarm(int8_t th, int8_t tl) {
    if (!ds18b20_reset()) return;
    ds18b20_write_byte(0xCC); // Skip ROM
    ds18b20_write_byte(0x4E); // Write Scratchpad
    ds18b20_write_byte(th);   // TH (high alarm)
    ds18b20_write_byte(tl);   // TL (low alarm)
    ds18b20_write_byte(0x7F); // Configuration register (default: 12-bit resolution)
}

// Calculate CRC8 for DS18B20 scratchpad
static uint8_t ds18b20_crc8(const uint8_t *data, int len) {
    uint8_t crc = 0;
    for (int i = 0; i < len; i++) {
        uint8_t inbyte = data[i];
        for (int j = 0; j < 8; j++) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}

float ds18b20_read_temp(void) {
    if (!ds18b20_reset()) return -1000.0f;
    ds18b20_write_byte(0xCC); // Skip ROM
    ds18b20_write_byte(0x44); // Convert T
    for (volatile int i = 0; i < 800000; i++) __NOP(); // Wait for conversion

    if (!ds18b20_reset()) return -1000.0f;
    ds18b20_write_byte(0xCC); // Skip ROM
    ds18b20_write_byte(0xBE); // Read Scratchpad

    uint8_t scratchpad[9];
    for (int i = 0; i < 9; i++) {
        scratchpad[i] = ds18b20_read_byte();
    }

    // CRC check
    if (ds18b20_crc8(scratchpad, 8) != scratchpad[8]) {
        return -1000.0f; // CRC error
    }

    int16_t temp = (scratchpad[1] << 8) | scratchpad[0];
    return temp / 16.0f;
}