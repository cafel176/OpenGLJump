#ifndef BOX_H
#define BOX_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
//地板砖快类，设置每个砖块的位置便于进行碰撞检测
class Box {
public:
	glm::vec3 pos;
	Box *next=NULL;

	Box(glm::vec3 nowPosition = glm::vec3(0.0f, 0.0f, 0.0f)) {
		pos = nowPosition;
	}
};
#endif

