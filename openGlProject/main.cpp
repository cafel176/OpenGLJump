#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

#include <shader_m.h>
#include <camera.h>
#include <player.h>
#include <Box.h>

#include <iostream>
using namespace std;

//��ȷ�����ڰ���GLFW��ͷ�ļ�֮ǰ������GLAD��ͷ�ļ���GLAD��ͷ�ļ���������ȷ��
//OpenGLͷ�ļ�������GL/gl.h����������Ҫ������������OpenGL��ͷ�ļ�֮ǰ����GLAD��

//���ڳߴ�
 int SCR_WIDTH = 800;
 int SCR_HEIGHT = 600;
 //�����λ��ƫ����
 glm::vec3 offestPos(-3.0f, 4.0f, 3.0f);
 //�����
 Camera camera(offestPos, glm::vec3(1.0f, 1.0f, -1.0f), -45.0f, -30.0f);
 //���
 Player Aplayer(glm::vec3(0.0f, 0.49f, 0.0f));
 //�����Ծ����
 int direction = 0;//0Ϊx����1Ϊz����

 float time = 0;
 //�ذ�ש��ĳߴ�
 float Size = 1.0f;
 //�ذ�ש������
 int boxNum;
 //��ǰש���ָ��
 Box *nowBox=NULL;

 //����ש���ƫ����
 glm::vec3 xOffest(1.0f, 0.0f, 0.0f);
 glm::vec3 zOffest(0.0f, 0.0f, -1.0f);

 //���ڳߴ���Ļص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

//�����⺯��
void processInput(GLFWwindow *window)
{
	//��ESC�˳�
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//����¼��ص�����
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//������곤��ʱ����������Ծ
	if (action == GLFW_PRESS && button== GLFW_MOUSE_BUTTON_LEFT)
	{
		time = glfwGetTime();
	}else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT)
	{
		float y=glfwGetTime() - time;
		if (direction)
		{
			Aplayer.jump(glm::vec3(0,0,-y));
		}
		else {
			Aplayer.jump(glm::vec3(y, 0, 0));
		}
	}
}

//��ײ���
bool CheckCollision(Player &one, Box &two) // AABB - AABB collision
{
	// x�᷽����ײ��
	bool collisionX = one.nowPos.x + one.xSize/2 >= two.pos.x- Size / 2 &&
		two.pos.x + Size/2 >= one.nowPos.x- one.xSize / 2;
	// y�᷽����ײ��
	bool collisionY = one.nowPos.y + one.ySize/2 >= two.pos.y - Size / 2 &&
		two.pos.y + Size/2 >= one.nowPos.y - one.ySize / 2;
	// z�᷽����ײ��
	bool collisionZ = one.nowPos.z + one.zSize/2 >= two.pos.z - Size / 2 &&
		two.pos.z + Size/2 >= one.nowPos.z - one.zSize / 2;
	// ֻ��������������ײʱ����ײ
	return collisionX && collisionY&& collisionZ;
}

