#ifndef __ASTROID_H__
#define __ASTROID_H__

#include <stdlib.h>

#define ASTROID_WIDTH 16
#define ASTROID_PAGES 2

uint8_t astroid_form[ASTROID_WIDTH][ASTROID_PAGES] =
{
	0xC0, 0x04, 0x30, 0x0B, 0x10, 0x10, 0x1C, 0x13, 0x82, 0x64, 0x42, 0x86, 0x84, 0x41, 0x32, 0x22,
	0x49, 0x25, 0xB1, 0x48, 0x82, 0x44, 0x02, 0x83, 0x44, 0x50, 0xA8, 0x2C, 0x98, 0x06, 0x00, 0x01
};

typedef struct astroid
{
	int x;
	int y;
	int dx;
	int dy;
	int t;
} astroid_t;

void astroid_update(astroid_t *astroid)
{
	if (astroid->t == 0)
		return;

	if ((astroid->x < -ASTROID_WIDTH || astroid->x >= 256))
	//  &&
	//     (astroid->y < 0 || astroid->y >= 64))
	{
		astroid->t = 0;
		return;
	}

	astroid->x -= astroid->dx;
	// astroid->y += astroid->dy;
}


void astroid_init(astroid_t *astroid)
{
	astroid->x = 255;
	astroid->y = rand() % 32;
	astroid->dx = 1 + (rand() % 3);
	astroid->dy = 0;
	astroid->t = 1;
}

#endif