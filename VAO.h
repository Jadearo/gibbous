#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
	public:
		GLuint ID;	//used to identify the VAO, initially null
		VAO()
		{
			glGenVertexArrays(1, &ID);		//constructor gives the VAO the unique ID and VAO attributes
		}

		void LinkVBO(VBO& VBO, GLuint Layout)		//links VBO
		{
			Bind();
			glVertexAttribPointer(Layout, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);	//defines the VBO
			glEnableVertexAttribArray(Layout);	//enables the attribute
			Unbind();
		}
		void Bind()	//binds the VAO
		{
			glBindVertexArray(ID);
		}
		void Unbind()	//unbinds the VAO
		{
			glBindVertexArray(0);	//sets the current binded VAO to id 0, which is non existant
		}
		void Delete()	//deletes the VAO
		{
			glDeleteVertexArrays(1, &ID);
			ID = 0;
		}

};

#endif