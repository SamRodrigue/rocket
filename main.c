// Keypad GPIO index and order
#define KEYPAD_IN 12, 13, 14, 15
#define KEYPAD_OUT 9, 10, 11

#include "keypad.h"
#include "display.h"
#include "field.h"
#include "painter.h"
#include "collider.h"

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#define DISPLAY0_ADDR 0x3c
#define DISPLAY1_ADDR 0x3d


static display_t displays[2];
// static field_t fields[2];
static rocket_t rocket;
static astroidfield_t astroidfield;
static starfield_t starfield;

static rocket_control_t controls[4][3] = {
	rocket_control_none, rocket_control_none, rocket_control_none,
	rocket_control_none, rocket_control_up, rocket_control_none,
	rocket_control_left, rocket_control_fire, rocket_control_right,
	rocket_control_none, rocket_control_down, rocket_control_none
};


// Action to perform on a keypad event
void key_action(keypad_event_t const *event)
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
		control = controls[event->row][event->col];
		if (control == rocket_control_none)
			break;

		rocket_control(control, true, &rocket);
		break;

	case UNDEFINED:
		passert(false, "Undefined event received");
	}
}


int main()
{
	stdio_init_all();

	displays[0].address = DISPLAY0_ADDR;
	displays[1].address = DISPLAY1_ADDR;

	for (uint8_t d = 0; d < 2; ++d)
		display_init(4, 5, &displays[d]);

	painter_init(displays, 2, 2 * DISPLAY_COMS, 8 * DISPLAY_PAGES);

	// field_init(32, 6, 14, 20, &fields[0]);
	// field_init(32, 38, 14, 14, &fields[1]);

	keypad_init();

	rocket_init(&rocket);
	astroidfield_init(&astroidfield);
	starfield_init(&starfield);

	multicore_launch_core1(&keypad_main);

	while (1)
	{
		keypad_update(&key_action);
		starfield_update(&starfield);
		astroidfield_update(&astroidfield);
		rocket_update(&rocket);

		collider_missle_astroid(&rocket, &astroidfield);

		painter_clear();

		painter_starfield(&starfield);
		painter_astroidfield(&astroidfield);
		painter_rocket(&rocket);

		// painter_text('0' + rocket.control / 10, 0, 0);
		// painter_text('0' + rocket.control % 10, 6, 0);

		painter_flush();
	}

	return 0;
}