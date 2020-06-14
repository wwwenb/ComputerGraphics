/*
*        Computer Graphics Course - Shenzhen University
*    Week 8 - Phong Reflectance Model (per-vertex shading)
* ============================================================
*
* - ����������ǲο����룬����Ҫ����ο���ҵ˵��������˳������ɡ�
* - ��������OpenGL�����������������У���ο���һ��ʵ��γ�����ĵ���
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
GLuint vertexNormalID;
GLuint vertexIndexBuffer;

GLuint vPositionID;
GLuint vNormalID;
GLuint modelViewMatrixID;
GLuint modelViewProjMatrixID;

GLuint modelMatrixID;
GLuint viewMatrixID;
GLuint projMatrixID;
GLuint fColorID;

GLuint lightPosID;
GLuint distToLightID;

TriMesh* mesh = new TriMesh();
vec3 lightPos(-0.5, 2.0, 0.5);
float rotationAngle = 0;

GLfloat radius = 1;
GLfloat thera = 3.14 / 2 / 2;
GLfloat phi = 3.01 / 2 / 2;

vec4 black(0.0, 0.0, 0.0, 1.0);
vec4 red(1.0, 0.0, 0.0, 1.0);
vec4 white(1.0, 1.0, 1.0, 1.0);  // White

const GLfloat dr = 4 * DegreesToRadians;

GLdouble distTL = 0.1;		//��Դ������ľ���

//////////////////////////////////////////////////////////////////////////
// ����������ã���Ϊ���ܲ��漰����۲�任��ͶӰ�任����˿���ʹ�ù̶�����ӽǺ�ͶӰ�任��
// ��ʹ��Ĭ�����ã�ע��Ĭ�����ò�ͬ�ڵ�λ����


namespace Camera
{
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projMatrix;

	mat4 ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar)
	{
		// TODO �밴��ʵ������ݲ�ȫ����۲����ļ���
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

	mat4 perspective(const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar)
	{
		// TODO �밴��ʵ������ݲ�ȫ����۲����ļ���
		GLfloat top = tan(fovy * M_PI / 180 / 2) * zNear;
		GLfloat right = top * aspect;

		mat4 c;
		c[0][0] = zNear / right;
		c[1][1] = zNear / top;
		c[2][2] = -(zFar + zNear) / (zFar - zNear);
		c[2][3] = -(2.0 * zFar * zNear) / (zFar - zNear);
		c[3][2] = -1.0;
		c[3][3] = 0.0;
		return c;
	}

	mat4 lookAt(const vec4& eye, const vec4& at, const vec4& up)
	{
		// TODO �밴��ʵ������ݲ�ȫ����۲����ļ���
		vec4 n = normalize(eye - at);
		vec4 u = normalize(vec4(cross(up, n), 0.0));
		vec4 v = normalize(vec4(cross(n, u), 0.0));

		vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
		mat4 c = mat4(u, v, n, t);
		return c * Translate(-eye);
	}
}

//////////////////////////////////////////////////////////////////////////
// OpenGL ��ʼ��

void init()
{
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	programID = InitShader("vshader_vert.glsl", "fshader_vert.glsl");

	// �Ӷ�����ɫ����ƬԪ��ɫ���л�ȡ������λ��
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

	// TODO ��ȡ�ⲿ��άģ��
	mesh->read_off("sphere.off");

	vector<vec3f> vs = mesh->v();
	vector<vec3i> fs = mesh->f();
	vector<vec3f> ns;

	// TODO ������ģ����ÿ������ķ����������洢��ns������
	for (int i = 0; i < vs.size(); ++i) {
		vs[i] = vs[i] + vec3(0, 0.5, 0);
		ns.push_back(vs[i] - vec3(0, 0, 0));
	}


	// ����VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// ����VBO�����󶨶�������
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(vec3f), vs.data(), GL_STATIC_DRAW);

	// ����VBO�����󶨷���������
	glGenBuffers(1, &vertexNormalID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalID);
	glBufferData(GL_ARRAY_BUFFER, ns.size() * sizeof(vec3f), ns.data(), GL_STATIC_DRAW);

	// ����VBO�����󶨶�������
	glGenBuffers(1, &vertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fs.size() * sizeof(vec3i), fs.data(), GL_STATIC_DRAW);

	// OpenGL��Ӧ״̬����
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

//////////////////////////////////////////////////////////////////////////
// ��Ⱦ

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

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

	glDrawElements(
		GL_TRIANGLES,
		int(mesh->f().size() * 3),
		GL_UNSIGNED_INT,
		(void*)0
	);


	// TODO ��������۲�����ͶӰ���󣬲����붥����ɫ��
	vec4 eye = vec4(radius * sin(thera) * cos(phi),
		radius * sin(thera) * sin(phi),
		radius * cos(thera),
		1.0);
	vec4 at = vec4(0, 0, 0, 1);
	vec4 up = vec4(0, 1, 0, 0);

	Camera::modelMatrix = RotateY(rotationAngle);
	Camera::viewMatrix = LookAt(eye, at, up);
	Camera::projMatrix = Camera::ortho(-1, 1, -1, 1, -3, 3);

	// TODO ����Դλ�ô��붥����ɫ��
	glUniform3fv(lightPosID, 1, &lightPos[0]);

	//����Դ������ľ��봫�붥����ɫ��
	glUniform1f(distToLightID, distTL);

	//��ת��Դ
	vec4 temp = Camera::modelMatrix * vec4(lightPos, 1);
	lightPos[0] = temp.x;
	lightPos[1] = temp.y;
	lightPos[2] = temp.z;
	rotationAngle = 0;

	float lx = lightPos[0];
	float ly = lightPos[1];
	float lz = lightPos[2];
	mat4 shadowMatrix = mat4(-ly, 0, 0, 0,
		lx, 0, lz, 1,
		0, 0, -ly, 0,
		0, 0, 0, -ly);

	// ���Ķ�������ɫ�������ݣ�����ʵ��1����ͬ
	mat4 shadowProMatrix = shadowMatrix * Camera::modelMatrix;
	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &shadowProMatrix[0][0]);

	glUniform4fv(fColorID, 1, black);
	glDrawElements(
		GL_TRIANGLES,
		int(mesh->f().size() * 3),
		GL_UNSIGNED_INT,
		(void*)0
	);

	mat4 modelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;
	mat4 modelViewProjMatrix = Camera::projMatrix * modelViewMatrix;

	glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, &Camera::viewMatrix[0][0]);
	glUniformMatrix4fv(projMatrixID, 1, GL_TRUE, &Camera::projMatrix[0][0]);
	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &Camera::modelMatrix[0][0]);

	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, &modelViewMatrix[0][0]);
	glUniformMatrix4fv(modelViewProjMatrixID, 1, GL_TRUE, &modelViewProjMatrix[0][0]);
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
// �������ô���

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

//////////////////////////////////////////////////////////////////////////
// �����Ӧ����

void mouse(int button, int state, int x, int y)
{
	// TODO �������ƹ�Դ��λ��lightPos����ʵʱ���¹���Ч��
	lightPos[0] = (x / 500.0 - 0.5);
	lightPos[1] = 2;
	lightPos[2] = (y / 500.0 - 0.5);
	glutPostRedisplay();
	return;
}

//////////////////////////////////////////////////////////////////////////
// ������Ӧ����

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 033:	// ESC�� �� 'q' ���˳���Ϸ
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		exit(EXIT_SUCCESS);
	case 'x':distTL *= 2; break;
	case 'X':distTL /= 2; break;
	case 'y':		// 'y' ��ʹ����������ת-1.0��
		rotationAngle -= 5.0;
		break;
	case 'Y':		// 'Y' ��ʹ����������ת+1.0��
		rotationAngle += 5.0;
		break;
	case 'r':
		if (radius < 2)
			radius *= 1.2;		//�ı�۲���rֵ
		break;
	case 'R': radius *= 0.5;
		break;
	case 'o': thera += dr;		//�ı�۲���theraֵ
		break;
	case 'O':thera -= dr;
		break;
	case 'p': phi += dr;	//�ı�۲���phiֵ
		break;
	case 'P': phi -= dr;
		break;
	case 't':				//����r��thera��phi��ֵ
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

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Designed by 2017153009");

	glewInit();
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	//glutMotionFunc(mouse);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();

	clean();

	return 0;
}