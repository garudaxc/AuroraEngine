// OpenGLTest.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <tchar.h>
#include <math.h>

#include "gl/glew.h"

#include "glApp.h"

#include "stdheader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "AurMath.h"
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "MathFunction.h"
#include "Controller.h"
#include "Camera.h"
#include "DeviceWindow.h"


#include "Timer.h"

using namespace std;
using namespace Aurora;

#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */


GLuint vertex;
GLuint vertexBuffer;
GLuint indexBuffer;
GLuint program;
GLuint texture;
GLuint texture2;

GLint unifromScale;
GLint unifromTransfrom;

Timer timer;

CameraPerspectiveFov gCamera;
EditorCameraMover gController;

int keyDown = 0;


#define BUFFER_OFFSET(offset) ((void *)(offset))
#define COLOR(r, g, b, a) (a << 24 | b << 16 | g << 8 | r)

struct MyVertex
{
	float pos[3];
	float color[4];
	float uv[2];
};



bool LoadFile(string& code, const char* fileName)
{
	ifstream fin(fileName);

	if (!fin)
		return false;

	stringstream ss;
	ss << fin.rdbuf();
	code = ss.str();

	return true;
}

bool CompileShader(GLuint shader, const char* code)
{
	GLint state = 0;
	char error[512];

	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
	if (state != GL_TRUE)
	{
		glGetShaderInfoLog(shader, 512, NULL, error);
		cout << error << endl;
		return false;
	}

	return true;
}

GLuint LoadShader()
{
	GLint state = 0;
	char error[512];
	string code;

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	LoadFile(code, "vs.glsl");
	CompileShader(vs, code.c_str());

	GLuint ps = glCreateShader(GL_FRAGMENT_SHADER);
	LoadFile(code, "ps.glsl");
	CompileShader(ps, code.c_str());

	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, ps);
	glDeleteShader(vs);
	glDeleteShader(ps);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &state);
	if (state != GL_TRUE)
	{
		glGetProgramInfoLog(vs, 512, NULL, error);
		cout << error << endl;
		return GL_FALSE;
	}


	return program;
}

void APIENTRY DebugCallbackFunction(GLenum source,
									GLenum type,
									GLuint id,
									GLenum severity,
									GLsizei length,
									const GLchar* message,
									void* userParam)
{
	printf("Debug Message: SOURCE(0x%04X),"
		"TYPE(0x%04X),"
		"ID(0x%08X),"
		"SEVERITY(0x%04X), \"%s\"\n",
		source, type, id, severity, message);
}


void LoadTexture()
{

	// The following is an 8x8 checkerboard pattern using
	// GL_RED, GL_UNSIGNED_BYTE data.
	static const GLubyte tex_checkerboard_data[] =
	{
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF
	};
	// The following data represents a 2x2 texture with red,
	// green, blue, and yellow texels represented as GL_RGBA,
	// GL_FLOAT data.
	static const GLfloat tex_color_data[] =
	{
		// Red texel Green texel
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		// Blue texel Yellow texel
		0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	const GLuint texcolor[] = {		
		COLOR(0, 0, 0, 255), COLOR(0, 255, 0, 255), COLOR(0, 0, 255, 255), COLOR(255, 0, 0, 255), 
		COLOR(255, 0, 0, 255), COLOR(0, 255, 0, 255), COLOR(0, 0, 255, 255), COLOR(128, 128, 128, 255), 
		COLOR(255, 0, 0, 255), COLOR(0, 255, 0, 255), COLOR(0, 0, 255, 255), COLOR(255, 0, 0, 255), 
		COLOR(255, 255, 255, 255), COLOR(0, 255, 0, 255), COLOR(0, 0, 255, 255), COLOR(255, 0, 0, 255), 
	};


	glGenTextures(1, &texture);

	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, texcolor);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);




	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, 8, 8);

	glTexSubImage2D(GL_TEXTURE_2D,
		0,
		0, 0,
		8, 8,
		GL_RED, GL_UNSIGNED_BYTE,
		tex_checkerboard_data);

	static const GLint swizzles[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);



	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

};


