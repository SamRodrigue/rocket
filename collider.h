#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "rocket.h"
#include "astroid.h"

static bool collider_overlap(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (x1 >= x2) && (x1 + w1 <= x2 + w2) &&
	       (y1 >= y2) && (y1 + h1 <= y2 + h2);
}


void collider_missle_astroid(rocket_t *rocket, astroidfield_t *astroidfield)
{
	for (uint8_t m = 0; m < MISSLE_COUNT; ++m)
	{
		if (rocket->missles[m].t == 0)
			continue;

		for (uint8_t a = 0; a < ASTROID_COUNT; ++a)
		{
			if (astroidfield->astroids[a].t == 0)
				continue;

			if ((rocket->missles[m].x >= astroidfield->astroids[a].x) &&
			    (rocket->missles[m].x + MISSLE_WIDTH <= astroidfield->astroids[a].x + ASTROID_WIDTH) &&
			    (rocket->missles[m].y >= astroidfield->astroids[a].y) &&
			    (rocket->missles[m].y + 2 <= astroidfield->astroids[a].y + (8 * ASTROID_PAGES)))
			{
				rocket->missles[m].t = 0;
				astroidfield->astroids[a].t = 0;
				break;
			}
		}
	}
}

#endif