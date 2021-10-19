// Keypad GPIO index and order
#define KEYPAD_IN 12, 13, 14, 15
#define KEYPAD_OUT 9, 10, 11

#include "keypad.h"
#include "display.h"
#include "field.h"
#include "painter.h"

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#define DISPLAY0_ADDR 0x3c
#define DISPLAY1_ADDR 0x3d


static display_t displays[2];
static field_t fields[2];
// 	#define ROCKETS_COUNT 100
// static rocket_t rockets[ROCKETS_COUNT];
// static uint8_t rockets_count;
static rocket_t rocket;
#define ASTROID_COUNT 12
static astroid_t astroids[ASTROID_COUNT];

static rocket_control_t controls[4][3] = {
	rocket_control_none, rocket_control_none, rocket_control_none,
	rocket_control_none, rocket_control_up, rocket_control_none,
	rocket_control_left, rocket_control_fire, rocket_control_right,
	rocket_control_none, rocket_control_down, rocket_control_none
};


// Action to perform on a keypad event
void key_action(struct keypad_event const *event)
{
	switch (event->type)
	{
		rocket_control_t control;
	case KEY_RELEASE:
		control = controls[event->row][event->col];
		if (control == rocket_control_none)
			break;

		rocket_control(control, false, &rocket);
		break;

	case KEY_PRESSED:
		// rocket_init(&rockets[rockets_count]);
		// rockets_count++;
		// if (rockets_count == ROCKETS_COUNT)
		// 	rockets_count = 0;

		control = controls[event->row][event->col];
		if (control == rocket_control_none)
			break;

		rocket_control(control, true, &rocket);
		break;

	case UNDEFINED:
		passert(false, "Undefined event received");
	}
}


#define ASTROID_TIMER 50 + (rand() % 50)

int main()
{
	stdio_init_all();

	displays[0].address = DISPLAY0_ADDR;
	displays[1].address = DISPLAY1_ADDR;

	for (uint8_t d = 0; d < 2; ++d)
		display_init(4, 5, &displays[d]);

	painter_init(displays, 2, 2 * DISPLAY_COMS, 8 * DISPLAY_PAGES);

	field_init(32, 6, 14, 20, &fields[0]);
	field_init(32, 38, 14, 14, &fields[1]);

	keypad_init();

	// rockets_count = 0;
	// for (uint8_t i = 0; i < ROCKETS_COUNT; ++i)
	// 	rockets[i].t = 0;

	rocket_init(&rocket);

	for (uint8_t i = 0; i < ASTROID_COUNT; ++i)
		astroids[i].t = 0;

	multicore_launch_core1(&keypad_main);

	uint8_t astroid_timer = ASTROID_TIMER;
	astroid_init(&astroids[0]);

	while (1)
	{
		if (multicore_fifo_rvalid())
		{
			uint32_t index = multicore_fifo_pop_blocking();
			struct keypad_event event;
			keypad_get_event(index, &event);
			key_action(&event);
		}

		painter_clear();

		// uint8_t count = 0;
		// for (uint8_t i = 0; i < ROCKETS_COUNT; ++i)
		// {
		// 	if (rockets[i].t != 0)
		// 		count++;

		// 	rocket_update(&rockets[i]);
		// 	painter_rocket(&rockets[i]);
		// }

		// painter_text('0' + count / 10, 0, 0);
		// painter_text('0' + count % 10, 6, 0);

		if (astroid_timer == 0)
		{
			for (uint8_t i = 0; i < ASTROID_COUNT; ++i)
			{
				if (astroids[i].t == 0)
				{
					astroid_init(&astroids[i]);
					break;
				}
			}

			astroid_timer = ASTROID_TIMER;
		}
		else
			astroid_timer--;

		for (uint8_t i = 0; i < ASTROID_COUNT; ++i)
		{
			astroid_update(&astroids[i]);
			painter_astroid(&astroids[i]);
		}

		rocket_update(&rocket);
		painter_rocket(&rocket);

		// painter_text('0' + rocket.control / 10, 0, 0);
		// painter_text('0' + rocket.control % 10, 6, 0);

		painter_flush();
	}

	return 0;
}