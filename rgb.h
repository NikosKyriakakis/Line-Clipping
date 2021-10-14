#ifndef RGB_H
#define RGB_H

#define RED		0b100
#define GREEN	0b010
#define BLUE	0b001
#define WHITE	0b111

typedef struct RGB {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} RGB;

#endif