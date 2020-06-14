/*
*        Computer Graphics Course - Shenzhen University
*      Week 6 - Camera Position and Control Skeleton Code
* ============================================================
*
* - 本代码仅仅是参考代码，具体要求请参考作业说明，按照顺序逐步完成。
* - 关于配置OpenGL开发环境、编译运行，请参考第一周实验课程相关文档。
*/

#include "include/Angel.h"
#include "include/TriMesh.h"

#pragma comment(lib, "glew32.lib")

#include <cstdlib>
#include <iostream>

using namespace std;

GLuint programID;
GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint vertexIndexBuffer;

GLuint vPositionID;
GLuint modelViewprojectMatrixID;

GLfloat radius = 0.1;
GLfloat thera = 0.2;
GLfloat phi = 0;

const GLfloat dr =  4*DegreesToRadians;


TriMesh* mesh = new TriMesh();

namespace Camera
{
    mat4 modelMatrix; //模型变换矩阵
    mat4 viewMatrix;  //相机观察矩阵
	mat4 projectMatrix; //投影矩阵

	mat4 lookAt( const vec4& eye, const vec4& at, const vec4& up )
	{
		// TODO 请按照实验课内容补全相机观察矩阵的计算
		vec4 n = normalize(eye - at);
		vec3 uu = normalize(cross(up, n));
		vec4 u = vec4(uu.x, uu.y, uu.z, 0.0);
		vec3 vv = normalize(cross(n, u));
		vec4 v = vec4(vv.x, vv.y, vv.z, 0.0);
		vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
		mat4 c = mat4(u, v, n, t);
		return c * Translate(-eye);
	}
}

//////////////////////////////////////////////////////////////////////////
// OpenGL 初始化

void init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// 加载shader并且获取变量的位置
	programID = InitShader("vshader.glsl", "fshader.glsl");
	vPositionID = glGetAttribLocation(programID, "vPosition");
	modelViewprojectMatrixID = glGetUniformLocation(programID, "modelViewprojectMatrix");

	// 从外部读取三维模型文件
	mesh->read_off("cube.off");

	vector<vec3f> vs = mesh->v();
	vector<vec3i> fs = mesh->f();

	// 生成VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// 生成VBO，并绑定顶点坐标
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(vec3f), vs.data(), GL_STATIC_DRAW);

	// 生成VBO，并绑定顶点索引
	glGenBuffers(1, &vertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fs.size() * sizeof(vec3i), fs.data(), GL_STATIC_DRAW);



	// OpenGL相应状态设置
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

//////////////////////////////////////////////////////////////////////////
// 渲染

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	// TODO 设置相机参数
	 vec4 eye = vec4(radius*sin(thera)*cos(phi),
		 radius*sin(thera)*sin(phi),
		 radius* cos(thera),
		 1.0);
	 vec4 at = vec4(0, 0, 0, 1);
	 vec4 up = vec4(0, 1, 0, 0);
	
	 //计算机相机观察矩阵并传入顶点着色器
	Camera::modelMatrix = mat4(1.0);
	Camera::viewMatrix = LookAt(eye, at, up);
	Camera::projectMatrix = mat4(1.0);
	mat4 modelViewprojectMatrix = Camera::viewMatrix* Camera::modelMatrix *Camera::projectMatrix;
	glUniformMatrix4fv(modelViewprojectMatrixID, 1, GL_TRUE, &modelViewprojectMatrix[0][0]);

	glEnableVertexAttribArray(vPositionID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(
		vPositionID,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);

	glDrawElements(
		GL_TRIANGLES,
		int(mesh->f().size() * 3),
		GL_UNSIGNED_INT,
		(void*)0
	);

	glDisableVertexAttribArray(vPositionID);
	glUseProgram(0);

	glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////
// 重新设置窗口

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

//////////////////////////////////////////////////////////////////////////
// 鼠标响应函数

void mouse(int button, int state, int x, int y)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
// 键盘响应函数

void keyboard(unsigned char key, int x, int y)
{
	
	// Todo：键盘控制相机的位置和朝向
	switch(key) 
	{
	case 033:	// ESC键 和 'q' 键退出游戏
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit (EXIT_SUCCESS);
		break;

	case 'r': 
		if(radius< 2)
			radius *= 1.2;		//改变观察点的r值
		break;
	case 'R': radius *= 0.5;
		break;
	case 'o': thera += dr;		//改变观察点的thera值
		break;
	case 'O':thera -= dr;
		break;
	case 'p': phi += dr;	//改变观察点的phi值
		break;
	case 'P': phi -= dr;
		break;
	case 't':				//重置r、thera和phi的值
		radius = 0.1;
		thera = 0;
		phi = 0;
	}

	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////

void idle(void)
{
	glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////

void clean()
{
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayID);

	if (mesh) {
		delete mesh;
		mesh = NULL;
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Designed by 2017153009");

	glewInit();
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();

	clean();

	return 0;
}