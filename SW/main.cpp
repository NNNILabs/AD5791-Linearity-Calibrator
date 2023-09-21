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

// Pinout
#define SDO   3
#define SDIN  4
#define SCLK  5
#define SYNC  6 // Inverting input
#define RESET 7 // Inverting input
#define CLEAR 8 // Inverting input
#define LDAC  9 // Inverting input

// Life Indicator
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

    while(true)
    {
        
    }

    return 0;
}