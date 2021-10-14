#include "window.h"
#include "cohen_sutherland.h"

#include <stdio.h>
#include <stdlib.h>

static int getCode(const float x, const float y) {
	int code = INSIDE;
	
	if (y > CLIP_MAX)
		code |= TOP;
	else if (y < CLIP_MIN)
		code |= BOTTOM;
	else if (x > CLIP_MAX)
		code |= RIGHT;
	else if (x < CLIP_MIN)
		code |= LEFT;
	
	return code;
}

static int isInside(const int code1, const int code2) {
    return (!(code1 | code2));
}

static int isOutside(const int code1, const int code2)
{
	return (code1 & code2);
}

static int getCodeOut(const int code1, const int code2) {
	return (code2 > code1 ? code2 : code1);
}

static int isAbove(const int codeOut) {
	return codeOut & TOP;
}

static int isBelow(const int codeOut) {
	return codeOut & BOTTOM;
}

static int isLeft(const int codeOut) {
	return codeOut & LEFT;
}

static int isRight(const int codeOut) {
	return codeOut & RIGHT;
}

void cohenSutherland(Line *line) {
	float x1 = line->start.x;
	float x2 = line->end.x;
	float y1 = line->start.y;
	float y2 = line->end.y;
	
	int code1 = getCode(x1, y1);
	int code2 = getCode(x2, y2);
	int accepted = 0;
	
	for(;;) {
		if(isInside(code1, code2)) {
			accepted = 1;
			break;
		} else if(isOutside(code1, code2)) {
			break; 
		} else {
			float x, y;
			
			int codeOut = getCodeOut(code1, code2);
			
			if(isAbove(codeOut)) {
				x = x1 + (x2 - x1) * (CLIP_MAX - y1) / (y2 - y1);
				y = CLIP_MAX;
			} else if(isBelow(codeOut)) {
				x = x1 + (x2 - x1) * (CLIP_MIN - y1) / (y2 - y1);
				y = CLIP_MIN;
			} else if(isLeft(codeOut)) {
				y = y1 + (y2 - y1) * (CLIP_MIN - x1) / (x2 - x1);
				x = CLIP_MIN;
			} else if(isRight(codeOut)) {
				y = y1 + (y2 - y1) * (CLIP_MAX - x1) / (x2 - x1);
				x = CLIP_MAX;
			}
			
			if(codeOut == code1) {
				x1 = x;
				y1 = y;
				code1 = getCode(x1, y1);
			} else {
				x2 = x;
				y2 = y;
				code2 = getCode(x2, y2);
			}
		}
	}
	
	if(accepted)
		setLineColor(GREEN, &line->color);
	
    const Point start = { x1, y1 };
    const Point end = { x2, y2 };
	setLinePoints(line, &start, &end);
	drawLine(line);
}