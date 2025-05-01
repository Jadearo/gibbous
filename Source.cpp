#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "VAO.h"
#include "VBO.h"
#include "Matrix4.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

const char* vertexShaderSource = "#version 330 core\n" //works with openGL 3.3 core profile
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 transform;\n"	//location for transform matrix to be loade
"uniform mat4 perspective;\n"	//location for the perspective matrix to be loaded
"uniform mat4 view;\n"	// view matrix
"void main()\n"
"{\n"
"   gl_Position = perspective * view * transform * vec4(aPos, 1.0);\n"	//applies all matrix multiplicaitons to final vertex positions
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
"}\0";

std::array<GLfloat, 24> getVertices(GLfloat x, GLfloat y, GLfloat z) {
	return {
		x,y,z,				//0
		x,y - 1,z,			//1
		x + 1,y,z,			//2
		x + 1,y - 1,z,		//3	
		x,y,z - 1,			//4
		x,y - 1,z - 1,		//5
		x + 1,y,z - 1,		//6
		x + 1,y - 1,z - 1,	//7
	};
}
std::vector<GLfloat> genChunk(GLint x,GLint y,GLint z)
{
	const GLint chunkLength = 8;		//the length of a chunk will always be 8
	GLfloat offset = 0;
	//the co-ordinates x,y,z are measured in terms of chunks
	//*8 makes it so the chunks are offset correctly:
	x *= chunkLength;		
	y *= chunkLength;
	z *= chunkLength;
	std::vector<GLfloat> vertices;	//will hold the vertices for this run of the function
	vertices.reserve(chunkLength * chunkLength * chunkLength * 24);	//Reserves data for the new vector array
	for (GLfloat i = x; i < chunkLength+x; i++)	//iterates through every x co-ordinate
	{
		for (GLfloat o = y; o < chunkLength+y; o++)	//every y co-ordinate
		{
			for (GLfloat p = z; p < chunkLength+z; p++)	//every z co-ordinate
			{
				std::array<GLfloat, 24> temp = getVertices(i, -o, p); //use static array for small data for speed
				vertices.insert(vertices.end(), temp.begin(), temp.end());	//<vector> allows for insercion of arrays
				/*std::cout << "vertices loop number " << offset / 24 << " finished\n";
				for (float n = offset; n < offset +24; n++)
				{
					std::cout << "vertex no. " << n << " : " << vertices[n] << "\n";
				}
				offset += 24; */
				

			}

		}
	}
	return vertices;	//this returns the vertices for all the voxels in the chunk
}

std::vector<GLuint> genIndices(GLuint total)
{
	const int chunkLength = 8;
	std::vector<GLuint> indices;
	int offset = 0;
	int indexoffset = 0;
	for (int n = 0; n < chunkLength*chunkLength*chunkLength*total; n++)
	{
		GLuint currentindices[] =	//temporary indecies array
		{
			//front face
			offset + 0,  offset + 1,  offset + 2,
			offset + 2,  offset + 1,  offset + 3,
			//bottom face
			offset + 1,  offset + 5,  offset + 3,
			offset + 3,  offset + 7,  offset + 5,
			//back face
			offset + 7,  offset + 5,  offset + 4,
			offset + 4,  offset + 6,  offset + 7,
			//top face
			offset + 4,  offset + 6,  offset + 2,
			offset + 2,  offset + 0,  offset + 4,
			//left face
			offset + 4,  offset + 5,  offset + 1,
			offset + 1,  offset + 0,  offset + 4,
			//right face
			offset + 2,  offset + 3,  offset + 7,
			offset + 7,  offset + 6,  offset + 2
		
		};
		indices.insert(indices.end(), std::begin(currentindices), std::end(currentindices));
		offset += 8;	
		/*std::cout << "loop number " << offset / 8 << " finished\n";
		for (float n = offset*4.5-36; n < offset * 4.5; n++)
		{
			std::cout<<"no. " << n << " : " << indices[n] << "\n";
		}*/
		//increase offset to reference vertices for next voxel in chunk
	}
		return indices;
}

std::vector<GLfloat> bulkGenChunks(GLint xdist, GLint ydist, GLint zdist)
{
	std::vector<GLfloat> vertices, tempVertices;
	for (int x = 0; x < xdist; x++)
	{
		for (int y = 0; y < ydist; y++)
		{
			for (int z = 0; z < zdist; z++)
			{
				tempVertices = genChunk(x, y, z);	//Temporarily get vertcies for a chunk then insert each
				vertices.insert(vertices.end(), std::begin(tempVertices), tempVertices.end());
				std::cout << "the function looped   (" << x << "," << y << "," << z<<")\n";	//shows location the iteration creates the chunk
			}
		}
	}
	return vertices;
}

