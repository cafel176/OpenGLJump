#ifndef BOX_H
#define BOX_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
//�ذ�ש���࣬����ÿ��ש���λ�ñ��ڽ�����ײ���
class Box {
public:
	glm::vec3 pos;
	Box *next=NULL;

	Box(glm::vec3 nowPosition = glm::vec3(0.0f, 0.0f, 0.0f)) {
		pos = nowPosition;
	}
};
#endif

