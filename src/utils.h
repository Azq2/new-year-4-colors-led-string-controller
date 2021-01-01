#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __WFI() __asm__ volatile ("wfi")
#define __WFE() __asm__ volatile ("wfe")
#define COUNT_OF(array) (sizeof(array) / sizeof(array[0]))
#define gpio_write_bit(port, pins, value) ((value) ? gpio_set(port, pins) : gpio_clear(port, pins))

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

__attribute__((always_inline)) static inline void __enable_irq(void) {
	__asm__ volatile ("cpsie i" : : : "memory");
}

__attribute__((always_inline)) static inline uint32_t __disable_irq(void) {
	uint32_t result;
	__asm__ volatile ("MRS %0, primask\ncpsid i" : "=r" (result));
	return result;
}

int _write(int file, char *ptr, int len);
void gpio_set_all_analog(void);
void uart_simple_setup(uint32_t usart, uint32_t baudrate, bool printf);

#ifdef __cplusplus
}
#endif
