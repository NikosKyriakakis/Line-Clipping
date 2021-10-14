#include "line.h"
#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

void deallocate(Line **collection, int current) {
    for (; current >= 0; current--)
        free(collection[current]);
    free(collection);
}

Line **allocate(const int lines) {
    Line **collection = (Line **)calloc(lines, sizeof(Line *));
    if (collection == NULL) {
        perror("Allocation of line collection failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int line = 0; line < lines; line++) {
        collection[line] = (Line *)calloc(1, sizeof(Line));
        if (collection[line] == NULL) {
            perror("Allocation of line collection (single line) failed.\n");
            deallocate(collection, line);
            exit(EXIT_FAILURE);           
        }
    }

    return collection;
}

void drawLine(const Line *line) {
    glBegin(GL_LINES);
		glColor3f(line->color.red, line->color.green, line->color.blue);
		glVertex2i(line->start.x, line->start.y);
		glVertex2i(line->end.x, line->end.y);
	glEnd();
}

void drawLines(const Line **collection, const int lines) {
    for (int line = 0; line < lines; line++)
        drawLine(collection[line]);
}

void setRandomLine(Line *line) {
    line->start.x = rand() % WINDOW_SIZE;
    line->start.y = rand() % WINDOW_SIZE;
    line->end.x = rand() % WINDOW_SIZE;
    line->end.y = rand() % WINDOW_SIZE;
}

void setLineColor(const unsigned char selection, RGB *color) {
    switch(selection) {
		case RED:
			color->red = 255;
			color->green = 0;
			color->blue = 0;
			break;
		case GREEN:
			color->red = 0;
			color->green = 255;
			color->blue = 0;
			break;
		case BLUE:
			color->red = 0;
			color->green = 0;
			color->blue = 255;
			break;
		case WHITE:
			color->red = 255;
			color->green = 255;
			color->blue = 255;
			break;
	}
}

void setLinePoints(Line *line, const Point *start, const Point *end) {
    line->start = *start;
    line->end = *end;
}