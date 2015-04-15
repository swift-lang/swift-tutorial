#ifndef MANDELBROT_H
#define MANDELBROT_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern int X_MAX;
extern int Y_MAX;

#define SLICE_SIZE 100

// Returns 0 if nothing fails
// buffer point to a buffer with 3*(sizeof) char * Num yslices * 10000
int mandelbrot(int YSLICE_START, int YSLICE_END, char *buffer, int intermax);

#endif
