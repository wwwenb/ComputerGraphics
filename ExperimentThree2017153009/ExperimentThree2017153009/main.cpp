/*
*        Computer Graphics Course - Shenzhen University
*    Week 9 - Phong Reflectance Model (per-fragment shading)
* ============================================================
*
* - 本代码仅仅是参考代码，具体要求请参考作业说明，按照顺序逐步完成。
* - 关于配置OpenGL开发环境、编译运行，请参考第一周实验课程相关文档。
*/

#include "include/Angel.h"
#include "include/TriMesh.h"

#include <cstdlib>
#include <iostream>

#pragma comment(lib, "glew32.lib")

using namespace std;

GLuint programID;
GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint vertexNormalID;
GLuint vertexIndexBuffer;

GLuint vPositionID;
GLuint vNormalID;
GLuint modelViewMatrixID;
GLuint modelViewProjMatrixID;
GLuint fColorID;

GLuint modelMatrixID;
GLuint viewMatrixID;
GLuint projMatrixID;

GLuint lightPosID;
GLuint distToLightID;

GLdouble distTL = 0.1;	//默认光源到物体的距离

GLfloat radius = 1;		
GLfloat thera = 3.14 / 2 / 2;
GLfloat phi = 3.01/2/2;

vec4 black(0.0, 0.0, 0.0, 1.0);	//black
vec4 red(1.0, 0.0, 0.0, 1.0);	//red
vec4 white(1.0, 1.0, 1.0, 1.0);  // White

const GLfloat dr = 4 * DegreesToRadians;

TriMesh* mesh = new TriMesh();
vec3 lightPos(-0.5, 3.0, 0.5);
float rotationAngle = 0;
float objectUp = 0.5;

///////////////////////////////////////////////////////////////////////////
//声明函数
void print();

//////////////////////////////////////////////////////////////////////////
// 相机参数设置

namespace Camera
{
	mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projMatrix;

	mat4 ortho( const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar )
	{
		// TODO 请按照实验课内容补全相机观察矩阵的计算
		mat4 c;
		c[0][0] = 2.0 / (right - left);
		c[1][1] = 2.0 / (top - bottom);
		c[2][2] = -2.0 / (zFar - zNear);
		c[3][3] = 1.0;
		c[0][3] = -(right + left) / (right - left);
		c[1][3] = -(top + bottom) / (top - bottom);
		c[2][3] = -(zFar + zNear) / (zFar - zNear);
		return c;
	}

	mat4 perspective( const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar)
	{
		// TODO 请按照实验课内容补全相机观察矩阵的计算
		GLfloat top = tan(fovy * M_PI / 180 / 2) * zNear;
		GLfloat right = top * aspect;

		mat4 c;
		c[0][0] = zNear / right;
		c[1][1] = zNear / top;
		c[2][2] = -(zFar + zNear) / (zFar - zNear);
		c[2][3] = -(2.0*zFar*zNear) / (zFar - zNear);
		c[3][2] = -1.0;
		c[3][3] = 0.0;
		return c;
	}

	mat4 lookAt( const vec4& eye, const vec4& at, const vec4& up )
	{
		// TODO 请按照实验课内容补全相机观察矩阵的计算
		vec4 n = normalize(eye - at);
		vec4 u = normalize(vec4(cross(up, n), 0.0));
		vec4 v = normalize(vec4(cross(n, u), 0.0));

		vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
		mat4 c = mat4(u, v, n, t);
		return c * Translate(-eye);
	}
}

//////////////////////////////////////////////////////////////////////////
// OpenGL 初始化

