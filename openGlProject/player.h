#ifndef PLAYER_H
#define PLAYER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

//�����
/*	�������ʵ����ʾ�����Ծ�����ᶯ��
	����˼�������main�Ļ���ѭ���ﲻ�ϵĵ���update���������ϵĸ�����ҵ�λ�ú���ת����ʵ��
	�������ʾ��ҵ���Ծ����

	1.ͨ������¼����������jump�����������Ҫ��Ծ��Ŀ��λ�ã�firstjump������ֹ��Ծ�����ж�
	���������ٴμ��
	2.һ��Ŀ��λ�ø��ģ�����ѭ����update�����ͻῪʼ������ҵ�λ�ú���ת��
	3.ͨ����update����Ƹ��Ļ��Ƶļ��ΪdeltaTime��ʹ������ʾ����Ȼ��allTime����������Ծ��
	�̵ĳ���ʱ�䡣��ÿһ����Ծ��allTime������ʾ��ɣ�����ÿ��deltaTime�����һ�����λ��
	4.������ҵ�ǰλ�ú�Ŀ��λ�õĲ��allTime�õ��ٶȣ�֮���ٶȳ�deltaTime�õ�ÿ�θ���
	���λ�ú���ת�ĸı���������λ�ú���ת�Ǽ���

	�������ͬ��
*/
class Player {
private:
	float allTime = 0.3f;
	float gravity;
	float ySpeed = 15.0f;
	float deltaTime = 0.02f;
	float moveSpeed = 5.0f;

	bool firstJump = true;
	float tt,t;
public:
	glm::vec3 nowPos;
	glm::vec3 targetPos;
	glm::vec3 speed;

	float nowAngle = 0.0f;
	float xSize = 0.3f;
	float zSize = 0.3f;
	float ySize = 1.0f;

	//���캯��
	Player(glm::vec3 nowPosition = glm::vec3(0.0f, 0.5f, 0.0f)) {
		nowPos = nowPosition;
		targetPos.y = nowPos.y;
	}

	//������ҿ�ʼ��Ծ
	void jump(glm::vec3 mov) {
		if (firstJump) {
			firstJump = false;
			//����Ŀ��λ��
			targetPos = mov + nowPos;

			//�����ٶ�
			speed.x = moveSpeed*mov.x/allTime;
			speed.z = moveSpeed*mov.z/allTime;
			speed.y = ySpeed;

			//��������
			gravity = 2*speed.y/allTime;

			//��¼��ǰʱ��
			tt=t= glfwGetTime();
		}
	}

	//�������λ�ü��Ƕ�
	void update(bool nofall) {
		if (glfwGetTime()-tt>allTime&&nofall)//��ʱ�䳬��allTime����������ھ�ֹ״̬
		{
			nowPos.y = targetPos.y;//���ֵ�ǰλ�õ���Ŀ��λ��
			nowAngle = 0;//���ֵ�ǰ�Ƕȵ���0
			firstJump = true;
		}
		else if (glfwGetTime() - t >= deltaTime) {//ÿ��deltaTime����һ�����λ�ú���ת��
			t = glfwGetTime();
			nowAngle += (360.0f / allTime)*deltaTime;//ͨ���Ƕȸı�ֵ���������ת�Ƕ�
			nowPos += speed*deltaTime;//�������λ��
			speed.y -= gravity*deltaTime;//ͨ����������y�����ٶ�
		}
	}
};

#endif
