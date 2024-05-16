#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
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

// Linearity compensation bits (see datasheet pg. 24: https://www.analog.com/media/en/technical-documentation/data-sheets/ad5791.pdf)
#define LINCOMP 0b1100

// Pinout
#define SDIN  4
#define SCLK  5
#define LDAC  3 // Inverting input
#define SYNC  6 // Inverting input

// V1 extra pins:
// #define SDO   3
// #define RESET 7 // Inverting input
// #define CLEAR 8 // Inverting input

// PIO global variables
PIO pio;
uint sm;
uint offset;

// Life indicator - use core 2 to blink built-in LED
void core2()
{
    gpio_init(25);
    gpio_set_dir(25, true);

    while (true)
    {
        gpio_put(25, 1);
        sleep_ms(10);
        gpio_put(25, 0);
        sleep_ms(4990);
    }
    
}

void dacWrite(uint32_t readwrite, uint32_t reg, uint32_t data)
{
    data = data % (0xFFFFF + 1);                                                      // Limit data value to 0xFFFFF max                              
    uint32_t packedData = packedData | (readwrite << 31) | (reg << 28) | (data << 8); // Pack read/write bit, register address bits and data bits
    pio_sm_put_blocking(pio, sm, packedData);                                         // Send packed data to PIO
    pio_sm_get_blocking(pio, sm);                                                     // SPI readback, to prevent stalling
}

int main() 
{
    stdio_init_all();
    
    set_sys_clock_khz(200000, true);
    multicore_launch_core1(core2);

    // Configure SMPS to operate in PWM mode
    gpio_init(23);
    gpio_set_dir(23, true);
    gpio_put(23, 1);
    
    // V1 extra pins:
    // gpio_pull_up(RESET); // Pull RESET pin high
    // gpio_pull_up(CLEAR); // Pull CLEAR pin high

    gpio_put(LDAC, 0);   // Set LDAC low to enable synchronous update

    // Initialize and configure PIO
    pio = pio0;
    sm = pio_claim_unused_sm(pio, true);
    offset = pio_add_program(pio, &output_program);
    output_program_init(pio, sm, offset, (float)10);
    pio_sm_set_enabled(pio, sm, true);

    // V1 extra pins:
    // gpio_put(RESET, 0);
    // sleep_ms(10);
    // gpio_put(RESET, 1);

    // Data: 00000000001100110010   RBUF       OPGND      DACTRI     BIN/2sC    SDODIS     LINCOMP
    uint32_t initData = initData | (1 << 1) | (0 << 2) | (0 << 3) | (1 << 4) | (0 << 5) | (LINCOMP << 6);

    dacWrite(WRITE, NOP, 0);         // Important - sets PIO pins to known state!
    dacWrite(WRITE, CTRL, initData); // Write initial configuration data to DAC

    uint32_t count = 0;

    uint32_t requestedCode = 0;
    uint32_t newInput = 0;
    char inputBuffer[32] = {0};

    while(true)
    {
        newInput = scanf("%s", &inputBuffer, 31);         // Read input from serial port
        requestedCode = atoi(inputBuffer);                // Convert input to integer
        inputBuffer[32] = {0};                            // Clear input buffer
        dacWrite(WRITE, DAC, requestedCode);              // Write requested code to DAC
        // printf("Returned code: %d\n", requestedCode);

    }

    return 0;
}