void init()
{
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);		//将窗口背景设置为灰色

	programID = InitShader("vshader_frag.glsl", "fshader_frag.glsl");

	// 从顶点着色器和片元着色器中获取变量的位置
	vPositionID = glGetAttribLocation(programID, "vPosition");
	vNormalID = glGetAttribLocation(programID, "vNormal");
	modelViewMatrixID = glGetUniformLocation(programID, "modelViewMatrix");
	modelViewProjMatrixID = glGetUniformLocation(programID, "modelViewProjMatrix");
	lightPosID = glGetUniformLocation(programID, "lightPos");
	distToLightID = glGetUniformLocation(programID, "distToLight");
	fColorID = glGetUniformLocation(programID, "fColor");

	modelMatrixID = glGetUniformLocation(programID, "modelMatrix");
	viewMatrixID = glGetUniformLocation(programID, "viewMatrix");
	projMatrixID = glGetUniformLocation(programID, "projMatrix");

	// 读取外部三维模型
	mesh->read_off("sphere.off");

	vector<vec3f> vs = mesh->v();
	vector<vec3i> fs = mesh->f();
	vector<vec3f> ns;

	// TODO 计算球模型在每个顶点的法向量，并存储到ns数组中
	for (int i = 0; i < vs.size(); ++i) {
		vs[i] = vs[i] + vec3(0, objectUp, 0);	//将物体在y轴的方向上上移0.5
		ns.push_back(vs[i] - vec3(0, 0, 0));	//计算点的法向量
	}

	// 生成VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// 生成VBO，并绑定顶点数据
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(vec3f), vs.data(), GL_STATIC_DRAW);

	// 生成VBO，并绑定法向量数据
	glGenBuffers(1, &vertexNormalID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
	glBufferData(GL_ARRAY_BUFFER, ns.size() * sizeof(vec3f), ns.data(), GL_STATIC_DRAW);

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

	// TODO 给Lookat函数需要的eye，at，up赋值

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

	 glEnableVertexAttribArray(vNormalID);
	 glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
	 glVertexAttribPointer(
		 vNormalID,
		 3,
		 GL_FLOAT,
		 GL_FALSE,
		 0,
		 (void*)0
	 );

	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);

	 //glDrawElements(
		// GL_TRIANGLES,
		// int(mesh->f().size() * 3),
		// GL_UNSIGNED_INT,
		// (void*)0
	 //);

	 vec4 eye = vec4(radius * sin(thera) * cos(phi),
		 radius * sin(thera) * sin(phi),
		 radius * cos(thera),
		 1.0);
	 vec4 at = vec4(0, 0, 0, 1);
	 vec4 up = vec4(0, 1, 0, 0);

	 Camera::modelMatrix = RotateY(rotationAngle);
	 Camera::viewMatrix = LookAt(eye, at, up);
	 Camera::projMatrix = Camera::ortho(-1, 1, -1, 1, -4, 4);

	 // 将光源位置传入顶点着色器
	 glUniform3fv(lightPosID, 1, &lightPos[0]);
	 //将距离传入顶点着色器
	 glUniform1f(distToLightID, distTL);

	 //旋转光源
	 vec4 temp = Camera::modelMatrix * vec4(lightPos, 1);
	 lightPos[0] = temp.x;
	 lightPos[1] = temp.y;
	 lightPos[2] = temp.z;
	 rotationAngle = 0;

	 //计算投影矩阵
	 float lx = lightPos[0];
	 float ly = lightPos[1];
	 float lz = lightPos[2];
	 mat4 shadowMatrix = mat4(-ly, 0, 0, 0,
		 lx, 0, lz, 1,
		 0, 0, -ly, 0,
		 0, 0, 0, -ly);

	 // 请阅读顶点着色器的内容，对照实验1的异同
	 mat4 shadowProMatrix = shadowMatrix * Camera::modelMatrix;
	 glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &shadowProMatrix[0][0]);

	 //绘制阴影且为黑色
	 glUniform4fv(fColorID, 1, black);
	 glDrawElements(
		 GL_TRIANGLES,
		 int(mesh->f().size() * 3),
		 GL_UNSIGNED_INT,
		 (void*)0
	 );

	// TODO 计算相机观察矩阵和投影矩阵，并传入顶点着色器
	 mat4 modelViewMatrix = Camera::viewMatrix* Camera::modelMatrix;
	 mat4 modelViewProjMatrix = Camera::projMatrix* modelViewMatrix;

	 glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, &Camera::viewMatrix[0][0]);
	 glUniformMatrix4fv(projMatrixID, 1, GL_TRUE, &Camera::projMatrix[0][0]);
	 glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &Camera::modelMatrix[0][0]);

	 glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, &modelViewMatrix[0][0]);
	 glUniformMatrix4fv(modelViewProjMatrixID, 1, GL_TRUE, &modelViewProjMatrix[0][0]);
	 
	 //绘制物体且为白色
	 glUniform4fv(fColorID, 1, white);
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
	// TODO 用鼠标控制光源的位置，并实时更新光照效果
	lightPos[0] = (x / 500.0 - 0.5);
	lightPos[1] = 3;
	lightPos[2] = (y / 500.0 - 0.5);
	glutPostRedisplay();
	return;
}

/////////////////////////////////////////////////////////////////////////
//打印键盘提示信息
void print() {
	printf("%s\n\n", "Designed by 2017153009");
	printf("Keyboard options:\n");
	printf("x: 增加物体到光源的距离\n");
	printf("X: 减少物体到光源的距离\n");
	printf("y、Y:旋转光源\n");
	printf("v: 将物体上移\n");
	printf("V: 将物体下移\n");
	printf("Mouse options:\n");
	printf("改变光源位置\n");
	printf("t: Reset all transformations and deltas\n");
}

//////////////////////////////////////////////////////////////////////////
// 键盘响应函数
void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
	case 033:	// ESC键 和 'q' 键退出游戏
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit (EXIT_SUCCESS);
		break;
	case 'x':distTL *= 2;break;
	case 'X':distTL /= 2; break;
	case 'y':		// 'y' 键使得三角形旋转-1.0度
		rotationAngle -= 5.0;
		break;
	case 'Y':		// 'Y' 键使得三角形旋转+1.0度
		rotationAngle += 5.0;
		break;
	case 'r':
		if (radius < 2)
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
	case 'v':
		if (objectUp < 1) {
			objectUp += 0.1; init();	//上移物体
		}
		break;
	case 'V':
		if (objectUp > 0.5) {
			objectUp -= 0.1;		//下移物体
			init();
		}
		break;
	case 't':				//重置r、thera和phi的值
		radius = 1;
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

//////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	print();
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