#include "effects.h"
#include "leds.h"
#include "utils.h"

static uint32_t generic_effect_run(int leds_states[][LED_COUNT], int states_cnt, uint32_t speed, uint32_t fade_in, uint32_t fade_out, float min, float max, uint8_t max_loops, bool reset, bool *can_stop) {
	static int cursor;
	static int last_cursor;
	static int loops;
	
	if (reset) {
		cursor = 0;
		last_cursor = -1;
		loops = max_loops;
	}
	
	for (int i = 0; i < LED_COUNT; i++) {
		if (last_cursor != -1 && leds_states[last_cursor][i] == leds_states[cursor][i])
			continue;
		
		if (leds_states[cursor][i]) {
			led_set_animated_value(i, min, max, fade_in);
		} else {
			led_set_animated_value(i, max, min, fade_out);
		}
	}
	
	last_cursor = cursor;
	cursor++;
	
	if (cursor >= states_cnt)
		cursor = 0;
	
	
	if (!loops) {
		*can_stop = true;
		loops = max_loops;
	} else {
		loops--;
	}
	
	return speed;
}

uint32_t effect_1_run(bool reset, bool *can_stop) {
	int leds_states[][LED_COUNT] = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	};
	return generic_effect_run(leds_states, COUNT_OF(leds_states), 800, 800, 800, 0, 100, 12, reset, can_stop);
}

uint32_t effect_2_run(bool reset, bool *can_stop) {
	int leds_states[][LED_COUNT] = {
		{1, 0, 0, 0},
		{1, 1, 0, 0},
		{1, 1, 1, 0},
		{1, 1, 1, 1},
		
		{1, 1, 1, 0},
		{1, 1, 0, 0},
		{1, 0, 0, 0},
		{0, 0, 0, 0},
	};
	return generic_effect_run(leds_states, COUNT_OF(leds_states), 800, 200, 200, 0, 100, 12, reset, can_stop);
}

uint32_t effect_3_run(bool reset, bool *can_stop) {
	int leds_states[][LED_COUNT] = {
		{1, 0, 0, 0},
		{1, 1, 0, 0},
		{1, 1, 1, 0},
		{1, 1, 1, 1},
		
		{0, 1, 1, 1},
		{0, 0, 1, 1},
		{0, 0, 0, 1},
		{0, 0, 0, 0},
	};
	return generic_effect_run(leds_states, COUNT_OF(leds_states), 800, 200, 200, 0, 100, 12, reset, can_stop);
}

uint32_t effect_4_run(bool reset, bool *can_stop) {
	int leds_states[][LED_COUNT] = {
		{1, 0, 1, 0},
		{0, 1, 0, 1},
		{1, 1, 0, 0},
		{0, 0, 1, 1},
		{1, 0, 0, 1},
		{0, 1, 1, 0}
	};
	return generic_effect_run(leds_states, COUNT_OF(leds_states), 800, 200, 200, 0, 100, 12, reset, can_stop);
}

uint32_t effect_5_run(bool reset, bool *can_stop) {
	int leds_states[][LED_COUNT] = {
		{1, 1, 1, 1},
		{0, 0, 0, 0}
	};
	return generic_effect_run(leds_states, COUNT_OF(leds_states), 800, 200, 200, 0, 100, 12, reset, can_stop);
}

uint32_t effect_6_run(bool reset, bool *can_stop) {
	int leds_states[][LED_COUNT] = {
		{1, 0, 0, 0},
		{0, 0, 0, 1},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
		{1, 0, 0, 0},
		{0, 0, 1, 0},
		{0, 1, 0, 0},
	};
	return generic_effect_run(leds_states, COUNT_OF(leds_states), 333, 200, 200, 0, 100, 12, reset, can_stop);
}

uint32_t effect_7_run(bool reset, bool *can_stop) {
	int leds_states[][LED_COUNT] = {
		{1, 0, 0, 0},
		{0, 0, 0, 0},
		
		{0, 1, 0, 0},
		{0, 0, 0, 0},
		
		{0, 0, 1, 0},
		{0, 0, 0, 0},
		
		{0, 0, 0, 1},
		{0, 0, 0, 0},
	};
	return generic_effect_run(leds_states, COUNT_OF(leds_states), 333, 200, 200, 0, 100, 12, reset, can_stop);
}

uint32_t effect_8_run(bool reset, bool *can_stop) {
	int leds_states[][LED_COUNT] = {
		{1, 0, 0, 0},
		{0, 0, 0, 0},
		
		{0, 1, 0, 0},
		{0, 0, 0, 0},
		
		{0, 0, 1, 0},
		{0, 0, 0, 0},
		
		{0, 0, 0, 1},
		{0, 0, 0, 0},
	};
	return generic_effect_run(leds_states, COUNT_OF(leds_states), 333, 200, 200, 0, 100, 12, reset, can_stop);
}
