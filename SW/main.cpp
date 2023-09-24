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
#define WRITE 0b0
#define READ  0b1

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

void dacWrite(uint32_t readwrite, uint32_t reg, uint32_t data)
{
    data = data % 0xFFFFF;                                                            // Remove everything except 20 data bits                                 
    uint32_t packedData = packedData | (readwrite << 31) | (reg << 28) | (data << 8); // Pack read/write bit, register address bits and data bits
    pio_sm_put_blocking(pio, sm, packedData);                                         // Send packed data to PIO
    pio_sm_get_blocking(pio, sm);                                                     // SPI readback, to prevent stalling
    printf("%032b\n", packedData);
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
    output_program_init(pio, sm, offset, SDO, (float)10);
    pio_sm_set_enabled(pio, sm, true);

    gpio_put(RESET, 0);
    sleep_ms(10);
    gpio_put(RESET, 1);

    // Data: 00000000001100110010   RBUF       OPGND      DACTRI     BIN/2sC    SDODIS     LINCOMP
    uint32_t initData = initData | (1 << 1) | (0 << 2) | (0 << 3) | (1 << 4) | (0 << 5) | (0b1100 << 6);

    dacWrite(WRITE, NOP, 0);         // Important - sets PIO pins to known state!
    dacWrite(WRITE, CTRL, initData); // Write initial configuration data to DAC

    uint32_t count = 0;

    while(true)
    {
        dacWrite(WRITE, DAC, count);
        count = count + 1;
        if(count > 0xFFFFF)
        {
            count = 0;
        }
    }

    return 0;
}