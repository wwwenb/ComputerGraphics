#include "Angel.h"

#include <vector>
#include <string>
#include <fstream>
#include<iostream>


#pragma comment(lib, "glew32.lib")
using namespace std;

// 三角面片中的顶点序列
typedef struct vIndex {
	unsigned int a, b, c;
	vIndex(int ia, int ib, int ic) : a(ia), b(ib), c(ic) {}
} vec3i;

std::string filename;
std::vector<vec3> vertices; //从文件中读取的顶点坐标
std::vector<vec3i> faces;   //从文件中读取的面顶点索引

int nVertices = 0;
int nFaces = 0;
int nEdges = 0;

std::vector<vec3> points;   //传入着色器的绘制点
std::vector<vec3> colors;   //传入着色器的颜色

const int NUM_VERTICES = 8;

const vec3 vertex_colors[NUM_VERTICES] = {
	vec3(1.0, 1.0, 1.0),  // White
	vec3(1.0, 1.0, 0.0),  // Yellow
	vec3(0.0, 1.0, 0.0),  // Green
	vec3(0.0, 1.0, 1.0),  // Cyan
	vec3(1.0, 0.0, 1.0),  // Magenta
	vec3(1.0, 0.0, 0.0),  // Red
	vec3(0.0, 0.0, 0.0),  // Black
	vec3(0.0, 0.0, 1.0)   // Blue
};

void read_off(const std::string filename)
{
	if (filename.empty()) {
		return;
	}

	std::ifstream fin;
	fin.open(filename);

	
	// @TODO：修改此函数读取OFF文件中三维模型的信息

	//读入首行字符串"OFF"
	string line= "";
	getline(fin, line);

	//读入点、面、边数目
	fin >> nVertices >> nFaces >> nEdges;

	// 读取每个顶点的坐标
	for (int i = 0; i < nVertices; i++) {
		double x, y, z;
		fin >> x >> y >> z;
		vertices.push_back(vec3(x, y, z));
	}

	// 读取面的顶点序列
	for (int i = 0; i < nFaces; i++) {
		int n, a, b, c;
		fin >> n >> a >> b >> c;
		faces.push_back(vec3i(a, b, c));
	}

	fin.close();
}

void storeFacesPoints()
{
	points.clear();
	colors.clear();

	// @TODO： 修改此函数在points和colors容器中存储每个三角面片的各个点和颜色信息
	for (vec3i ve : faces) {
		cout << ve.a << endl;
		points.push_back(vertices[ve.a]);
		points.push_back(vertices[ve.b]);
		points.push_back(vertices[ve.c]);

		colors.push_back(vertex_colors[ve.a]);
		colors.push_back(vertex_colors[ve.b]);
		colors.push_back(vertex_colors[ve.c]);
	}
}

void init()
{
	storeFacesPoints();

	// 创建顶点数组对象
	GLuint vao[1];
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	// 创建并初始化顶点缓存对象
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3) + colors.size() * sizeof(vec3), NULL, GL_STATIC_DRAW);

	// @TODO：修改完成后再打开下面注释，否则程序会报错
	// 分别读取数据
	glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(vec3), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3), colors.size() * sizeof(vec3), &colors[0]);

	// 读取着色器并使用
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// 从顶点着色器中初始化顶点的位置
	GLuint pLocation = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(pLocation);
	glVertexAttribPointer(pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	// 从片元着色器中初始化顶点的颜色
	GLuint cLocation = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(cLocation);
	glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(vec3)));

	// 黑色背景
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(void)
{
	// 清理窗口，包括颜色缓存和深度缓存
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 绘制面
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// 消除背面光照
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glDrawArrays(GL_TRIANGLES, 0, points.size());
	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	// @TODO：窗口显示模式支持深度测试
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Designed by 2017153009");

	glewExperimental = GL_TRUE;
	glewInit();

	// 读取off模型文件
	read_off("cube.off");

	init();
	glutDisplayFunc(display);

	// @TODO：启用深度测试
	glEnable(GL_DEPTH_TEST);


	glutMainLoop();
	return 0;
}
