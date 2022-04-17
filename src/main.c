#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "hw.h"
#include "utils.h"
#include "swtimer.h"
#include "color.h"

struct {
	swtimer_t *key;
	swtimer_t *effect;
} tasks;

static void onclick_handler(uint8_t clicks, bool hold) {
	//printf("onclick, clicks=%d, hold=%d\r\n", clicks, hold);
}

static void set_rgb_color(uint8_t r, uint8_t g, uint8_t b) {
	int pwm_r = PWM_PERIOD * r / 255;
	int pwm_g = PWM_PERIOD * g / 255;
	int pwm_b = PWM_PERIOD * b / 255;
	
	pwm_set_value(TIM3, TIM_OC1, pwm_r);
	pwm_set_value(TIM3, TIM_OC2, pwm_g);
	pwm_set_value(TIM1, TIM_OC2, pwm_b);
}

static void set_hsv_color(double h, double s, double v) {
	hsv_t hsv_color = {h, s, v};
	rgb_t rgb_color = hsv2rgb(hsv_color);
	
	pwm_set_value(TIM3, TIM_OC1, PWM_PERIOD * rgb_color.r);
	pwm_set_value(TIM3, TIM_OC2, PWM_PERIOD * rgb_color.g);
	pwm_set_value(TIM1, TIM_OC2, PWM_PERIOD * rgb_color.b);
}

static void set_color_wheel(int color) {
	color = color % 1531;
	
	if (color <= 255) {
		set_rgb_color(255, color, 0);
	} else if (color > 255 && color <= 510) {
		set_rgb_color(510 - color, 255, 0);
	} else if (color > 510 && color <= 765) {
		set_rgb_color(0, 255, color - 510);
	} else if (color > 765 && color <= 1020) {
		set_rgb_color(0, 1020 - color, 255);
	} else if (color > 1020 && color <= 1275) {
		set_rgb_color(color - 1020, 0, 255);
	} else if (color > 1275 && color <= 1530) {
		set_rgb_color(255, 0, 1530 - color);
	}
}

static void set_color_kelvin(int temperature) {
	int r, g, b;
	float kelvin = temperature / 100;

	// red
	if (kelvin <= 66) {
		r = 255;
	} else {
		r = kelvin - 60;
		r = (float) pow(r, -0.1332047592);
		r *= 329.698727446f;
		
		if (r > 255)
			r = 255;
		if (r < 0)
			r = 0;
	}

	// green
	if (kelvin <= 66) {
		g = kelvin;
		g = 99.4708025861f * log(g) - 161.1195681661f;
		
		if (g > 255)
			g = 255;
		if (g < 0)
			g = 0;
	} else {
		g = kelvin - 60;
		g = (float) pow(g, -0.0755148492f);
		g *= 288.1221695283f;
		
		if (g > 255)
			g = 255;
		if (g < 0)
			g = 0;
	}

	// blue
	if (kelvin >= 66) {
		b = 255;
	} else if (kelvin <= 19) {
		b = 0;
	} else {
		b = kelvin - 10;
		b = 138.5177312231f * log(b) - 305.0447927307f;
		
		if (b > 255)
			b = 255;
		if (b < 0)
			b = 0;
	}
	
	set_rgb_color(r, g, b);
}



static void effect_task(void) {
	static int hue = 0;
	
	// set_color_wheel(hue);
	
	set_color_kelvin(1000 + (hue % 39000));
	
	// set_hsv_color(hue % 360, 1, 1);
	
	hue++;
}

static void key_handler_task(void) {
	static uint32_t start = 0;
	static bool pressed = false;
	static uint8_t clicks = 0;
	
	if (swtimer_counter() - start > 80) {
		bool state = !gpio_get(GPIOA, GPIO5);
		if (pressed && !state) {
			//printf("key release [ %ld ms ]\r\n", swtimer_counter() - start);
			pressed = false;
			start = swtimer_counter();
		} else if (!pressed && state) {
			//printf("key press\r\n");
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
	//rcc_clock_setup_in_hsi_out_48mhz();
	main_hw_setup();
	debug_setup();
	
	//printf("Start! PWM_PERIOD=%ld\r\n", PWM_PERIOD);
	
	tasks.effect = swtimer_create(effect_task);
	tasks.key = swtimer_create(key_handler_task);
	
	swtimer_set_interval(tasks.effect, 1000 / 60, false);
	swtimer_set_interval(tasks.key, 1000 / 60, false);
	
//	set_hsv_color(23, 1, 0.1);
	
	while (true) {
		swtimer_schedule();
	}
	
	return 0;
}
