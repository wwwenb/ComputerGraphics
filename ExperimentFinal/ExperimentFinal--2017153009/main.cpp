/*
*        Computer Graphics Course - Shenzhen University
*    Week 9 - Phong Reflectance Model (per-fragment shading)
* ============================================================
*
* - ����������ǲο����룬����Ҫ����ο���ҵ˵��������˳������ɡ�
* - ��������OpenGL�����������������У���ο���һ��ʵ��γ�����ĵ���
*/

#include "include/Angel.h"
#include "include/TriMesh.h"
#include "designHead.h"

#include <cstdlib>
#include <iostream>

#pragma comment(lib, "glew32.lib")

using namespace std;

GLfloat test;

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

vector<vec3f> vs;
vector<vec3i> fs;
vector<vec3f> ns;
vector<vec3f> points;

MatrixStack mvstack;
mat4 modelView;


const GLfloat dr = 4 * DegreesToRadians;

TriMesh* mesh = new TriMesh();
vec3 lightPos(-0.5, 3.0, 0.5);
float rotationAngle = 0;
float objectUp = 0.5;

///////////////////////////////////////////////////////////////////////////
//��������
void print();

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

	mat4 perspective( const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar)
	{
		// TODO �밴��ʵ������ݲ�ȫ����۲����ļ���
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
		// TODO �밴��ʵ������ݲ�ȫ����۲����ļ���
		vec4 n = normalize(eye - at);
		vec4 u = normalize(vec4(cross(up, n), 0.0));
		vec4 v = normalize(vec4(cross(n, u), 0.0));

		vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
		mat4 c = mat4(u, v, n, t);
		return c * Translate(-eye);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
quad(int a, int b, int c, int d)
{
	vs.push_back(points[a]); vs.push_back(points[b]); vs.push_back(points[c]);
	vs.push_back(points[a]); vs.push_back(points[c]); vs.push_back(points[d]);
}
//////////////////////////////////////////////////////////////////////////
//void 
//initCube() {
//	//������
//	points.push_back(vec3f(-0.5, -0.5, 0.5));
//	points.push_back(vec3f(-0.5, 0.5, 0.5));
//	points.push_back(vec3f(0.5, 0.5, 0.5));
//	points.push_back(vec3f(0.5, -0.5, 0.5));
//	points.push_back(vec3f(-0.5, -0.5, -0.5));
//	points.push_back(vec3f(-0.5, 0.5, -0.5));
//	points.push_back(vec3f(0.5, 0.5, -0.5));
//	points.push_back(vec3f(0.5, -0.5, -0.5)); 
//
//	quad(1, 0, 3, 2); 
//	quad(2, 3, 7, 6); 
//	quad(3, 0, 4, 7); 
//	quad(6, 5, 1, 2); 
//	quad(4, 5, 6, 7); 
//	quad(5, 4, 0, 1);
//
//	for (int i = 0; i < vs.size(); i++) {
//		ns.push_back(vs[i] - vec3(0, 0, 0));
//	}
//
//}

void
initCube() {
	//������
	vs.push_back(vec3f(-0.5, -0.5, 0.5));
	vs.push_back(vec3f(-0.5, 0.5, 0.5));
	vs.push_back(vec3f(0.5, 0.5, 0.5));
	vs.push_back(vec3f(0.5, -0.5, 0.5));
	vs.push_back(vec3f(-0.5, -0.5, -0.5));
	vs.push_back(vec3f(-0.5, 0.5, -0.5));
	vs.push_back(vec3f(0.5, 0.5, -0.5));
	vs.push_back(vec3f(0.5, -0.5, -0.5));

	fs.push_back(vec3i(1, 0, 3)); fs.push_back(vec3i(1, 3, 2));
	fs.push_back(vec3i(2, 3, 7)); fs.push_back(vec3i(2, 7, 6));
	fs.push_back(vec3i(3, 0, 4)); fs.push_back(vec3i(3, 4, 7));
	fs.push_back(vec3i(6, 5, 1)); fs.push_back(vec3i(6, 1, 2));
	fs.push_back(vec3i(4, 5, 6)); fs.push_back(vec3i(4, 6, 7));
	fs.push_back(vec3i(5, 4, 0)); fs.push_back(vec3i(5, 0, 1));

	for (int i = 0; i < vs.size(); i++) {
		ns.push_back(vs[i] - vec3(0, 0, 0));
	}

}

//////////////////////////////////////////////////////////////////////////
// OpenGL ��ʼ��

void init()
{
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);		//�����ڱ�������Ϊ��ɫ

	programID = InitShader("vshader_frag.glsl", "fshader_frag.glsl");

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

	// ��ȡ�ⲿ��άģ��
	//mesh->read_off("sphere.off");

	initCube();

	// TODO ������ģ����ÿ������ķ����������洢��ns������
	//for (int i = 0; i < vs.size(); ++i) {
	//	vs[i] = vs[i] + vec3(0, objectUp, 0);	//��������y��ķ���������0.5
	//	ns.push_back(vs[i] - vec3(0, objectUp, 0));	//�����ķ�����
	//}

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
void initDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	// TODO ��Lookat������Ҫ��eye��at��up��ֵ

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


}

