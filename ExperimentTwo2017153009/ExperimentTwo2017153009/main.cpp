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
int selectAxis= 0; //默认X坐标轴
bool direction = false;

vec3 scaleTheta(1.0, 1.0, 1.0);
vec3 rotateTheta(0.0, 0.0, 0.0);
vec3 translateTheta(0.0, 0.0, 0.0);

GLint matrixLocation;
int currentTransform = TRANSFORM_TRANSLATE;
int mainWindow;
const int NUM_COLORS = 8;


// 每个顶点的对应颜色
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


std::vector<vec3> fpoints;		//文件中点的坐标
std::vector<vec4> points;
std::vector<vec4> colors;

// 存储每个三角的各个顶点并为点赋色
void storeTrianglePoints(int a, int b, int c)
{
	points.push_back(fpoints[a]);
	points.push_back(fpoints[b]);
	points.push_back(fpoints[c]);

	//对颜色的索引模颜色的总数防止数组越界
	colors.push_back(vertexColors[a%NUM_COLORS]);
	colors.push_back(vertexColors[b % NUM_COLORS]);
	colors.push_back(vertexColors[c % NUM_COLORS]);

}

// 立方体生成的三角形的点和颜色
void generateCube()
{
	// @TODO: 修改此函数调用storeTrianglePoints(),
	// 存储每个三角形的顶点位置和颜色信息。
	//string filename = "cube.off";
	string filename = "cow.off";
	ifstream fin;
	fin.open(filename);

	//读取首行
	string line = "";
	getline(fin, line);

	//读取点的位置信息
	fin >> nVertices >> nFaces >> nEdges;
	for (int i = 0; i < nVertices; i++) {
		double x, y, z;
		fin >> x >> y >> z;
		fpoints.push_back(vec3(x, y, z));
	}

	//读取面的点索引
	for (int i = 0; i < nFaces; i++) {
		int n, a, b, c;
		fin >> n >> a >> b >> c;
		storeTrianglePoints(a, b, c);
	}

}

void init()
{
	generateCube();

	// 创建顶点数组对象
	GLuint vao[1];
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	// 创建并初始化顶点缓存对象
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4) + colors.size() * sizeof(vec4),
		NULL, GL_STATIC_DRAW);

	// @TODO：修改完成后再打开下面注释，否则程序会报错
	// 分别读取数据
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec4), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4), colors.size() * sizeof(vec4), &colors[0]);

	// 读取着色器并使用
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// 从顶点着色器中初始化顶点的位置
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	// 从片元着色器中初始化顶点的颜色
	GLuint cLocation = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec4)));
	// 获得矩阵存储位置
	matrixLocation = glGetUniformLocation(program, "matrix");

	// 白色背景
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void display()
{
	// 清理窗口
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 生成变换矩阵
	mat4 m(1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);

	// @TODO: 在此处修改函数，计算变换矩阵
	m = m * Translate(translateTheta.x, translateTheta.y, translateTheta.z);
	m = m * RotateX(rotateTheta.x);
	m = m * RotateY(rotateTheta.y);
	m = m * RotateZ(rotateTheta.z);
	m = m * Scale(scaleTheta.x, scaleTheta.y, scaleTheta.z);

	// 从指定位置中传入变换矩阵
	glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, m);
	glDrawArrays(GL_TRIANGLES, 0, points.size());

	glutSwapBuffers();
}

// 通过Delta值更新Theta
void updateTheta(int axis, int sign) {

		rotateTheta[axis] += sign * rotateDelta;

}

// 复原Theta和Delta
void resetTheta()
{
	scaleTheta = vec3(1.0, 1.0, 1.0);
	rotateTheta = vec3(0.0, 0.0, 0.0);
	translateTheta = vec3(0.0, 0.0, 0.0);
	scaleDelta = DEFAULT_DELTA;
	rotateDelta = DEFAULT_DELTA;
	translateDelta = DEFAULT_DELTA;
}

//更新旋转的参数
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

//鼠标点击事件
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
	//判断用户输入的xyz按钮
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
		// Esc按键
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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);    // 窗口支持双重缓冲和深度测试
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	mainWindow = glutCreateWindow("Designed by 2017153009");

	glewExperimental = GL_TRUE;
	glewInit();

	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseClick); //监听鼠标事件

	// 输出帮助信息
	printHelp();
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	return 0;
}