void init(void)
{
	gController.Reset(Vector3f(0.f, -10.f, 0.f));	

	//glDebugMessageCallback(DebugCallbackFunction, &a);


	glGenVertexArrays(1, &vertex);
	glBindVertexArray(vertex);


	MyVertex vertices[] = 
	{	
		{{ -0.90f, 0.f, -0.90f}, {1.f, 0.f, 0.f, 1.f}, {0.f, 1.f}},
		{{  0.90f, 0.f, -0.90f}, {0.f, 1.f, 0.f, 1.f}, {1.f, 1.f}},
		{{ -0.90f, 0.f,  0.90f}, {0.f, 0.f, 1.f, 1.f}, {0.f, 0.f}},
		{{  0.90f, 0.f,  0.90f}, {1.f, 0.f, 1.f, 1.f}, {1.f, 0.f}},		
	};


	//GLfloat vertices[NumVertices][2] = {
	//	{ -0.90, -0.90 }, // Triangle 1
	//	{ 0.85, -0.90 },
	//	{ -0.90, 0.85 },
	//	{ 0.90, -0.85 }, // Triangle 2
	//	{ 0.90, 0.90 },
	//	{ -0.85, 0.90 }
	//};

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
		vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT,
		GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT,
		GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(12));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT,
		GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(28));


	unsigned int indexdata[] = 
	{
		0, 1, 2, 2, 1, 3
	};

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexdata), 
					indexdata, GL_STATIC_DRAW);

	glBindVertexArray(0);

	program = LoadShader();

	LoadTexture();

	timer.Start();
}




//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	gCamera.SetAspect(1.f);

	gCamera.SetTransform(gController.GetTransfrom());
	Matrix4f matView = gCamera.GetViewMatrix();
	Matrix4f matProj = gCamera.GetProjMatrix();
	matProj._33 = matProj._33 * 2.f + matProj._34 * -1.f;
	matProj._43 = matProj._43 * 2.f;


	unifromScale = glGetUniformLocation(program, "fScale");
	unifromTransfrom = glGetUniformLocation(program, "mTransfrom");
	GLint uniformView = glGetUniformLocation(program, "mView");
	GLint uniformProj = glGetUniformLocation(program, "mProj");
	GLint unifromSampler = glGetUniformLocation(program, "texture1");
	GLint unifromSampler2 = glGetUniformLocation(program, "texture2");

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	//glClear(GL_COLOR_BUFFER_BIT);

	//float scale = (sinf(timer.GetTime()) + 1.f) * 0.5f;
	float scale = sinf(timer.GetTime() * 0.5f);

	Matrix4f matRot = Matrix4f::IDENTITY;
	//matRot._41 = scale;

	MatrixRotationAxis(matRot, Vector3f::UNIT_Y, scale);	

	glBindVertexArray(vertex);
	glUseProgram(program);
	glUniform1f(unifromScale, scale);
	glUniformMatrix4fv(unifromTransfrom, 1, GL_TRUE, Matrix4f::IDENTITY.Ptr());
	glUniformMatrix4fv(uniformView, 1, GL_TRUE, matView.Ptr());
	glUniformMatrix4fv(uniformProj, 1, GL_TRUE, matProj.Ptr());

	glUniform1i(unifromSampler, 0);
	glUniform1i(unifromSampler2, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	//if (keyDown % 2)
	//{
	//}
	//else
	//{
	//	glBindTexture(GL_TEXTURE_2D, texture2);
	//}

	//glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	//glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0, 3);

	glBindVertexArray(0);

	glFlush();
	//glutSwapBuffers();

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		int i  =0;
		i++;
	}
}


class GLApp : public Application
{
public:
	GLApp()	{}
	~GLApp() {}


	virtual bool		OnInitApp();
	virtual bool		OnCreateDevice();
	virtual void		OnDestroyDevice();
	virtual void		RenderFrame();
};


bool GLApp::OnInitApp()
{

	return true;
}


bool InitOpengl(HWND hwnd, HDC hDC);

bool GLApp::OnCreateDevice()
{
	if (!InitOpengl(m_hWnd, m_hDC)) {
		return false;
	}

	if (glewInit()) {
		cerr << "Unable to initialize GLEW ... exiting" << endl;
		exit(EXIT_FAILURE);
	}

	init();

	return true;
}

void GLApp::OnDestroyDevice()
{
	
}

void GLApp::RenderFrame()
{
	display();
	SwapBuffers(m_hDC);
}


int WINAPI _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{

	//hWnd = FindWindow("FREEGLUT", argv[0]);
	//if(hWnd != NULL)
	//{
	//	pDeviceWindow = new DeviceWindowWinAPI(hWnd);
	//}


	GLApp app;
	app.Create();
	app.Run();
	
	return 0;
}
