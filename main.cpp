/*
Type your name and student ID here
	- Name:Tong Kai Hin
	- Student ID: 1155126304
*/

#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"

#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <fstream>
#include <math.h>


GLint programID;
int x_press = 0;
float x_delta = 0.1f;
int z_press = 0;
float z_delta = 0.1f;
int camera_press = 0;
int tank_move = 0;
float tank_x_delta = 1.0f;
int dead = 0;
int rotate_tankman = 0;
float scroll_value = 45.0f;
int scale_key = 10;



void get_OpenGL_info() {
	// OpenGL information
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	std::cout << "OpenGL company: " << name << std::endl;
	std::cout << "Renderer name: " << renderer << std::endl;
	std::cout << "OpenGL version: " << glversion << std::endl;

}

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		std::cout << buffer << std::endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID) {
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID) {
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

std::string readShaderCode(const char* fileName) {
	std::ifstream meInput(fileName);
	if (!meInput.good()) {
		std::cout << "File failed to load ... " << fileName << std::endl;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void installShaders() {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	//adapter[0] = vertexShaderCode;
	std::string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	//adapter[0] = fragmentShaderCode;
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

GLuint VAOs[20]; //global buffer
GLuint VBOs[20]; //global buffer
GLuint EBOs[20]; //global buffer


void sendDataToOpenGL() {

	//pond
	{
		float pond[] = {

		   -6.0f, -0.5f,-6.75f,  //pos 0
		   +0.08f, +0.66f, +0.95f,

		  -1.0f, -0.5f, -2.75f,  //pos 1
			 +0.0f, +1.0f, +1.0f,

		   -1.0f, -0.5f, -6.75f, //pos 2
			 +0.08f, +0.66f, +0.95f,

			-6.0f, -0.5f, -2.75f, //pos 3
			 +0.0f, +1.0f, +1.0f,


		};

		const GLushort pondIndices[] = {
		0,1,3,
		0,1,2
		};

		glGenVertexArrays(1, &VAOs[0]);
		glGenBuffers(1, &VBOs[0]);


		glBindVertexArray(VAOs[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pond), pond, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

		glGenBuffers(1, &EBOs[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pondIndices), pondIndices, GL_STATIC_DRAW);



		const GLfloat root1[] = {

		   +2.25f, 0.5f,-5.7f,  //pos 0
		   +0.64f, +0.16f, +0.16f,

		   +2.75f, 0.5f, -5.7f,  //pos 1
		   +0.55f, +0.35f, +0.05f,

		   +2.75f, 0.5f, -5.2f, //pos 2
			+0.64f, +0.16f, +0.16f,

		   +2.25f, 0.5f, -5.2f, //pos 3
			 +0.55f, +0.35f, +0.05f,

		   +2.25f, -0.5f, -5.7f, //pos 4
			+0.64f, +0.16f, +0.16f,

		   +2.75f, -0.5f, -5.7f, //pos 5
			 +0.55f, +0.35f, +0.05f,

		   +2.75f, -0.5f, -5.2f, //pos 6
		+0.64f, +0.16f, +0.16f,

		   +2.25f, -0.5f, -5.2f, //pos 7
			 +0.55f, +0.35f, +0.05f,

		};

		const GLushort root1Indices[] = {
			0,1,2,
			0,2,3,
			0,4,5,
			0,5,1,
			1,5,6,
			1,6,2,
			0,4,7,
			0,7,3,
			3,7,6,
			3,6,2,
			7,4,5,
			7,5,1,
		};

		glGenVertexArrays(1, &VAOs[1]);
		glBindVertexArray(VAOs[1]);


		glGenBuffers(1, &VBOs[1]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(root1), root1, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

		glGenBuffers(1, &EBOs[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(root1Indices), root1Indices, GL_STATIC_DRAW);
	}

	//Tree1BottomPyramid
	{
		const GLfloat Tree1BottomPyramid[] = {

			   +2.0f, 0.5f, -5.0f,  //pos 0
			   +0.5f, +1.0f, +0.5f,

			   +3.0f, 0.5f, -5.0f,  //pos 1
			   +0.0f, +0.5f, +0.0f,

			   +3.0f, 0.5f, -6.0f,  //pos 2
			   +0.5f, +1.0f, +0.5f,

			   +2.0f, 0.5f, -6.0f,  //pos 3
			   +0.0f, +0.5f, +0.0f,

			   +2.5f, +1.5f, -5.5f,  //pos 4
			  +0.5f, +1.0f, +0.5f,


		};
		const GLushort Tree1BottomPyramidIndices[] = {
				0,1,2,
				0,2,3,
				0,1,4,
				1,2,4,
				2,3,4,
				0,3,4,

		};

		glGenVertexArrays(1, &VAOs[2]);
		glBindVertexArray(VAOs[2]);


		glGenBuffers(1, &VBOs[2]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Tree1BottomPyramid), Tree1BottomPyramid, GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

		glGenBuffers(1, &EBOs[2]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Tree1BottomPyramidIndices), Tree1BottomPyramidIndices, GL_STATIC_DRAW);
	}



	{
		const GLfloat Tree1MiddlePyramid[] = {

		   +2.0f, +1.3f, -5.0f,  //pos 0
		   +0.5f, +1.0f, +0.5f,

		   +3.0f, +1.3f, -5.0f,  //pos 1
		   +0.0f, +0.5f, +0.0f,

		   +3.0f, +1.3f, -6.0f,  //pos 2
		   +0.5f, +1.0f, +0.5f,

		   +2.0f, +1.3f, -6.0f,  //pos 3
		   +0.0f, +0.5f, +0.0f,

		   +2.5f, +2.2f, -5.5f,  //pos 4
		  +0.5f, +1.0f, +0.5f,


		};
		const GLushort Tree1MiddlePyramidIndices[] = {
			0,1,2,
			0,2,3,
			0,1,4,
			1,2,4,
			2,3,4,
			0,3,4,

		};

		glGenVertexArrays(1, &VAOs[3]);
		glBindVertexArray(VAOs[3]);

		glGenBuffers(1, &VBOs[3]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Tree1MiddlePyramid), Tree1MiddlePyramid, GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

		glGenBuffers(1, &EBOs[3]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Tree1MiddlePyramidIndices), Tree1MiddlePyramidIndices, GL_STATIC_DRAW);
	}

	//Tree1TopPyramid
	{
		const GLfloat Tree1TopPyramid[] = {

		   +2.0f, +2.1f, -5.0f,  //pos 0
		   +0.5f, +1.0f, +0.5f,

		   +3.0f, +2.1f, -5.0f,  //pos 1
		   +0.0f, +0.5f, +0.0f,

		   +3.0f, +2.1f, -6.0f,  //pos 2
		   +0.5f, +1.0f, +0.5f,

		   +2.0f, +2.1f, -6.0f,  //pos 3
		   +0.0f, +0.5f, +0.0f,

		   +2.5f, +3.0f, -5.5f,  //pos 4
		   +0.5f, +1.0f, +0.5f,


		};
		const GLushort Tree1TopPyramidIndices[] = {
			0,1,2,
			0,2,3,
			0,1,4,
			1,2,4,
			2,3,4,
			0,3,4,

		};

		glGenVertexArrays(1, &VAOs[4]);
		glBindVertexArray(VAOs[4]);

		glGenBuffers(1, &VBOs[4]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[4]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Tree1TopPyramid), Tree1TopPyramid, GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

		glGenBuffers(1, &EBOs[4]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[4]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Tree1TopPyramidIndices), Tree1TopPyramidIndices, GL_STATIC_DRAW);
	}

	//ground
	{
		const GLfloat ground[]{
		+50.0f,-2.5f,+50.0f,
		+0.16f,+0.93f,+0.62f,

		+50.0f,-2.5f,-50.0f,
			+0.5,+1.0f,+0.0f,

		-50.0f,-2.5f,+50.0f,
		+0.16f,+0.93f,+0.62f,

		-50.0f,-2.5f,-50.0f,
		+0.5,+1.0f,+0.0f,

		};
		const GLushort groundIndices[] = {
			0,1,3,2,3,0
		};

		glGenVertexArrays(1, &VAOs[5]);
		glBindVertexArray(VAOs[5]);

		glGenBuffers(1, &VBOs[5]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[5]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

		glGenBuffers(1, &EBOs[5]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[5]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);

		float shark[] = {



			 -3.5f, -0.5f,-4.75f,
			  +0.0f,0.0f,0.1f,

			 -3.5f, +1.5f,-4.75f,
			  +0.0f,0.0f,0.1f,

			 -2.5f, -0.5f,-4.75f,
			  +0.0f,0.0f,0.1f,


		};

		const GLushort sharkIndices[] = {
		0,1,2,

		};

		glGenVertexArrays(1, &VAOs[6]);
		glGenBuffers(1, &VBOs[6]);


		glBindVertexArray(VAOs[6]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[6]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(shark), shark, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[6]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[6]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sharkIndices), sharkIndices, GL_STATIC_DRAW);
	}

	//supermanbody
	{
		float supermanbody[]{



	   -0.35f,0.5f ,2.0f,  //pos 0
	 +0.0f,+0.8f, +0.8f,

		-0.35f,0.5f , 2.5f, //pos 1
		 +0.0f,+0.8f, +0.8f,

	   -1.0f,0.5f , 2.5f, //pos 2
		+0.0f,+0.8f, +0.8f,

	   -1.0f,0.5f , 2.0f,  //pos 3
		+0.0f,+0.8f, +0.8f,



	 -0.35f,1.5f ,2.0f,  //pos 4
		+0.0f,+0.8f, +0.8f,

	   -0.35f,1.5f ,  2.5f, //pos 5
		+0.0f,+0.8f, +0.8f,

	   -1.0f,1.5f ,  2.5f, //pos 6
		 +0.0f,+0.8f, +0.8f,

	   -1.0f,+1.5f , 2.0f,  //pos 7
	+0.0f,+0.8f, +0.8f,

	//cloak
	 -0.35f,1.5f ,2.0f,  //8
	 1.0f,-0.0f,-0.0f,

	  -1.0f,+1.5f , 2.0f,//9
	 0.5f,0.0f,0.0f,

	   -0.0f,0.0f ,0.0f,//10
	   0.5f,0.0f,0.0f,

	   -1.65f,+0.0f , 0.0f,//11
	   1.0f,-0.0f,-0.0f,

	   //right arm
	   -0.35f,1.3f,2.1f,  //12
	   +0.0f,+0.8f, +0.8f,

	   -0.35f,1.3f,2.4f,  //13
	   +0.0f,+0.8f, +0.8f,

	   -0.35f,1.0f,2.1f,//14
	   +0.0f,+0.8f, +0.8f,

	   -0.35f,1.0f,2.4f,//15
	   +0.0f,+0.8f, +0.8f,

	   -0.0f,1.3f,2.1f,//16
	   +0.0f,+0.8f, +0.8f,

	   -0.0f,1.3f,2.4f,//17
	   +0.0f,+0.8f, +0.8f,

	   -0.00f,1.0f,2.1f,//18
	   +0.0f,+0.8f, +0.8f,

	   -0.0f,1.0f,2.4f,//19
		+0.0f,+0.8f, +0.8f,


		//right top arm
		0.30f,1.0f,2.4f,//20
		+1.0f,+0.65f, +0.35f, //color of arm

		0.001f,1.0f,2.4f,//21
		+1.0f,+0.65f, +0.35f, //color of arm

		0.001f,1.0f,2.1f,//22
		+1.0f,+0.65f, +0.35f, //color of arm

		0.30f,1.0f,2.1f,//23
		+1.0f,+0.65f, +0.35f, //color of arm

		0.30f,2.0f,2.4f,//24
		+1.0f,+0.65f, +0.35f, //color of arm

		0.001f,2.0f,2.4f,//25
		+1.0f,+0.65f, +0.35f, //color of arm

		0.001f,2.0f,2.1f,//26
		+1.0f,+0.65f, +0.35f, //color of arm

		0.3f,2.0f,2.1f,//27
		+1.0f,+0.65f, +0.35f, //color of arm



	   //left arm
		-1.0f,1.3f,2.1f,  //28
	   +0.0f,+0.8f, +0.8f,

	   -1.0f,1.3f,2.4f,  //29
	   +0.0f,+0.8f, +0.8f,

	   -1.0f,1.0f,2.1f,//30
	   +0.0f,+0.8f, +0.8f,

	   -1.0f,1.0f,2.4f,//31
	   +0.0f,+0.8f, +0.8f,

	   -1.35f,1.3f,2.1f,//32
	   +0.0f,+0.8f, +0.8f,

		   -1.35f,1.3f,2.4f,//33
	   +0.0f,+0.8f, +0.8f,

		 -1.35f,1.0f,2.1f,//34
	   +0.0f,+0.8f, +0.8f,

		 -1.35f,1.0f,2.4f,//35
		+0.0f,+0.8f, +0.8f,

		//left top arm 
		 //edit here

		-1.35f, 1.3f, 2.1f,  //36
		  +1.0f, +0.65f, +0.35f, //color of arm

		-1.35f, 1.3f, 2.4f,  //37
	 +1.0f, +0.65f, +0.35f, //color of arm

		-1.35f, 1.0f, 2.1f,//38
	  +1.0f, +0.65f, +0.35f, //color of arm

		-1.35f, 1.0f, 2.4f,//39
	   +1.0f, +0.65f, +0.35f, //color of arm

		-2.35f, 1.3f, 2.1f,//40
		+1.0f, +0.65f, +0.35f, //color of arm

		-2.35f, 1.3f, 2.4f,//41
		+1.0f, +0.65f, +0.35f, //color of arm

		-2.35f, 1.0f, 2.1f,//42
		+1.0f, +0.65f, +0.35f, //color of arm

		-2.35f, 1.0f, 2.4f,//43
		+1.0f, +0.65f, +0.35f, //color of arm


	 //hair


		};
		const GLushort supermanbodyIndices[] = {
			//right face
			0,1,2,
			2,3,0,
			//top face
			4,5,6,
			6,7,4,
			//left face
			0,1,4,
			4,5,1,
			//front face
			1,2,5,
			5,6,2,
			//right face
			2,3,6,
			6,7,3,
			//back face
			3,0,7,
			7,6,3,

			//cloak
			8,9,10,
			9,10,11,

			//right arm
			12,13,14,
			13,14,15,

			16,17,18,
			17,18,19,

			15,19,17,
			13,17,15,

			18,14,12,
			12,16,18,

			13,12,17,
			16,17,12,

			18,19,14,
			19,14,15,

			//right top arm
			20,21,22,
			21,22,23,

			24,25,26,
			25,26,27,

			20,21,24,
			21,24,25,

			21,22,25,
			22,25,26,

			22,23,26,
			23,26,27,

			23,24,27,
			24,27,20,

			//left arm
			28,29,30,
			29,30,31,

			32,33,34,
			33,34,35,

			31,35,33,
			29,33,31,

			34,30,28,
			28,32,34,

			29,28,33,
			32,33,28,

			32,35,30,
			35,30,31,

			//left top arm
			36,37,38,
			37,38,39,

			40,41,42,
			41,42,43,

			39,43,41,
			37,41,39,

			42,38,36,
			36,40,42,

			37,36,41,
			40,41,36,

			40,43,38,
			43,38,39,


			//hair

			//EDIT HERE





		};
		glGenVertexArrays(1, &VAOs[7]);
		glGenBuffers(1, &VBOs[7]);


		glBindVertexArray(VAOs[7]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[7]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(supermanbody), supermanbody, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[7]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[7]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(supermanbodyIndices), supermanbodyIndices, GL_STATIC_DRAW);

	}

	//supermanlegs
	{
		float supermanlegs[] = {

			-0.35f,-0.25f ,2.0f,  //pos 0
			0.29f,0.14f,0.98f,

			-0.35f,-0.25f ,  2.5f, //pos 1
			 0.29f,0.14f,0.98f,

		   -1.0f,-0.25f ,  2.5f, //pos 2
			  0.29f,0.14f,0.98f,

		   -1.0f,-0.25f , 2.0f,  //pos 3
		   0.29f,0.14f,0.98f,

			-0.35f,0.5f ,2.0f,  //pos 4
			0.29f,0.14f,0.98f,

		   -0.35f,0.5f ,  2.5f, //pos 5
			0.29f,0.14f,0.98f,

		   -1.0f,0.5f , 2.5f, //pos 6
			 0.29f,0.14f,0.98f,

		   -1.0f,+0.5f,2.0f,  //pos 7
			0.29f,0.14f,0.98f,

		};
		const GLushort supermanlegsIndices[] = {
			//right face
			0,1,2,
			2,3,0,
			//top face
			4,5,6,
			6,7,4,
			//left face
			0,1,4,
			4,5,1,
			//front face
			1,2,5,
			5,6,2,
			//right face
			2,3,6,
			6,7,3,
			//back face
			3,0,7,
			7,6,3


		};
		glGenVertexArrays(1, &VAOs[8]);
		glGenBuffers(1, &VBOs[8]);


		glBindVertexArray(VAOs[8]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[8]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(supermanlegs), supermanlegs, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[8]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[8]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(supermanlegsIndices), supermanlegsIndices, GL_STATIC_DRAW);

	}


	//supermanshoes
	{
		float supermanshoes[]{



		-0.35f,-0.5f ,2.0f,  //pos 0
		0.5f,0.5f,0.5f,

		-0.35f,-0.5f ,  2.5f, //pos 1
		0.5f,0.5f,0.5f,

	   -1.0f,-0.5f ,  2.5f, //pos 2
		 0.5f,0.5f,0.5f,

	   -1.0f,-0.5f , 2.0f,  //pos 3
	  0.5f,0.5f,0.5f,



	 -0.35f,-0.255f ,2.0f,  //pos 4
		0.5f,0.5f,0.5f,
	  -0.35f,-0.25f ,  2.5f, //pos 5
		 0.5f,0.5f,0.5f,

	   -1.0f,-0.25f ,  2.5f, //pos 6
		 0.5f,0.5f,0.5f,

	   -1.0f,-0.25f , 2.0f,  //pos 7
		 0.5f,0.5f,0.5f,

		};
		const GLushort supermanshoesIndices[] = {
			//right face
			0,1,2,
			2,3,0,
			//top face
			4,5,6,
			6,7,4,
			//left face
			0,1,4,
			4,5,1,
			//front face
			1,2,5,
			5,6,2,
			//right face
			2,3,6,
			6,7,3,
			//back face
			3,0,7,
			7,6,3


		};
		glGenVertexArrays(1, &VAOs[9]);
		glGenBuffers(1, &VBOs[9]);


		glBindVertexArray(VAOs[9]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[9]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(supermanshoes), supermanshoes, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[9]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[9]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(supermanshoesIndices), supermanshoesIndices, GL_STATIC_DRAW);
	}
	//supermanhead
	{
		float supermanhead[]{



		   -0.3f,2.5f ,1.95f,  //pos 0
		+0.82f,+0.56865f, +0.345f,

			-0.3f,2.5f , 2.55f, //pos 1
			+1.0f,+0.65f, +0.35f,

		   -1.05f,2.5f , 2.55f, //pos 2
			+0.82f,+0.56865f, +0.345f,

		   -1.05f,2.5f , 1.95f,  //pos 3
			+1.0f,+0.65f, +0.35f,



		 -0.3f,1.5f ,1.95f,  //pos 4
			+0.82f,+0.56865f, +0.345f,

		   -0.3f,1.5f ,  2.55f, //pos 5
			+1.0f,+0.65f, +0.35f,

		   -1.05f,1.5f ,  2.55f, //pos 6
			+0.82f,+0.56865f, +0.345f,

		   -1.05f,+1.5f ,1.95f,  //pos 7
		+1.0f,+0.65f, +0.35f,

		//eye
		-0.42f, 2.0f,2.6f, //pos 8
		1.0f,1.0f,1.0f,

		-0.57f, 2.0f,2.6f,//pos 9
		1.0f,1.0f,1.0f,

			-0.42f, 2.1f,2.6f,//pos 10
		1.0f,1.0f,1.0f,

			-0.57f, 2.1f,2.6f,//pos 11
		1.0f,1.0f,1.0f,


		-0.73f, 2.0f,2.6f, //pos 12
		1.0f,1.0f,1.0f,

		-0.88f, 2.0f,2.6f,//pos 13
		1.0f,1.0f,1.0f,

			-0.73f, 2.1f,2.6f,//pos 14
		1.0f,1.0f,1.0f,

			-0.88f, 2.1f,2.6f,//pos 15
		1.0f,1.0f,1.0f,




		};
		const GLushort supermanheadIndices[] = {

			0,1,2,
			2,3,0,

			4,5,6,
			6,7,4,

			0,1,4,
			4,5,1,

			1,5,6,
			1,2,6,

			2,7,3,
			2,7,6,

			3,0,7,
			0,4,7,

			//eye
			8,9,10,
			9,10,11,

			//eye
			12,13,14,
			13,14,15,




		};
		glGenVertexArrays(1, &VAOs[10]);
		glGenBuffers(1, &VBOs[10]);


		glBindVertexArray(VAOs[10]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[10]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(supermanhead), supermanhead, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[10]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[10]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(supermanheadIndices), supermanheadIndices, GL_STATIC_DRAW);
	}

	//tianamen
	{
		float tianamen[]{

			//tianament bottom
			20.0f,-2.5f,-50.0f, //0
			1.0f,0.0f,0.0f,

			-20.0,-2.5f,-50.0f, //1
			1.0f,0.0f,0.0f,

			17.5f,5.0f,-50.0f,//2
			1.0f,0.0f,0.0f,

			-17.5,5.0f,-50.0f,//3
			1.0f,0.0f,0.0f,

			//right small roof
			12.5f,7.0f,-50.0f, //4
			0.8f,0.4f,0.0f,

			6.5f,7.0f,-50.0f,//5
			0.8f,0.4f,0.0f,

			10.5f,10.0f,-50.0f,//6
			0.8f,0.4f,0.0f,

			8.5f,10.0f,-50.0f,//7
			0.8f,0.4f,0.0f,

			//left small roof
			-12.5f,7.0f,-50.0f, //8
			0.8f,0.4f,0.0f,

			-6.5f,7.0f,-50.0f,//9
			0.8f,0.4f,0.0f,

			-10.5f,10.0f,-50.0f,//10
			0.8f,0.4f,0.0f,

			-8.5f,10.0f,-50.0f,//11
			0.8f,0.4f,0.0f,

			//right small house
			11.8f,5.0f ,-50.0f,  //12
			1.0f,0.0f,0.0f,

			11.8f, 7.0f,-50.0f,//13
			0.6f,0.0f,0.0f,

			7.2f,5.0f ,-50.0f,//14
			1.0f,0.0f,0.0f,

			7.2f, 7.0f,-50.0f,//15
			0.6f,0.0f,0.0f,

			//left small house
			-11.8f,5.0f ,-50.0f,  //16
			0.6f,0.0f,0.0f,

			-11.8f, 7.0f,-50.0f,//17
			1.0f,0.0f,0.0f,

			-7.2f,5.0f ,-50.0f,//18
			0.6f,0.0f,0.0f,

			-7.2f, 7.0f,-50.0f,//19
			1.0f,0.0f,0.0f,

			//white border
			17.83333f,4.0f,-49.9f, //20
			1.0f,1.0f,1.0f,

			-17.83333f,4.0f,-49.9f, //21
			1.0f,1.0f,1.0f,

			17.5f,5.0f,-49.9f,//22
			1.0f,1.0f,1.0f,

			-17.5f,5.0f,-49.9f,//23
			1.0f,1.0f,1.0f,

			//middle house
			5.5f,5.0f,-50.0f, //24
			0.6f,0.0f,0.0f,

			-5.5f,5.0f,-50.0f, //25
			1.0f,0.0f,0.0f,

			5.5f,10.0f,-50.0f, //26
			1.0f,0.0f,0.0f,

			-5.5f,10.0f,-50.0f, //27
			0.6f,0.0f,0.0f,

			//middle roof
			6.5f,10.0f,-50.0f, //28
			0.8f,0.4f,0.0f,

			-6.5f,10.0f,-50.0f,//29
			0.8f,0.4f,0.0f,

			5.5f,12.0f,-50.0f,//30
			0.8f,0.4f,0.0f,

			-5.5f,12.0f,-50.0f,//31
			0.8f,0.4f,0.0f,

			//middle top 
			5.5f, 12.0f, -50.0f,//32
			0.8f, 0.4f, 0.0f,

			-5.5f, 12.0f, -50.0f,//33
			0.8f, 0.4f, 0.0f,

			5.5f, 14.0f, -50.0f,//34
			0.8f, 0.4f, 0.0f,

			-5.5f, 14.0f, -50.0f,//35
			0.8f, 0.4f, 0.0f,

			//middle middle roof
			6.5f, 7.5f, -49.9f, //36
			0.8f, 0.4f, 0.0f,

			-6.5f, 7.5f, -49.9f,//37
			0.8f, 0.4f, 0.0f,

			5.5f, 9.0f, -49.9f,//38
			0.8f, 0.4f, 0.0f,

			-5.5f, 9.0f, -49.9f,//39
			0.8f, 0.4f, 0.0f,




		};
		const GLushort tianamenIndices[] = {

			0,1,2,
			1,2,3,

			4,5,6,
			5,6,7,

			8,9,10,
			9,10,11,

			12,13,14,
			13,14,15,

			16,17,18,
			17,18,19,

			20,21,22,
			21,22,23,

			24,25,26,
			25,26,27,

			28,29,30,
			29,30,31,

			32,33,34,
			33,34,35,

			36,37,38,
			37,38,39

		};
		glGenVertexArrays(1, &VAOs[11]);
		glGenBuffers(1, &VBOs[11]);

		glBindVertexArray(VAOs[11]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[11]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tianamen), tianamen, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

		glGenBuffers(1, &EBOs[11]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[11]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tianamenIndices), tianamenIndices, GL_STATIC_DRAW);
	}

	//tank
	{
		float tank[]{

			//rectangle body
			5.0f,-2.5f,-20.0f,//0
			0.0f,0.2f,0.0f,

			5.0f,-2.5f,-30.0f,//1
			0.125f,0.125f,0.125f,

			20.0f,-2.5f,-20.0f,//2
			0.0f,0.2f,0.0f,

			20.0f,-2.5f,-30.0f,//3
			0.125f,0.125f,0.125f,

			5.0f,0.0f,-20.0f,//4
			0.0f,0.2f,0.0f,

			5.0f,0.0f,-30.0f,//5
			0.125f,0.125f,0.125f,

			20.0f,0.0f,-20.0f,//6
			0.0f,0.2f,0.0f,

			20.0f,0.0f,-30.0f,//7
			0.125f,0.125f,0.125f,

			//Trapezoid body
			8.0f,0.0f,-21.0f,//8
			0.333, 0.420, 0.184,

			8.0f,0.0f,-29.0f,//9
			0.133, 0.545, 0.133,

			19.0f,0.0f,-21.0f,//10
			0.333, 0.420, 0.184,

			19.0f,0.0f,-29.0f,//11
			0.133, 0.545, 0.133,

			9.0f,2.0f,-22.0f,//12
			0.333, 0.420, 0.184,

			9.0f,2.0f,-28.0f,//13
			0.133, 0.545, 0.133,

			18.0f,2.0f,-22.0f,//14
			0.333, 0.420, 0.184,

			18.0f,2.0f,-28.0f,//15
			0.133, 0.545, 0.133,

			//gun


			9.0f,0.75f,-24.75f,//16
			0.000, 1.000, 0.498,

			9.0f,1.25f,-24.75f,//17
			0.420, 0.557, 0.137,

			9.0f,1.25f,-25.25f,//18
			0.420, 0.557, 0.137,

			9.0f,0.75f,-25.25f,//19
			0.000, 1.000, 0.498,

			-1.0f,0.75f,-24.75f,//20
			0.196, 0.804, 0.196,

			-1.0f,1.25f,-24.75f,//21
			0.196, 0.804, 0.196,

			-1.0f,1.25f,-25.25f,//22
			0.000, 1.000, 0.498,

			-1.0f,0.75f,-25.25f,//23
			0.000, 1.000, 0.498,

			//tank tire
			5.0f,-2.489f,-20.0f,//24
			0.0, 0.0, 0.0,

			5.0f,-2.489f,-19.0f,//25
			0.0, 0.0, 0.0,

			20.0f,-2.489f,-20.0f,//26
			0.0, 0.0, 0.0,

			20.0f,-2.489f,-19.0f,//27
			0.0, 0.0, 0.0,

			4.5f,-0.5f,-20.0f,//28
			0.663, 0.663, 0.663,

			4.5f,-0.5f,-19.0f,//29
			0.663, 0.663, 0.663,

			20.5f,-0.5f,-20.0f,//30
			0.663, 0.663, 0.663,

			20.5f,-0.5f,-19.0f,//31
			0.663, 0.663, 0.663,

			//tank behind
			5.0f,-2.489f,-31.0f,//32
			0.0, 0.0, 0.0,

			5.0f,-2.489f,-30.0f,//33
			0.0, 0.0, 0.0,

			20.0f,-2.489f,-31.0f,//34
			0.0, 0.0, 0.0,

			20.0f,-2.489f,-30.0f,//35
			0.0, 0.0, 0.0,

			4.5f,-0.5f,-31.0f,//36
			0.663, 0.663, 0.663,

			4.5f,-0.5f,-30.0f,//37
			0.663, 0.663, 0.663,

			20.5f,-0.5f,-31.0f,//38
			0.663, 0.663, 0.663,

			20.5f,-0.5f,-30.0f,//39
			0.663, 0.663, 0.663,


		};
		const GLushort tankIndices[] = {
			0,1,2,
			1,2,3,

			4,5,6,
			5,6,7,

			0,1,5,
			0,4,5,

			1,3,7,
			1,5,7,

			3,2,6,
			3,7,6,

			2,0,4,
			2,6,4,

			8,9,10,
			9,10,11,

			12,13,14,
			13,14,15,

			8,9,13,
			8,12,13,

			9,11,15,
			9,13,15,

			11,10,14,
			11,15,14,

			10,8,12,
			10,14,12,

			16,17,18,
			16,18,19,

			20,21,22,
			20,22,23,

			16,17,20,
			17,20,21,

			17,18,21,
			18,21,22,

			18,19,23,
			19,22,23,

			16,19,20,
			19,20,23,

			//108 indices here
			24,25,26,
			25,26,27,

			28,29,30,
			29,30,31,

			24,25,28,
			25,28,29,

			25,27,29,
			27,29,31,

			26,27,30,
			27,30,31,

			//138
			32,33,34,
			33,34,35,

			36,37,38,
			37,38,39,

			32,33,36,
			33,36,37,

			33,35,37,
			35,37,39,

			34,35,38,
			35,38,39,




		};
		glGenVertexArrays(1, &VAOs[12]);
		glGenBuffers(1, &VBOs[12]);

		glBindVertexArray(VAOs[12]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[12]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tank), tank, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[12]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[12]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tankIndices), tankIndices, GL_STATIC_DRAW); }

	//road
	{
		float road[]{
			-50.0f,-2.49f,-15.0f,
			0.184, 0.310, 0.310,

			-50.0f,-2.49f,-35.0f,
			0.184, 0.310, 0.310,

			50.0f,-2.49f,-15.0f,
			0.663, 0.663, 0.66,

			50.0f,-2.49f,-35.0f,
			0.663, 0.663, 0.66,

			//black lines
			-40.0f,-2.48f,-24.0f,  //4
			0.0, 0.0, 0.0,

			-40.0f,-2.48f,-26.0f,//5
			0.0, 0.0, 0.0,

			-30.0f,-2.48f,-24.0f,//6
			0.0, 0.0, 0.0,

			-30.0f,-2.48f,-26.0f,//7
			0.0, 0.0, 0.0,

			//black lines
			-20.0f,-2.48f,-24.0f,  //8
			0.0, 0.0, 0.0,

			-20.0f,-2.48f,-26.0f,//9
			0.0, 0.0, 0.0,

			-10.0f,-2.48f,-24.0f,//10
			0.0, 0.0, 0.0,

			-10.0f,-2.48f,-26.0f,//11
			0.0, 0.0, 0.0,

			//black lines
			0.0f,-2.48f,-24.0f,  //12
			0.0, 0.0, 0.0,

			-0.0f,-2.48f,-26.0f,//13
			0.0, 0.0, 0.0,

			10.0f,-2.48f,-24.0f,//14
			0.0, 0.0, 0.0,

			10.0f,-2.48f,-26.0f,//15
			0.0, 0.0, 0.0,

		};
		const GLushort roadIndices[] = {
			0,1,2,
			1,2,3,

			4,5,6,
			5,6,7,

			8,9,10,
			9,10,11,

			12,13,14,
			13,14,15

		};
		glGenVertexArrays(1, &VAOs[13]);
		glGenBuffers(1, &VBOs[13]);

		glBindVertexArray(VAOs[13]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[13]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(road), road, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[13]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[13]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(roadIndices), roadIndices, GL_STATIC_DRAW);
	}

	//road
	{
		float blood[]{

			-10.0f,-2.489f,-15.0f,
			1.0,0.0,0.0,

			-10.0f,-2.489f,-35.0f,
			1.0,0.0,0.0,

			0.0f,-2.489f,-15.0f,
			1.0,0.0,0.0,

			0.0f,-2.489f,-35.0f,
			1.0,0.0,0.0,
		};
		const GLushort bloodIndices[] = {
			0,1,2,
			1,2,3,

		};
		glGenVertexArrays(1, &VAOs[14]);
		glGenBuffers(1, &VBOs[14]);

		glBindVertexArray(VAOs[14]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[14]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(blood), blood, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[14]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[14]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bloodIndices), bloodIndices, GL_STATIC_DRAW);
	}


	//cone
	{
		float cone[]{

			7.0f,-2.2f,-40.0f,
			1.000, 0.271, 0.000,


			9.0f,-2.2f,-40.0f,
			1.000, 0.271, 0.000,


			7.0f,-2.2f,-42.0f,
			1.000, 0.271, 0.000,


			9.0f,-2.2f,-42.0f,
			1.000, 0.271, 0.000,

			8.0f,0.3f,-41.0f,
			1.000, 0.388, 0.278,

			//bottom reactangle


			6.5,-2.5f,-39.5,   //5
			1.000, 0.000, 0.000,

			9.5,-2.5f,-39.5, //6
			1.000, 0.000, 0.000,

			6.5,-2.5f,-42.5,//7
			1.000, 0.000, 0.000,

			9.5,-2.5f,-42.5,//8
			1.000, 0.000, 0.000,

			6.5,-2.2f,-39.5,//9
			1.000, 0.000, 0.000,

			9.5,-2.2f,-39.5,//10
			1.000, 0.000, 0.000,

			6.5,-2.2f,-42.5,//11
			1.000, 0.000, 0.000,

			9.5,-2.2f,-42.5,//12
			1.000, 0.000, 0.000,

			//top pyramid
			7.3f,-1.0f,-40.3f,//13
			1.000, 1.0, 1.000,


			8.7f,-1.0f,-40.3f,
		1.000, 1.0, 1.000,


			7.3f,-1.0f,-41.7f,
		1.000, 1.0, 1.000,


			8.7f,-1.0f,-41.7f,
			1.000, 1.0, 1.000,

			8.0f,0.3f,-41.0f,//17
			1.000, 1.0, 1.000,



		};
		const GLushort coneIndices[] = {

			0,1,2,
			1,2,3,

			0,1,4,
			0,2,4,

			2,3,4,
			1,3,4,

			//rectangle

			5,6,7,
			6,7,8,

			9,10,11,
			10,11,12,

			5,6,9,
			6,9,10,

			5,7,9,
			7,9,11,

			6,8,10,
			8,10,12,

			7,8,11,
			8,11,12,

			13,14,15,
			14,15,16,

			13,14,17,
			13,15,17,


			15,16,17,
			14,16,17,
		};
		glGenVertexArrays(1, &VAOs[15]);
		glGenBuffers(1, &VBOs[15]);

		glBindVertexArray(VAOs[15]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[15]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cone), cone, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[15]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[15]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(coneIndices), coneIndices, GL_STATIC_DRAW);
	}
	//tankman 
	//tankmanbody
	{
		float tankmanbody[]{



	   -0.35f,0.5f ,2.0f,  //pos 0
	 +0.0f,+0.8f, +0.8f,

		-0.35f,0.5f , 2.5f, //pos 1
		 +0.0f,+0.8f, +0.8f,

	   -1.0f,0.5f , 2.5f, //pos 2
		+0.0f,+0.8f, +0.8f,

	   -1.0f,0.5f , 2.0f,  //pos 3
		+0.0f,+0.8f, +0.8f,



	 -0.35f,1.5f ,2.0f,  //pos 4
		+0.0f,+0.8f, +0.8f,

	   -0.35f,1.5f ,  2.5f, //pos 5
		+0.0f,+0.8f, +0.8f,

	   -1.0f,1.5f ,  2.5f, //pos 6
		 +0.0f,+0.8f, +0.8f,

	   -1.0f,+1.5f , 2.0f,  //pos 7
	+0.0f,+0.8f, +0.8f,

	//cloak
	 -0.35f,1.5f ,2.0f,  //8
	 1.0f,-0.0f,-0.0f,

	  -1.0f,+1.5f , 2.0f,//9
	 0.5f,0.0f,0.0f,

	   -0.0f,0.0f ,0.0f,//10
	   0.5f,0.0f,0.0f,

	   -1.65f,+0.0f , 0.0f,//11
	   1.0f,-0.0f,-0.0f,

	   //right arm
	   -0.35f,1.3f,2.1f,  //12
	   +0.0f,+0.8f, +0.8f,

	   -0.35f,1.3f,2.4f,  //13
	   +0.0f,+0.8f, +0.8f,

	   -0.35f,1.0f,2.1f,//14
	   +0.0f,+0.8f, +0.8f,

	   -0.35f,1.0f,2.4f,//15
	   +0.0f,+0.8f, +0.8f,

	   -0.0f,1.3f,2.1f,//16
	   +0.0f,+0.8f, +0.8f,

	   -0.0f,1.3f,2.4f,//17
	   +0.0f,+0.8f, +0.8f,

	   -0.00f,1.0f,2.1f,//18
	   +0.0f,+0.8f, +0.8f,

	   -0.0f,1.0f,2.4f,//19
		+0.0f,+0.8f, +0.8f,


		//right top arm
	  -0.0f,1.3f,2.1f,  //20
	  +1.0f, +0.65f, +0.35f, //color of arm

	  -0.0f,1.3f,2.4f,  //21
	  +1.0f, +0.65f, +0.35f, //color of arm

	  -0.0f,1.0f,2.1f,//22
	  +1.0f, +0.65f, +0.35f, //color of arm

	  -0.0f,1.0f,2.4f,//23
	   +1.0f, +0.65f, +0.35f, //color of arm

	  1.0f,1.3f,2.1f,//24
	  +1.0f, +0.65f, +0.35f, //color of arm

	  1.0f,1.3f,2.4f,//25
	   +1.0f, +0.65f, +0.35f, //color of arm

	  1.0f,1.0f,2.1f,//26
	   +1.0f, +0.65f, +0.35f, //color of arm

	  1.0f,1.0f,2.4f,//17
	   +1.0f, +0.65f, +0.35f, //color of arm



	   //left arm
		-1.0f,1.3f,2.1f,  //28
	   +0.0f,+0.8f, +0.8f,

	   -1.0f,1.3f,2.4f,  //29
	   +0.0f,+0.8f, +0.8f,

	   -1.0f,1.0f,2.1f,//30
	   +0.0f,+0.8f, +0.8f,

	   -1.0f,1.0f,2.4f,//31
	   +0.0f,+0.8f, +0.8f,

	   -1.35f,1.3f,2.1f,//32
	   +0.0f,+0.8f, +0.8f,

		   -1.35f,1.3f,2.4f,//33
	   +0.0f,+0.8f, +0.8f,

		 -1.35f,1.0f,2.1f,//34
	   +0.0f,+0.8f, +0.8f,

		 -1.35f,1.0f,2.4f,//35
		+0.0f,+0.8f, +0.8f,

		//left top arm 
		 //edit here

		-1.35f, 1.3f, 2.1f,  //36
		  +1.0f, +0.65f, +0.35f, //color of arm

		-1.35f, 1.3f, 2.4f,  //37
	 +1.0f, +0.65f, +0.35f, //color of arm

		-1.35f, 1.0f, 2.1f,//38
	  +1.0f, +0.65f, +0.35f, //color of arm

		-1.35f, 1.0f, 2.4f,//39
	   +1.0f, +0.65f, +0.35f, //color of arm

		-2.35f, 1.3f, 2.1f,//40
		+1.0f, +0.65f, +0.35f, //color of arm

		-2.35f, 1.3f, 2.4f,//41
		+1.0f, +0.65f, +0.35f, //color of arm

		-2.35f, 1.0f, 2.1f,//42
		+1.0f, +0.65f, +0.35f, //color of arm

		-2.35f, 1.0f, 2.4f,//43
		+1.0f, +0.65f, +0.35f, //color of arm


	 //hair


		};
		const GLushort tankmanbodyIndices[] = {
			//right face
			0,1,2,
			2,3,0,
			//top face
			4,5,6,
			6,7,4,
			//left face
			0,1,4,
			4,5,1,
			//front face
			1,2,5,
			5,6,2,
			//right face
			2,3,6,
			6,7,3,
			//back face
			3,0,7,
			7,6,3,

			//cloak
			//8,9,10,
			//9,10,11,

			//right arm
			12,13,14,
			13,14,15,

			16,17,18,
			17,18,19,

			15,19,17,
			13,17,15,

			18,14,12,
			12,16,18,

			13,12,17,
			16,17,12,

			18,19,14,
			19,14,15,

			//right top arm
			20,21,22,
			21,22,23,

			24,25,26,
			25,26,27,

			20,21,24,
			21,24,25,

			21,22,25,
			22,25,26,

			22,23,26,
			23,26,27,

			23,24,27,
			24,27,20,

			//left arm
			28,29,30,
			29,30,31,

			32,33,34,
			33,34,35,

			31,35,33,
			29,33,31,

			34,30,28,
			28,32,34,

			29,28,33,
			32,33,28,

			32,35,30,
			35,30,31,

			//left top arm
			36,37,38,
			37,38,39,

			40,41,42,
			41,42,43,

			39,43,41,
			37,41,39,

			42,38,36,
			36,40,42,

			37,36,41,
			40,41,36,

			40,43,38,
			43,38,39,


			//hair

			//EDIT HERE





		};
		glGenVertexArrays(1, &VAOs[16]);
		glGenBuffers(1, &VBOs[16]);


		glBindVertexArray(VAOs[16]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[16]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tankmanbody), tankmanbody, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[16]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[16]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tankmanbodyIndices), tankmanbodyIndices, GL_STATIC_DRAW);

	}

	//tankmanlegs
	{
		float tankmanlegs[] = {

			-0.35f,-0.25f ,2.0f,  //pos 0
			0.29f,0.14f,0.98f,

			-0.35f,-0.25f ,  2.5f, //pos 1
			 0.29f,0.14f,0.98f,

		   -1.0f,-0.25f ,  2.5f, //pos 2
			  0.29f,0.14f,0.98f,

		   -1.0f,-0.25f , 2.0f,  //pos 3
		   0.29f,0.14f,0.98f,

			-0.35f,0.5f ,2.0f,  //pos 4
			0.29f,0.14f,0.98f,

		   -0.35f,0.5f ,  2.5f, //pos 5
			0.29f,0.14f,0.98f,

		   -1.0f,0.5f , 2.5f, //pos 6
			 0.29f,0.14f,0.98f,

		   -1.0f,+0.5f,2.0f,  //pos 7
			0.29f,0.14f,0.98f,

		};
		const GLushort tankmanlegsIndices[] = {
			//right face
			0,1,2,
			2,3,0,
			//top face
			4,5,6,
			6,7,4,
			//left face
			0,1,4,
			4,5,1,
			//front face
			1,2,5,
			5,6,2,
			//right face
			2,3,6,
			6,7,3,
			//back face
			3,0,7,
			7,6,3


		};
		glGenVertexArrays(1, &VAOs[17]);
		glGenBuffers(1, &VBOs[17]);


		glBindVertexArray(VAOs[17]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[17]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tankmanlegs), tankmanlegs, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[17]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[17]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tankmanlegsIndices), tankmanlegsIndices, GL_STATIC_DRAW);

	}


	//tankmanshoes
	{
		float tankmanshoes[]{



		-0.35f,-0.5f ,2.0f,  //pos 0
		0.5f,0.5f,0.5f,

		-0.35f,-0.5f ,  2.5f, //pos 1
		0.5f,0.5f,0.5f,

	   -1.0f,-0.5f ,  2.5f, //pos 2
		 0.5f,0.5f,0.5f,

	   -1.0f,-0.5f , 2.0f,  //pos 3
	  0.5f,0.5f,0.5f,



	 -0.35f,-0.255f ,2.0f,  //pos 4
		0.5f,0.5f,0.5f,
	  -0.35f,-0.25f ,  2.5f, //pos 5
		 0.5f,0.5f,0.5f,

	   -1.0f,-0.25f ,  2.5f, //pos 6
		 0.5f,0.5f,0.5f,

	   -1.0f,-0.25f , 2.0f,  //pos 7
		 0.5f,0.5f,0.5f,

		};
		const GLushort tankmanshoesIndices[] = {
			//right face
			0,1,2,
			2,3,0,
			//top face
			4,5,6,
			6,7,4,
			//left face
			0,1,4,
			4,5,1,
			//front face
			1,2,5,
			5,6,2,
			//right face
			2,3,6,
			6,7,3,
			//back face
			3,0,7,
			7,6,3


		};
		glGenVertexArrays(1, &VAOs[18]);
		glGenBuffers(1, &VBOs[18]);


		glBindVertexArray(VAOs[18]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[18]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tankmanshoes), tankmanshoes, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[18]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[18]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tankmanshoesIndices), tankmanshoesIndices, GL_STATIC_DRAW);
	}
	//tankmanhead
	{
		float tankmanhead[]{



		   -0.3f,2.5f ,1.95f,  //pos 0
		+0.82f,+0.56865f, +0.345f,

			-0.3f,2.5f , 2.55f, //pos 1
			+1.0f,+0.65f, +0.35f,

		   -1.05f,2.5f , 2.55f, //pos 2
			+0.82f,+0.56865f, +0.345f,

		   -1.05f,2.5f , 1.95f,  //pos 3
			+1.0f,+0.65f, +0.35f,



		 -0.3f,1.5f ,1.95f,  //pos 4
			+0.82f,+0.56865f, +0.345f,

		   -0.3f,1.5f ,  2.55f, //pos 5
			+1.0f,+0.65f, +0.35f,

		   -1.05f,1.5f ,  2.55f, //pos 6
			+0.82f,+0.56865f, +0.345f,

		   -1.05f,+1.5f ,1.95f,  //pos 7
		+1.0f,+0.65f, +0.35f,

		//eye
		-0.42f, 2.0f,2.6f, //pos 8
		1.0f,1.0f,1.0f,

		-0.57f, 2.0f,2.6f,//pos 9
		1.0f,1.0f,1.0f,

			-0.42f, 2.1f,2.6f,//pos 10
		1.0f,1.0f,1.0f,

			-0.57f, 2.1f,2.6f,//pos 11
		1.0f,1.0f,1.0f,


		-0.73f, 2.0f,2.6f, //pos 12
		1.0f,1.0f,1.0f,

		-0.88f, 2.0f,2.6f,//pos 13
		1.0f,1.0f,1.0f,

			-0.73f, 2.1f,2.6f,//pos 14
		1.0f,1.0f,1.0f,

			-0.88f, 2.1f,2.6f,//pos 15
		1.0f,1.0f,1.0f,




		};
		const GLushort tankmanheadIndices[] = {

			0,1,2,
			2,3,0,

			4,5,6,
			6,7,4,

			0,1,4,
			4,5,1,

			1,5,6,
			1,2,6,

			2,7,3,
			2,7,6,

			3,0,7,
			0,4,7,

			//eye
			8,9,10,
			9,10,11,

			//eye
			12,13,14,
			13,14,15,




		};
		glGenVertexArrays(1, &VAOs[19]);
		glGenBuffers(1, &VBOs[19]);


		glBindVertexArray(VAOs[19]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[19]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tankmanhead), tankmanhead, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));


		glGenBuffers(1, &EBOs[19]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[19]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tankmanheadIndices), tankmanheadIndices, GL_STATIC_DRAW);
	}
}




