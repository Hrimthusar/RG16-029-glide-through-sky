#include <GL/glut.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include "misc_functions.h"


void RenderString(float x, float y, void *font, const char* string, float r, float g, float b) {
  GLfloat emission_coeffs[] = { r, g, b, 1 };
  glMaterialfv(GL_FRONT, GL_EMISSION, emission_coeffs);
  glRasterPos3f(x, y, .3);
  glutBitmapString(font, string);
  emission_coeffs[0] = 0;
  emission_coeffs[1] = 0;
  emission_coeffs[2] = 0;
  emission_coeffs[3] = 1;
  glMaterialfv(GL_FRONT, GL_EMISSION, emission_coeffs);
}

#define DT_MAX 60
static int newTime;
static int oldTime = 0;
static int timeSum = 0;

void updateDeltaTime() {
    newTime = glutGet(GLUT_ELAPSED_TIME);
    dt = newTime - oldTime;
    oldTime = newTime;
    timeSum += dt;
    if (dt>DT_MAX)
        dt=DT_MAX;
}

void fps(int print) {
    static int frame = 0;
    frame++;
    if (print && timeSum >= 1000){
        printf("fps:%f\n",frame*1000/(float)timeSum);
        timeSum = 0;
        frame = 0;
    }
}
