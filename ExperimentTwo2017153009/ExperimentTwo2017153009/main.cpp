#include "Angel.h"
#include "mat.h"
#include <string>
#include<vector>
#include<iostream>
#include<fstream>

#pragma comment(lib, "glew32.lib")
using namespace std;

const int X_AXIS = 0;
const int Y_AXIS = 1;
const int Z_AXIS = 2;

const int TRANSFORM_SCALE = 0;
const int TRANSFORM_ROTATE = 1;
const int TRANSFORM_TRANSLATE = 2;

const double DELTA_DELTA = 0.1;
const double DEFAULT_DELTA = 0.3;

double scaleDelta = DEFAULT_DELTA;
double rotateDelta = DEFAULT_DELTA;
double translateDelta = DEFAULT_DELTA;

int nVertices;
int nFaces;
int nEdges;
int selectAxis= 0; //Ĭ��X������
bool direction = false;

vec3 scaleTheta(1.0, 1.0, 1.0);
vec3 rotateTheta(0.0, 0.0, 0.0);
vec3 translateTheta(0.0, 0.0, 0.0);

GLint matrixLocation;
int currentTransform = TRANSFORM_TRANSLATE;
int mainWindow;
const int NUM_COLORS = 8;


// ÿ������Ķ�Ӧ��ɫ
const vec4 vertexColors[NUM_COLORS] = {
	vec4(0.0, 0.0, 0.0, 1.0),  // Black
	vec4(1.0, 1.0, 1.0, 1.0),  // White

	vec4(1.0, 1.0, 0.0, 1.0),  // Yellow
	vec4(1.0, 0.0, 0.0, 1.0),  // Red
	vec4(0.0, 1.0, 1.0, 1.0),   // Cyan
	vec4(0.0, 1.0, 0.0, 1.0),  // Green
	vec4(0.0, 0.0, 1.0, 1.0),  // Blue
	vec4(1.0, 0.0, 1.0, 1.0),  // Magenta
	
};


std::vector<vec3> fpoints;		//�ļ��е������
std::vector<vec4> points;
std::vector<vec4> colors;

// �洢ÿ�����ǵĸ������㲢Ϊ�㸳ɫ
void storeTrianglePoints(int a, int b, int c)
{
	points.push_back(fpoints[a]);
	points.push_back(fpoints[b]);
	points.push_back(fpoints[c]);

	//����ɫ������ģ��ɫ��������ֹ����Խ��
	colors.push_back(vertexColors[a%NUM_COLORS]);
	colors.push_back(vertexColors[b % NUM_COLORS]);
	colors.push_back(vertexColors[c % NUM_COLORS]);

}

// ���������ɵ������εĵ����ɫ
void generateCube()
{
	// @TODO: �޸Ĵ˺�������storeTrianglePoints(),
	// �洢ÿ�������εĶ���λ�ú���ɫ��Ϣ��
	//string filename = "cube.off";
	string filename = "cow.off";
	ifstream fin;
	fin.open(filename);

	//��ȡ����
	string line = "";
	getline(fin, line);

	//��ȡ���λ����Ϣ
	fin >> nVertices >> nFaces >> nEdges;
	for (int i = 0; i < nVertices; i++) {
		double x, y, z;
		fin >> x >> y >> z;
		fpoints.push_back(vec3(x, y, z));
	}

	//��ȡ��ĵ�����
	for (int i = 0; i < nFaces; i++) {
		int n, a, b, c;
		fin >> n >> a >> b >> c;
		storeTrianglePoints(a, b, c);
	}

}