void paintGL() {
	// always run
	// TODO:
	// render your objects and control the transformation here
	 // always run
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);



	glClearColor(0.028, 0.098, 0.439, 0.0); //specify the background color
	glClear(GL_COLOR_BUFFER_BIT);

	//viewMatrix with rotation 
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(-20.0f * sin(camera_press % 22 * glm::degrees(0.005f)), 5.0f, 20.0f * cos(camera_press % 22 * glm::degrees((0.005f)))), glm::vec3(0.0, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//projection matrix with scroll (idea from internet)
	glm::mat4 projectionMatrix = glm::perspective(glm::radians((int)5.0 + scroll_value), 1.0f, 1.0f, 100.0f);

	glm::mat4 modelTransformMatrix = glm::mat4(1.0f);


	GLint modelTransformMatrixUniformLocation = glGetUniformLocation(programID, "modelTransformMatrix");
	GLint viewMatrixUniformLocation = glGetUniformLocation(programID, "viewMatrix");
	GLint projectionMatrixUniformLocation = glGetUniformLocation(programID, "projectionMatrix");

	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	glClear(GL_COLOR_BUFFER_BIT);


	//draw pond (commented)
	//glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
	//modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-10.0f, 0.0f, 20.0f));
	//glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	//glBindVertexArray(VAOs[0]);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	{
		glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
		//translate whole object statically
		modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(8.0f, 0.0f, 20.0f));
		glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		//draw root1
		glBindVertexArray(VAOs[1]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

		//draw tree1bottompyramid
		glBindVertexArray(VAOs[2]);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);

		//draw tree1middlepyramid
		glBindVertexArray(VAOs[3]);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);

		//draw tree1toppyramid
		glBindVertexArray(VAOs[4]);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
	}

	{
		glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
		//translate whole object statically
		modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(4.0f, 0.0f, 20.0f));
		glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		//draw root1
		glBindVertexArray(VAOs[1]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

		//draw tree1bottompyramid
		glBindVertexArray(VAOs[2]);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);

		//draw tree1middlepyramid
		glBindVertexArray(VAOs[3]);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);

		//draw tree1toppyramid
		glBindVertexArray(VAOs[4]);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
	}

	{
		glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
		//translate whole object statically
		modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-4.0f, 0.0f, 20.0f));
		glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		//draw root1
		glBindVertexArray(VAOs[1]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

		//draw tree1bottompyramid
		glBindVertexArray(VAOs[2]);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);

		//draw tree1middlepyramid
		glBindVertexArray(VAOs[3]);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);

		//draw tree1toppyramid
		glBindVertexArray(VAOs[4]);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
	}

	{
		glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
		//translate whole object statically
		modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-8.0f, 0.0f, 20.0f));
		glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		//draw root1
		glBindVertexArray(VAOs[1]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

		//draw tree1bottompyramid
		glBindVertexArray(VAOs[2]);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);

		//draw tree1middlepyramid
		glBindVertexArray(VAOs[3]);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);

		//draw tree1toppyramid
		glBindVertexArray(VAOs[4]);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
	}

	//reintialize matrix (gound no need translate statically)
	modelTransformMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	//draw ground
	glBindVertexArray(VAOs[5]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	//draw shark head (commented)

	//glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
	//	modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-10.0f, 0.0f, 20.0f));
	//glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	//glBindVertexArray(VAOs[6]);
	//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);

	{	//draw whole superman

		//superman translate
		//glm::mat4 modelTransformMatrix = glm::mat4(1.0f);

		//modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(x_press * x_delta+2.5, -2.0f, 5.0f));
		//testing
		//modelTransformMatrix = glm::rotate(modelTransformMatrix, glm::radians(rotate_tankman*4.5f), glm::vec3(0.0f, 1.0f, 0.0f));

	//	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);

		//draw supermanbody
	//	glBindVertexArray(VAOs[7]);
	//	glDrawElements(GL_TRIANGLES, 6 * 31, GL_UNSIGNED_SHORT, 0);

		//draw supermanlegs
	//	glBindVertexArray(VAOs[8]);
	//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

		//draw supermanshoes
	//	glBindVertexArray(VAOs[9]);
	//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

		//draw supermanhead
	//	glBindVertexArray(VAOs[10]);
	//	glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_SHORT, 0);
	}

	//reinitialize transalte matrix
	modelTransformMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);

	//draw tianamen square (2D) 
	{
		//translate whole object statically
		modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-0.0f, 0.0f, 10.0f));
		glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		glBindVertexArray(VAOs[11]);

		glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_SHORT, 0);
	}

	//draw tank
	{
		glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
		//translate whole object statically
		modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-0.0f, 0.0f, 20.0f));
		glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		//translate tank when key pressed (translation)
		modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(tank_move * tank_x_delta, 0.0f, 0.0f));
		glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		glBindVertexArray(VAOs[12]);
		glDrawElements(GL_TRIANGLES, 168, GL_UNSIGNED_SHORT, 0);
	}

	//reinitialize transalte matrix
	modelTransformMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);

	//draw road
	modelTransformMatrix = glm::mat4(1.0f);
	//translate whole object statically
	modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-0.0f, 0.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glBindVertexArray(VAOs[13]);
	glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_SHORT, 0);

	//draw blood when dead
	if (tank_move < -6) {
		dead = 1;
	}
	if (dead == 1)
	{
		glBindVertexArray(VAOs[14]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	}


	//draw cone1
	modelTransformMatrix = glm::mat4(1.0f);
	//translate whole object statically
	modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(0.0f, 0.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glBindVertexArray(VAOs[15]);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);

	//draw cone2
	modelTransformMatrix = glm::mat4(1.0f);
	//translate whole object statically
	modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-10.0f, 0.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glBindVertexArray(VAOs[15]);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);

	//draw cone3
	modelTransformMatrix = glm::mat4(1.0f);
	//translate whole object statically
	modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-20.0f, 0.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glBindVertexArray(VAOs[15]);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);

	//draw cone4
	modelTransformMatrix = glm::mat4(1.0f);
	//translate whole object statically
	modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-30.0f, 0.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glBindVertexArray(VAOs[15]);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);


	//draw cone5
	modelTransformMatrix = glm::mat4(1.0f);
	//translate whole object statically
	modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-40.0f, 0.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glBindVertexArray(VAOs[15]);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);

	//draw cone6
	modelTransformMatrix = glm::mat4(1.0f);
	//translate whole object statically
	modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-50.0f, 0.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glBindVertexArray(VAOs[15]);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);


	//draw cone7
	modelTransformMatrix = glm::mat4(1.0f);
	//translate whole object statically
	modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(+10.0f, 0.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glBindVertexArray(VAOs[15]);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);

	//draw cone8
	modelTransformMatrix = glm::mat4(1.0f);
	//translate whole object statically
	modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(+20.0f, 0.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glBindVertexArray(VAOs[15]);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);


	//draw cone9
	modelTransformMatrix = glm::mat4(1.0f);
	//translate whole object statically
	modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(+30.0f, 0.0f, 20.0f));
	glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);
	glBindVertexArray(VAOs[15]);
	glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, 0);

	//when tank havent kill the tankman, draw the tankman
	if (dead == 0 && tank_move >= -6) {
		{

			//draw whole tankman

			//tankman 
			glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
			//translate whole object statically
			modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-5.0f, -2.0f, -5.5f));
			//rotate tankman when key press
			modelTransformMatrix = glm::rotate(modelTransformMatrix, glm::radians((int)90.0f + rotate_tankman * 5.5f), glm::vec3(0.0f, 1.0f, 0.0f));
			//adjust position by translating after rotation
			modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(-rotate_tankman * 0.08, 0.0, -rotate_tankman * 0.08));
			//scale up the tankman
			modelTransformMatrix = glm::scale(modelTransformMatrix, glm::vec3(scale_key * 0.1, scale_key * 0.1, +1.0f));
			//adjust position by translating after scale
			modelTransformMatrix = glm::translate(modelTransformMatrix, glm::vec3(scale_key * 0.008, scale_key * 0.005, 1.0));

			glUniformMatrix4fv(modelTransformMatrixUniformLocation, 1, GL_FALSE, &modelTransformMatrix[0][0]);

			//draw tankmanbody
			glBindVertexArray(VAOs[16]);
			glDrawElements(GL_TRIANGLES, 6 * 31, GL_UNSIGNED_SHORT, 0);

			//draw tankmanlegs
			glBindVertexArray(VAOs[17]);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

			//draw tankmanshoes
			glBindVertexArray(VAOs[18]);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

			//draw tankmanhead
			glBindVertexArray(VAOs[19]);
			glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_SHORT, 0);
		}
	}

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// TODO:
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//change camera angle
	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		camera_press += 1;
	}

	//rotate camera angle
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {

		camera_press -= 1;
	}

	//move tank backwards
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		tank_move -= 1;
	}
	//move tank backwards
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {

		tank_move += 1;
	}

	//rotate tankman leftwards to some extend
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		if (rotate_tankman >= 10) {
			rotate_tankman = 10;
		}
		rotate_tankman += 1;
	}

	//rotate tankman rightwards to some extend
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		if (rotate_tankman <= -5) {
			rotate_tankman = -5;
		}
		rotate_tankman -= 1;
	}
	//scaling up tankman
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {

		scale_key += 1;
	}
	//scaling down tankman
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		if (scale_key <= 5) {

			scale_key = 5;
		}
		else {
			scale_key -= 1;
		}
	}


}

//search scroll events for opengl from Internet
void scroll(GLFWwindow* window, double xoffset, double yoffset) {

	//Scroll to adjust the scroll angle between 1.0f and 90.0f
	if (scroll_value >= 1.0f && scroll_value <= 150.0f)
		scroll_value -= yoffset; //mouse scroll
	if (scroll_value <= 1.0f)
		//minimum scroll
		scroll_value = 1.0f;
	if (scroll_value >= 150.0f)
		//maximum scroll
		scroll_value = 150.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void initializedGL() {
	// run only once
	// TODO:
	sendDataToOpenGL();
	installShaders();
}

int main(int argc, char* argv[]) {
	GLFWwindow* window;

	/* Initialize the glfw */
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	/* glfw: configure; necessary for MAC */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(800, 600, "Assignment 1", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll);


	/* Initialize the glew */
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	//define VAO and VBO as global variables for passing VAOs and VBOs for each  objects

	get_OpenGL_info();
	initializedGL();


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		paintGL();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
