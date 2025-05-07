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
#include <ChString.h>
#include <string>
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

std::array<GLfloat, 24> getVertices(GLfloat x, GLfloat y, GLfloat z) {	//generates the vertices for a voxel
	return {
		x,y,z,				//vertex 0
		x,y - 1,z,			//vertex 1
		x + 1,y,z,			//vertex 2
		x + 1,y - 1,z,		//vertex 3	
		x,y,z - 1,			//vertex 4
		x,y - 1,z - 1,		//vertex 5
		x + 1,y,z - 1,		//vertex 6
		x + 1,y - 1,z - 1,	//vertex 7
	};
}
std::vector<GLfloat> genChunk(GLint x,GLint y,GLint z)	//recursively calls getvertices to load the data for a whole chunk
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

			}

		}
	}
	return vertices;	//this returns the vertices for all the voxels in the chunk
}

std::vector<GLuint> genIndices(GLuint total)	//generates the indices for each chunk
{
	const int chunkLength = 8;	//the length of a chunk in each direction
	std::vector<GLuint> indices;	//stores indices
	int offset = 0;		//offsets the values of the indcies
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
		//inset values into the vector
		offset += 8;	

	}
		return indices;
}

std::vector<GLfloat> bulkGenChunks(GLint xdist, GLint ydist, GLint zdist)	
//function generates multiple chunks at once calling the genChunk() function recursively
{
	std::vector<GLfloat> vertices, tempVertices;	//declare 2 vectors
	for (int x = 0; x < xdist; x++)					//iterate through every x,y,z
	{
		for (int y = 0; y < ydist; y++)
		{
			for (int z = 0; z < zdist; z++)	
			{
				tempVertices = genChunk(x, y, z);	//Temporarily get vertcies for a chunk then insert each
				vertices.insert(vertices.end(), std::begin(tempVertices), tempVertices.end());	//insert into the vector
				std::cout << "the function looped   (" << x << "," << y << "," << z<<")\n";	//shows location the iteration creates the chunk
			}
		}
	}
	return vertices;
}

bool tryParseInt(const std::string& stringInput, int& outValue) {	//tryparse function to convert strings to intagers
	try {
		size_t stringSize;		//stores the size of the parsed string
		outValue = std::stoi(stringInput, &stringSize);	//attempts to convert string
		return stringSize == stringInput.size() ;		//returns true if the string and parsed string are the same size
	}
	catch (const std::invalid_argument&) {	//for invalid input
		return false;
	}
	catch (const std::out_of_range&) {	//for values too large
		return false;
	}
}

