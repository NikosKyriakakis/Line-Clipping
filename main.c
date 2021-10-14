#include <GL/glut.h>
#include <GL/glui.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#include "window.h"
#include "skala.h"
#include "cohen_sutherland.h"

#define SKALA 0
#define COHEN_SUTHERLAND 1
#define ALGORITHMS 2
#define SINGLE_LINE 0
#define MULTIPLE_LINES 1
#define INPUTS 2
#define DEFAULT 5
#define INPUT_SIZE 32
#define Clock clock_t

static void swapCoordinates(Point *point);
static void executeCohenSutherland(void);
static void executeSkala(void);
static void display(void);
static void drawGrid(void);
static void drawRandomSegments(void);
static void setRandomSegments(void);
static void cleanMemoryResources(void);
static void allocateMemoryResources(void);
static void initGLUI(void);
static void float2Text(void);
static void enableSingleLine(void);
static void enableMultipleLines(void);
static void drawAxis(void);

static Line **collection;
static Line *line;
static int lines;
static int algorithm;
static GLUI *glui;
static GLUI_StaticText *staticText;
static GLUI_Spinner *lineSpinner;
static GLUI_Spinner *X1_Spinner;
static GLUI_Spinner *Y1_Spinner;
static GLUI_Spinner *X2_Spinner;
static GLUI_Spinner *Y2_Spinner;
static int mainWindow;
static float total;
static float x1, y1, x2, y2;
static Point p1, p2;
static int inputType;

void (*execute[ALGORITHMS])() = {
	executeSkala,
	executeCohenSutherland
};

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
	glutInitWindowPosition(0, 0);
	mainWindow = glutCreateWindow("COHEN_SUTHERLAND - SKALA");
	initGLUI();
	glClearColor(0, 0, 0, 0);  
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, WINDOW_SIZE, 0, WINDOW_SIZE);
	glui->set_main_gfx_window(mainWindow);
	glui->sync_live();
	GLUI_Master.set_glutDisplayFunc(display);
	glutMainLoop();

    return EXIT_SUCCESS;
}

static void initGLUI(void) {
	glui = GLUI_Master.create_glui("MENU", 0, WINDOW_SIZE + 75, 0);

	GLUI_Panel *algorithmPanel = glui->add_panel("ALGORITHMS");	
	GLUI_RadioGroup *algorithmGroup = glui->add_radiogroup_to_panel(algorithmPanel, &algorithm);
	glui->add_radiobutton_to_group(algorithmGroup, "SKALA");
	glui->add_radiobutton_to_group(algorithmGroup, "COHEN-SUTHERLAND");

	GLUI_Panel *inputPanel = glui->add_panel("INPUT");
	GLUI_RadioGroup *inputGroup = glui->add_radiogroup_to_panel(inputPanel, &inputType);
	glui->add_radiobutton_to_group(inputGroup, "SINGLE-LINE");
	glui->add_radiobutton_to_group(inputGroup, "MULTIPLE-LINES");
	
	lineSpinner = glui->add_spinner_to_panel(inputPanel, "LINES: ", GLUI_SPINNER_INT, &lines);
	lineSpinner->set_int_limits(2, 1000);
	X1_Spinner = glui->add_spinner_to_panel(inputPanel, "X1: ", GLUI_SPINNER_FLOAT, &x1);
	X1_Spinner->set_float_limits(0, 600);
	X1_Spinner->set_float_val(250);
	Y1_Spinner = glui->add_spinner_to_panel(inputPanel, "Y1: ", GLUI_SPINNER_FLOAT, &y1);
	Y1_Spinner->set_float_limits(0, 600);
	Y1_Spinner->set_float_val(350);
	X2_Spinner = glui->add_spinner_to_panel(inputPanel, "X2: ", GLUI_SPINNER_FLOAT, &x2);
	X2_Spinner->set_float_limits(0, 600);
	X2_Spinner->set_float_val(500);
	Y2_Spinner = glui->add_spinner_to_panel(inputPanel, "Y2: ", GLUI_SPINNER_FLOAT, &y2);
	Y2_Spinner->set_float_limits(0, 600);
	Y2_Spinner->set_float_val(450);
	staticText = glui->add_statictext("");
}

