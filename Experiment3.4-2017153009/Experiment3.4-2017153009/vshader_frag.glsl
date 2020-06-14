#version 330 core

in vec3 vPosition;
in vec3 vNormal;

uniform vec3 lightPos;
uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrix;
uniform float distToLight;

out vec3 N;
out vec3 V;
out vec3 lightPos_new;
out float dist;

// Phong 光照模型的实现 (per-fragment shading)

void main()
{
	dist= distToLight;
	gl_Position = modelViewProjMatrix * vec4(vPosition, 1.0);
	vec4 v1= vec4(gl_Position.xyz/gl_Position.w, 1);
	gl_Position= v1;

	// TODO 将顶点变换到相机坐标系下
	 vec4 vertPos_cameraspace = modelViewMatrix* vec4(vPosition, 1);
	
	// 对顶点坐标做透视投影
	 V = vertPos_cameraspace.xyz/vertPos_cameraspace.w;
	
	// TODO 将光源位置变换到相机坐标系下
	 vec4 lightPos_cameraspace = modelViewMatrix* vec4(lightPos, 1);
	
	// 对光源坐标做透视投影
	 lightPos_new = lightPos_cameraspace.xyz/lightPos_cameraspace.w;
	
	// TODO 将法向量变换到相机坐标系下并传入片元着色器
	 N = (modelViewMatrix* vec4(vNormal, 0)).xyz;
}