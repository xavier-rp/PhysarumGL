#include "utils.h"

const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 1024;
const int numAgents = 10000;

const float PI = 3.1415926535f;
const bool vSync = true;

const float diffuseWeight = 0.2f;
const float evaporationRate = 0.01f;

AgentSettings agentSettings = 
{
	22.5f * PI / 180.f,			//float sensorAngleOffset
	10,							//int sensorOffsetDistance
	1,							//int sensorWidth
	0.2							//float turningSpeed
};

// Vertices coordinates (first 3) and texture coordinates (last 2)
GLfloat vertices[] =
{
	-1.0f, -1.0f , 0.0f, 0.0f, 0.0f,
	-1.0f,  1.0f , 0.0f, 0.0f, 1.0f,
	 1.0f,  1.0f , 0.0f, 1.0f, 1.0f,
	 1.0f, -1.0f , 0.0f, 1.0f, 0.0f
};

// Indices for vertices order
GLuint indices[] =
{
	0, 3, 1,
	1, 2, 3
};

std::vector<Agent> spawnAgentsOnCircle(const int numAgents, float radius) {

	std::vector<Agent> agentsVector(numAgents);

	for (int i = 0; i < numAgents; ++i) {
		agentsVector[i].pos[0] = radius * std::cos(static_cast<float>(i) * 2.0f * PI / numAgents) + static_cast<float>(SCREEN_WIDTH / 2);
		agentsVector[i].pos[1] = radius * std::sin(static_cast<float>(i) * 2.0f * PI / numAgents) + static_cast<float>(SCREEN_HEIGHT / 2); 
		agentsVector[i].angle = static_cast<float>(i) * 2.0f * PI / numAgents;
		agentsVector[i].velocity = 0.5f;
	}

	return agentsVector;
}

std::vector<Agent> spawnAgentsOnCircleRandom(const int numAgents, float radius) {

	std::vector<Agent> agentsVector(numAgents);

	std::random_device rd{};
	std::mt19937 gen{ rd() };

	std::uniform_real_distribution<float> dis_theta{ 0.0, 2 * PI };
	std::uniform_real_distribution<float> dis_velocity{ 0.5, 0.55 };

	for (int i = 0; i < numAgents; ++i) {
		agentsVector[i].pos[0] = radius * std::cos(static_cast<float>(i) * 2.0f * PI / numAgents) + static_cast<float>(SCREEN_WIDTH / 2);  // Start at (0, 0)
		agentsVector[i].pos[1] = radius * std::sin(static_cast<float>(i) * 2.0f * PI / numAgents) + static_cast<float>(SCREEN_HEIGHT / 2);  // Start at (0, 0)
		agentsVector[i].angle = dis_theta(gen);
		agentsVector[i].velocity = dis_velocity(gen);
	}

	return agentsVector;
}

std::vector<Agent> spawnAgentsInsideCircleRandom(const int numAgents, float maxRadius) {

	std::vector<Agent> agentsVector(numAgents);

	std::random_device rd{};
	std::mt19937 gen{ rd() };

	std::uniform_real_distribution<float> dis_theta{ 0.0, 2 * PI };
	std::uniform_real_distribution<float> dis_radius{ 0.0, maxRadius};
	std::uniform_real_distribution<float> dis_velocity{ 0.2, 0.3 };

	float radius;
	for (int i = 0; i < numAgents; ++i) {
		radius = dis_radius(gen);
		agentsVector[i].pos[0] = radius * std::cos(static_cast<float>(i) * 2.0f * PI / numAgents) + static_cast<float>(SCREEN_WIDTH / 2);  // Start at (0, 0)
		agentsVector[i].pos[1] = radius * std::sin(static_cast<float>(i) * 2.0f * PI / numAgents) + static_cast<float>(SCREEN_HEIGHT / 2);  // Start at (0, 0)
		agentsVector[i].angle = dis_theta(gen);
		agentsVector[i].velocity = dis_velocity(gen);
	}

	return agentsVector;
}