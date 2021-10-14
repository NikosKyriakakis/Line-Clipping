#ifndef LINE_H
#define LINE_H

#include "rgb.h"
#include "point.h"

typedef struct Line {
    Point start;
    Point end;
    RGB color;
} Line;

Line **allocate(const int lines);

void deallocate(Line **collection, int current);
void setRandomLine(Line *line);
void drawLine(const Line *line);
void drawLines(const Line **collection, const int lines);
void setLineColor(const unsigned char selection, RGB *color);
void setLinePoints(Line *line, const Point *start, const Point *end);

#endif