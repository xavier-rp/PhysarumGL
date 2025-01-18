#include <iostream>
#include <vector>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

bool vSync = true;

float PI = 3.141592653589793;

int numAgents = 4194304;

struct Agent
{
	float pos[2];
	float angle;
	float velocity;
};

struct AgentSettings {
	float color[4] = {1.0f, 0.0f, 1.0f, 1.0f};
	float sensor_angle_offset{22.5f * PI / 180.f};
	int sensor_offset_distance{9};
	int sensor_width{1};
};

// Vertices coordinates (first 3) and texture coordinates (last 2)
GLfloat vertices[] =
{
	-1.0f, -1.0f , 0.0f, 0.0f, 0.0f,
	-1.0f,  1.0f , 0.0f, 0.0f, 1.0f,
	 1.0f,  1.0f , 0.0f, 1.0f, 1.0f,
	 1.0f, -1.0f , 0.0f, 1.0f, 0.0f,
};

// Indices for vertices order
GLuint indices[] =
{
	0, 3, 1,
	1, 2, 3
};
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//function to adjust height and width
	glViewport(0, 0, width, height);
}

int main()
{
	// Initialize GLFW
	glfwInit();

	const unsigned int SCREEN_WIDTH = 1024;
	const unsigned int SCREEN_HEIGHT = 1024;

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Create a GLFWwindow object of width by height pixels, naming it "OpenGL"
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(vSync);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
	}
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Sets the function to call when the window is resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Load GLAD so it configures OpenGL
	gladLoadGL();

	// Generates Vertex Array Object and binds it
	VAO VAO1;

	// Generates Vertex Buffer Object and links it to vertices. 
	// VBO and EBO objects are binded when the constructor is called
	VBO VBO1(vertices, sizeof(vertices));

	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	VAO1.EnableAttrib();

	glVertexArrayVertexBuffer(VAO1.ID, 0, VBO1.ID, 0, 5 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO1.ID, EBO1.ID);

	GLuint screenTex;
	glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
	glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(screenTex, 1, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag", "default.comp", "comp2.comp");

	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	std::cout << "Max work groups per compute shader" <<
		" x:" << work_grp_cnt[0] <<
		" y:" << work_grp_cnt[1] <<
		" z:" << work_grp_cnt[2] << "\n";

	int work_grp_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	std::cout << "Max work group sizes" <<
		" x:" << work_grp_size[0] <<
		" y:" << work_grp_size[1] <<
		" z:" << work_grp_size[2] << "\n";

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	std::cout << "Max invocations count per work group: " << work_grp_inv << "\n";


	// Gets ID of uniform called "iTime" and "iResolution"
	GLuint timeUniformID = glGetUniformLocation(shaderProgram.ID, "iTime");
	GLuint timeUniformComputeID = glGetUniformLocation(shaderProgram.computeID, "iTime");
	float iTime;
	GLuint resolutionUniformID = glGetUniformLocation(shaderProgram.ID, "iResolution");
	GLuint numAgentsUniformID = glGetUniformLocation(shaderProgram.computeID, "numAgents");

	GLuint agentsBuffer;
	glGenBuffers(1, &agentsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, agentsBuffer);
	// Bind the buffer to binding point 1
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, agentsBuffer);

	// Allocate memory for the agents (numAgents * sizeof(Agent))
	glBufferData(GL_SHADER_STORAGE_BUFFER, numAgents * sizeof(Agent), nullptr, GL_DYNAMIC_DRAW);

	std::vector<Agent> agentsVector(numAgents);

	for (int i = 0; i < numAgents; ++i) {
		agentsVector[i].pos[0] = 50 * std::cos(static_cast<float>(i) * 2.0f * PI / numAgents) + static_cast<float>(SCREEN_WIDTH / 2);  // Start at (0, 0)
		agentsVector[i].pos[1] = 50 * std::sin(static_cast<float>(i) * 2.0f * PI / numAgents) + static_cast<float>(SCREEN_HEIGHT / 2);  // Start at (0, 0)
		agentsVector[i].angle = static_cast<float>(i) * 2.0f * PI / numAgents;
		agentsVector[i].velocity = 1.0f;
	}

	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, agentsVector.size() * sizeof(Agent), agentsVector.data());

	GLuint agentSettingsBuffer;
	glGenBuffers(1, &agentSettingsBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, agentSettingsBuffer);
	AgentSettings as;
	// Allocate memory for the agents (sizeof(Agent))
	glBufferData(GL_UNIFORM_BUFFER, sizeof(AgentSettings), &as, GL_STATIC_DRAW);

	// Bind the buffer to binding point 2
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, agentSettingsBuffer);

	int framebufferWidth, framebufferHeight;
	// Restart the GLFW timer and start main while loop 
	glfwSetTime(0.0);
	std::cout << numAgents / 128.0f << "  " << ceil(numAgents / 128.0f) << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		iTime = float(glfwGetTime());
		shaderProgram.ActivateCompute();
		glUniform1f(timeUniformComputeID, iTime);
		glUniform1i(numAgentsUniformID, numAgents);
		glDispatchCompute(ceil(numAgents / 128.0f), 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		shaderProgram.ActivateCompute2();
		//glUniform1f(timeUniformComputeID, iTime);
		//glUniform1i(numAgentsUniformID, numAgents);
		glDispatchCompute(ceil(SCREEN_WIDTH / 8), ceil(SCREEN_HEIGHT / 8), 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		glBindTextureUnit(0, screenTex);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "screen"), 0);
		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Assigns a value to the iTime and iResolution uniforms; NOTE: Must always be done after activating the Shader Program
		glUniform1f(timeUniformID, iTime);
		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
		glUniform2f(resolutionUniformID, GLfloat(framebufferWidth), GLfloat(framebufferHeight));

		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();

	// Delete window before ending the program
	glfwDestroyWindow(window);

	// Terminate GLFW before ending the program
	glfwTerminate();

	return 0;
}
