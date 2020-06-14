#include <time.h>
#include "Code_Rules.h"
#include "Const_Head.h"
#include "mesh.h"
#include "Mesh_Painter.h"

int frameIndex = 0;

//��ÿ֡�Ķ���
extern GLfloat theta[NumJointAngles];
extern int dogDirection;extern int dogAxisZ;extern int dogAngleY;
extern int game_start;
void Rules::actions() {

	if (!game_start) return;

	//��ӹ�����ÿ֡�Ķ���
	for (int i = 0; i < NumJointAngles; i++) {
		theta[i] = dogMoveFrame[frameIndex * 13 + i];
	}
	frameIndex = (frameIndex + 1) % (NumFrame / 13);

	//�������ϻ����µ�ͷ
	if (dogDirection) {
		//����
		if (dogDirection == 1) {
			dogAxisZ++;
			dogAngleY += (topTrackZ / 2 - dogAxisZ) * perTurnAngle;
		}
		else if (dogDirection == -1) {  //����
			dogAxisZ--;
			dogAngleY += (topTrackZ / 2 - dogAxisZ) * perTurnAngle;
		}

		//��������һ���ܵ���
		if (dogAxisZ == 0 || dogAxisZ == topTrackZ) {
			dogDirection = 0;
			dogAngleY = 270;
		}

	}
}


//�漴����ϰ���
extern std::vector<int> freePosition;
extern Mesh_Painter* mp_;
extern mat4 view;
extern mat4 projection;
extern int score;
void Rules::addBarrier(int id) {

	if (game_start) {
		int add_index = 0;
		My_Mesh* barrier = new My_Mesh;

		srand((unsigned)time(NULL));
		int temp = rand() % 2;

		barrier->xAxis = -30;
		barrier->zAxis = -10 * temp;
		barrier->viewMatrix = view;
		barrier->proMatrix= projection;

		srand((unsigned)time(NULL));
		temp = rand() % 2;

		if (temp % 2) {
			barrier->generate_cylinder(50, 3);//����Բ������
			barrier->set_texture_file("texture/wood.jpg");//ָ������ͼ���ļ�
			barrier->moMatrix = Translate(0, 0.5, 0) * Scale(1.5, 1.5, 1);

			if (!freePosition.empty()) {
				add_index = freePosition[0];
				mp_->m_my_meshes_[add_index] = barrier;
				freePosition.erase(freePosition.begin());
			}
			else {
				add_index = mp_->m_my_meshes_.size();
				mp_->add_mesh(barrier);
			}
		}
		else {
			barrier->generate_cone(50, 2);  //����Բ׶��
			barrier->set_texture_file("texture/sand.jpg");
			barrier->moMatrix = RotateX(270) * Scale(3, 3, 3);

			if (!freePosition.empty()) {
				add_index = freePosition[0];
				mp_->m_my_meshes_[add_index] = barrier;
				freePosition.erase(freePosition.begin());
			}
			else {
				add_index = mp_->m_my_meshes_.size();
				mp_->add_mesh(barrier);
			}

		}

		mp_->init_shaders("vshader82.glsl", "fshader82.glsl");
		mp_->update_vertex_buffer(add_index);
		mp_->update_texture(add_index);
		score+= 5;
	}

	glutTimerFunc(6000, addBarrier, 1);
}
