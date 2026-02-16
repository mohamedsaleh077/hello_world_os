#include <stddef.h>
#include <stdbool.h>

/* Get Size of String, The maximum possible memory exist */
size_t strlen(const char* str)
 /* using pointer to not decalre var again, 
    just point to its adderss and keep it const to make it safe */
{
	size_t len = 0;
	while (str[len]) {
		len++;
    }
	return len;
}

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

void get_time(char* time){
    outb(0x70, 0x04 | 0x80);
    uint8_t raw_time = inb(0x71);

    time[0]= (raw_time >> 4) + '0';
    time[1]= (raw_time & 0x0F) + '0';
    time[2]= ':';

    outb(0x70, 0x02 | 0x80);
    uint8_t raw_min = inb(0x71);

    time[3]= (raw_min >> 4) + '0';
    time[4]= (raw_min & 0x0F) + '0';
}