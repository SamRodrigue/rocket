#ifndef __FIELD_H__
#define __FIELD_H__

#define FIELD_MAX_SIZE 32

typedef struct field
{
	uint8_t x;
	uint8_t y;
	uint8_t width; // Number of characters to display on screen
	uint8_t text[FIELD_MAX_SIZE]; // Text buffer
	uint8_t size; // Size of occupied text buffer
	uint8_t max_size; // Max size to allow in the text buffer
	//uint8_t cursor;
	uint8_t shift;
} field_t;

void field_putchar(char const character, field_t *field)
{
	if (field->size == field->max_size)
	{
		memset(field->text, 0, field->max_size);
		field->size = 0;
		field->shift = 0;
		// return;
	}

	field->text[field->size] = character;
	field->size++;

   if (field->size > field->width)
      field->shift = field->size - field->width;
}


void field_init(uint8_t x, uint8_t y, uint8_t width, uint8_t max_size, field_t *field)
{
	field->x = x;
	field->y = y;
	field->width = width;
	memset(field->text, 0, field->max_size);
	field->size = 0;
	field->max_size = max_size;
	field->shift = 0;
}

#endif