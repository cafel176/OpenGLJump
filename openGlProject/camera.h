#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Ĭ����ֵ
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// �������
class Camera
{
public:
	// �������
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// ŷ����
	float Yaw;
	float Pitch;
	// �����ѡ��
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	//����������
	float allTime = 0.3f, updateTime = 0.02f, tt, t;
	glm::vec3 moveSpeed;
	glm::vec3 targetPos;

	//���캯��
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		targetPos=Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// ��ȡ LookAt ����
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	//�����������λ��
	void movePos(glm::vec3 position) {
		targetPos = position;
		moveSpeed = (targetPos - Position)/allTime;//�����ٶ�
		t=tt = glfwGetTime();
	}

	//�������λ��
	void updatePos() {
			if (glfwGetTime() - tt > allTime) {//��ʱ�䳬��allTime���ھ�ֹ״̬
				Position = targetPos;
			}
			else if (glfwGetTime() - t >= updateTime) {////ÿ��deltaTime����һ�����λ��
				t = glfwGetTime();
				Position += moveSpeed*updateTime;
			}
	}

private:
	// �����������
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif
