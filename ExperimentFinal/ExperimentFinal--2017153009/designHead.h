#pragma once
double Body_Long= 4;
double Body_Width= 2;
double Body_Height= 2;


GLfloat lef = -10.0, rig = 10.0;
GLfloat bot = -10.0, top = 10.0;
GLfloat zNear = -10.0, zFar = 10.0;

GLdouble distTL = 0.1;	//默认光源到物体的距离

GLfloat radius = 1;
GLfloat thera = 3.14 / 2 / 2;
GLfloat phi = 3.01 / 2 / 2;

vec4 black(0.0, 0.0, 0.0, 1.0);	//black
vec4 red(1.0, 0.0, 0.0, 1.0);	//red
vec4 white(1.0, 1.0, 1.0, 1.0);  // White