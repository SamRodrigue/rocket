#ifndef __PAINTER_H__
#define __PAINTER_H__

#include "display.h"
#include "field.h"
#include "shape.h"
#include "rocket.h"
#include "astroid.h"

#include <string.h>

#define PAINTER_FONT_WIDTH 5
#define PAINTER_FONT_COUNT 12

#define PAINTER_FONT_COMS 16
#define PAINTER_FONT_PAGES 3

static uint8_t const painter_font[PAINTER_FONT_COUNT][PAINTER_FONT_WIDTH] = {
	0x7E, 0xA1, 0x91, 0x89, 0x7E, // 0
	0x80, 0x86, 0xFF, 0x80, 0x80, // 1
	0xC6, 0xA1, 0x91, 0x91, 0x8E, // 2
	0x46, 0x81, 0x91, 0x91, 0x6E, // 3
	0x30, 0x28, 0x26, 0x21, 0xFF, // 4
	0x4F, 0x89, 0x89, 0x89, 0x71, // 5
	0x78, 0x96, 0x91, 0x91, 0x60, // 6
	0x07, 0x01, 0xE1, 0x11, 0x0F, // 7
	0x6E, 0x91, 0x91, 0x91, 0x6E, // 8
	0x0E, 0x91, 0x91, 0x51, 0x3E, // 9
	0x10, 0x10, 0x7E, 0x10, 0x10, // +
	0x10, 0x10, 0x10, 0x10, 0x10  // -
};

typedef struct end
{
	uint8_t com;
	uint8_t page;
} end_t;

typedef struct shelf
{
	bool needs_flush;
	end_t high_end;
	end_t low_end;
} shelf_t;

typedef struct painter
{
	display_t *display;
	uint8_t count;
	shelf_t shelf;
	int width;
	int height;
} painter_t;

static painter_t painter;


static uint8_t const * painter_font_bitmap(char text)
{
	if (text >= '0' && text <= '9')
		return painter_font[text - '0'];
	else if (text == '+')
		return painter_font[10];
	else if (text == '-')
		return painter_font[11];

	return NULL;
}


static void painter_make_dirty(uint8_t x, uint8_t y, uint16_t width, uint8_t height)
{
	x = x < painter.width ? x : (painter.width - 1);
	y = y < painter.height ? y : (painter.height - 1);

	width = (x + width) < painter.width ? width : (painter.width - x);
	height = (y + height) < painter.height ? height : (painter.height - y);

	uint8_t com = x;
	uint8_t page = y / 8;

	if (!painter.shelf.needs_flush)
	{
		painter.shelf.needs_flush = true;

		painter.shelf.low_end.com = com;
		painter.shelf.low_end.page = page;

		com += width - 1;
		page = (y + height) / 8 - 1;

		painter.shelf.high_end.com = com;
		painter.shelf.high_end.page = page;

		return;
	}

	if (com < painter.shelf.low_end.com)
		painter.shelf.low_end.com = com;

	if (page < painter.shelf.low_end.page)
		painter.shelf.low_end.page = page;

	com += width - 1;
	if (com > painter.shelf.high_end.com)
		painter.shelf.high_end.com = com;

	page = (y + height) / 8 - 1;
	if (page > painter.shelf.high_end.page)
		painter.shelf.high_end.page = page;
}


static void painter_map_page(uint8_t byte, int x, int y, bool overlay)
{
	if ((x < 0 || x >= painter.width) || (y <= -8 || y >= painter.height))
		return;

	display_t *display = &painter.display[x / DISPLAY_COMS];
	uint8_t com = x % DISPLAY_COMS;
	uint8_t page = (y < 0 ? 0 : y) / 8;

	// Y position is at the page boundary
	if (y % 8 == 0)
	{
		if (overlay)
			display->buffer.data[com][page] |= byte;
		else
			display->buffer.data[com][page] = byte;
		return;
	}

	// Y position crosses the page boundary
	// TODO: Clean up variable usage
	uint8_t overlap;
	uint8_t data;

	if (y >= 0)
	{
		overlap = y - (8 * (y / 8));
		data = byte << overlap;

		if (!overlay)
			display->buffer.data[com][page] &= ~(0xff << overlap);

		display->buffer.data[com][page] |= data;

		page++;
	}

	if (y < painter.height)
	{
		overlap = 8 - overlap;
		data = byte >> overlap;

		if (!overlay)
			display->buffer.data[com][page] &= ~(0xff >> overlap);

		display->buffer.data[com][page] |= data;
	}
}


void painter_text(char const text, uint8_t x, uint8_t y)
{
	uint8_t const *bitmap = painter_font_bitmap(text);
	if (!bitmap)
		return;

	for (uint8_t i = 0; i < PAINTER_FONT_WIDTH; ++i)
		painter_map_page(bitmap[i], x+i, y, false);

	painter_make_dirty(x, y, PAINTER_FONT_WIDTH, 8);
}


