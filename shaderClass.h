
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	// Reference ID of the Shader Program
	GLuint ID;
	GLuint computeID;
	GLuint computeID2;
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const char* vertexFile, const char* fragmentFile, const char* computeFile, const char* computeFile2);

	// Activates the Shader Program
	void Activate();
	// Activates the Compute Shader Program
	void ActivateCompute();
	// Activates the Compute Shader Program
	void ActivateCompute2();
	// Deletes both Shader Programs
	void Delete();

	void printMaxGroups();
private:
	// Checks if the different Shaders have compiled properly
	void compileErrors(unsigned int shader, const char* type);
};


#endif
