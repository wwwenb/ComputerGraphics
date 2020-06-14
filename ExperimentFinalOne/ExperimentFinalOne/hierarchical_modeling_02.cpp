
#include "Angel.h"
#include <assert.h>
#include<iostream>
#include "designHead.h"
#include "FreeImage.h"

using namespace std;
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "FreeImage.lib")

void drawLight();
//----------------------------------------------------------------------------

int Index = 0;

void
quad( int a, int b, int c, int d )
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; ns[Index] = (vertices[a]- vec4(0, 0, 0, 1)); Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; ns[Index] = (vertices[b] - vec4(0, 0, 0, 1)); Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; ns[Index] = (vertices[c] - vec4(0, 0, 0, 1)); Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; ns[Index] = (vertices[a] - vec4(0, 0, 0, 1)); Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; ns[Index] = (vertices[c] - vec4(0, 0, 0, 1)); Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; ns[Index] = (vertices[d] - vec4(0, 0, 0, 1)); Index++;
}

void
colorcube( void )//这里生成单位立方体的六个表面
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}


void load_texture_FreeImage(string file_name, GLuint& m_texName)
{
	//1 获取图片格式
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(file_name.c_str(), 0);

	//2 加载图片
	FIBITMAP* dib = FreeImage_Load(fifmt, file_name.c_str(), 0);

	//3 转化为rgb 24色;
	dib = FreeImage_ConvertTo24Bits(dib);

	//4 获取数据指针
	BYTE* pixels = (BYTE*)FreeImage_GetBits(dib);

	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);

	/**
	* 产生一个纹理Id,可以认为是纹理句柄，后面的操作将书用这个纹理id
	*/

	/**
	* 使用这个纹理id,或者叫绑定(关联)
	*/
	glBindTexture(GL_TEXTURE_2D, m_texName);
	/**
	* 指定纹理的放大,缩小滤波，使用线性方式，即当图片放大的时候插值方式
	*/
	/**
	* 将图片的rgb数据上传给opengl.
	*/
	glTexImage2D(
		GL_TEXTURE_2D, //! 指定是二维图片
		0, //! 指定为第一级别，纹理可以做mipmap,即lod,离近的就采用级别大的，远则使用较小的纹理
		GL_RGB, //! 纹理的使用的存储格式
		width, //! 宽度，老一点的显卡，不支持不规则的纹理，即宽度和高度不是2^n。
		height, //! 宽度，老一点的显卡，不支持不规则的纹理，即宽度和高度不是2^n。
		0, //! 是否的边
		GL_BGR_EXT, //! 数据的格式，bmp中，windows,操作系统中存储的数据是bgr格式
		GL_UNSIGNED_BYTE, //! 数据是8bit数据
		pixels
	);
	/**
	* 释放内存
	*/
	FreeImage_Unload(dib);
}

void update_texture(string file_name)
{

		GLuint textures;

		glGenTextures(1, &textures);
		//调用FreeImage生成纹理
		load_texture_FreeImage(file_name, textures);

		//指定最大最小过滤方法，此两行代码必须添加，否则将无法显示纹理
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//将生成的纹理传给shader
		glBindTexture(GL_TEXTURE_2D, textures);
		glUniform1i(glGetUniformLocation(program, "texture"), 0);

};
//----------------------------------------------------------------------------

void
left_upper_arm()
{
	mvstack.push(model_view);
	mat4 instance = (Translate(0, 0.5*UPPER_ARM_HEIGHT, 0)*
		Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));
	model_view = model_view * instance;
	drawLight();

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glUniform4fv(draw_color, 1, vertex_colors[1]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	model_view = mvstack.pop();
}

void
left_lower_arm()
{
	mvstack.push(model_view);
	mat4 instance = (Translate(0, 0.5 * LOWER_ARM_HEIGHT, 0) *
		Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));
	model_view = model_view * instance;
	drawLight();

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glUniform4fv(draw_color, 1, vertex_colors[2]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	model_view = mvstack.pop();
}

void
right_upper_arm()
{
	mvstack.push(model_view);
	mat4 instance = (Translate(0, 0.5 * UPPER_ARM_HEIGHT, 0) *
		Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));
	model_view = model_view * instance;
	drawLight();

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glUniform4fv(draw_color, 1, vertex_colors[1]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	model_view = mvstack.pop();
}

