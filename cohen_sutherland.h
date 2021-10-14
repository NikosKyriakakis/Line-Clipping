#ifndef COHEN_SUTHERLAND_H
#define COHEN_SUTHERLAND_H

#include "line.h"

#define INSIDE	0
#define LEFT	1
#define RIGHT	2
#define BOTTOM	4
#define TOP		8

void cohenSutherland(Line *line);

#endif