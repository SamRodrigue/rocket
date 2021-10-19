// SSD1306 https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdlib.h>
#include <string.h>

#define DISPLAY_PAGES 8  // 32 column, each page is 8 segments
#define DISPLAY_COMS 128 // 128 rows
#define DISPLAY_BUFFER (DISPLAY_PAGES * DISPLAY_COMS)

static uint8_t display_init_sequence[] = {
	0x00,
	0xae,       // [F] Set display enable: [0] 0 off 1 on
	0xd5, 0x80, // [T] Set display clock divide: [3:0] divide ratio (+1). [7:4] oscillator frequency
	0xa8, 0x3f, // [H] Set multiplex ratio: [5:0] 16-64 MUX (+1), 0-14 is invalid
	0xd3, 0x00, // [H] Set display offset: [5:0] vertical shift by COM
	0x40,       // [H] Set display start line: [5:0] RAM display start line
	0x8d, 0x14, // [C] Set charge pump: [2] 0 disable charge pump 1 enable
	0x20, 0x01, // [A] Set memory addressing mode: [1:0] 00 horizontal 01 vertical 10 page 11 invalid
	0xa0,       // [H] Set segment re-map, column address to map to SEG0: a0 = 0, a1 = 127
	0xc0,       // [H] Set COM output scan direction: [3] 0 normal COM0-COM[N-1] 1 reverse
	0xda, 0x12, // [H] Set COM pins order: [4] 0 sequential 1 alternative [5] 0 disable left/right 1 enable left/right
	0x81, 0xcf, // [F] Set contract control: [7:0] 1 to 256 contrast value (+1)
	//0xd9, 0xf1, // [T] Set pre-charge period: [3:0] phase 1 period, [7:4] phase 2 period
	//0xdb, 0x20, // [T] Set Vcomh deselect level
	0xa4,       // [F] Set display output according to GDDRAM: [0] 0 enable 1 ignore
	0xa6,       // [F] Set display iversion: [0] 0 normal (active high) 1 inverse (active low)
	//0x2e,       // [S] Stop scrolling
	0xaf        // [F] Set display enable: [0] 0 off 1 on
};

// static uint8_t display_init_sequence[] = {
// 	0x00,       // commands
// 	0xa8, 0x3f, // set MUX ratio
// 	0xd3, 0x00, // set display offset
// 	0x40,       // set display start line
// 	0xa0,       // set segment re-map, a0 = start at 0, a1 = start at 127
// 	0xc0,       // set COM output scan direction, c0 = normal, c8 = reverse
// 	0xda, 0x02, // set COM pins hardware configuration
// 	0x81, 0x7f, // set contrast control
// 	0xa4,       // disable entiree display on
// 	0xa6,       // set normal display
// 	0xd5, 0x80, // set osccillator frequency
// 	0x8d, 0x14, // enable charge pump regulator
// 	0xaf        // display on
// };

// TODO: Is this needed after a reset
static uint8_t display_pager[] = {
	// non-continuous/command
	0x00,
	// set com address: start address, end address
	0x21, 0x00, DISPLAY_COMS - 1,
	// set page address: start address, end address
	0x22, 0x00, DISPLAY_PAGES - 1
};

typedef struct __attribute__((__packed__)) display_pager {
	uint8_t req1;
	uint8_t req2;
	uint8_t start_com;
	uint8_t end_com;
	uint8_t req3;
	uint8_t start_page;
	uint8_t end_page;
} display_pager_t;

typedef struct __attribute__((__packed__)) display_buffer {
	uint8_t header;
	uint8_t data[DISPLAY_COMS][DISPLAY_PAGES];
} display_buffer_t;

typedef struct display {
	uint8_t address;
	i2c_inst_t *port;
	display_buffer_t buffer;
} display_t;


static display_pager_t display_make_pager(uint8_t start_com, uint8_t start_page, uint8_t end_com, uint8_t end_page)
{
	display_pager_t pager = { 0x00, 0x21, start_com, end_com, 0x22, start_page, end_page };
	return pager;
}


static void display_set_pager(display_pager_t *pager, display_t const *display)
{
	i2c_write_blocking(display->port, display->address, (uint8_t*)pager, sizeof(display_pager_t), false);
}


void display_resize(uint8_t start_com, uint8_t start_page, uint8_t end_com, uint8_t end_page, display_t const *display)
{
	display_pager_t pager = display_make_pager(start_com, start_page, end_com, end_page);
	display_set_pager(&pager, display);
}


// void display_draw(uint8_t start_com, uint8_t start_page, uint8_t end_com, uint8_t end_page, display_t const *display)
// {
// 	uint8_t display_sub_buffer[DISPLAY_BUFFER + 1];
// 	uint8_t width = end_com - start_com;
// 	uint8_t height = end_page - start_page;
// 	size_t size = width * height + 1;

// 	display_sub_buffer[0] = 0x40;

// 	size_t index = 0;
// 	for (uint8_t com = start_com; com < width; ++com)
// 		for (uint8_t page = 0; page < height; ++page)
// 			display_sub_buffer[++index] = display->buffer.data[com][page];

// 	i2c_write_blocking(display->port, display->address, display_sub_buffer, size, false);
// }


void display_draw(display_t const *display)
{
	printf("draw %d\n", display->address);
	i2c_write_blocking(display->port, display->address, (uint8_t*)&display->buffer, DISPLAY_BUFFER + 1, false);
}


inline void display_clear(display_t *display)
{
	memset(display->buffer.data, 0, DISPLAY_BUFFER);
}


void display_init(uint8_t sda_gpio, uint8_t scl_gpio, display_t *display)
{
	if ((sda_gpio == 0 &&  scl_gpio == 1)  ||
	    (sda_gpio == 4 &&  scl_gpio == 5)  ||
	    (sda_gpio == 8 &&  scl_gpio == 9)  ||
	    (sda_gpio == 12 && scl_gpio == 13) ||
	    (sda_gpio == 16 && scl_gpio == 17) ||
	    (sda_gpio == 20 && scl_gpio == 21))
		display->port = i2c0;
	else if ((sda_gpio == 2 && scl_gpio == 3) ||
	         (sda_gpio == 6 && scl_gpio == 7) ||
	         (sda_gpio == 10 && scl_gpio == 11) ||
	         (sda_gpio == 14 && scl_gpio == 15) ||
	         (sda_gpio == 18 && scl_gpio == 19) ||
	         (sda_gpio == 26 && scl_gpio == 27))
		display->port = i2c1;
	else
		passert(false, "Incorrect gpoi pins used for i2c sda/scl");

	i2c_init(display->port, 400 * 1000);
	gpio_set_function(sda_gpio, GPIO_FUNC_I2C);
	gpio_set_function(scl_gpio, GPIO_FUNC_I2C);
	gpio_pull_up(sda_gpio);
	gpio_pull_up(scl_gpio);

	display->buffer.header = 0x40; // Set display start line register from 0-63 [5:0]
	memset(display->buffer.data, 0, DISPLAY_BUFFER);

	i2c_write_blocking(display->port, display->address, display_init_sequence, sizeof(display_init_sequence), false);

	i2c_write_blocking(display->port, display->address, display_pager, sizeof(display_pager), false);

//#define DISPLAY_DEBUG
#ifdef DISPLAY_DEBUG
	uint8_t i = 0;
	while(1)
	{
		for (uint8_t c = 0; c < DISPLAY_COMS; ++c)
		{
			for (uint8_t p = 0; p < DISPLAY_PAGES; ++p)
			{
				display->buffer.data[c][p] = c + i;
			}
		}

		display_draw(display);
		i++;
	}
#endif
}

#endif