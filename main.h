#include <stdlib.h>
#include "images.h"
#define _USE_MATH_DEFINES //cause vs is dumb :-(
#include <math.h>
#include <glut/glut.h>
//#include "images.h"

/* In VC++ 2008, Ryan got a warning about min redefinition,
 so let's not redefine it if something else already did; sigh */
#define min(a,b) ((a) < (b)? a:b)
#ifndef min
#endif

#define FALSE 0 
#define TRUE  1

typedef struct _LITE{
	GLfloat amb[4];
	GLfloat diff[4];
	GLfloat spec[4];
	GLfloat pos[4];
	GLfloat dir[3];
	GLfloat angle;
}LITE;

/* define index constants into the colors array */
#define BLACK   0
#define RED     1
#define YELLOW  2
#define MAGENTA 3
#define GREEN   4
#define CYAN    5
#define BLUE    6
#define GREY    7
#define WHITE   8
#define DARK_RED 9
#define ORANGE  10
#define LIGHT_GREY 11

GLfloat colors [][4] = {
  {0.0, 0.0, 0.0, 1.0},  /* black   */
  {1.0, 0.0, 0.0, 1.0},  /* red     */
  {1.0, 1.0, 0.0, 1.0},  /* yellow  */
  {1.0, 0.0, 1.0, 1.0},  /* magenta */
  {0.0, 0.65, 0.6, 1.0}, /* green   */
  {0.0, 1.0, 1.0, 1.0},  /* cyan    */
  {0.0, 0.0, 1.0, 1.0},  /* blue    */
  {0.5, 0.5, 0.5, 1.0},  /* 50%grey */
  {1.0, 1.0, 1.0, 1.0},  /* white   */
  {0.55, 0.0, 0.0, 1.0},  /* Dark red*/
  {1.0, 0.5, 0.0, 1.0},   /* ORANGE  */
  {.75, .75, .75, .75}  /* 75%grey */
};

void my_TimeOut(int t);