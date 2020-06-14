/*
*        Computer Graphics Course - Shenzhen University
*  Week 7 - Orthogonal Projection and Perspective Projection
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
GLuint modelViewProjMatrixID;


GLfloat radius = 2;
GLfloat thera = 0;
GLfloat phi = 0;

const GLfloat dr = 4 * DegreesToRadians;

GLfloat zLeft = -1, zRight = 1;
GLfloat bottom = -1, top = 1;
GLfloat zNear = 1, zFar = 100;
int CHOOCE = 1;

TriMesh* mesh = new TriMesh();

namespace Camera
{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projMatrix;

	mat4 ortho( const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar )
	{
		// TODO �밴��ʵ������ݲ�ȫ����۲����ļ���
		return mat4(2 / (right - left), 0, 0, 0,
			0, 2 / (top - bottom), 0, 0,
			0, 0, -2 / (zFar - zNear), 0,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1);
		
	}

	mat4 perspective( const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar)
	{
		// TODO �밴��ʵ������ݲ�ȫ����۲����ļ���
		
		return mat4(zNear / (zNear*tan(fovy)*aspect), 0, 0, 0,
			0, zNear / (zNear*tan(fovy)), 0, 0,
			0, 0, -(zFar + zNear) / (zFar - zNear), -1,
			0, 0, -2 * zFar*zNear / (zFar - zNear), 0);
		return mat4(1);
	}

	mat4 lookAt( const vec4& eye, const vec4& at, const vec4& up )
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// ����shader���һ�ȡ������λ��
	programID = InitShader("vshader.glsl", "fshader.glsl");
	vPositionID = glGetAttribLocation(programID, "vPosition");
	modelViewProjMatrixID = glGetUniformLocation(programID, "modelViewProjMatrix");

	// ���ⲿ��ȡ��άģ���ļ�
	mesh->read_off("cube.off");

	vector<vec3f> vs = mesh->v();
	vector<vec3i> fs = mesh->f();

	// ����VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// ����VBO�����󶨶�������
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(vec3f), vs.data(), GL_STATIC_DRAW);

	// ����VBO�����󶨶�������
	glGenBuffers(1, &vertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fs.size() * sizeof(vec3i), fs.data(), GL_STATIC_DRAW);

	// OpenGL��Ӧ״̬����
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

//////////////////////////////////////////////////////////////////////////
// ��Ⱦ

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	// TODO �����������
	vec4 eye = vec4(radius*sin(thera)*cos(phi),
		radius*sin(thera)*sin(phi),
		radius* cos(thera),
		1.0);
	vec4 at = vec4(0, 0, 0, 1);
	vec4 up = vec4(0, 1, 0, 0);

	 Camera::modelMatrix = mat4(1.0);  //����ʵ����Ϊ��λ��
	 Camera::viewMatrix = LookAt(eye, at, up);   //LookAt

	 switch (CHOOCE)
	 {
		 case 1:
			Camera::projMatrix = Camera::ortho(zLeft, zRight, bottom, top, zNear, zFar);   //ƽ��ͶӰ����
			break;
		 case 2:
			Camera::projMatrix = Camera::perspective(atan(top/zNear), zRight/top, zNear, zFar); //͸��ͶӰ����
			break;
	 }
	 
	

	 mat4 modelViewProjMatrix = Camera::projMatrix*Camera::viewMatrix*Camera::modelMatrix;  //�������ˣ�ע��˳��
	 glUniformMatrix4fv(modelViewProjMatrixID, 1, GL_TRUE, &modelViewProjMatrix[0][0]);

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
// �������ô���

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

//////////////////////////////////////////////////////////////////////////
// �����Ӧ����

void mouse(int button, int state, int x, int y)
{
	return;
}

//////////////////////////////////////////////////////////////////////////
// ������Ӧ����

void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
	case 033:	// ESC�� �� 'q' ���˳���Ϸ
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit (EXIT_SUCCESS);
		break;

	case '1':CHOOCE = 1;
		break;
	case '2':CHOOCE = 2;
		break;
	case 'r':
		if (radius< 2)
			radius *= 1.2;		//�ı�۲���rֵ
		break;
	case 'R': radius *= 0.5;
		break;
	case 'o': thera += dr;		//�ı�۲���theraֵa
		break;
	case 'O':thera -= dr;
		break;
	case 'p': phi += dr;	//�ı�۲���phiֵ
		break;
	case 'P': phi -= dr;
		break;

	case 'x':zLeft *= 1.1; zRight *= 1.1;
		break;
	case 'X':zLeft *= 0.9; zRight *= 0.9;
		break;
	case 'y': bottom *= 1.1; top *= 1.1;
		break;
	case 'Y':bottom *= 0.9; top *= 0.9;
		break;
	case 'z': zNear *= 1.1; zFar *= 1.1;
		break;
	case 'Z':zNear *= 0.9; zFar *= 0.9;
		break;

	case 't':				//����r��thera��phi��ֵ
		radius = 0.1;
		thera = 0;
		phi = 0;
		zLeft = -1;
		zRight = 1;
		zNear = 0.01;
		zFar = 100;
		break;
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