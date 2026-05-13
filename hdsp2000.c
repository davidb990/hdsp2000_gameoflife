#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hdsp2000.pio.h"
#include "hardware/regs/pio.h"
#include "hardware/structs/pio.h"
#include "pico/multicore.h"
#include "pico/rand.h"

volatile uint col_index;
volatile uint32_t column_data[5];
volatile uint32_t new_data[5];
uint32_t xy_data[7];


void core1_entry() {
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &hdsp2000_program);
    printf("Loaded program at %d\n", offset);
    hdsp2000_program_init(pio, 0, offset, 14, 15);

    for (int i = 0; i < 5; i++) {
        column_data[i] = get_rand_32();
    }

    while (true) {
        for (int i = 0; i < 5; i++) {
            put_column_data(pio, 0, column_data[i]);
            while(!(pio0_hw->intr)) {};
            gpio_put(i, true);
            sleep_ms(1);
            gpio_put(i, false);
            irq_clear(PIO0_IRQ_0);
        }
    }
}   


void xy_to_col_data() {
    for (int row = 4; row >= 0; row--) {
        for (int digit = 3; digit >= 0; digit--) {
            for (int i = 6; i >= 0; i--) {
                new_data[4-row] = new_data[4-row] | (((xy_data[i] & (0x1u << (5*digit+row))) ? 0x1u : 0x0u) << ((6-i) + 7*(digit)));
            }
        }
    }
}


int main() {
    stdio_init_all();
    col_index=0;

    gpio_init_mask(0b11111u);
    gpio_set_dir_out_masked(0b11111u);

    gpio_put_all(false);
    multicore_launch_core1(core1_entry);
    bool alive;

    sleep_ms(500);
    xy_data[0] = 0b11111111110111011111;
    xy_data[1] = 0b00100100001000100100;
    xy_data[2] = 0b00100100001000000100;
    xy_data[3] = 0b00100111000111000100;
    xy_data[4] = 0b00100100000000100100;
    xy_data[5] = 0b00100100001000100100;
    xy_data[6] = 0b00100111110111000100;

    xy_to_col_data();
    sleep_ms(500);
    column_data[0] = new_data[0];
    column_data[1] = new_data[1];
    column_data[2] = new_data[2];
    column_data[3] = new_data[3];
    column_data[4] = new_data[4];

    while (true) {
        tight_loop_contents();       
    }
}

