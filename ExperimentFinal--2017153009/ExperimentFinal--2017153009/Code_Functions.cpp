#include "Code_Functions.h"
#include "Const_Head.h"

point4 points[NumVertices];
color4 colors[NumVertices];
point4 ns[NumVertices];


int Index = 0;
void Functions::quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; ns[Index] = (vertices[a] - vec4(0, 0, 0, 1)); Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; ns[Index] = (vertices[b] - vec4(0, 0, 0, 1)); Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; ns[Index] = (vertices[c] - vec4(0, 0, 0, 1)); Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; ns[Index] = (vertices[a] - vec4(0, 0, 0, 1)); Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; ns[Index] = (vertices[c] - vec4(0, 0, 0, 1)); Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; ns[Index] = (vertices[d] - vec4(0, 0, 0, 1)); Index++;
}

void Functions::colorcube(void)
{
	Index = 0;
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

void Functions::printUsage() {

	std::cout << "Q��q:�˳�����\n s:��ʼ��Ϸ����ͣ��Ϸ\nd:�����Ŵ�\nD:������С\nr:�����ӵ�İ뾶\nR:��С�ӵ�İ뾶\no:�����ӵ��theraֵ\nO:��С�ӵ��theraֵ\np;�����ӵ��phiֵ\nP:��С�ӵ��phiֵ\nx : �����Ӿ����left��rightֵ\nX : ��С�Ӿ����left��rightֵ\ny : �����Ӿ����bottom��topֵ\nY : ��С�Ӿ����bottom��topֵ\nz : �����Ӿ����near��farֵ\nZ : ��С�Ӿ����near��farֵ\nt : ����radius��thera��phi�͹������ű�" << std::endl;

	std::cout << "���Ϸ����:���ƹ������ƶ�\n���·����:���ƹ������ƶ�" << std::endl;

	std::cout << "����м��������˵�\n������������Ƕ�\n����Ҽ�����С�Ƕ�" << std::endl;
}