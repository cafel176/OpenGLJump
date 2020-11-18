#ifndef PLAYER_H
#define PLAYER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

//玩家类
/*	关于如何实现显示玩家跳跃的连贯动作
	核心思想就是在main的绘制循环里不断的调用update函数来不断的更新玩家的位置和旋转角来实现
	连贯的显示玩家的跳跃过程

	1.通过鼠标事件监测来触发jump函数设置玩家要跳跃的目标位置，firstjump用来防止跳跃过程中对
	鼠标输入的再次监测
	2.一旦目标位置更改，绘制循环里update函数就会开始更新玩家的位置和旋转角
	3.通过在update里控制更改绘制的间隔为deltaTime来使动作显示的自然，allTime控制整个跳跃过
	程的持续时间。即每一次跳跃用allTime秒来显示完成，其中每隔deltaTime秒更新一次玩家位置
	4.根据玩家当前位置和目标位置的差，除allTime得到速度，之后速度乘deltaTime得到每次更新
	玩家位置和旋转的改变量，更新位置和旋转角即可

	相机跟随同理
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

	//构造函数
	Player(glm::vec3 nowPosition = glm::vec3(0.0f, 0.5f, 0.0f)) {
		nowPos = nowPosition;
		targetPos.y = nowPos.y;
	}

	//控制玩家开始跳跃
	void jump(glm::vec3 mov) {
		if (firstJump) {
			firstJump = false;
			//更改目标位置
			targetPos = mov + nowPos;

			//计算速度
			speed.x = moveSpeed*mov.x/allTime;
			speed.z = moveSpeed*mov.z/allTime;
			speed.y = ySpeed;

			//计算重力
			gravity = 2*speed.y/allTime;

			//记录当前时间
			tt=t= glfwGetTime();
		}
	}

	//更新玩家位置及角度
	void update(bool nofall) {
		if (glfwGetTime()-tt>allTime&&nofall)//当时间超过allTime并且落地则处于静止状态
		{
			nowPos.y = targetPos.y;//保持当前位置等于目标位置
			nowAngle = 0;//保持当前角度等于0
			firstJump = true;
		}
		else if (glfwGetTime() - t >= deltaTime) {//每隔deltaTime更新一次玩家位置和旋转角
			t = glfwGetTime();
			nowAngle += (360.0f / allTime)*deltaTime;//通过角度改变值更改玩家旋转角度
			nowPos += speed*deltaTime;//更改玩家位置
			speed.y -= gravity*deltaTime;//通过重力更改y方向速度
		}
	}
};

#endif
