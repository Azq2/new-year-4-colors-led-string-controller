#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define LED_COUNT	4

#define LED_G	0
#define LED_Y	1
#define LED_B	2
#define LED_R	3

struct led_state {
	float from;
	float to;
	float current;
	uint32_t start;
	uint32_t period;
	bool enabled;
};

float led_get_value(uint8_t led);
void led_set_value(uint8_t led, float v);
void led_set_animated_value(uint8_t n, float from, float to, uint32_t period);
void led_set_immediate_value(uint8_t n, float v);
void led_animator_task(void);
