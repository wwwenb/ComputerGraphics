#pragma once
#include "Angel.h"
#include <assert.h>
#include<iostream>
#include <cstring>
#include <vector>

using namespace std;

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];
point4 ns[NumVertices];
vector<string> imgs_name;

point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// RGBA colors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};
point4 color_torso = point4(0, 0, 1, 1);
//----------------------------------------------------------------------------

class MatrixStack {
	int    _index;
	int    _size;
	mat4* _matrices;

public:
	MatrixStack(int numMatrices = 100) :_index(0), _size(numMatrices)
	{
		_matrices = new mat4[numMatrices];
	}

	~MatrixStack()
	{
		delete[]_matrices;
	}

	void push(const mat4& m) {
		assert(_index + 1 < _size);
		_matrices[_index++] = m;

	}

	mat4& pop(void) {
		assert(_index - 1 >= 0);
		_index--;
		return _matrices[_index];
	}
};

GLuint program;

MatrixStack  mvstack;
mat4 model_view;
mat4 model;
mat4 view;
mat4 shadowModel;

GLuint ModelView, Projection;
GLuint draw_color;
GLuint lightPosID;
GLuint distToLightID;
GLuint modelMatrix;
GLuint viewMatrix;
GLuint shadowProMatrix;
GLuint is_background;

vector<My_Mesh*> my_meshs;
Mesh_Painter* mp_;

GLfloat lef = -15.0, rig = 15.0;
GLfloat bottom = -15, top = 20;
GLfloat zNear = -15.0, zFar = 15.0;

//----------------------------------------------------------------------------

double TORSO_HEIGHT = 2.0;
double TORSO_WIDTH= 3.0;
double TORSO_LONG = 6;
double UPPER_ARM_HEIGHT = 3.0;
double UPPER_ARM_WIDTH = 0.5;
double LOWER_ARM_HEIGHT =2.0;
double LOWER_ARM_WIDTH = 0.5;

double UPPER_LEG_HEIGHT = 3.0;
double UPPER_LEG_WIDTH  =0.5;

double LOWER_LEG_HEIGHT = 2.0;
double LOWER_LEG_WIDTH  =0.5;

double HEAD_HEIGHT= 1.5;
double HEAD_WIDTH= 1.5;
double HEAD_LONG = 2.5;

double NECK_HEIGHT = 2;
double NECK_WIDTH = 1;

double EAR_HEIGTH = 1;
double EAR_WIDTH = 0.5;
double EAR_LONG = 0.5;

double Body_Offset = UPPER_LEG_HEIGHT+ LOWER_LEG_HEIGHT;



// Set up menu item indices, which we can also use with the joint angles
enum {
	Torso,
	Neck,
	Head,
	RightUpperArm,
	RightLowerArm,
	LeftUpperArm,
	LeftLowerArm,
	RightUpperLeg,
	RightLowerLeg,
	LeftUpperLeg,
	LeftLowerLeg,
	LeftEar,
	RightEar,
	NumJointAngles,
	Quit
};

// Joint angles with initial values
GLfloat
theta[NumJointAngles] = {
	0.0,    // Torso
	0.0,    // Neck
	0.0,    // Head2
	200.0,    // RightUpperArm
	300.0,    // RightLowerArm
	200.0,    // LeftUpperArm
	300.0,    // LeftLowerArm
	200.0,  // RightUpperLeg
	300.0,    // RightLowerLeg
	200.0,  // LeftUpperLeg
	300.0 ,    // LeftLowerLeg
	210.0,	//LeftEar
	150		//RightEar
};

GLint angle = Torso;

GLdouble distTL = 0.1;	//默认光源到物体的距离

GLfloat radius = 1;
GLfloat thera = 3.14 / 2 / 2;
GLfloat phi = 3.01 / 2 / 2;

vec4 black(0.0, 0.0, 0.0, 1.0);	//black
vec4 red(1.0, 0.0, 0.0, 1.0);	//red
vec4 white(1.0, 1.0, 1.0, 1.0);  // White

vec3 lightPos(-5, 20, 5);
float rotationAngle = 0;
float objectUp = 0.5;

double test = 0;

