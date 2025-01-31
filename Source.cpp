#include <iostream>
#include <vector>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fftw3.h>

#include "utils.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "AudioStream.h"
#include "fftUtils.h"

bool vSync = true;

int main()
{
	// Initialize GLFW
	glfwInit();

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
	Shader shaderProgram("default.vert", "default.frag", "agentSimulation.comp", "trailProcessing.comp");

	shaderProgram.printMaxGroups();

	// Gets ID of uniform called "iTime" and "iResolution"
	GLuint timeUniformID = glGetUniformLocation(shaderProgram.ID, "iTime");
	GLuint timeUniformComputeID = glGetUniformLocation(shaderProgram.computeID, "iTime");
	GLuint timeUniformComputeID2 = glGetUniformLocation(shaderProgram.computeID2, "iTime");
	float iTime;
	GLuint resolutionUniformID = glGetUniformLocation(shaderProgram.ID, "iResolution");
	GLuint numAgentsUniformID = glGetUniformLocation(shaderProgram.computeID, "numAgents");
	GLuint highEnergyUniformID = glGetUniformLocation(shaderProgram.computeID, "highEnergy");
	GLuint bassEnergyuniformID = glGetUniformLocation(shaderProgram.computeID, "iBass");

	GLuint diffuseWeightID = glGetUniformLocation(shaderProgram.computeID2, "diffuseWeight");
	GLuint evaporationRateID = glGetUniformLocation(shaderProgram.computeID2, "evaporationRate");
	GLuint bassEnergyuniformID2 = glGetUniformLocation(shaderProgram.computeID2, "iBass");

	GLuint agentsBuffer;
	glGenBuffers(1, &agentsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, agentsBuffer);
	// Bind the buffer to binding point 1
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, agentsBuffer);

	// Allocate memory for the agents (numAgents * sizeof(Agent))
	glBufferData(GL_SHADER_STORAGE_BUFFER, numAgents * sizeof(Agent), nullptr, GL_DYNAMIC_DRAW);

	std::vector<Agent> agentsVector;
	//agentsVector = spawnAgentsOnCircle(numAgents, 512.0f);
	//agentsVector = spawnAgentsOnCircleRandom(numAgents, 350.0f);
	agentsVector = spawnAgentsInsideCircleRandom(numAgents, 20.0f);

	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, agentsVector.size() * sizeof(Agent), agentsVector.data());


	GLuint trailMapBuffer;
	glGenBuffers(1, &trailMapBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, trailMapBuffer);
	std::vector<float> trailMap(SCREEN_WIDTH* SCREEN_HEIGHT, 0.0f);
	// Allocate memory for the agents (numAgents * sizeof(Agent))
	glBufferData(GL_SHADER_STORAGE_BUFFER, trailMap.size() * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	// Bind the buffer to binding point 3
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, trailMapBuffer);


	GLuint agentSettingsBuffer;
	glGenBuffers(1, &agentSettingsBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, agentSettingsBuffer);
	// Allocate memory for the agents (sizeof(Agent))
	glBufferData(GL_UNIFORM_BUFFER, sizeof(AgentSettings), &agentSettings, GL_STATIC_DRAW);

	// Bind the buffer to binding point 2
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, agentSettingsBuffer);

	int framebufferWidth, framebufferHeight;
	// Restart the GLFW timer and start main while loop 
	glfwSetTime(0.0);
	std::cout << numAgents / 128.0f << "  " << ceil(numAgents / 128.0f) << std::endl;

	// load an audio buffer from a sound file
	const sf::SoundBuffer buffer("audio/16mm.wav");
	std::cout << buffer.getSampleRate() << std::endl;
	std::cout << buffer.getChannelCount() << std::endl;
	AudioStream music;
	music.load(buffer);
	music.play();
	//sf::Music music("audio/16mm.wav");
	std::vector<float> amplitudes;
	//music.play();
	double lastTriggerTime{ 1.0 }; // Non value zero to let time to the music object to fill its first fftBuffer
	float maxBassEnergy = 0.0f;
	float bassEnergy{ 0.0 };
	float maxHighEnergy = 0.0f;
	float highEnergy{ 0.0 };
	while (!glfwWindowShouldClose(window))
	{

		// Specify the color of the background
		
		if (glfwGetTime() - lastTriggerTime > 0.007f) {
			//std::cout << lastTriggerTime << std::endl;
			amplitudes = computeFrequencyAmplitudes(music.fftBuffer);
			bassEnergy = computeBassEnergy(amplitudes);
			highEnergy = computeHighEnergy(amplitudes);
			//std::cout << bassEnergy << "  " << maxBassEnergy << std::endl;
			maxBassEnergy = (bassEnergy > maxBassEnergy) ? bassEnergy : maxBassEnergy;
			maxHighEnergy = (highEnergy > maxHighEnergy) ? highEnergy : maxHighEnergy;
			std::cout << highEnergy << "  " << maxHighEnergy << std::endl;
			lastTriggerTime = glfwGetTime();
		}
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		iTime = float(glfwGetTime());
		shaderProgram.ActivateCompute();
		glUniform1f(timeUniformComputeID, iTime);
		glUniform1i(numAgentsUniformID, numAgents);
		glUniform1f(highEnergyUniformID, highEnergy/maxHighEnergy);
		glUniform1f(bassEnergyuniformID, bassEnergy/maxBassEnergy);
		glDispatchCompute(ceil(numAgents / 128.0f), 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		shaderProgram.ActivateCompute2();
		glUniform1f(diffuseWeightID, diffuseWeight);
		glUniform1f(evaporationRateID, evaporationRate);
		glUniform1f(bassEnergyuniformID2, bassEnergy);
		glUniform1f(timeUniformComputeID2, iTime);
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
