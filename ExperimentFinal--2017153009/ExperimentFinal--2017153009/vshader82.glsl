#version 330 core

in vec4 vPosition;
in vec3 vNormal;
in  vec4 vColor;
in  vec2 vTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 proMatrix;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 draw_color;

uniform vec3 lightPos;
uniform float distToLight;
uniform int is_texture;

out vec3 N;
out vec3 V;
out vec3 lightPos_new;
out float dist;
out vec4 color;
out vec2 texCoord;

void main() 
{
    //color = vColor;
	dist= distToLight;

	if(is_texture== 1){
		vec4 v1= proMatrix* viewMatrix* modelMatrix* vPosition;	
		gl_Position= vec4(v1.xyz/v1.w, 1);

		//gl_Position= vPosition;

	}
	else{
		vec4 v1 = ModelView*vPosition;  
		// ����modelMatrix�п���Ϊ��Ӱ����Ϊ�˵õ���ȷλ�ã�������Ҫ��һ��͸�ӳ���
		vec4 v2 = vec4(v1.xyz / v1.w, 1.0);
		// ���������ͶӰ
		vec4 v3 =  Projection*viewMatrix*v2;
		// ����һ��͸�ӳ��� (Ϊ����ȷ���������ʽ��һ��͸�ӳ���)
		vec4 v4 = vec4(v3.xyz / v3.w, 1.0);
		gl_Position = v4;	
	}

	// TODO ������任���������ϵ��
	 vec4 vertPos_cameraspace =  ModelView* vPosition;
	
	// �Զ���������͸��ͶӰ
	 V = vertPos_cameraspace.xyz/vertPos_cameraspace.w;
	
	// TODO ����Դλ�ñ任���������ϵ��
	 vec4 lightPos_cameraspace = viewMatrix* ModelView* vec4(lightPos, 1);
	
	// �Թ�Դ������͸��ͶӰ
	 lightPos_new = lightPos_cameraspace.xyz/lightPos_cameraspace.w;
	
	// TODO ���������任���������ϵ�²�����ƬԪ��ɫ��
	 N = (viewMatrix* ModelView* vec4(vNormal, 0)).xyz;

	texCoord  = vTexCoord;
    color = draw_color;

} 