int main()
{
	glfwInit();
	//glfwWindowHint�����ĵ�һ����������ѡ������ƣ��ڶ�����������һ�����Σ������������ѡ���ֵ��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//������Ҫ����GLFW����Ҫʹ�õ�OpenGL�汾��3.3�����ǽ����汾��(Major)�ʹΰ汾��(Minor)����Ϊ3��ͬ����ȷ����GLFW����ʹ�õ��Ǻ���ģʽ(Core-profile)
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	//���ʹ�õ���Mac OS Xϵͳ���㻹��Ҫ���������д��뵽��ĳ�ʼ����������Щ���ò��������ã�

	//��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//ע��ص�����
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//�ڵ����κ�OpenGL�ĺ���֮ǰ������Ҫ��ʼ��GLAD��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	//������ɫ��
	Shader ourShader("vertex.vs", "fragment.fs");

	//��ʼ���ɵ���Һ�ש��λ��
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.5f,  0.0f),

		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(3.0f,  0.0f, 0.0f),
	};

	//ͳ��ש������
	boxNum = sizeof(cubePositions)/12-1;

	//����������Ķ�������
	float vertices[] = {
		-Size / 2, 0, -Size / 2,  0.0f, 0.0f,
		Size / 2, 0, -Size / 2,  Size, 0.0f,
		Size / 2,  Size / 2, -Size / 2,  Size, Size,
		Size / 2,  Size / 2, -Size / 2,  Size, Size,
		-Size / 2,  Size / 2, -Size / 2,  0.0f, Size,
		-Size / 2, 0, -Size / 2,  0.0f, 0.0f,

		-Size / 2, 0,  Size / 2,  0.0f, 0.0f,
		Size / 2, 0,  Size / 2,  Size, 0.0f,
		Size / 2,  Size / 2,  Size / 2,  Size, Size,
		Size / 2,  Size / 2,  Size / 2,  Size, Size,
		-Size / 2,  Size / 2,  Size / 2,  0.0f, Size,
		-Size / 2, 0,  Size / 2,  0.0f, 0.0f,

		-Size / 2,  Size / 2,  Size / 2,  Size, 0.0f,
		-Size / 2,  Size / 2, -Size / 2,  Size, Size,
		-Size / 2, 0, -Size / 2,  0.0f, Size,
		-Size / 2, 0, -Size / 2,  0.0f, Size,
		-Size / 2, 0,  Size / 2,  0.0f, 0.0f,
		-Size / 2,  Size / 2,  Size / 2,  Size, 0.0f,

		Size / 2,  Size / 2,  Size / 2,  Size, 0.0f,
		Size / 2,  Size / 2, -Size / 2,  Size, Size,
		Size / 2, 0, -Size / 2,  0.0f, Size,
		Size / 2, 0, -Size / 2,  0.0f, Size,
		Size / 2, 0,  Size / 2,  0.0f, 0.0f,
		Size / 2,  Size / 2,  Size / 2,  Size, 0.0f,

		-Size / 2, 0, -Size / 2,  0.0f, Size,
		Size / 2, 0, -Size / 2,  Size, Size,
		Size / 2, 0,  Size / 2,  Size, 0.0f,
		Size / 2, 0,  Size / 2,  Size, 0.0f,
		-Size / 2, 0,  Size / 2,  0.0f, 0.0f,
		-Size / 2, 0, -Size / 2,  0.0f, Size,

		-Size / 2,  Size / 2, -Size / 2,  0.0f, Size,
		Size / 2,  Size / 2, -Size / 2,  Size, Size,
		Size / 2,  Size / 2,  Size / 2,  Size, 0.0f,
		Size / 2,  Size / 2,  Size / 2,  Size, 0.0f,
		-Size / 2,  Size / 2,  Size / 2,  0.0f, 0.0f,
		-Size / 2,  Size / 2, -Size / 2,  0.0f, Size
	};

	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	// 1. ��VAO
	glBindVertexArray(VAO);
	// 2. ���ƶ������鵽�����й�OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣGL_STREAM_DRAW ������ÿ�λ���ʱ����ı�
	// 3. ���ö�������ָ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//���ò���
	unsigned int texture1, texture2;

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	//���ò���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); //����y�ᷭת
	unsigned char *data = stbi_load("textures/wall.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	//���������õ���ɫ����
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);
	//���ɵذ�ש������
	Box *head = NULL, *now = NULL;
	for (unsigned int i = 1; i < boxNum+1; i++)
	{
		if (i == 1) {
			head = new Box(cubePositions[i]);
			now = head;
		}
		else {
			now->next = new Box(cubePositions[i]);
			now = now->next;
		}
	}
	nowBox = head;

	//����ѭ��
	while (!glfwWindowShouldClose(window))
	{
		// ����
		processInput(window);
		// ��Ⱦָ��
		glClearColor(1.0f, 0.682f, 0.784f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//ʹ����ɫ��������Ⱦ
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		ourShader.use();
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		ourShader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		glBindVertexArray(VAO);
		//�����������ͬ�����Ա��������������İ�״̬��
		//VAO��ʱ���ڰ󶨵������������ᱻ����ΪVAO��Ԫ�ػ�����󡣰�VAO��ͬʱҲ���Զ���EBO��
		//mixdֵΪ0ʱ���Ƶذ�����
		ourShader.setFloat("mixd", 0);
		bool check = false;
		Box *point = head;
		//���λ���ÿ���ذ�ש�죬������ҽ�����ײ���
		for (unsigned int i = 1; i < boxNum+1; i++)
		{
			glm::mat4 model;
			model = glm::translate(model, point->pos);

			if (CheckCollision(Aplayer, *point)) {
				check = true;
				if (nowBox != point) {
					nowBox = point;
					camera.movePos(nowBox->pos + offestPos);
					direction=rand() % 2;
					glm::vec3 www= direction ? zOffest : xOffest;
					now->next = new Box(nowBox->pos+(www*(float)(rand() % 3+2)));
					now = now->next;

					Box *temp = head;
					head = head->next;
					delete temp;
				}
			}
			ourShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			point = point->next;
		}
		//���������Ծ����ת
		Aplayer.update(check);
		//mixdֵΪ1ʱ�������
		ourShader.setFloat("mixd",1.0f);
		glm::mat4 player;
		player = glm::translate(player,Aplayer.nowPos);
		player = glm::rotate(player, glm::radians(Aplayer.nowAngle), direction?glm::vec3(1.0f, 0, 0): glm::vec3(0, 0, -1.0f));
		player = glm::scale(player, glm::vec3(Aplayer.xSize, Aplayer.ySize, Aplayer.zSize));
		ourShader.setMat4("model", player);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//�������λ��
		camera.updatePos();

		//��鲢�����¼�����������
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//ɾ����ӦԪ��
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

	return 0;
}