void painter_field(field_t const *field)
{
	for (uint8_t i = field->shift; i < field->size; ++i)
	{
		uint8_t x = field->x + 6 * (i - field->shift);
		painter_text(field->text[i], x, field->y);
	}

	if (field->size >= field->width)
		return;

	for (uint8_t i = field->size - field->shift; i < field->width; ++i)
		painter_text(' ', field->x + 6 * i, field->y);
}


void painter_shape(shape_t const *shape)
{
	if (shape->t == 0)
		return;

	for (uint8_t i = 0; i < SHAPE_WIDTH; ++i)
		painter_map_page(shape_form[shape->type][i], shape->x + i, shape->y, true);

	painter_make_dirty(shape->x, shape->y, SHAPE_WIDTH, 8);
}


void painter_rocket(rocket_t const *rocket)
{
	for (uint8_t i = 0; i < ROCKET_WIDTH; ++i)
	{
		painter_map_page(rocket_form[i][0], rocket->x + i, rocket->y, true);
		painter_map_page(rocket_form[i][1], rocket->x + i, rocket->y + 8, true);
	}

	painter_make_dirty(rocket->x, rocket->y, ROCKET_WIDTH, 8);

	for (uint8_t m = 0; m < MISSLE_COUNT; ++m)
	{
		if (rocket->missles[m].t == 0)
			continue;

		for (uint8_t i = 0; i < MISSLE_WIDTH; ++i)
			painter_map_page(rocket_missle[i], rocket->missles[m].x + i, rocket->missles[m].y, true);

		painter_make_dirty(rocket->missles[m].x, rocket->missles[m].y, MISSLE_WIDTH, 8);
	}
}


void painter_astroidfield(astroidfield_t const *astroidfield)
{
	for (uint8_t a = 0; a < ASTROID_COUNT; ++a)
	{
		if (astroidfield->astroids[a].t == 0)
			continue;

		for (uint8_t i = 0; i < ASTROID_WIDTH; ++i)
		{
			painter_map_page(astroid_form[i][0], astroidfield->astroids[a].x + i, astroidfield->astroids[a].y, true);
			painter_map_page(astroid_form[i][1], astroidfield->astroids[a].x + i, astroidfield->astroids[a].y + 8, true);
		}

		painter_make_dirty(astroidfield->astroids[a].x, astroidfield->astroids[a].y, ASTROID_WIDTH, 8);
	}
}


void painter_starfield(starfield_t const *starfield)
{
	for (uint8_t i = 0; i < STAR_COUNT; ++i)
	{
		if (starfield->stars[i].t == 0)
			continue;

		painter_map_page(0x1, starfield->stars[i].x, starfield->stars[i].y, true);
		painter_make_dirty(starfield->stars[i].x, starfield->stars[i].y, 1, 1);
	}
}


void painter_clear()
{
	for (uint8_t d = 0; d < painter.count; ++d)
		display_clear(&painter.display[d]);

	painter_make_dirty(0, 0, painter.width, painter.height);
}


void painter_flush()
{
	if (!painter.shelf.needs_flush)
		return;

	bool flush_display[2] = { false, false };

	// TODO: This is specific to a 1x2 configuration
	if (painter.shelf.low_end.com < DISPLAY_COMS)
		flush_display[0] = true;
	if (painter.shelf.high_end.com >= DISPLAY_COMS)
		flush_display[1] = true;

	// TODO: rework some of these variables. high_end being reused across multiple scopes.
	end_t low_end = painter.shelf.low_end;
	end_t high_end = painter.shelf.high_end;

	if (flush_display[0])
	{
		display_draw(&painter.display[0]);
		// if (flush_display[1])
		// 	high_end.com = DISPLAY_COMS - 1;

		// display_resize(low_end.com, low_end.page, high_end.com, high_end.page, &painter.display[0]);
		// display_draw(low_end.com, low_end.page, high_end.com, high_end.page, &painter.display[0]);
	}

	high_end = painter.shelf.high_end;

	if (flush_display[1])
	{
		display_draw(&painter.display[1]);
		// if (flush_display[0])
		// 	low_end.com = DISPLAY_COMS;

		// display_resize(low_end.com % DISPLAY_COMS, low_end.page, high_end.com % DISPLAY_COMS, high_end.page, &painter.display[0]);
		// display_draw(low_end.com % DISPLAY_COMS, low_end.page, high_end.com % DISPLAY_COMS, high_end.page, &painter.display[1]);
	}

	painter.shelf.needs_flush = false;

	printf("\ndirty %d %d, %d %d\n", painter.shelf.low_end.com, painter.shelf.low_end.page, painter.shelf.high_end.com, painter.shelf.high_end.page);
}


void painter_init(display_t *display, uint8_t count, int width, int height)
{
	painter.display = display;
	painter.count = count;
	painter.width = width;
	painter.height = height;
	painter.shelf.needs_flush = false;

	painter_clear();
	painter_flush();
}

#endif