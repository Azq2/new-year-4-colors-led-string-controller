#pragma once

#include <libopencm3/stm32/rcc.h>

// 32 khz
#define PWM_PERIOD (rcc_apb1_frequency / 1000)

void pwm_set_value(uint32_t tim, uint32_t oc, uint32_t v);
void pwm_oc_setup(uint32_t tim, uint32_t oc);
void pwm_setup(void);
void main_hw_setup(void);
void debug_setup(void);
void sys_tick_handler(void);
void hard_fault_handler(void);
void mem_manage_handler(void);
void bus_fault_handler(void);
void usage_fault_handler(void);
