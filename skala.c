#include "skala.h"
#include "window.h"

#define VERTEX_NUMBER 4
#define X 0
#define Y 1
#define Z 2

static int code(Point *p) {
	int c = 0;

	if (p->x < CLIP_MIN)
		c = 8;
	else if(p->x > CLIP_MAX)
		c = 2;

	if (p->y < CLIP_MIN)
		c |= 1;
	else if (p->y > CLIP_MAX)
		c |= 4;

	return c;
}

static float dotProduct(float *u, float *v) {
	return u[X] * v[X] + u[Y] * v[Y] + u[Z] * v[Z];
}

static void pointCrossProduct(float *v, Point *a, Point *b) {
	v[X] = a->y - b->y;
	v[Y] = b->x - a->x;
	v[Z] = a->x * b->y - a->y * b->x;
}

static void crossProduct(Point *p, float *u, float *v) {
	const float z = u[X] * v[Y] - u[Y] * v[X];
	p->x = (u[Y] * v[Z] - u[Z] * v[Y]) / z;
	p->y = (u[Z] * v[X] - u[X] * v[Z]) / z;
}

void skala(Line *line) {
	float x[][3] = {
		{ CLIP_MIN, CLIP_MIN, 1 },
		{ CLIP_MAX, CLIP_MIN, 1 },
		{ CLIP_MAX, CLIP_MAX, 1 },
		{ CLIP_MIN, CLIP_MAX, 1 }
	};

	float e[][3] = {
		{ 0, 1, -CLIP_MIN },
		{ 1, 0, -CLIP_MAX },
		{ 0, 1, -CLIP_MAX },
		{ 1, 0, -CLIP_MIN }
	};

	float pCross[3] = { 0, 0, 0 };
	
	pointCrossProduct(pCross, &line->start, &line->end);
	const int codeA = code(&line->start);
	const int codeB = code(&line->end);

	setLineColor(RED, &line->color);
	
	if ((codeA | codeB) == 0) {
		setLineColor(GREEN, &line->color);
		drawLine(line);
		return; 
	}

	if ((codeA & codeB) != 0) {
		drawLine(line);
		return;
	}

	int cc = 0;
	for (int k = 0; k < 4; k++)
		if (dotProduct(pCross, x[k]) <= 0)
			cc |= (1 << k);

	if (cc == 0 || cc == 15) {
		drawLine(line);
		return;
	}

	const int MASK[] = { 0, 1, 2, 2, 4, 0, 4, 4, 8, 1, 0, 2, 8, 1, 8, 0 };
	const int TAB1[] = { 4, 3, 0, 3, 1, 4, 0, 3, 2, 2, 4, 2, 1, 1, 0, 4 };
	const int TAB2[] = { 4, 0, 1, 1, 2, 4, 2, 2, 3, 0, 4, 1, 3, 0, 3, 4 };

	const int i = TAB1[cc];
	const int j = TAB2[cc];

	if (codeA != 0 && codeB != 0) {
		crossProduct(&line->start, pCross, e[i]);
		crossProduct(&line->end, pCross, e[j]);
		setLineColor(GREEN, &line->color);
		drawLine(line);
		return;
	}

	if (codeA == 0) {
		if ((codeB & MASK[cc]) == 0)
			crossProduct(&line->end, pCross, e[i]);
		else
			crossProduct(&line->end, pCross, e[j]);
		setLineColor(GREEN, &line->color);
		drawLine(line);
		return;
	}

	if (codeB == 0) {
		if ((codeA & MASK[cc]) == 0)
			crossProduct(&line->start, pCross, e[i]);
		else
			crossProduct(&line->start, pCross, e[j]);
		setLineColor(GREEN, &line->color);
		drawLine(line);
		return;
	}
}