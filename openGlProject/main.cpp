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

//请确认是在包含GLFW的头文件之前包含了GLAD的头文件。GLAD的头文件包含了正确的
//OpenGL头文件（例如GL/gl.h），所以需要在其它依赖于OpenGL的头文件之前包含GLAD。

//窗口尺寸
 int SCR_WIDTH = 800;
 int SCR_HEIGHT = 600;
 //摄像机位置偏移量
 glm::vec3 offestPos(-3.0f, 4.0f, 3.0f);
 //摄像机
 Camera camera(offestPos, glm::vec3(1.0f, 1.0f, -1.0f), -45.0f, -30.0f);
 //玩家
 Player Aplayer(glm::vec3(0.0f, 0.49f, 0.0f));
 //玩家跳跃方向
 int direction = 0;//0为x方向，1为z方向

 float time = 0;
 //地板砖快的尺寸
 float Size = 1.0f;
 //地板砖块数量
 int boxNum;
 //当前砖块的指针
 Box *nowBox=NULL;

 //生成砖块的偏移量
 glm::vec3 xOffest(1.0f, 0.0f, 0.0f);
 glm::vec3 zOffest(0.0f, 0.0f, -1.0f);

 //窗口尺寸更改回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

//输入检测函数
void processInput(GLFWwindow *window)
{
	//按ESC退出
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//鼠标事件回调函数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//根据鼠标长按时间控制玩家跳跃
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

//碰撞检测
bool CheckCollision(Player &one, Box &two) // AABB - AABB collision
{
	// x轴方向碰撞？
	bool collisionX = one.nowPos.x + one.xSize/2 >= two.pos.x- Size / 2 &&
		two.pos.x + Size/2 >= one.nowPos.x- one.xSize / 2;
	// y轴方向碰撞？
	bool collisionY = one.nowPos.y + one.ySize/2 >= two.pos.y - Size / 2 &&
		two.pos.y + Size/2 >= one.nowPos.y - one.ySize / 2;
	// z轴方向碰撞？
	bool collisionZ = one.nowPos.z + one.zSize/2 >= two.pos.z - Size / 2 &&
		two.pos.z + Size/2 >= one.nowPos.z - one.zSize / 2;
	// 只有三个轴向都有碰撞时才碰撞
	return collisionX && collisionY&& collisionZ;
}

int main()
{
	glfwInit();
	//glfwWindowHint函数的第一个参数代表选项的名称；第二个参数接受一个整形，用来设置这个选项的值。
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//我们需要告诉GLFW我们要使用的OpenGL版本是3.3，我们将主版本号(Major)和次版本号(Minor)都设为3，同样明确告诉GLFW我们使用的是核心模式(Core-profile)
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	//如果使用的是Mac OS X系统，你还需要加上面这行代码到你的初始化代码中这些配置才能起作用：

	//创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//注册回调函数
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//在调用任何OpenGL的函数之前我们需要初始化GLAD。
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	//创建着色器
	Shader ourShader("vertex.vs", "fragment.fs");

	//初始生成的玩家和砖块位置
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.5f,  0.0f),

		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(3.0f,  0.0f, 0.0f),
	};

	//统计砖块数量
	boxNum = sizeof(cubePositions)/12-1;

	//绘制立方体的顶点数组
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
	// 1. 绑定VAO
	glBindVertexArray(VAO);
	// 2. 复制顶点数组到缓冲中供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//GL_STATIC_DRAW ：数据不会或几乎不会改变。GL_DYNAMIC_DRAW：数据会被改变很多。GL_STREAM_DRAW ：数据每次绘制时都会改变
	// 3. 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//设置材质
	unsigned int texture1, texture2;

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	//设置参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); //设置y轴翻转
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

	//将材质设置到着色器中
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);
	//生成地板砖快链表
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

	//绘制循环
	while (!glfwWindowShouldClose(window))
	{
		// 输入
		processInput(window);
		// 渲染指令
		glClearColor(1.0f, 0.682f, 0.784f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//使用着色器程序渲染
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
		//顶点数组对象同样可以保存索引缓冲对象的绑定状态。
		//VAO绑定时正在绑定的索引缓冲对象会被保存为VAO的元素缓冲对象。绑定VAO的同时也会自动绑定EBO。
		//mixd值为0时绘制地板纹理
		ourShader.setFloat("mixd", 0);
		bool check = false;
		Box *point = head;
		//依次绘制每个地板砖快，并与玩家进行碰撞检测
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
		//更新玩家跳跃与旋转
		Aplayer.update(check);
		//mixd值为1时绘制玩家
		ourShader.setFloat("mixd",1.0f);
		glm::mat4 player;
		player = glm::translate(player,Aplayer.nowPos);
		player = glm::rotate(player, glm::radians(Aplayer.nowAngle), direction?glm::vec3(1.0f, 0, 0): glm::vec3(0, 0, -1.0f));
		player = glm::scale(player, glm::vec3(Aplayer.xSize, Aplayer.ySize, Aplayer.zSize));
		ourShader.setMat4("model", player);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//更新相机位置
		camera.updatePos();

		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//删除相应元素
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

	return 0;
}

