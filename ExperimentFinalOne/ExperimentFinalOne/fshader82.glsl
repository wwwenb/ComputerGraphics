#version 150 

in  vec4 color;
out vec4 fColor;
in vec2 texCoord;

uniform int has_texture;
uniform sampler2D texture;

in vec3 N;
in vec3 V;
in vec3 lightPos_new;
in float dist;

void main() 
{ 
	if(has_texture== 1){
		fColor= texture2D( texture, texCoord );
		return;
	}

	// TODO ������ά����Ĳ�������
	vec3 ambiColor = vec3(0.2, 0.2, 0.2);
	vec3 diffColor = vec3(0.5, 0.5, 0.5);
	vec3 specColor = vec3(0.3, 0.3, 0.3);

	// TODO ����N��L��V��R�ĸ���������һ��
	 vec3 N_norm = normalize(N);
	 vec3 L_norm = normalize(lightPos_new- V);
	 vec3 V_norm = normalize(-V);
	 vec3 R_norm = reflect(-L_norm, N_norm);

	// TODO ����������ϵ���;��淴��ϵ��
	 float lambertian = clamp(dot(L_norm, N_norm), 0, 1);
	 float specular = clamp(dot(R_norm, V_norm), 0, 1);
		
	// TODO ��������ÿ��ƬԪ�������ɫ
	float D= 1/(0.5+ dist+ dist*dist);

	if(color!= vec4(0, 0, 0, 1))
		 fColor = vec4(ambiColor+
			D* (diffColor*lambertian+
			specColor* pow(specular, 10)), 1)* color;
	else 
		fColor= color;

   // fColor = color;
} 

