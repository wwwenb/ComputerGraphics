#include "Mesh_Painter.h"
#include "FreeImage.h"
#include "Const_Head.h"

using namespace std;

Mesh_Painter::Mesh_Painter()
{
}


Mesh_Painter::~Mesh_Painter()
{
}

extern mat4 projection;
void Mesh_Painter::drawLight(int i) {
	GLuint is_texture = glGetUniformLocation(this->program_all[i], "is_texture");
	glUniform1i(is_texture, 1);

	//����ͶӰ����
	float lx = lightPos[0];
	float ly = lightPos[1];
	float lz = lightPos[2];
	mat4 shadowMatrix = mat4(-ly, 0, 0, 0,
		lx, 0, lz, 1,
		0, 0, -ly, 0,
		0, 0, 0, -ly);

	// ���Ķ�������ɫ�������ݣ�����ʵ��1����ͬ
	mat4 shadowPMatrix = shadowMatrix;

	GLuint ModelView = glGetUniformLocation(this->program_all[i], "ModelView");
	GLuint Projection = glGetUniformLocation(this->program_all[i], "Projection");
	GLuint draw_color = glGetUniformLocation(this->program_all[i], "draw_color");
	GLuint viewMatrix= glGetUniformLocation(this->program_all[i], "draw_color");

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, shadowPMatrix * this->m_my_meshes_[i]->modelMatrix);
	glUniformMatrix4fv(viewMatrix, 1, GL_TRUE, this->m_my_meshes_[i]->viewMatrix);

	mat4 proMatrix = projection;
	glUniformMatrix4fv(Projection, 1, GL_TRUE, proMatrix);
	// ����Դλ�ô��붥����ɫ��

	GLuint lightPosID = glGetUniformLocation(this->program_all[i], "lightPos");
	glUniform3fv(lightPosID, 1, lightPos);
	//�����봫�붥����ɫ��

	GLuint distToLightID = glGetUniformLocation(this->program_all[i], "distToLight");
	glUniform1f(distToLightID, distTL);

	glUniform4fv(draw_color, 1, BLACK);
	glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->num_faces() * 3);
}


void Mesh_Painter::draw_meshes()
{
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		
		glUseProgram(this->program_all[i]);//ָ��ʹ����Ⱦ������ͬ��ģ�Ϳ���ָ����ͬ����Ⱦ������������ʹ�õ���Ⱦ��������ͬ���ļ���ѧ�����Ը����Լ��İ��öԲ�ͬ��ģ���趨��ͬ����Ⱦ��
		glBindVertexArray(this->vao_all[i]);

		//drawLight(i);

 		glActiveTexture(GL_TEXTURE0);
 		glBindTexture(GL_TEXTURE_2D, this->textures_all[i]);//�������룬����ֻʹ��ͬһ��������л���

		GLuint is_texture = glGetUniformLocation(this->program_all[i], "is_texture");
		glUniform1i(is_texture, 1);

		GLuint viewMatrixID = glGetUniformLocation(this->program_all[i], "viewMatrix");
		glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, this->m_my_meshes_[i]->viewMatrix);

		GLuint modelMatrixID = glGetUniformLocation(this->program_all[i], "modelMatrix");
		glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, this->m_my_meshes_[i]->modelMatrix);

		GLuint proMatrixID = glGetUniformLocation(this->program_all[i], "proMatrix");
		glUniformMatrix4fv(proMatrixID, 1, GL_TRUE, this->m_my_meshes_[i]->proMatrix);

		glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->num_faces() * 3);
		glUseProgram(0);
	}
};


void Mesh_Painter::update_texture()
{
	this->textures_all.clear();

	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		GLuint temp;
		this->textures_all.push_back(temp);

		update_texture(i);

	}


};

void Mesh_Painter::update_texture(int i)
{

	GLuint textures;

	glGenTextures(1, &textures);
	//����FreeImage��������
	load_texture_FreeImage(this->m_my_meshes_[i]->get_texture_file(), textures);

	//ָ�������С���˷����������д��������ӣ������޷���ʾ����
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//�����ɵ�������shader
	glBindTexture(GL_TEXTURE_2D, textures);
	glUniform1i(glGetUniformLocation(this->program_all[i], "texture"), 0);

	this->textures_all[i]= textures;
};