int main()
{
	
	glfwInit();	//loads glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);		//sets max version of openGL to 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		//Min ver to 3 as well
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//Specifies we are using the core profile, we are not using the compatibility libraries for this project

	GLFWwindow* window = glfwCreateWindow(640, 360, "Main", NULL, NULL); //creates a 640x480 window with name "Main"
	glfwMakeContextCurrent(window);
	gladLoadGL();					//load glad so we can use graphics drivers
	glViewport(0, 0, 640, 360);		//specifies the viewport or the framebuffer size, this is seperate from the window size but should be made the same size
	glfwSwapInterval(1);	//enables vsync, locks framerate to monitor refreshrate
	GLint xdist,ydist,zdist = 1;
	//nuklear
	struct nk_context *nuklearMenuWindow = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);	//applies a menu to GLFW window
	//INSTALL_CALLBACKS makes the struct handle all userinput
	struct nk_font_atlas* atlas;
	nk_glfw3_font_stash_begin(&atlas);	//enables the font
	nk_glfw3_font_stash_end();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	bool menuOpen = true;
	while (menuOpen)
	{
		glfwPollEvents();		//processes events while the window is not closed, when this finishes the program terminates

		nk_glfw3_new_frame();
		if (nk_begin(nuklearMenuWindow, "Nuklear Window", nk_rect(0, 0, 500, 500),
			NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE)) {

			nk_layout_row_dynamic(nuklearMenuWindow, 35, 4);	//defines layout
			if (nk_button_label(nuklearMenuWindow, "Test Button")) {	//creates the button, true if clicked
				std::cout << "\nThe button was pressed";
			}

			// the second button
			nk_layout_row_dynamic(nuklearMenuWindow, 25, 1); // creates another row, 1 button per line
			if (nk_button_label(nuklearMenuWindow, "Start")) {
				std::cout << "\nbutton 2 pressed";
				menuOpen = false;	//flag closes the menu
			}

			// slider codeS
			nk_layout_row_dynamic(nuklearMenuWindow, 25, 1); // Creates a row with 1 element per line
			static GLint slider_xvalue = 50; //initial slider position
			if (nk_slider_int(nuklearMenuWindow, 0, &slider_xvalue, 100, 1)) {
				std::cout << "Slider X Value: " << slider_xvalue << "\n";
				xdist = slider_xvalue;
			}
			nk_layout_row_dynamic(nuklearMenuWindow, 25, 1); // Creates a row with 1 element per line
			static GLint slider_Yvalue = 50; //initial slider position
			if (nk_slider_int(nuklearMenuWindow, 0, &slider_Yvalue, 100, 1)) {
				std::cout << "Slider Y Value: " << slider_Yvalue << "\n";
				ydist = slider_Yvalue;
			}
			nk_layout_row_dynamic(nuklearMenuWindow, 25, 1); // Creates a row with 1 element per line
			static GLint slider_Zvalue = 50; //initial slider position
			if (nk_slider_int(nuklearMenuWindow, 0, &slider_Zvalue, 100, 1)) {
				std::cout << "Slider Z Value: " << slider_Zvalue << "\n";
				zdist = slider_Zvalue;
			}


			nk_end(nuklearMenuWindow);
		}
		//----------------------
		nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
		glfwSwapBuffers(window);			//displays the drawn image
		glClear(GL_COLOR_BUFFER_BIT);			//clears the back buffer, the new colour gets applied

	}

	const int chunkLength = 8;
	//defines the amount of chunks to generate in the x,y,z directions
	if (xdist < 0 || ydist < 0 || zdist < 0)
	{
		xdist = std::abs(xdist);
		ydist = std::abs(ydist);
		zdist = std::abs(zdist);
		//makes any negative values positive to prevent crash
	}

	const GLuint noOfChunks = xdist * ydist * zdist;
	std::vector<GLuint> indices = genIndices(noOfChunks);			//generates indices at a chunk level
	std::vector<GLfloat> vertices = bulkGenChunks(xdist,ydist,zdist);

	std::cout << "total " << chunkLength * chunkLength * chunkLength * noOfChunks << " voxels\n";


	std::cout << "\nsize of vertices:" << vertices.size()/3 << "\n size of indices" << indices.size();

	/*for (int n = 0; n<vertices.size(); n = n + 3)
	{
		std::cout << "(" << vertices[n] << "," << vertices[n + 1] << "," << vertices[n + 2] << ")\n";
	}*/
	
	
		
		
	//indices for a face with 4 verticies



	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);	//creates the reference for the vertex shader
	glShaderSource(vertShader, 1, &vertexShaderSource, NULL);	//gives the reference the data of the actual shader
	glCompileShader(vertShader);	//compiles the vertex shader, is now ready to be attached to the context

	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);  
	glShaderSource(fragShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragShader);

	GLuint shaderProgram = glCreateProgram();	//creates shaderprogram, holds the compiled shaders

	glAttachShader(shaderProgram, vertShader);	//attaches vertex shader
	glAttachShader(shaderProgram, fragShader);	//attaches fragment shader
	//the vertex shader and fragment shader need to be put into a single shaderprogram as they need to interact at certain points

	glLinkProgram(shaderProgram);
	//the shaders are now attached to the program, their references are no longer being used so they can be deleted
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	VAO VAO1;	//initalise VAO
	VAO1.Bind();	//bind the VAO


	GLuint EBO;		//reference for the EBO
	glGenBuffers(1, &EBO);	//generates the buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);	//binds it and specifies it is an EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);	//passes the data




	VBO VBO1(vertices);	//create VBO and pass the vertices array
	VAO1.LinkVBO(VBO1,0);	//links the VBO to the VAO
	VAO1.Unbind();	//unbind the VAO as this is the last time we alter it

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	int refreshRate = mode->refreshRate;
	double lastTime = glfwGetTime();

	GLfloat x = 0;
	GLfloat transX = 0;
	GLfloat transY = 0;
	GLfloat rotation = 0;
	GLfloat z_pos = -10;
	GLfloat x_pos = 0;
	GLfloat y_pos = 0;
	const GLfloat increment = 10;
	GLdouble cursorx, cursory;
	glm::mat4 identityMat(1.0f);	//creates 4x4 matrix transform, input 1.0f makes it an identity matrix
	glm::mat4 perspectiveMat = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
	//std::cout << "drawing function reached";
	
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	while (!glfwWindowShouldClose(window))
	{
		glfwGetCursorPos(window, &cursorx, &cursory);
		std::cout << "go " << x_pos << " , " << y_pos << " , " << z_pos<<"\n";
		glfwPollEvents();
		glfwSwapBuffers(window);			//displays the drawn image
		glUseProgram(shaderProgram);			//multiples shader programs may be used, so this specifies which one
		VAO1.Bind();	//binds the VAO
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			//clears the back buffer, the new colour gets applied
		x = x + 0.01;
		transX = transX + 0.0001;	//occilate x-pos of all vertices
		transY = transY + 0.00005;	//occilate y-pos of all vertices
		rotation = rotation + 0.0055f;	//incremenet the rotation of all objects around rotation line

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		int movementFoward = glfwGetKey(window, GLFW_KEY_W); //polls w-key inputntFoward == GLFW_PRESS)
		if(movementFoward)
		{
			z_pos += increment * deltaTime;	//moves camera foward
		}
		int movementBackwards = glfwGetKey(window, GLFW_KEY_S);
		if (movementBackwards)
		{
			z_pos -= increment * deltaTime;	//moves camera back
		}
		int movementRight = glfwGetKey(window, GLFW_KEY_D);
		if (movementRight)
		{
			x_pos -= increment * deltaTime;	//moves camera right 
		}
		int movementLeft = glfwGetKey(window, GLFW_KEY_A);
		if (movementLeft)
		{
			x_pos += increment * deltaTime;	//moves camera left
		}
		int movementUp = glfwGetKey(window, GLFW_KEY_SPACE);
		if (movementUp)
		{
			y_pos += increment * deltaTime;	//moves camera up
		}
		int movementDown = glfwGetKey(window, GLFW_KEY_C);
		if (movementDown)
		{
			y_pos -= increment * deltaTime;	//moves camera down
		}
		cursorx = cursorx / 1000;
		cursory = cursory / 1000;

		glm::vec3 transformVec(0.0f, 0.0f, 0.0f);	//creates 3d vector transform, parameters are in the x,y,z directions
		/*std::cout << "(";
		std::cout << x_pos;
		std::cout << ",";
		std::cout << z_pos;
		std::cout << ")\n";*/
		glm::vec3 rotationVec(1.0f, 0.0f, 0.0f);

		glm::mat4 transformMat = glm::translate(identityMat, transformVec); //creates a new matrix, tranform, by putting the values from the vector in the apropiate location in the matrix to perform a translation
		//transformMat = glm::rotate(transformMat, rotation, rotationVec);

		glm::vec3 cameraPosition(x_pos, y_pos, z_pos); //position of the camera
		glm::vec3 cameraTarget(x_pos - cursorx, -cursory +y_pos, z_pos+1);	//points the camera 1 unit infront itself and is offset by the cursor
		glm::vec3 upDirection(0.0f, 1.0f, 0.0f);	//take the y-direction as up

		glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upDirection);

		//transform matrix
		GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");	//retrives the location of the transformation function in the shader
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformMat));	//sends all the float values of the matrix to the transform function in the shader
		//perspective matrix
		GLuint perspectiveLoc = glGetUniformLocation(shaderProgram, "perspective");	//retrives the location of the perspective function in the shader
		glUniformMatrix4fv(perspectiveLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMat));	//sends all the float values of the matrix to the perspective function in the shader
		//view matrix
		GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");	//location of the view matrix functionality in the vertex shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		


		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);	//draws with index array
	};
	glfwDestroyWindow(window);
	glfwTerminate();
}