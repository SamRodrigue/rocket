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

#define ASTROID_COUNT 12
#define ASTROID_TIMER 25 + (rand() % 25);

typedef struct astroidfield
{
	astroid_t astroids[ASTROID_COUNT];
	int timer;
} astroidfield_t;

void astroid_update(astroid_t *astroid)
{
	if (astroid->t == 0)
		return;

	astroid->x -= astroid->dx;

	if (astroid->x < -ASTROID_WIDTH)
		astroid->t = 0;
}


void astroid_init(astroid_t *astroid)
{
	astroid->x = 255;
	astroid->y = rand() % (64 - (8 * ASTROID_PAGES));
	astroid->dx = 1 + (rand() % 3);
	astroid->dy = 0;
	astroid->t = 1;
}


void astroidfield_update(astroidfield_t *astroidfield)
{
	for (uint8_t i = 0; i < ASTROID_COUNT; ++i)
		astroid_update(&astroidfield->astroids[i]);

	if (astroidfield->timer)
	{
		astroidfield->timer--;
		return;
	}

	astroidfield->timer = ASTROID_TIMER;

	for (uint8_t i = 0; i < ASTROID_COUNT; ++i)
	{
		if (astroidfield->astroids[i].t)
			continue;

		astroid_init(&astroidfield->astroids[i]);
		break;
	}
}


void astroidfield_init(astroidfield_t *astroidfield)
{
	astroidfield->timer = ASTROID_TIMER;
	for (uint8_t i = 0; i < ASTROID_COUNT; ++i)
		astroidfield->astroids[i].t = 0;
}


typedef struct star
{
	int x;
	int y;
	int dx;
	int dy;
	int t;
} star_t;

#define STAR_COUNT 8

typedef struct starfield
{
	star_t stars[STAR_COUNT];
} starfield_t;


void star_init(star_t*);
void star_update(star_t *star)
{
	if (star->t == 0)
		return;

	star->x -= star->dx;

	if (star->x < 0)
		star_init(star);
}


void star_init(star_t *star)
{
	star->x = 255;
	star->y = rand() % 64;
	star->dx = 1 + (rand() % 5);
	star->dy = 0;
	star->t = 1;
}


void starfield_update(starfield_t *starfield)
{
	for (uint8_t i = 0; i < STAR_COUNT; ++i)
		star_update(&starfield->stars[i]);
}


void starfield_init(starfield_t *starfield)
{
	for (uint8_t i = 0; i < STAR_COUNT; ++i)
	{
		star_init(&starfield->stars[i]);
		starfield->stars[i].x = rand() % 256;
	}
}

#endif