void Mesh_Painter::load_texture_FreeImage(std::string file_name, GLuint& m_texName)
{
	//1 ��ȡͼƬ��ʽ
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(file_name.c_str(), 0);

	//2 ����ͼƬ
	FIBITMAP *dib = FreeImage_Load(fifmt, file_name.c_str(), 0);

	//3 ת��Ϊrgb 24ɫ;
	dib = FreeImage_ConvertTo24Bits(dib);

	//4 ��ȡ����ָ��
	BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);

	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);

	/**
	* ����һ������Id,������Ϊ��������������Ĳ����������������id
	*/

	/**
	* ʹ���������id,���߽а�(����)
	*/
	glBindTexture(GL_TEXTURE_2D, m_texName);
	/**
	* ָ������ķŴ�,��С�˲���ʹ�����Է�ʽ������ͼƬ�Ŵ��ʱ���ֵ��ʽ
	*/
	/**
	* ��ͼƬ��rgb�����ϴ���opengl.
	*/
	glTexImage2D(
		GL_TEXTURE_2D, //! ָ���Ƕ�άͼƬ
		0, //! ָ��Ϊ��һ�������������mipmap,��lod,����ľͲ��ü����ģ�Զ��ʹ�ý�С������
		GL_RGB, //! �����ʹ�õĴ洢��ʽ
		width, //! ��ȣ���һ����Կ�����֧�ֲ��������������Ⱥ͸߶Ȳ���2^n��
		height, //! ��ȣ���һ����Կ�����֧�ֲ��������������Ⱥ͸߶Ȳ���2^n��
		0, //! �Ƿ�ı�
		GL_BGR_EXT, //! ���ݵĸ�ʽ��bmp�У�windows,����ϵͳ�д洢��������bgr��ʽ
		GL_UNSIGNED_BYTE, //! ������8bit����
		pixels
		);
	/**
	* �ͷ��ڴ�
	*/
	FreeImage_Unload(dib);
};

void Mesh_Painter::update_vertex_buffer()
{

	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();

	for (unsigned int m_i = 0; m_i < this->m_my_meshes_.size(); m_i++)
	{
		GLuint temp;
		this->vao_all.push_back(temp);
		this->buffer_all.push_back(temp);
		this->vPosition_all.push_back(temp);
		this->vColor_all.push_back(temp);
		this->vTexCoord_all.push_back(temp);
		this->vNormal_all.push_back(temp);

		update_vertex_buffer(m_i);

	}
};