void
right_lower_arm()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0, 0.5 * LOWER_ARM_HEIGHT, 0) *
		Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));
	model_view = model_view * instance;
	drawLight();

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glUniform4fv(draw_color, 1, vertex_colors[2]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	model_view = mvstack.pop();
}

void
left_upper_leg()
{
	mvstack.push(model_view);
	mat4 instance = (Translate(0, 0.5* UPPER_LEG_HEIGHT, 0) *
		Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));
	model_view = model_view * instance;
	drawLight();

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view );
	glUniform4fv(draw_color, 1, vertex_colors[3]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	model_view = mvstack.pop();
}

void
left_lower_leg()
{
	mvstack.push(model_view);
	mat4 instance = (Translate(0, 0.5 * LOWER_LEG_HEIGHT, 0) *
		Scale(LOWER_LEG_WIDTH,LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));
	model_view = model_view * instance;
	drawLight();

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glUniform4fv(draw_color, 1, vertex_colors[1]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	model_view = mvstack.pop();
}

void
right_upper_leg()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0, 0.5 * UPPER_LEG_HEIGHT, 0) *
		Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));
	model_view = model_view * instance;
	drawLight();

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glUniform4fv(draw_color, 1, vertex_colors[3]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	model_view = mvstack.pop();
}

void
right_lower_leg()
{
	mvstack.push(model_view);

	mat4 instance = (Translate(0, 0.5 * LOWER_LEG_HEIGHT, 0) *
		Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));


	model_view = model_view * instance;
	//绘制光照和阴影
	drawLight();

	glUniformMatrix4fv(ModelView, 1, GL_TRUE,  model_view);
	glUniform4fv(draw_color, 1, vertex_colors[1]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	model_view = mvstack.pop();
}

void
head()
{
	mvstack.push(model_view);
	mat4 instance = (Translate(0, 0, 0.5* HEAD_LONG- 0.5*NECK_WIDTH) *
		Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_LONG));

	model_view = model_view * instance;
	drawLight();

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glUniform4fv(draw_color, 1, vertex_colors[3]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	model_view = mvstack.pop();
}


void
torso()
{
	mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, 0.5*TORSO_HEIGHT+ Body_Offset, 0.0) *
		Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_LONG));//本节点局部变换矩阵

	model_view = model_view * instance;
	drawLight();

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);//父节点矩阵*本节点局部变换矩阵
	glUniform4fv(draw_color, 1, color_torso);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	model_view = mvstack.pop();//恢复父节点矩阵
}

void torso1() {
	mvstack.push(model_view);//保存父节点矩阵

	mat4 instance = (Translate(0.0, 0.5 * TORSO_HEIGHT + Body_Offset, 0.0) *
		Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_LONG));//本节点局部变换矩阵

	model_view = model_view * instance;
	drawLight();
	
	My_Mesh* Torso = new My_Mesh;
	Torso->generate_cube();
	Torso->ModelView = model_view;
	Torso->viewMatrix = view;
	//my_meshs.push_back(Torso);
	mp_->add_mesh(Torso);

	model_view = mvstack.pop();//恢复父节点矩阵
}