void drawLight() {
	vec4 eye = vec4(radius * sin(thera) * cos(phi),
		radius * sin(thera) * sin(phi),
		radius * cos(thera),
		1.0);
	vec4 at = vec4(0, 0, 0, 1);
	vec4 up = vec4(0, 1, 0, 0);

	// ����Դλ�ô��붥����ɫ��
	glUniform3fv(lightPosID, 1, &lightPos[0]);
	//�����봫�붥����ɫ��
	glUniform1f(distToLightID, distTL);

	//��ת��Դ
	vec4 temp = vec4(lightPos, 1);
	lightPos[0] = temp.x;
	lightPos[1] = temp.y;
	lightPos[2] = temp.z;
	rotationAngle = 0;

	//����ͶӰ����
	float lx = lightPos[0];
	float ly = lightPos[1];
	float lz = lightPos[2];
	mat4 shadowMatrix = mat4(-ly, 0, 0, 0,
		lx, 0, lz, 1,
		0, 0, -ly, 0,
		0, 0, 0, -ly);


	Camera::modelMatrix = mat4(1);
	Camera::viewMatrix = LookAt(eye, at, up);
	Camera::projMatrix = Camera::ortho(lef, rig, bot, top, zNear, zFar);

	// ���Ķ�������ɫ�������ݣ�����ʵ��1����ͬ
	mat4 shadowProMatrix = shadowMatrix * Camera::modelMatrix;
	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &shadowProMatrix[0][0]);

	//������Ӱ��Ϊ��ɫ
	//glUniform4fv(fColorID, 1, black);
	//glDrawElements(
	//    GL_TRIANGLES,
	//    int(fs.size() * 3),
	//    GL_UNSIGNED_INT,
	//    (void*)0
	//);

	// TODO ��������۲�����ͶӰ���󣬲����붥����ɫ��
	mat4 modelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;
	mat4 modelViewProjMatrix = Camera::projMatrix * modelViewMatrix;

	glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, &Camera::viewMatrix[0][0]);
	glUniformMatrix4fv(projMatrixID, 1, GL_TRUE, &Camera::projMatrix[0][0]);
	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &Camera::modelMatrix[0][0]);
	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, &modelViewMatrix[0][0]);
	glUniformMatrix4fv(modelViewProjMatrixID, 1, GL_TRUE, &modelViewProjMatrix[0][0]);
}

void 
body() {
	mvstack.push(Camera::modelMatrix);
	mat4 instance = (Translate(0, 0.5*Body_Height, 0)*
		Scale(Body_Width, Body_Height, Body_Long));

	Camera::modelMatrix = Camera::modelMatrix * instance;
	mat4 modelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;
	mat4 modelViewProjMatrix = Camera::projMatrix * modelViewMatrix;
	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &Camera::modelMatrix[0][0]);
	glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, &modelViewMatrix[0][0]);
	//����������Ϊ��ɫ
	glUniform4fv(fColorID, 1, white);
	glDrawElements(GL_TRIANGLES, int(fs.size() * 3), GL_UNSIGNED_INT, (void*)0);

	Camera::modelMatrix = mvstack.pop();
}

void display()
{
	 initDisplay();
	 drawLight();
	 
	 Camera::modelMatrix = RotateY(test);
	 body();

	 //Camera::modelMatrix = Translate(0, 0, 0) * RotateY(test) * Scale(1, 2, 1);
	 //modelViewMatrix = Camera::viewMatrix * Camera::modelMatrix;
	 //modelViewProjMatrix = Camera::projMatrix * modelViewMatrix;
	 //glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &Camera::modelMatrix[0][0]);
	 //glUniformMatrix4fv(modelViewMatrixID, 1, GL_TRUE, &modelViewMatrix[0][0]);
	 ////����������Ϊ��ɫ
	 //glUniform4fv(fColorID, 1, white);
	 //glDrawElements(GL_TRIANGLES, int(fs.size() * 3), GL_UNSIGNED_INT, (void*)0);

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
	// TODO �������ƹ�Դ��λ�ã���ʵʱ���¹���Ч��
	lightPos[0] = (x / 500.0 - 0.5);
	lightPos[1] = 3;
	lightPos[2] = (y / 500.0 - 0.5);
	glutPostRedisplay();
	return;
}

/////////////////////////////////////////////////////////////////////////
//��ӡ������ʾ��Ϣ
void print() {
	printf("%s\n\n", "Designed by 2017153009");
	printf("Keyboard options:\n");
	printf("x: �������嵽��Դ�ľ���\n");
	printf("X: �������嵽��Դ�ľ���\n");
	printf("y��Y:��ת��Դ\n");
	printf("v: ����������\n");
	printf("V: ����������\n");
	printf("Mouse options:\n");
	printf("�ı��Դλ��\n");
	printf("t: Reset all transformations and deltas\n");
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
	case 'x':distTL *= 2;break;
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
	case 'v':
		if (objectUp < 1) {
			objectUp += 0.1; init();	//��������
		}
		break;
	case 'V':
		if (objectUp > 0.5) {
			objectUp -= 0.1;		//��������
			init();
		}
		break;
	case 't':				//����r��thera��phi��ֵ
		test+= 5;
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