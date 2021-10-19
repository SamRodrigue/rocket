#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <stdlib.h>

typedef enum shape_type
{
	circle = 0,
	square,
	triangle,
	shape_count
} shape_type_t;

#define SHAPE_WIDTH 8

uint8_t shape_form[shape_count][SHAPE_WIDTH] =
{
	{ 0xC0, 0xB0, 0x8C, 0x83, 0x83, 0x8C, 0xB0, 0xC0 },
	{ 0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3C },
	{ 0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF }
};

typedef struct shape
{
	shape_type_t type;
	int x;
	int y;
	int dx;
	int dy;
	int t;
} shape_t;

void shape_update(shape_t *shape)
{
	if (shape->t == 0)
		return;

	shape->t--;

	shape->x += shape->dx;
	shape->y += shape->dy;

	if (shape->x < 0)
	{
		shape->x *= -1;
		shape->dx *= -1;
	}

	if (shape->x >= 256 - 8)
	{
		shape->x = 512 - 16 - shape->x;
		shape->dx *= -1;
	}

	if (shape->y < 0)
	{
		shape->y *= -1;
		shape->dy *= -1;
	}

	if (shape->y >= 64 - 8)
	{
		shape->y = 128 - 16 - shape->y;
		shape->dy *= -1;
	}
}


void shape_init(shape_type_t type, shape_t *shape)
{
	shape->type = type;
	shape->x = rand() % 256;
	shape->y = rand() % 32;

	do
	{
		shape->dx = rand() % 6 - 3;
		shape->dy = rand() % 6 - 3;
	}
	while(shape->dx == 0 && shape->dy == 0);

	shape->t = 100;
}

#endif