void neck() {
	mvstack.push(model_view);
	mat4 instance = (Translate(0, 0.5 * NECK_HEIGHT, 0) *
		Scale(NECK_WIDTH, NECK_HEIGHT, NECK_WIDTH));

	model_view = model_view * instance;
	drawLight();

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glUniform4fv(draw_color, 1, vertex_colors[3]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	model_view = mvstack.pop();
}

void left_ear() {
	mvstack.push(model_view);
	mat4 instance = (Translate(0, 0.5* EAR_HEIGTH, 0) *
		Scale(EAR_WIDTH, EAR_HEIGTH, EAR_LONG));

	model_view = model_view * instance;
	drawLight();

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glUniform4fv(draw_color, 1, vertex_colors[1]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	model_view = mvstack.pop();
}

void right_ear() {
	mvstack.push(model_view);
	mat4 instance = (Translate(0, 0.5 * EAR_HEIGTH, 0) *
		Scale(EAR_WIDTH, EAR_HEIGTH, EAR_LONG));

	model_view = model_view * instance;
	drawLight();
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glUniform4fv(draw_color, 1, vertex_colors[1]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	model_view = mvstack.pop();
}
//----------------------------------------------------------------------------
void drawLight() {

	//计算投影矩阵
	float lx = lightPos[0];
	float ly = lightPos[1];
	float lz = lightPos[2];
	mat4 shadowMatrix = mat4(-ly, 0, 0, 0,
		lx, 0, lz, 1,
		0, 0, -ly, 0,
		0, 0, 0, -ly);

	// 请阅读顶点着色器的内容，对照实验1的异同
	mat4 shadowPMatrix = shadowMatrix ;
	glUniformMatrix4fv(ModelView, 1, GL_TRUE,  shadowPMatrix* model_view);
	glUniformMatrix4fv(viewMatrix, 1, GL_TRUE, view);
	mat4 projection = Ortho(lef, rig, bottom, top, zNear, zFar);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
	// 将光源位置传入顶点着色器
	glUniform3fv(lightPosID, 1, lightPos);
	//将距离传入顶点着色器
	glUniform1f(distToLightID, distTL);

	glUniform4fv(draw_color, 1, vertex_colors[0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

void initEye() {
	vec4 eye = vec4(radius * sin(test) * cos(phi),
		radius * sin(thera) * sin(phi),
		radius * cos(thera),
		1.0);
	vec4 at = vec4(0, 0, 0, 1);
	vec4 up = vec4(0, 1, 0, 0);

	view = LookAt(eye, at, up);
}

void initDisplay() {
	initEye();

}

void
display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	initEye();
	model_view = RotateY(theta[Torso]);//躯干变换矩阵
	torso1();


	mp_->init_shaders("vshader82.glsl", "fshader82.glsl");
	mp_->update_vertex_buffer();
	mp_->update_texture();
	mp_->draw_meshes();
	//torso();//躯干绘制

	//mvstack.push(model_view);
	//model_view *= (Translate(0, TORSO_HEIGHT + Body_Offset, 0.5* TORSO_LONG) *
	//	RotateX(theta[Neck]));
	//neck();
	//model_view *= (Translate(0, NECK_HEIGHT+ 0.5 * HEAD_HEIGHT, 0) *
	//	RotateX(theta[Head]));
	//head();

	//mvstack.push(model_view);
	//model_view *= (Translate(-0.5 * HEAD_WIDTH, 0.5 * HEAD_HEIGHT, 0) *
	//	RotateZ(theta[RightEar]));
	//right_ear();
	//model_view = mvstack.pop();

	//mvstack.push(model_view);
	//model_view *= (Translate(0.5* HEAD_WIDTH, 0.5*HEAD_HEIGHT, 0) *
	//	RotateZ(theta[LeftEar]));
	//left_ear();
	//model_view = mvstack.pop();

	//model_view = mvstack.pop();

	//mvstack.push(model_view);
	//model_view *= (Translate(0.5 * TORSO_WIDTH, Body_Offset, 0.5 * TORSO_LONG) *
	//	RotateX(theta[LeftUpperArm]));
	//left_upper_arm();
	//model_view *= (Translate(0, UPPER_ARM_HEIGHT, 0) *
	//	RotateX(theta[LeftLowerArm]));
	//left_lower_arm();
	//model_view = mvstack.pop();

	//mvstack.push(model_view);
	//model_view *= (Translate(-0.5 * TORSO_WIDTH, Body_Offset, 0.5 * TORSO_LONG) *
	//	RotateX(theta[RightUpperArm]));
	//right_upper_arm();
	//model_view *= (Translate(0, UPPER_ARM_HEIGHT, 0) *
	//	RotateX(theta[RightLowerArm]));
	//right_lower_arm();
	//model_view = mvstack.pop();

	//mvstack.push(model_view);
	//model_view *= (Translate(0.5 * TORSO_WIDTH, Body_Offset, -0.5* TORSO_LONG) *
	//	RotateX(theta[LeftUpperLeg]));
	//left_upper_leg();
	//model_view *= (Translate(0, UPPER_LEG_HEIGHT, 0) *
	//	RotateX(theta[LeftLowerLeg]));
	//left_lower_leg();
	//model_view = mvstack.pop();

	//mvstack.push(model_view);
	//model_view *= (Translate(-0.5 * TORSO_WIDTH, Body_Offset, -0.5 * TORSO_LONG) *
	//	RotateX(theta[RightUpperLeg]));
	//right_upper_leg();
	//model_view *= (Translate(0, UPPER_LEG_HEIGHT, 0) *
	//	RotateX(theta[RightLowerLeg]));
	//right_lower_leg();
	//
	//model_view = mvstack.pop();

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
mouse( int button, int state, int x, int y )
{
    if ( button == GLUT_LEFT_BUTTON ) {
        theta[angle] += 5.0;
        if ( theta[angle] > 360.0 ) { theta[angle] -= 360.0; }
    }

    if ( button == GLUT_RIGHT_BUTTON ) {
        theta[angle] -= 5.0;
        if ( theta[angle] < 0.0 ) { theta[angle] += 360.0; }
    }

	// TODO 用鼠标控制光源的位置，并实时更新光照效果
	//lightPos[0] = (x / 500.0 - 0.5);
	//lightPos[1] = 3;
	//lightPos[2] = (y / 500.0 - 0.5);

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
menu( int option )
{
    if ( option == Quit ) {
	exit( EXIT_SUCCESS );
    }

    angle = option;
}

//----------------------------------------------------------------------------

void
reshape( int width, int height )
{
    glViewport( 0, 0, width, height );

    GLfloat aspect = GLfloat( width ) / height;

    if ( aspect > 1.0 ) {
        lef *= aspect;
        rig *= aspect;
    }
    else {
        bottom /= aspect;
        top /= aspect;
    }

    mat4 projection = Ortho( lef, rig, bottom, top, zNear, zFar );
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );

    model_view = mat4( 1.0 );   // An Identity matrix
}

//----------------------------------------------------------------------------

void
init( void )
{
    colorcube();
	imgs_name.push_back("texture/background.jpg");
	mp_ = new Mesh_Painter;

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors)+ sizeof(ns),
                  NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors),
                     colors );
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(ns), ns);

    // Load shaders and use the resulting shader program
    program = InitShader( "vshader82.glsl", "fshader82.glsl" );
    glUseProgram( program );

    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(points)) );

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
				BUFFER_OFFSET(sizeof(points)+ sizeof(vColor)));

    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
	draw_color = glGetUniformLocation(program, "draw_color");

	modelMatrix = glGetUniformLocation(program, "modelMatrix");
	viewMatrix = glGetUniformLocation(program, "viewMatrix");
	shadowProMatrix = glGetUniformLocation(program, "shadowMatrix");

	lightPosID = glGetUniformLocation(program, "lightPos");
	distToLightID = glGetUniformLocation(program, "distToLight");
	is_background = glGetUniformLocation(program, "is_background");

    glEnable( GL_DEPTH_TEST );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glClearColor( 1.0, 1.0, 1.0, 1.0 );

}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case '1':  theta[angle] += 5.0;
		if (theta[angle] > 360.0) { theta[angle] -= 360.0; }break;
	case 'w':cout << theta[angle] << endl; break;

	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
    }
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 500, 500 );
    glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutCreateWindow( "Designed by 2017153009" );

	glewExperimental = GL_TRUE;
    glewInit();

    init();

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutMouseFunc( mouse );

    glutCreateMenu( menu );
    glutAddMenuEntry( "torso", Torso );
    glutAddMenuEntry( "neck", Neck );
    glutAddMenuEntry( "head", Head );
    glutAddMenuEntry( "right_upper_arm", RightUpperArm );
    glutAddMenuEntry( "right_lower_arm", RightLowerArm );
    glutAddMenuEntry( "left_upper_arm", LeftUpperArm );
    glutAddMenuEntry( "left_lower_arm", LeftLowerArm );
    glutAddMenuEntry( "right_upper_leg", RightUpperLeg );
    glutAddMenuEntry( "right_lower_leg", RightLowerLeg );
    glutAddMenuEntry( "left_upper_leg", LeftUpperLeg );
    glutAddMenuEntry( "left_lower_leg", LeftLowerLeg );
	glutAddMenuEntry("left_ear", LeftEar);
	glutAddMenuEntry("right_ear", RightEar);
    glutAddMenuEntry( "quit", Quit );
    glutAttachMenu( GLUT_MIDDLE_BUTTON );

    glutMainLoop();
    return 0;
}
