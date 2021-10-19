#ifndef __ROCKET_H__
#define __ROCKET_H__

#define ROCKET_WIDTH 32
#define ROCKET_PAGES 2

typedef enum rocket_control
{
	rocket_control_none  = 0,
	rocket_control_up    = 1,
	rocket_control_down  = 1 << 1,
	rocket_control_left  = 1 << 2,
	rocket_control_right = 1 << 3,
	rocket_control_fire  = 1 << 4,
} rocket_control_t;

uint8_t rocket_form[ROCKET_WIDTH][ROCKET_PAGES] = {
	0xE0, 0x07, 0x20, 0x04, 0x40, 0x02, 0xFE, 0x7F, 0xC3, 0x83, 0xB2, 0x4D, 0x8C, 0x31, 0x84, 0x21,
	0xA4, 0x25, 0xB4, 0x2D, 0xB4, 0x2D, 0xB4, 0x2D, 0xB4, 0x2D, 0xB4, 0x2D, 0xB4, 0x2D, 0xB4, 0x2D,
	0xB4, 0x2D, 0xA4, 0x25, 0x84, 0x21, 0x84, 0x21, 0xF4, 0x2F, 0x0C, 0x30, 0x04, 0x20, 0xE4, 0x27,
	0x14, 0x28, 0x14, 0x28, 0x28, 0x14, 0x48, 0x12, 0x90, 0x09, 0x20, 0x04, 0x40, 0x02, 0x80, 0x01
};

#define MISSLE_WIDTH 4
uint8_t rocket_missle[MISSLE_WIDTH] = {
	0x03, 0x03, 0x03, 0x03
};

#define MISSLE_COUNT 12

typedef struct missle
{
	int x;
	int y;
	int t;
} missle_t;

typedef struct rocket
{
	int x;
	int y;
	int dx;
	int dy;
	uint8_t control;
	missle_t missles[MISSLE_COUNT];
} rocket_t;


#define ROCKET_FORCE 3

bool rocket_has_missle(rocket_t const *rocket)
{
	for (uint8_t i = 0; i < MISSLE_COUNT; ++i)
	{
		if (rocket->missles[i].t)
			return true;
	}

	return false;
}

void rocket_control(rocket_control_t control, bool enabled, rocket_t *rocket)
{
	if (enabled)
		rocket->control |= (uint8_t)control;
	else
		rocket->control &= ~(uint8_t)control;
}

void rocket_update(rocket_t *rocket)
{
	// if (rocket->t == 0)
	// 	return;

	//rocket->t--;

	if (rocket_has_missle(rocket))
	{
		for (uint8_t i = 0; i < MISSLE_COUNT; ++i)
		{
			rocket->missles[i].x += 4;
			if (rocket->missles[i].x >= 256)
				rocket->missles[i].t = 0;
		}
	}

	if (rocket->control & rocket_control_up)
		rocket->y -= rocket->dy;
	if (rocket->control & rocket_control_down)
		rocket->y += rocket->dy;
	if (rocket->control & rocket_control_right)
		rocket->x += rocket->dx;
	if (rocket->control & rocket_control_left)
		rocket->x -= rocket->dx;
	if (rocket->control & rocket_control_fire)
	{
		rocket->control &= ~(uint8_t)rocket_control_fire;

		for (uint8_t i = 0; i < MISSLE_COUNT; ++i)
		{
			if (rocket->missles[i].t)
				continue;

			rocket->missles[i].t = 1;
			rocket->missles[i].x = rocket->x + ROCKET_WIDTH;
			rocket->missles[i].y = rocket->y + (8 * ROCKET_PAGES / 2);
			break;
		}
	}

	// if (rocket->x < 0)
	// {
	// 	rocket->x *= -1;
	// 	rocket->dx *= -1;
	// }

	// if (rocket->x >= 256 - ROCKET_WIDTH)
	// {
	// 	rocket->x = 512 - (2 * ROCKET_WIDTH) - rocket->x;
	// 	rocket->dx *= -1;
	// }

	// if (rocket->y < 0)
	// {
	// 	rocket->y *= -1;
	// 	rocket->dy *= -1;
	// }

	// if (rocket->y >= 64 - (8 * ROCKET_PAGES))
	// {
	// 	rocket->y = 128 - (2 * (8 * ROCKET_PAGES)) - rocket->y;
	// 	rocket->dy *= -1;
	// }

	if (rocket->x < 0)
		rocket->x = 0;
	else if (rocket->x >= 256 - ROCKET_WIDTH)
		rocket->x = 256 - 1 - ROCKET_WIDTH;

	if (rocket->y < 0)
		rocket->y = 0;
	else if (rocket->y >= (64 - 8 * ROCKET_PAGES))
		rocket->y = 64 - 1 - 8 * ROCKET_PAGES;
}


void rocket_init(rocket_t *rocket)
{
	rocket->control = rocket_control_none;
	rocket->x = 0;
	rocket->y = 0;
	rocket->dx = ROCKET_FORCE;
	rocket->dy = ROCKET_FORCE;
	for (uint8_t i = 0; i < MISSLE_COUNT; ++i)
		rocket->missles[i].t = 0;
}

#endif