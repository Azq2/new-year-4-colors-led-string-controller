#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
#include <stdio.h>

#include "hw.h"
#include "swtimer.h"
#include "utils.h"

void swtimer_enable_hook(bool enable) {
	if (enable) {
		printf("swtimer enable!\r\n");
	} else {
		printf("swtimer idle!\r\n");
	}
}

void swtimer_idle_hook(void) {
	__WFI();
}

void sys_tick_handler(void) {
	swtimer_interrupt();
}

void hard_fault_handler(void) {
	printf("hard_fault_handler!\r\n");
	while (true);
}

void mem_manage_handler(void) {
	printf("hard_fault_handler!\r\n");
	while (true);
}

void bus_fault_handler(void) {
	printf("hard_fault_handler!\r\n");
	while (true);
}

void usage_fault_handler(void) {
	printf("hard_fault_handler!\r\n");
	while (true);
}

void pwm_set_value(uint32_t tim, uint32_t oc, uint32_t v) {
	timer_set_oc_value(tim, oc, v);
}

void pwm_oc_setup(uint32_t tim, uint32_t oc) {
	timer_set_oc_mode(tim, oc, TIM_OCM_PWM1);
	timer_enable_oc_preload(tim, oc);
	timer_set_oc_polarity_low(tim, oc);
	timer_enable_oc_output(tim, oc);
	
	timer_set_oc_value(tim, oc, 0);
}

void pwm_setup(void) {
	// TIM1
	rcc_periph_reset_pulse(RST_TIM1);
	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_prescaler(TIM1, 0);
	timer_set_period(TIM1, PWM_PERIOD);
	timer_continuous_mode(TIM1);
	timer_enable_break_main_output(TIM1);
	
	pwm_oc_setup(TIM1, TIM_OC2);
	
	timer_enable_preload(TIM1);
	timer_enable_counter(TIM1);
	
	// TIM3
	rcc_periph_reset_pulse(RST_TIM3);
	timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_prescaler(TIM3, 0);
	timer_set_period(TIM3, PWM_PERIOD);
	timer_continuous_mode(TIM3);
	
	pwm_oc_setup(TIM3, TIM_OC1);
	pwm_oc_setup(TIM3, TIM_OC2);
	
	timer_enable_preload(TIM3);
	timer_enable_counter(TIM3);
}

void main_hw_setup(void) {
	gpio_set_all_analog();
	
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_TIM1);
	rcc_periph_clock_enable(RCC_TIM3);
	
	// PWM
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7 | GPIO9);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO6);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO7);
	gpio_set_af(GPIOA, GPIO_AF2, GPIO9);
	
	pwm_setup();
	
	// systick
	systick_set_clocksource(STK_CSR_CLKSOURCE_EXT);
	STK_CVR = 0;
	systick_set_reload(rcc_ahb_frequency / 8 / 1000);
	systick_counter_enable();
	systick_interrupt_enable();
	
	// key
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO5);
}

void debug_setup(void) {
	// UART
	rcc_periph_clock_enable(RCC_USART1);
	
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	
	gpio_set_af(GPIOA, GPIO_AF1, GPIO2);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO3);
	
	uart_simple_setup(USART1, 115200, 1);
}
