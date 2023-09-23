#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/rand.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "output.pio.h"

// Register addresses
#define NOP   0b000
#define DAC   0b001
#define CTRL  0b010
#define CLR   0b011
#define SCTRL 0b100

// Read/write control bit
#define WRITE (0b0 << 3)
#define READ  (0b1 << 3)

// Pinout
#define SDO   3
#define SDIN  4
#define SCLK  5
#define SYNC  6 // Inverting input
#define RESET 7 // Inverting input
#define CLEAR 8 // Inverting input
#define LDAC  9 // Inverting input

// PIO global variables
PIO pio;
uint sm;
uint offset;

// Life Indicator
void core2()
{
    gpio_init(25);
    gpio_set_dir(25, true);

    while (true)
    {
        gpio_put(25, 1);
        sleep_ms(500);
        gpio_put(25, 0);
        sleep_ms(500);
    }
    
}

void dacWrite(uint32_t data, uint8_t control)
{
    control = control | WRITE;                        // Combine read/write bit with the three register address bits
    data = data & 0b00000000000011111111111111111111; // Truncate unused higher bits
    data = data | (control << 20);                    // Combine read/write bit, register address, and data bit
    data = data << 8;                                 // Aligning data correctly for PIO
    pio_sm_put_blocking(pio, sm, data);
    pio_sm_get_blocking(pio, sm);
    printf("%024b\n", (data >> 8));
}

int main() 
{
    gpio_pull_up(RESET); // Pull RESET pin high
    gpio_pull_up(CLEAR); // Pull CLEAR pin high 
    gpio_put(LDAC, 0);   // Set LDAC low to enable synchronous update

    stdio_init_all();

    set_sys_clock_khz(200000, true);
    multicore_launch_core1(core2);

    pio = pio0;
    sm = pio_claim_unused_sm(pio, true);
    offset = pio_add_program(pio, &output_program);
    output_program_init(pio, sm, offset, SDO, (float)100);
    pio_sm_set_enabled(pio, sm, true);

    while(true)
    {
        dacWrite(get_rand_32(), DAC);
        sleep_ms(10);
    }

    return 0;
}