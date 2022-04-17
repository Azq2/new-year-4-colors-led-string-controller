#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/dwt.h>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "utils.h"

static int32_t uart_for_prinf = -1;

void gpio_set_all_analog(void) {
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	
	// PA13 & PA14 - SWDIO / SWDCLK
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_ALL & ~(GPIO13 | GPIO14));
	gpio_mode_setup(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_ALL);
	gpio_mode_setup(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_ALL);
	
	rcc_periph_clock_disable(RCC_GPIOB);
	rcc_periph_clock_disable(RCC_GPIOC);
}

void uart_simple_setup(uint32_t usart, uint32_t baudrate, bool use_for_printf) {
	usart_set_baudrate(usart, baudrate);
	usart_set_databits(usart, 8);
	usart_set_stopbits(usart, USART_STOPBITS_1);
	usart_set_parity(usart, USART_PARITY_NONE);
	usart_set_flow_control(usart, USART_FLOWCONTROL_NONE);
	usart_set_mode(usart, USART_MODE_TX_RX);
	usart_enable(usart);
	
	if (use_for_printf)
		uart_for_prinf = USART1;
}

/*
__attribute__((used))
int _write(int file, char *ptr, int len) {
	int i;
	if (uart_for_prinf > -1 && (file == STDOUT_FILENO || file == STDERR_FILENO)) {
		for (i = 0; i < len; ++i)
			usart_send_blocking(uart_for_prinf, ptr[i]);
		return i;
	}
	errno = EIO;
	return -1;
}

int puts(const char *s) {
	int c = 0;
	while (s[c]) {
		usart_send_blocking(uart_for_prinf, s[c]);
		c++;
	}
	return c;
}

int *__errno(void) {
	static int err = 0;
	return &err;
}
*/
