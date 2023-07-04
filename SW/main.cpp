#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"

#include "output.pio.h"

// Register addresses
#define NOP   0
#define DAC   1
#define CTRL  2
#define CLR   3
#define SCTRL 4

// Read/write control bit
#define WRITE 0
#define READ  1


uint8_t random_bits(uint8_t num_bits, uint8_t seed) {
    uint8_t result = 0;

    for (uint8_t i = 0; i < num_bits; ++i) {
        uint8_t pos;
        do {
            pos = (seed + i) % 8;
            seed = (seed * 1103515245 + 12345) % 256;
        } while (result & (1 << pos));

        result |= (1 << pos);
    }

    return result;
}

void print_byte_as_bits(uint8_t byte) {
    for (int i = 7; i >= 0; --i) {
        printf("%d", (byte >> i) & 1);
    }
}

void core2()
{
    while (true)
    {
        gpio_put(25, 1);
        sleep_ms(500);
        gpio_put(25, 0);
        sleep_ms(500);
    }
    
}

int main() 
{
    sleep_ms(10000);
    uint8_t num_bits = 2;

    for (uint8_t seed = 0; seed <= 255; ++seed) {
        uint8_t random_byte = random_bits(num_bits, seed);
        print_byte_as_bits(random_byte);
        printf("\n");
    }

    return 0;
}