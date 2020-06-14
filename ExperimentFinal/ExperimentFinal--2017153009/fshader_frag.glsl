#version 330 core

in vec3 N;
in vec3 V;
in vec3 lightPos_new;
in float dist;

uniform vec4 fColor;

out vec4 fragmentColor;
//out vec4 color;

void main()
{
	//color= fColor;

	// TODO 设置三维物体的材质属性
	vec3 ambiColor = vec3(0.2, 0.2, 0.2);
	vec3 diffColor = vec3(0.5, 0.5, 0.5);
	vec3 specColor = vec3(0.3, 0.3, 0.3);

	// TODO 计算N，L，V，R四个向量并归一化
	 vec3 N_norm = normalize(N);
	 vec3 L_norm = normalize(lightPos_new- V);
	 vec3 V_norm = normalize(-V);
	 vec3 R_norm = reflect(-L_norm, N_norm);

	// TODO 计算漫反射系数和镜面反射系数
	 float lambertian = clamp(dot(L_norm, N_norm), 0, 1);
	 float specular = clamp(dot(R_norm, V_norm), 0, 1);

	//Blinn-Phong模型
	//vec3 H_norm= normalize(L_norm+ V_norm);
	//float lambertian = clamp(dot(N_norm, L_norm), 0.0, 1.0);
	//float specular = clamp(dot(H_norm, N_norm), 0.0, 1.0);
		
	// TODO 计算最终每个片元的输出颜色
	float D= 1/(0.5+ dist+ dist*dist);

	if(fColor!= vec4(0, 0, 0, 1))
	 fragmentColor = vec4(ambiColor+
		D* (diffColor*lambertian+
		specColor* pow(specular, 10)), 1);
	else 
		fragmentColor= fColor;
}