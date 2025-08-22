#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
// #include "hardware/timer.h"

#include "quadrature_encoder.pio.h"

uint PWM_init();

/* Define PWM Pins, GPIO12: IN1, GPIO13: IN2 */
#define IN1 8
#define IN2 9

/*
int main() {
    int new_value, delta, old_value = 0; 
    int last_value = -1, last_delta = -1;

    // Base pin to connect the A phase of the encoder.
    // The B phase must be connected to the next pin
    const uint PIN_AB = 12;

    stdio_init_all();
    printf("Hello from quadrature encoder\n");


    uint slice_num = PWM_init();

    PIO pio = pio0;
    const uint sm = 0;

    // we don't really need to keep the offset, as this program must be loaded
    // at offset 0
    pio_add_program(pio, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio, sm, PIN_AB, 0);

    pwm_set_chan_level(slice_num, PWM_CHAN_A, 200);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 255 - 200);

    while (1) {
        // note: thanks to two's complement arithmetic delta will always
        // be correct even when new_value wraps around MAXINT / MININT
        new_value = quadrature_encoder_get_count(pio, sm);
        delta = new_value - old_value;
        old_value = new_value;

        if (new_value != last_value || delta != last_delta ) {
            printf("position %8d, delta %6d\n", new_value, delta);
            last_value = new_value;
            last_delta = delta;
        }
        sleep_ms(100);
    }
}*/

int main()
{
    stdio_init_all();
    uint slice_num = PWM_init();

    // Initialize GPIO6 as output
    gpio_init(6);
    gpio_set_dir(6, GPIO_OUT);
    
    // Enable the motor driver (set sleep pin HIGH)
    gpio_put(6, 1);


    while (true) {
        /* Cycle through full PWM range */
        for(int dc = 0; dc <= 255; dc++) 
        {
            pwm_set_chan_level(slice_num, PWM_CHAN_A, dc);
            pwm_set_chan_level(slice_num, PWM_CHAN_B, 255 - dc);
            sleep_ms(1);
        }
        for(int dc = 255; dc >= 0; dc--) 
        {
            pwm_set_chan_level(slice_num, PWM_CHAN_A, dc);
            pwm_set_chan_level(slice_num, PWM_CHAN_B, 255 - dc);
            sleep_ms(1);
        }
    }
}


uint PWM_init()
{
    /* Set pins for PWM */
    gpio_set_function(IN1, GPIO_FUNC_PWM);
    gpio_set_function(IN2, GPIO_FUNC_PWM);

    /* Get PWM slice for the pins, should be 6 for 12 and 13 */
    uint slice_num = pwm_gpio_to_slice_num(IN1);

    /* Set wrapping value to 255 for 8-bit resolution */
    pwm_set_wrap(slice_num, 255);

    /* Set clock division for freqency of 1khz, using formula clockdiv=sysclock/frequency*(wrap+1) */
    pwm_set_clkdiv(slice_num, 48.8f);

    /* Start both channels at 0 */
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 0);

    /* Enable the slice */
    pwm_set_enabled(slice_num, true);

    return slice_num;
}
