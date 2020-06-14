/*
*        Computer Graphics Course - Shenzhen University
*       Week 7 - Shadow Projection Matrix Skeleton Code
* ============================================================
*
* - ����������ǲο����룬����Ҫ����ο���ҵ˵��������˳������ɡ�
* - ��������OpenGL�����������������У���ο���һ��ʵ��γ�����ĵ���
*
* ============================================================
* ���̣��ⲩ��(Bojian Wu)
* ���䣺bj.wu@siat.ac.cn
* ���������˵������ҵ���κ����⣬��ӭ���ʼ���ѯ��
*/

#include "Angel.h"

#pragma comment(lib, "glew32.lib")

#include <cstdlib>
#include <iostream>

using namespace std;

GLuint programID;
GLuint vertexArrayID;
GLuint vertexBufferID;
GLuint vertexIndexBuffer;

GLuint vPositionID;
GLuint rotationMatrixID;
GLuint modelMatrixID;
GLuint viewMatrixID;
GLuint projMatrixID;
GLuint fColorID;


// ����ӽǲ���
float l = -3.0, r = 3.0;    // ���Ҳü�ƽ��
float b = -3.0, t = 3.0;    // ���²ü�ƽ��
float n = -3.0, f = 3.0;    // Զ���ü�ƽ��
float rotationAngle = 0.0; // ��ת�Ƕ�

vec4 red(1.0, 0.0, 0.0, 1.0);
vec4 black(0.0, 0.0, 0.0, 1.0);

float lightPos[3] = {-0.5, 2.0, 0.5};

vec3 points[3] = {
	vec3(-0.5, 0.5, 0.5),
	vec3(0.5, 0.5, 0.5),
	vec3(0.0, 0.75, 0.0)
};

//////////////////////////////////////////////////////////////////////////
// �����������

namespace Camera
{
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projMatrix;

	mat4 ortho( const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar )
	{
		// TODO �밴��ʵ��ν��ⲹȫ����ͶӰ����ļ���
		return mat4(2 / (right - left), 0, 0, 0,
			0, 2 / (top - bottom), 0, 0,
			0, 0, -2 / (zFar - zNear), 0,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1);
	}


	mat4 lookAt(const vec4& eye, const vec4& at, const vec4& up)
	{
		// TODO �밴��ʵ������ݲ�ȫ����۲����ļ���
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
// OpenGL ��ʼ��

void init()
{
	glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
	programID = InitShader("vshader.glsl", "fshader.glsl");

	// �Ӷ�����ɫ���л�ȡ��Ӧ������λ��
	vPositionID = glGetAttribLocation(programID, "vPosition");

	modelMatrixID = glGetUniformLocation(programID, "modelMatrix");
	viewMatrixID = glGetUniformLocation(programID, "viewMatrix");
	projMatrixID = glGetUniformLocation(programID, "projMatrix");
	fColorID = glGetUniformLocation(programID, "fColor");

	// ����VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// ����VBO�����󶨶�������
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	// OpenGL��Ӧ״̬����
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

//////////////////////////////////////////////////////////////////////////
// ��Ⱦ

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	// TODO ����ģ-�ӱ任����ͶӰ����
	// ����ʹ��Ĭ���������ʱ��������ķ�����ͶӰƽ��ƽ�У���ˣ�����һ������ķ����λ��
	vec4 eye(0.5, 2.0, -0.5, 1.0); // ��Դ����y-zƽ��ĶԳƷ���
	vec4 at(0, 0, 0, 1);   // ԭ��
	vec4 up(0, 1, 0, 1);      // Ĭ�Ϸ���

	// ����Ϊ�����ص㲻����ͶӰ�任�����Խ�ͶӰ��������Ϊ����ͶӰ���ɣ�
	 Camera::modelMatrix = mat4(1.0);
	 Camera::viewMatrix = LookAt(eye, at, up);
	 Camera::projMatrix = Camera::ortho(l, r, b, t, n, f);

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

	// TODO ��������ͶӰ�����»���ԭʼ�������Σ��ú�ɫ��ʾ��
	Camera::modelMatrix = RotateY(rotationAngle);
	glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, &Camera::viewMatrix[0][0]);
	glUniformMatrix4fv(projMatrixID, 1, GL_TRUE, &Camera::projMatrix[0][0]);
	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &Camera::modelMatrix[0][0]);
	glUniform4fv(fColorID, 1, red);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// TODO ������ӰͶӰ���󣬻���ͶӰ֮��������Σ��ú�ɫ��ʾ��
	float lx = lightPos[0];
	float ly = lightPos[1];
	float lz = lightPos[2];
	 mat4 shadowProjMatrix = mat4(-ly, 0, 0, 0,
		 lx, 0, lz, 1,
		 0, 0, -ly, 0,
		 0, 0, 0, -ly);

	// ���Ķ�������ɫ�������ݣ�����ʵ��1����ͬ
	 //shadowProjMatrix = mat4(1);
	 Camera::modelMatrix = shadowProjMatrix*RotateY(rotationAngle);
	 glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &Camera::modelMatrix[0][0]);



	glUniform4fv(fColorID, 1, black);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(vPositionID);
	glUseProgram(0);

	glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////
// �������ô���

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

//////////////////////////////////////////////////////////////////////////
// �����Ӧ����

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		lightPos[0] = x/500.0- 0.5;
		lightPos[2] = y/500.0- 0.5;
		glutPostRedisplay();
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
// ������Ӧ����

void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
	case 033:
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit (EXIT_SUCCESS);
		break;
	case 'y':		// 'y' ��ʹ����������ת-1.0��
		rotationAngle -= 1.0;
		break;
	case 'Y':		// 'Y' ��ʹ����������ת+1.0��
		rotationAngle += 1.0;
		break;
	}
	glutPostRedisplay();
}

void printHelp()
{
	printf("%s\n\n", "Camera");
	printf("Keyboard options:\n");
	printf("y: Decrease rotation angle (aroud Y axis)\n");
	printf("Y: Increase rotation angle (aroud Y axis)\n");
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
}

//////////////////////////////////////////////////////////////////////////

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

	printHelp();
	glutMainLoop();

	clean();

	return 0;
}