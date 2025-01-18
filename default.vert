#version 460 core

// Positions/Coordinates
layout (location = 0) in vec3 pos;

// Texture Coordinates
layout (location = 1) in vec2 aTex;
// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;


void main()
{
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
	texCoord = aTex;
}