static void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	allocateMemoryResources();
	if (inputType == MULTIPLE_LINES)
		enableMultipleLines();
	else 
		enableSingleLine();
	drawGrid();
	for (int line = 0; line < lines; line++) {
		drawLine(collection[line]);
		if (inputType == SINGLE_LINE) {
			glColor3f(1.0, 1.0, 1.0);
			glRasterPos2f(collection[line]->start.x, collection[line]->start.y);
			glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char *)"(X1, Y1)");
			glRasterPos2f(collection[line]->end.x, collection[line]->end.y);
			glutBitmapString(GLUT_BITMAP_HELVETICA_12, (unsigned char *)"(X2, Y2)");
		}
	}
		
	Clock start = clock();
	execute[algorithm]();
	Clock end = clock();
	total = (float)(end - start) / CLOCKS_PER_SEC;
	float2Text();
	drawAxis();
	glFlush();
	cleanMemoryResources();
}

static unsigned char *convert2UCHAR(int n) {
	unsigned char *chunk = (unsigned char *)calloc(32, sizeof(unsigned char));

	memset(chunk, 32, 0);
	sprintf((char *)chunk, "%d", n);
	return chunk;
}

static void drawAxis(void) {
	unsigned char *text;
	
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < WINDOW_SIZE; i += 50) {
		text = convert2UCHAR(i);
		glRasterPos2i(i, 0);
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, text);
		glRasterPos2i(0, i);
		glutBitmapString(GLUT_BITMAP_HELVETICA_10, text);
		free(text);
		text = NULL;
	}	
}

static void enableSingleLine(void) {
	lines = 1;
	lineSpinner->disable();
	X1_Spinner->enable();
	Y1_Spinner->enable();
	X2_Spinner->enable();
	Y2_Spinner->enable();
	p1.x = x1;
	p1.y = y1;
	p2.x = x2;
	p2.y = y2;
	setLinePoints(collection[0], &p1, &p2);
	setLineColor(RED, &collection[0]->color);
}

static void enableMultipleLines(void) {
	lineSpinner->enable();
	X1_Spinner->disable();
	Y1_Spinner->disable();
	X2_Spinner->disable();
	Y2_Spinner->disable();
	setRandomSegments();
}

static void float2Text(void) {
	char text[64];

	memset(text, 64, 0);
	sprintf(text, "DURATION: %f seconds", total);
	staticText->set_text(text);
}

static void allocateMemoryResources(void) {
	collection = allocate(lines);
}

static void cleanMemoryResources(void) {
	deallocate(collection, lines - 1);
	collection = NULL;
}

static void executeCohenSutherland(void) {
	for (int line = 0; line < lines; line++)
		cohenSutherland(collection[line]);
}

static void executeSkala(void) {
	for (int line = 0; line < lines; line++)
		skala(collection[line]);
}

static void swapCoordinates(Point *point) {
    point->x += point->y;
    point->y = point->x - point->y;
    point->x -= point->y;
}

static void drawGrid(void) {
	Line *line = (Line *)calloc(1, sizeof(Line));
    if (line == NULL) {
        perror("Line allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    Point startMin = { 0, CLIP_MIN };
    Point endMin = { WINDOW_SIZE, CLIP_MIN };
    Point startMax = { 0, CLIP_MAX };
    Point endMax = { WINDOW_SIZE, CLIP_MAX };

    setLineColor(WHITE, &line->color);
    setLinePoints(line, &startMin, &endMin);
    drawLine(line);
    setLinePoints(line, &startMax, &endMax);
    drawLine(line);
   
    swapCoordinates(&startMin);
    swapCoordinates(&startMax);
    swapCoordinates(&endMin);
    swapCoordinates(&endMax);
    
    setLinePoints(line, &startMin, &endMin);
    drawLine(line);
    setLinePoints(line, &startMax, &endMax);
    drawLine(line);

    const Point clipLeftBottom = { CLIP_MIN, CLIP_MIN };
    const Point clipLeftTop = { CLIP_MIN, CLIP_MAX };
    const Point clipRightBottom = { CLIP_MAX, CLIP_MIN };
    const Point clipRightTop = { CLIP_MAX, CLIP_MAX };

    glLineWidth(5);
    setLinePoints(line, &clipLeftBottom, &clipRightBottom);
    drawLine(line);
    setLinePoints(line, &clipLeftBottom, &clipLeftTop);
    drawLine(line);
    setLinePoints(line, &clipLeftTop, &clipRightTop);
    drawLine(line);
    setLinePoints(line, &clipRightTop, &clipRightBottom);
    drawLine(line);
    glLineWidth(1);

	free(line);
    line = NULL;
}

static void setRandomSegments(void) {
	for (int line = 0; line < lines; line++) {
		setLineColor(RED, &collection[line]->color);
		setRandomLine(collection[line]);
	}
}