void Mesh_Painter::update_vertex_buffer(int m_i)
{

	int num_face = this->m_my_meshes_[m_i]->num_faces();
	int num_vertex = this->m_my_meshes_[m_i]->num_vertices();

	const VertexList& vertex_list = this->m_my_meshes_[m_i]->get_vertices();
	const NormalList& normal_list = this->m_my_meshes_[m_i]->get_normals();
	const FaceList& face_list = this->m_my_meshes_[m_i]->get_faces();
	const STLVectorf& color_list = this->m_my_meshes_[m_i]->get_colors();
	const VtList& vt_list = this->m_my_meshes_[m_i]->get_vts();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3) * num_face * 3
		+ sizeof(vec3) * num_face * 3
		+ sizeof(vec3) * num_face * 3
		+ sizeof(vec2) * num_face * 3
		,
		NULL, GL_STATIC_DRAW)
		;
	//����㹻�Ŀռ�洢���꣬��ɫ�������Լ���������ȵ�

	vec3* points = new vec3[num_face * 3];

	for (int i = 0; i < num_face; i++)
	{
		int index = face_list[3 * i];
		points[3 * i] = vec3(
			(vertex_list[index * 3 + 0]),
			(vertex_list[index * 3 + 1]),
			(vertex_list[index * 3 + 2])
		);

		index = face_list[3 * i + 1];
		points[3 * i + 1] = vec3(
			(vertex_list[index * 3 + 0]),
			(vertex_list[index * 3 + 1]),
			(vertex_list[index * 3 + 2])
		);

		index = face_list[3 * i + 2];
		points[3 * i + 2] = vec3(
			(vertex_list[index * 3 + 0]),
			(vertex_list[index * 3 + 1]),
			(vertex_list[index * 3 + 2])
		);

	}

	GLintptr offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
	//�������괫��shader
	offset += sizeof(vec3) * num_face * 3;
	delete[] points;
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	points = new vec3[num_face * 3];
	for (int i = 0; i < num_face; i++)
	{
		int index = face_list[3 * i];
		points[3 * i] = vec3(
			(normal_list[index * 3 + 0]),
			(normal_list[index * 3 + 1]),
			(normal_list[index * 3 + 2])
		);

		index = face_list[3 * i + 1];
		points[3 * i + 1] = vec3(
			(normal_list[index * 3 + 0]),
			(normal_list[index * 3 + 1]),
			(normal_list[index * 3 + 2])
		);

		index = face_list[3 * i + 2];
		points[3 * i + 2] = vec3(
			(normal_list[index * 3 + 0]),
			(normal_list[index * 3 + 1]),
			(normal_list[index * 3 + 2])
		);
	}
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
	offset += sizeof(vec3) * num_face * 3;
	delete[] points;
	//���ߴ���shader

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	points = new vec3[num_face * 3];
	for (int i = 0; i < num_face; i++)
	{
		int index = face_list[3 * i];
		points[3 * i] = vec3(
			(color_list[index * 3 + 0]),
			(color_list[index * 3 + 1]),
			(color_list[index * 3 + 2])
		);

		index = face_list[3 * i + 1];
		points[3 * i + 1] = vec3(
			(color_list[index * 3 + 0]),
			(color_list[index * 3 + 1]),
			(color_list[index * 3 + 2])
		);

		index = face_list[3 * i + 2];
		points[3 * i + 2] = vec3(
			(color_list[index * 3 + 0]),
			(color_list[index * 3 + 1]),
			(color_list[index * 3 + 2])
		);
	}
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
	//��ɫ����shader
	offset += sizeof(vec3) * num_face * 3;
	delete[] points;


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	vec2* points_2 = new vec2[num_face * 3];
	for (int i = 0; i < num_face; i++)
	{
		points_2[i * 3] = vec2(vt_list[i * 6 + 0], vt_list[i * 6 + 1]);
		points_2[i * 3 + 1] = vec2(vt_list[i * 6 + 2], vt_list[i * 6 + 3]);
		points_2[i * 3 + 2] = vec2(vt_list[i * 6 + 4], vt_list[i * 6 + 5]);
	}
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec2) * num_face * 3, points_2);
	offset += sizeof(vec2) * num_face * 3;
	delete points_2;
	//�������괫��shader
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	// Load shaders and use the resulting shader program

	// set up vertex arrays
	offset = 0;
	GLuint vPosition;
	vPosition = glGetAttribLocation(this->program_all[m_i], "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += sizeof(vec3) * num_face * 3;
	//ָ��vPosition��shader��ʹ��ʱ��λ��

	GLuint vNormal;
	vNormal = glGetAttribLocation(this->program_all[m_i], "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += sizeof(vec3) * num_face * 3;
	//ָ��vNormal��shader��ʹ��ʱ��λ��

	GLuint vColor;
	vColor = glGetAttribLocation(this->program_all[m_i], "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += sizeof(vec3) * num_face * 3;
	//ָ��vColor��shader��ʹ��ʱ��λ��

	GLuint vTexCoord;
	vTexCoord = glGetAttribLocation(this->program_all[m_i], "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));

	//ָ��vTexCoord��shader��ʹ��ʱ��λ��

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	this->vao_all[m_i]= vao;
	this->buffer_all[m_i]= buffer;
	this->vPosition_all[m_i]= vPosition;
	this->vColor_all[m_i]= vColor;
	this->vTexCoord_all[m_i]= vTexCoord;
	this->vNormal_all[m_i]= vNormal;

};

void Mesh_Painter::init_shaders(std::string vs, std::string fs)
{
	this->program_all.clear();
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		GLuint program = InitShader(vs.c_str(), fs.c_str());
		//ָ��ʹ�õ���Ⱦ������ͬ��ģ�Ϳ���ָ����ͬ����Ⱦ������������ʹ�õ���Ⱦ��������ͬ���ļ���ѧ�����Ը����Լ��İ��öԲ�ͬ��ģ���趨��ͬ����Ⱦ��
		this->program_all.push_back(program);

	}

};
void Mesh_Painter::add_mesh(My_Mesh* m)
{
	this->m_my_meshes_.push_back(m);

	//��ӵ�ͬʱ����ռ�
	GLuint temp;
	this->textures_all.push_back(temp);
	this->program_all.push_back(temp);
	this->vao_all.push_back(temp);
	this->buffer_all.push_back(temp);
	this->vPosition_all.push_back(temp);
	this->vColor_all.push_back(temp);
	this->vTexCoord_all.push_back(temp);
	this->vNormal_all.push_back(temp);
};


void Mesh_Painter::clear_mehs()
{
	this->m_my_meshes_.clear();

	this->textures_all.clear();
	this->program_all.clear();
	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();
};
