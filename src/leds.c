#include "leds.h"
#include "hw.h"
#include "utils.h"
#include "swtimer.h"

#include <libopencm3/stm32/timer.h>

static struct led_state leds[LED_COUNT] = {0};

float led_get_value(uint8_t n) {
	struct led_state *led = &leds[n];
	return led->current;
}

void led_set_value(uint8_t n, float v) {
	struct led_state *led = &leds[n];
	led->current = v;
	
	uint32_t pwm = (uint32_t) ((float) PWM_PERIOD / 100.0f * MAX(0, MIN(v, 100)));
	
	switch (n) {
		case LED_G:
			pwm_set_value(TIM3, TIM_OC1, pwm);
		break;
		
		case LED_R:
			pwm_set_value(TIM1, TIM_OC2, pwm);
		break;
		
		case LED_B:
			pwm_set_value(TIM3, TIM_OC2, pwm);
		break;
		
		case LED_Y:
			pwm_set_value(TIM1, TIM_OC3, pwm);
		break;
	}
}

void led_set_animated_value(uint8_t n, float from, float to, uint32_t period) {
	struct led_state *led = &leds[n];
	
	float current;
	float target;
	float estimated;
	
	if (to > from) {
		current = to - led->current;
		target = to - from;
		estimated = (target / current);
	} else {
		current = led->current - to;
		target = from - to;
		estimated = (current / target);
	}
	
	period = period * estimated;
	
	if (period == 0) {
		led->enabled = false;
		return;
	}
	
	led->from = from;
	led->to = to;
	led->start = swtimer_counter();
	led->period = period;
	led->enabled = true;
}

void led_set_immediate_value(uint8_t n, float v) {
	struct led_state *led = &leds[n];
	led->enabled = false;
	led_set_value(n, v);
}

void led_animator_task(void) {
	for (int i = 0; i < LED_COUNT; i++) {
		struct led_state *led = &leds[i];
		
		if (led->enabled) {
			uint32_t elapsed = MIN(swtimer_counter() - led->start, led->period);
			float value;
			
			if (elapsed >= led->period) {
				elapsed = led->period;
				led->enabled = false;
			}
			
			if (led->from > led->to) {
				value = led->from - ((led->from - led->to) / (float) led->period * (float) elapsed);
			} else {
				value = led->from + ((led->to - led->from) / (float) led->period * (float) elapsed);
			}
			
			led_set_value(i, value);
		}
	}
}
