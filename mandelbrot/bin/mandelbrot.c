/**  mandel.c   by Eric R. Weeks   written 9-28-96
 **  weeks@physics.emory.edu
 **  http://www.physics.emory.edu/~weeks/
 **
 **  This program is public domain, but this header must be left intact
 **  and unchanged.
 **
 **  to compile:  cc -o mand mandel.c
 **
 **  -Yadu Nand B <yadu@uchicago.edu>
 **  This code has been borrowed from http://www.physics.emory.edu/faculty/weeks//software/mandel.c
 **  as a base for HW2 problem 2. Code is modified to run in a parallel environment.
 **/

#include "mandelbrot.h"

extern int X_MAX;
extern int Y_MAX;

void color(char *buffer, int red, int green, int blue)  {
    buffer[0] = (char)red ;
    buffer[1] = (char)green ;
    buffer[2] = (char)blue ;
}


int mandelbrot(int YSLICE_START, int YSLICE_END, char * BUFFER, int itermax)
{
    double x,xx,y,cx,cy;
    int iteration,hx,hy;
    double magnify=1.0;/* no magnification*/
    int hxres = X_MAX;/* horizonal resolution*/
    int hyres = Y_MAX;/* vertical resolution*/
    //printf("YSLICE [%d] -> [%d] \n", YSLICE_START, YSLICE_END);
    for (hy=YSLICE_START; hy<=YSLICE_END; hy++)  {
        for (hx=1; hx<=hxres; hx++)  {
            cx = (((float)hx)/((float)hxres)-0.5)/magnify*3.0-0.7;
            cy = (((float)hy)/((float)hyres)-0.5)/magnify*3.0;
            x = 0.0; y = 0.0;
            for (iteration=1;iteration<itermax;iteration++)  {
                xx = x*x-y*y+cx;
                y = 2.0*x*y+cy;
                x = xx;
                if (x*x+y*y>100.0) break;
            }
            if (iteration == itermax){
                color(BUFFER, 0,0,0);
            } else {
                color(BUFFER, 255,(iteration*10)%255,(iteration*10)%255);
            }
            BUFFER += 3;
        }
    }
    return 0;
}
