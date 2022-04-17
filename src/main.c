#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/gpio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "leds.h"
#include "hw.h"
#include "utils.h"
#include "swtimer.h"
#include "effects.h"

// PA10	- YELLOW (TIM1_CH3)
// PA9	- RED (TIM1_CH2)
// PA7	- BLUE (TIM3_CH2)
// PA6	- GREEN (TIM3_CH1)

struct {
	swtimer_t *animator;
	swtimer_t *effect;
	swtimer_t *next_effect;
	swtimer_t *key;
} tasks;

static uint8_t current_effect = 0;
static bool effect_first_run = true;
static bool manual_mode = false;
static uint8_t force_color_mode = 0;

static led_effector_t led_effects[] = {
	effect_1_run,
	effect_2_run,
	effect_3_run,
	effect_4_run,
	effect_5_run,
	effect_6_run,
	effect_7_run,
	effect_8_run,
};

static void effects_loop_task(void) {
	if (force_color_mode > 0)
		return;
	
	led_effector_t effect_callback = led_effects[current_effect];
	
	bool can_stop = false;
	
	uint32_t next_run = effect_callback(effect_first_run, &can_stop);
	effect_first_run = false;
	
	if (can_stop && !manual_mode) {
		swtimer_stop(tasks.effect);
		swtimer_set_timeout(tasks.next_effect, next_run, false);
	} else {
		swtimer_set_timeout(tasks.effect, next_run, false);
	}
}

static void switch_effect(bool next) {
	if (next) {
		current_effect++;
		if (current_effect >= COUNT_OF(led_effects))
			current_effect = 0;
		
		printf("NEXT EFFECT! %d\r\n", current_effect);
	} else {
		if (current_effect == 0)
			current_effect = COUNT_OF(led_effects);
		current_effect--;
		
		printf("PREV EFFECT! %d\r\n", current_effect);
	}
	
	effect_first_run = true;
	
	swtimer_set_timeout(tasks.effect, 0, false);
	swtimer_set_timeout(tasks.next_effect, 800 * 12, false);
}

static void next_effect_task(void) {
	switch_effect(true);
}

static void onclick_handler(uint8_t clicks, bool hold) {
	printf("onclick, clicks=%d, hold=%d\r\n", clicks, hold);
	
	if (hold) {
		force_color_mode++;
		
		if (force_color_mode >= 6)
			force_color_mode = 1;
		
		switch (force_color_mode) {
			case 1:
				led_set_immediate_value(LED_G, 100);
				led_set_immediate_value(LED_Y, 100);
				led_set_immediate_value(LED_B, 100);
				led_set_immediate_value(LED_R, 100);
			break;
			
			case 2:
				led_set_immediate_value(LED_G, 100);
				led_set_immediate_value(LED_Y, 0);
				led_set_immediate_value(LED_B, 0);
				led_set_immediate_value(LED_R, 0);
			break;
			
			case 3:
				led_set_immediate_value(LED_G, 0);
				led_set_immediate_value(LED_Y, 100);
				led_set_immediate_value(LED_B, 0);
				led_set_immediate_value(LED_R, 0);
			break;
			
			case 4:
				led_set_immediate_value(LED_G, 0);
				led_set_immediate_value(LED_Y, 0);
				led_set_immediate_value(LED_B, 100);
				led_set_immediate_value(LED_R, 0);
			break;
			
			case 5:
				led_set_immediate_value(LED_G, 0);
				led_set_immediate_value(LED_Y, 0);
				led_set_immediate_value(LED_B, 0);
				led_set_immediate_value(LED_R, 100);
			break;
		}
	} else {
		force_color_mode = 0;
		
		switch (clicks) {
			case 1:
				manual_mode = true;
				switch_effect(true);
			break;
			
			case 2:
				manual_mode = true;
				switch_effect(false);
			break;
			
			case 3:
				current_effect = COUNT_OF(led_effects);
				switch_effect(true);
				manual_mode = false;
			break;
		}
	}
}

static void key_handler_task(void) {
	static uint32_t start = 0;
	static bool pressed = false;
	static uint8_t clicks = 0;
	
	if (swtimer_counter() - start > 80) {
		bool state = !gpio_get(GPIOA, GPIO4);
		if (pressed && !state) {
			printf("key release [ %ld ms ]\r\n", swtimer_counter() - start);
			pressed = false;
			start = swtimer_counter();
		} else if (!pressed && state) {
			printf("key press\r\n");
			clicks++;
			pressed = true;
			start = swtimer_counter();
		}
	}
	
	if (clicks > 0 && swtimer_counter() - start >= 500) {
		onclick_handler(clicks, pressed);
		clicks = 0;
	}
}

int main(void) {
	rcc_clock_setup_in_hsi_out_48mhz();
	main_hw_setup();
	debug_setup();
	
	printf("Start! PWM_PERIOD=%ld\r\n", PWM_PERIOD);
	
	for (int i = 0; i < LED_COUNT; i++)
		led_set_immediate_value(i, 0);
	
	tasks.effect = swtimer_create(effects_loop_task);
	tasks.animator = swtimer_create(led_animator_task);
	tasks.next_effect = swtimer_create(next_effect_task);
	tasks.key = swtimer_create(key_handler_task);
	
	swtimer_set_interval(tasks.animator, 1000 / 60, false);
	swtimer_set_timeout(tasks.effect, 0, false);
	swtimer_set_timeout(tasks.next_effect, 800 * 12, false);
	swtimer_set_interval(tasks.key, 10, false);
	
	while (true) {
		swtimer_schedule();
	}
	
	return 0;
}