int main()
{	
	glfwInit();	//loads glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);		//sets max version of openGL to 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		//Min ver to 3 as well
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	
	//Specifies we are using the core profile, we are not using the compatibility libraries for this project

	GLFWwindow* window = glfwCreateWindow(640, 480, "Main", NULL, NULL); //creates a 640x480 window with name "Main"
	glfwMakeContextCurrent(window);
	gladLoadGL();					//load glad so we can use graphics drivers
	glViewport(0, 0, 640, 480);		
	//specifies the viewport or the framebuffer size, this is seperate from the window size but should be made the same size
	glfwSwapInterval(1);	//enables vsync, locks framerate to monitor refreshrate
	GLint xdist = 1;	//inital values for the x,y,z directions
	GLint ydist = 1;
	GLint zdist = 1;
	//nuklear code for UI below
	struct nk_context *nuklearMenuWindow = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);	//applies a menu to GLFW window
	//INSTALL_CALLBACKS makes the struct handle all userinput
	struct nk_font_atlas* atlas;		//create pointer to the font atlas so it can be used
	nk_glfw3_font_stash_begin(&atlas);	//enables the font
	nk_glfw3_font_stash_end();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	std::string xVal = "1";		//initial values for the text displaying the values of the sliders representing the xdist,ydist and zdist
	std::string yVal = "1";
	std::string zVal = "1";
	bool menuOpen = true;		//creates flag, closes window when set to false
	char userInput[128] = "480";	//inital resolution value in textbox, can be changed by user
	int intUserInput = 480;
	bool failedParse = false;		//sets the flag for a failed converstion to int to false, when set to true the error text displays
	while (menuOpen)
	{
		glfwPollEvents();		//processes events while the window is not closed, when this finishes the program terminates

		nk_glfw3_new_frame();	//code runs every frame the window is open
		if (nk_begin(nuklearMenuWindow, "Nuklear Window", nk_rect(0, 0, 500, 500),	//creates GUI window
			NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE)) {

			nk_layout_row_dynamic(nuklearMenuWindow, 35, 4);	//defines layout for UI elements
			if (nk_button_label(nuklearMenuWindow, "Test Button")) {	//creates the button, returns true if clicked and runs embedded code
				std::cout << userInput;
			}
			nk_layout_row_dynamic(nuklearMenuWindow, 35, 4);	//changes the layout again

			// the second button
			nk_layout_row_dynamic(nuklearMenuWindow, 25, 1); // creates another row, 1 button per line
			if (nk_button_label(nuklearMenuWindow, "Start")) {		//creates start button
				if (tryParseInt(userInput, intUserInput))			//converts to int using my function, if sucessful program starts, otherwise error is shown.
				{
				menuOpen = false;	//flag closes the menu
				}
				else
				{
				failedParse = true;	//flag returns an error
				}
			}
			if (failedParse)		//when the flag is set to true, error text displays
			{
				nk_label(nuklearMenuWindow, "ERROR! INVALID RESOLUTION", NK_TEXT_CENTERED);	//code for creating error text
			}

			// slider code
			nk_layout_row_dynamic(nuklearMenuWindow, 25, 1); // Creates a row with 1 element per line
			static GLint slider_Xvalue = 1; //initial slider position
			if (nk_slider_int(nuklearMenuWindow, 0, &slider_Xvalue, 100, 1)) {
				std::cout << "Slider X Value: " << slider_Xvalue << "\n";
				xdist = slider_Xvalue;
				char buffer[20];
				sprintf(buffer, "%d", slider_Xvalue);
				xVal=buffer;
			}
			nk_layout_row_dynamic(nuklearMenuWindow, 25, 2); // Creates a row with 2 elements per line 
			nk_label(nuklearMenuWindow, "Chunk distance in x-direction:", NK_TEXT_CENTERED);
			nk_label(nuklearMenuWindow, xVal.c_str(), NK_TEXT_LEFT);
			nk_layout_row_dynamic(nuklearMenuWindow, 25, 1);
			static GLint slider_Yvalue = 1; //initial slider position
			if (nk_slider_int(nuklearMenuWindow, 0, &slider_Yvalue, 100, 1)) {
				std::cout << "Slider Y Value: " << slider_Yvalue << "\n";
				ydist = slider_Yvalue;
				char buffer[20];
				sprintf(buffer, "%d", slider_Yvalue);
				yVal = buffer;
			}
			nk_layout_row_dynamic(nuklearMenuWindow, 25, 2); // Creates a row with 2 elements per line 
			nk_label(nuklearMenuWindow, "Chunk distance in y-direction:", NK_TEXT_CENTERED);
			nk_label(nuklearMenuWindow, yVal.c_str(), NK_TEXT_LEFT);
			nk_layout_row_dynamic(nuklearMenuWindow, 25, 1); // Creates a row with 1 element per line
			static GLint slider_Zvalue = 1; //initial slider position
			if (nk_slider_int(nuklearMenuWindow, 0, &slider_Zvalue, 100, 1)) {
				std::cout << "Slider Z Value: " << slider_Zvalue << "\n";
				zdist = slider_Zvalue;
				char buffer[20];
				sprintf(buffer, "%d", slider_Zvalue);
				zVal = buffer;
			}
			nk_layout_row_dynamic(nuklearMenuWindow, 25, 2); // Creates a row with 2 elements per line 
			nk_label(nuklearMenuWindow, "Chunk distance in z-direction:", NK_TEXT_CENTERED);
			nk_label(nuklearMenuWindow, zVal.c_str(), NK_TEXT_LEFT);
			
				nk_layout_row_dynamic(nuklearMenuWindow, 30, 1);

				nk_label(nuklearMenuWindow, "Enter some text:", NK_TEXT_LEFT);

				nk_edit_string_zero_terminated(nuklearMenuWindow,NK_EDIT_FIELD | NK_EDIT_CLIPBOARD,
					userInput, 128, nk_filter_default);	//craetes a text-box with the parameters with attributes
			

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
		ydist = std::abs(ydist);X
		zdist = std::abs(zdist);
		//makes any negative values positive to prevent crash
	}
	glViewport(0, 0, (intUserInput*16)/9, intUserInput);
	glfwSetWindowSize( window, (intUserInput * 16) / 9, intUserInput);
	const GLuint noOfChunks = xdist * ydist * zdist;
	std::vector<GLuint> indices = genIndices(noOfChunks);			//generates indices at a chunk level
	std::vector<GLfloat> vertices = bulkGenChunks(xdist,ydist,zdist);

	std::cout << "total " << chunkLength * chunkLength * chunkLength * noOfChunks << " voxels\n";		
	std::cout << "\nsize of vertices:" << vertices.size()/3 << "\n size of indices" << indices.size();
	//logs to the console the size of the vertices and the number of voxels

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);	//creates the reference for the vertex shader
	glShaderSource(vertShader, 1, &vertexShaderSource, NULL);	//gives the reference the data of the actual shader
	glCompileShader(vertShader);	//compiles the vertex shader, is now ready to be attached to the context

	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);  //same process as before
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
	//transformation and movement variables
	GLfloat x = 0;
	GLfloat transX = 0;
	GLfloat transY = 0;
	GLfloat rotation = 0;
	GLfloat z_pos = -10;
	GLfloat x_pos = 0;
	GLfloat y_pos = 0;
	const GLfloat increment = 10;
	GLdouble cursorx, cursory;		//cursor position
	glm::mat4 identityMat(1.0f);	//creates 4x4 matrix transform, input 1.0f makes it an identity matrix
	glm::mat4 perspectiveMat = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
	//std::cout << "drawing function reached";
	
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	while (!glfwWindowShouldClose(window))	//loops every frame simulation is open
	{
		glfwGetCursorPos(window, &cursorx, &cursory);
		std::cout << "position " << x_pos << " , " << y_pos << " , " << z_pos<<"\n";
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

		glm::vec3 transformVec(0.0f, 0.0f, 0.0f);	
		//creates 3d vector transform, parameters are in the x,y,z directions
		glm::vec3 rotationVec(1.0f, 0.0f, 0.0f);

		glm::mat4 transformMat = glm::translate(identityMat, transformVec);
		//creates a new matrix, tranform, by putting the values from the vector in the apropiate location in the matrix to perform a translation
		//transformMat = glm::rotate(transformMat, rotation, rotationVec);

		glm::vec3 cameraPosition(x_pos, y_pos, z_pos); //position of the camera
		glm::vec3 cameraTarget(x_pos - cursorx, -cursory +y_pos, z_pos+1);	
		//points the camera 1 unit infront itself and is offset by the cursor
		glm::vec3 upDirection(0.0f, 1.0f, 0.0f);	//take the y-direction as up

		glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upDirection);

		//transform matrix
		GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");	
		//retrives the location of the transformation function in the shader
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformMat));	
		//sends all the float values of the matrix to the transform function in the shader
		//perspective matrix
		GLuint perspectiveLoc = glGetUniformLocation(shaderProgram, "perspective");	
		//retrives the location of the perspective function in the shader
		glUniformMatrix4fv(perspectiveLoc, 1, GL_FALSE, glm::value_ptr(perspectiveMat));	
		//sends all the float values of the matrix to the perspective function in the shader
		//view matrix
		GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");	
		//location of the view matrix functionality in the vertex shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		


		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);	//draws with index array
	};
	glfwDestroyWindow(window);
	glfwTerminate();
}