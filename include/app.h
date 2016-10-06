#pragma once
#include <loguru/loguru.hpp>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include "shader.h"

#include "file_watching.h"

#define MENUBAR_SIZE 19

class ShadeApp {
public:
	ShadeApp();
	~ShadeApp();

	bool init(const char* title, uint16_t width, uint16_t height);
	bool loadFragmentShader(const char* filename = NULL);
	int runLoop();
private:
	bool setupGLFW(const char* title);
	bool setupGLObjects();
	bool loadBuiltins();
	void cleanupShaders(bool cleanupBuiltins);

	bool loadErrorShader();
	bool relinkProgram();

	void initUI();
	void drawUI();

	Program* _program;
    Shader* _vertexShader;
    Shader* _fragmentShader;
    GLuint _vao;
    GLuint _vertexBuffer;
    GLuint _indexBuffer;

    GLFWwindow* _window;

    const char* _currentShaderFile;
    fwatch::Timestamp _currentShaderFileTimestamp;

    uint16_t _windowWidth;
    uint16_t _windowHeight;

    Shader* _builtinVertexShader;
    Shader* _builtinErrorShader;
    Shader* _builtinDefaultShader;

    GLint _uniform_Time;

    bool _showFramerate;
};