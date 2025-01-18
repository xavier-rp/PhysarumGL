
#include"VAO.h"

// Constructor that generates a VAO ID
VAO::VAO()
{
	glCreateVertexArrays(1, &ID);
}

void VAO::EnableAttrib()
{
	glEnableVertexArrayAttrib(ID, 0);
	glVertexArrayAttribBinding(ID, 0, 0);
	glVertexArrayAttribFormat(ID, 0, 3, GL_FLOAT, GL_FALSE, 0);

	//This attribute is actually the texture coordinates
	glEnableVertexArrayAttrib(ID, 1);
	glVertexArrayAttribBinding(ID, 1, 0);
	glVertexArrayAttribFormat(ID, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
}

// Links a VBO Attribute such as a position or color to the VAO
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

// Binds the VAO
void VAO::Bind()
{
	glBindVertexArray(ID);
}

// Unbinds the VAO
void VAO::Unbind()
{
	glBindVertexArray(0);
}

// Deletes the VAO
void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}
