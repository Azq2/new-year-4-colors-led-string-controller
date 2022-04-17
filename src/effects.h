#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint32_t (*led_effector_t)(bool reset, bool *can_stop);

uint32_t effect_1_run(bool reset, bool *can_stop);
uint32_t effect_2_run(bool reset, bool *can_stop);
uint32_t effect_3_run(bool reset, bool *can_stop);
uint32_t effect_4_run(bool reset, bool *can_stop);
uint32_t effect_5_run(bool reset, bool *can_stop);
uint32_t effect_6_run(bool reset, bool *can_stop);
uint32_t effect_7_run(bool reset, bool *can_stop);
uint32_t effect_8_run(bool reset, bool *can_stop);
