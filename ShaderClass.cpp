
/*
#include"shaderclass.h"
#include<iostream>


std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

shader::shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

		const char* vertexShaderSource = vertexCode.c_str();
		const char* fragmentShaderSource = fragmentCode.c_str();


		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);	//creates reference for vertex shader
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);	//passes the source code to the reference
		glCompileShader(vertexShader);

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	//creates reference for fragment shader
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);	//passes the source code to the reference
		glCompileShader(fragmentShader);

		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);	//attaches the shaders to the shader program
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);	//links the shaders to the shader program

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);		//the shaders are already in the program, so their objects are now useless and can be deleted

}

void shader::Activate()
{
	glUseProgram(ID);
}

void shader::Delete()
{
	glDeleteProgram(ID);
}

*/