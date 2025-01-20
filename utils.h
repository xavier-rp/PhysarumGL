#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <glad/glad.h>

extern const unsigned int SCREEN_WIDTH;
extern const unsigned int SCREEN_HEIGHT;
extern const int numAgents;
extern const float PI;

extern GLfloat vertices[20];
extern GLuint indices[6];

/* UPDATES OF THESE STRUCTS HAVE TO BE IMPLEMENTED IN THE SHADERS AS WELL*/
struct Agent
{
	float pos[2];
	float angle;
	float velocity;
};

struct AgentSettings {
	float sensorAngleOffset;
	int sensorOffsetDistance;
	int sensorWidth;
	float turningSpeed;
};

extern AgentSettings agentSettings;

std::vector<Agent> spawnAgentsOnCircle(const int numAgents, float radius);
std::vector<Agent> spawnAgentsOnCircleRandom(const int numAgents, float radius);