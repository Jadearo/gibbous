#ifndef VBO_CLASS_H
#define VBO_CLASS_H
#include <glad/glad.h>
#include <vector>
#include <array>

class VBO
{
	public:
		GLuint ID;	//used to identify the VBO, is given a value in the constructor

		VBO(const std::vector<GLfloat>& vertices)
		{
			glGenBuffers(1, &ID);
			glBindBuffer(GL_ARRAY_BUFFER, ID);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
			//generates and binds the VBO, and optimises it for drawing static attributes
		};

		void Bind()	//binds the VBO
		{
			glBindBuffer(GL_ARRAY_BUFFER, ID);
		};

		void Unbind()	//unbinds the VBO by setting the bound ID to 0 which is linked to nothing
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		};
		void Delete()	//deletes the VBO
		{
			glDeleteBuffers(1, &ID);
		};
};

#endif