void init()
{
	generateCube();

	// ���������������
	GLuint vao[1];
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	// ��������ʼ�����㻺�����
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4) + colors.size() * sizeof(vec4),
		NULL, GL_STATIC_DRAW);

	// @TODO���޸���ɺ��ٴ�����ע�ͣ��������ᱨ��
	// �ֱ��ȡ����
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec4), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4), colors.size() * sizeof(vec4), &colors[0]);

	// ��ȡ��ɫ����ʹ��
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// �Ӷ�����ɫ���г�ʼ�������λ��
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	// ��ƬԪ��ɫ���г�ʼ���������ɫ
	GLuint cLocation = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec4)));
	// ��þ���洢λ��
	matrixLocation = glGetUniformLocation(program, "matrix");

	// ��ɫ����
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void display()
{
	// ������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ���ɱ任����
	mat4 m(1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);

	// @TODO: �ڴ˴��޸ĺ���������任����
	m = m * Translate(translateTheta.x, translateTheta.y, translateTheta.z);
	m = m * RotateX(rotateTheta.x);
	m = m * RotateY(rotateTheta.y);
	m = m * RotateZ(rotateTheta.z);
	m = m * Scale(scaleTheta.x, scaleTheta.y, scaleTheta.z);

	// ��ָ��λ���д���任����
	glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, points.size());

	glutSwapBuffers();
}

// ͨ��Deltaֵ����Theta
void updateTheta(int axis, int sign) {

		rotateTheta[axis] += sign * rotateDelta;

}

// ��ԭTheta��Delta
void resetTheta()
{
	scaleTheta = vec3(1.0, 1.0, 1.0);
	rotateTheta = vec3(0.0, 0.0, 0.0);
	translateTheta = vec3(0.0, 0.0, 0.0);
	scaleDelta = DEFAULT_DELTA;
	rotateDelta = DEFAULT_DELTA;
	translateDelta = DEFAULT_DELTA;
}

//������ת�Ĳ���
void idleFunction() {

	if (direction) {
		switch (selectAxis)
		{
		case X_AXIS:
			updateTheta(X_AXIS, 1);
			break;
		case Y_AXIS:
			updateTheta(Y_AXIS, 1);
			break;
		case Z_AXIS:
			updateTheta(Z_AXIS, 1);
			break;
		}
		glutPostWindowRedisplay(mainWindow);
	}
	else {
		switch (selectAxis)
		{
		case X_AXIS:
			updateTheta(X_AXIS, -1);
			break;
		case Y_AXIS:
			updateTheta(Y_AXIS, -1);
			break;
		case Z_AXIS:
			updateTheta(Z_AXIS, -1);
			break;
		}
		glutPostWindowRedisplay(mainWindow);
	}

	

}

//������¼�
void mouseClick(int btn, int state, int x, int y) {

	if (state == GLUT_UP) {
		glutIdleFunc(NULL);
	}
	if (btn == GLUT_LEFT_BUTTON&& state== GLUT_DOWN) {
		direction = true;
		glutIdleFunc(idleFunction);
	}
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		direction = false;
		glutIdleFunc(idleFunction);

	}
}

void keyboard(unsigned char key, int x, int y)
{
	//�ж��û������xyz��ť
	switch (key) {
	case 'x':
		selectAxis = 0;
		break;
	case 'y':
		selectAxis = 1;
		break;
	case 'z':
		selectAxis= 2;
		break;
	case 't':
		resetTheta();
		break;
	case 033:
		// Esc����
		exit(EXIT_SUCCESS);
		break;
	}
	glutPostWindowRedisplay(mainWindow);
}


void printHelp() {
	printf("%s\n\n", "Designed by 2017153009");
	printf("Keyboard options:\n");
	printf("x: Change x axis\n");
	printf("y: Change y axis\n");
	printf("z: Change z axis\n");
	printf("Mouse options:\n");
	printf("left button: Increase axis\n");
	printf("right button: Decrease axis\n");
	printf("t: Reset all transformations and deltas\n");
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);    // ����֧��˫�ػ������Ȳ���
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	mainWindow = glutCreateWindow("Designed by 2017153009");

	glewExperimental = GL_TRUE;
	glewInit();

	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseClick); //��������¼�

	// ���������Ϣ
	printHelp();
	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	return 0;
}
