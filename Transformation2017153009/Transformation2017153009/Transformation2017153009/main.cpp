#include "Angel.h"
#include "mat.h"

#include<vector>
using namespace std;

#pragma comment(lib, "glew32.lib")

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

vec3 scaleTheta(1.0, 1.0, 1.0);
vec3 rotateTheta(0.0, 0.0, 0.0);
vec3 translateTheta(0.0, 0.0, 0.0);

GLint matrixLocation;
int currentTransform = TRANSFORM_TRANSLATE;
int mainWindow;
const int NUM_VERTICES = 8;

// ��λ������ĸ�����
const vec4 vertices[NUM_VERTICES] = {
	vec4(-0.5, -0.5,  0.5, 1.0),
	vec4(-0.5,  0.5,  0.5, 1.0),
	vec4(0.5,  0.5,  0.5, 1.0),
	vec4(0.5, -0.5,  0.5, 1.0),
	vec4(-0.5, -0.5, -0.5, 1.0),
	vec4(-0.5,  0.5, -0.5, 1.0),
	vec4(0.5,  0.5, -0.5, 1.0),
	vec4(0.5, -0.5, -0.5, 1.0)
};

// ÿ������Ķ�Ӧ��ɫ
const vec4 vertexColors[NUM_VERTICES] = {
	vec4(0.0, 0.0, 0.0, 1.0),  // Black
	vec4(1.0, 0.0, 0.0, 1.0),  // Red
	vec4(1.0, 1.0, 0.0, 1.0),  // Yellow
	vec4(0.0, 1.0, 0.0, 1.0),  // Green
	vec4(0.0, 0.0, 1.0, 1.0),  // Blue
	vec4(1.0, 0.0, 1.0, 1.0),  // Magenta
	vec4(1.0, 1.0, 1.0, 1.0),  // White
	vec4(0.0, 1.0, 1.0, 1.0)   // Cyan
};

std::vector<vec4> points;
std::vector<vec4> colors;

// �洢ÿ�����ǵĸ������㲢Ϊ�㸳ɫ
void storeTrianglePoints(int a, int b, int c)
{
	points.push_back(vertices[a]);
	points.push_back(vertices[b]);
	points.push_back(vertices[c]);

	colors.push_back(vertexColors[a]);
	colors.push_back(vertexColors[b]);
	colors.push_back(vertexColors[c]);
}

// ����������12�������Σ�36������Ͷ�Ӧ��ɫ
void generateCube()
{
	// @TODO: �޸Ĵ˺�������storeTrianglePoints(),
	// �洢ÿ�������εĶ���λ�ú���ɫ��Ϣ��
	vec3 faces[] = {
		vec3(0, 1, 2),
		vec3(0, 2, 3),
		vec3(0, 3, 4),
		vec3(3, 4, 7),
		vec3(0, 1, 5),
		vec3(0, 5, 4),
		vec3(2, 3, 6),
		vec3(3, 6, 7),
		vec3(5, 1, 2),
		vec3(5, 6, 2),
		vec3(4, 5, 6),
		vec3(4, 6, 7),
	};

	for (vec3 ve : faces) {
		storeTrianglePoints(ve.x, ve.y, ve.z);
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
	
	m = m* Translate(translateTheta.x, translateTheta.y, translateTheta.z);
	m = m* RotateX(rotateTheta.x);
	m = m* RotateY(rotateTheta.y);
	m = m* RotateZ(rotateTheta.z);
	m = m* Scale(scaleTheta.x, scaleTheta.y, scaleTheta.z);

	// ��ָ��λ���д���任����
	glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, points.size());

	glutSwapBuffers();
}

// ͨ��Deltaֵ����Theta
void updateTheta(int axis, int sign) {
	switch (currentTransform) {
	case TRANSFORM_SCALE:
		scaleTheta[axis] += sign * scaleDelta;
		break;
	case TRANSFORM_ROTATE:
		rotateTheta[axis] += sign * rotateDelta;
		break;
	case TRANSFORM_TRANSLATE:
		translateTheta[axis] += sign * translateDelta;
		break;
	}
	glutPostWindowRedisplay(mainWindow);
	
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
	//glutPostWindowRedisplay(mainWindow);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
		updateTheta(X_AXIS, 1);
		break;
	case 'a':
		updateTheta(X_AXIS, -1);
		break;
	case 'w':
		updateTheta(Y_AXIS, 1);
		break;
	case 's':
		updateTheta(Y_AXIS, -1);
		break;
	case 'e':
		updateTheta(Z_AXIS, 1);
		break;
	case 'd':
		updateTheta(Z_AXIS, -1);
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

void menuEvents(int menuChoice)
{
	currentTransform = menuChoice;
}

void setupMenu()
{
	glutCreateMenu(menuEvents);
	glutAddMenuEntry("Scale", TRANSFORM_SCALE);
	glutAddMenuEntry("Rotate", TRANSFORM_ROTATE);
	glutAddMenuEntry("Translate", TRANSFORM_TRANSLATE);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

void printHelp() {
	printf("%s\n\n", "3D Transfomations");
	printf("Keyboard options:\n");
	printf("q: Increase x\n");
	printf("a: Decrease x\n");
	printf("w: Increase y\n");
	printf("s: Decrease y\n");
	printf("e: Increase z\n");
	printf("d: Decrease z\n");
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
	setupMenu();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	// ���������Ϣ
	printHelp();
	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	return 0;
}
