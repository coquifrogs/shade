#include "app.h"

#include "builtins.h"

static void error_callback(int error, const char* description) {
    LOG_F(ERROR, "Error %d: %s\n", error, description);
}

ShadeApp::ShadeApp() {
	_vertexShader = nullptr;
	_fragmentShader = nullptr;
	_program = nullptr;
	_builtinVertexShader = nullptr;
	_builtinDefaultShader = nullptr;
	_builtinErrorShader = nullptr;
	_currentShaderFile = nullptr;
    _showFramerate = true;
}

ShadeApp::~ShadeApp() {
	cleanupShaders(true);
}

bool ShadeApp::init(const char* title, uint16_t width, uint16_t height) {
	_windowWidth = width;
	_windowHeight = height;
	if(!setupGLFW(title)) return false;
	if(!setupGLObjects()) return false;
	if(!loadBuiltins()) return false;
	initUI();
	return true;
}

/* Sets up VBO, index buffer, and VAO */
bool ShadeApp::setupGLObjects() {
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    CHECK_GL(glGenBuffers(1, &_vertexBuffer));
    CHECK_GL(glGenBuffers(1, &_indexBuffer));

    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(GLfloat), _quadVerts, GL_STATIC_DRAW));

    CHECK_GL(glEnableVertexAttribArray(0));
    CHECK_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + (0 * sizeof(GLfloat))));

    CHECK_GL(glEnableVertexAttribArray(1));
    CHECK_GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + (3 * sizeof(GLfloat))));

    CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer));
    CHECK_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLint), _quadIndices, GL_STATIC_DRAW));

    CHECK_GL(glDisableVertexAttribArray(0));
    CHECK_GL(glDisableVertexAttribArray(1));
    CHECK_GL(glBindVertexArray(0));

    return true;
}

void ShadeApp::cleanupShaders(bool cleanupBuiltins) {
	if(cleanupBuiltins) {
		if(_builtinVertexShader) {
			if (_vertexShader == _builtinVertexShader) {
				_vertexShader = nullptr;
			}
			delete _builtinVertexShader;
			_builtinVertexShader = nullptr;
		}
		if(_builtinDefaultShader) {
			if (_fragmentShader == _builtinDefaultShader) {
				_fragmentShader = nullptr;
			}
			delete _builtinDefaultShader;
			_builtinDefaultShader = nullptr;
		}
		if(_builtinErrorShader) {
			if (_fragmentShader == _builtinErrorShader) {
				_fragmentShader = nullptr;
			}
			delete _builtinErrorShader;
			_builtinErrorShader = nullptr;
		}
	}
	if(_vertexShader) {
		if (_vertexShader != _builtinVertexShader) {
			delete _vertexShader;
		}
		_vertexShader = nullptr;
	}
	if(_fragmentShader) {
		if(_fragmentShader != _builtinErrorShader && _fragmentShader != _builtinDefaultShader) {
			delete _fragmentShader;
		}
		_fragmentShader = nullptr;
	}
	if(_program) {
		delete _program;
		_program = nullptr;
	}
}

bool ShadeApp::loadBuiltins() {
	_builtinVertexShader = new Shader;
	if(!_builtinVertexShader->compile(GL_VERTEX_SHADER, 0, vertex_shader, "<built-in vertex>")) {
		cleanupShaders(true);
		return false;
	}
	_builtinDefaultShader = new Shader;
	if(!_builtinDefaultShader->compile(GL_FRAGMENT_SHADER, 0, frag_shader, "<built-in default fragment>")) {
		cleanupShaders(true);
		return false;
	}
	_builtinErrorShader = new Shader;
	if(!_builtinErrorShader->compile(GL_FRAGMENT_SHADER, 0, error_frag_shader, "<built-in error shader>")) {
		cleanupShaders(true);
		return false;
	}
	_program = new Program(_builtinVertexShader, _builtinDefaultShader);
	if(!_program->link()) {
		cleanupShaders(true);
		return false;
	}

	_vertexShader = _builtinVertexShader;
	_fragmentShader = _builtinDefaultShader;

	return true;
}

bool ShadeApp::relinkProgram() {
	if(_program) {
		delete _program;
		_program = nullptr;
	}

	Program* program = new Program(_vertexShader, _fragmentShader);
    program->bindAttribLocation(0, "Pos");
    program->bindAttribLocation(1, "UV");
    if(!program->link()) {
    	delete program;
        return false;
    }
    
    _program = program;

    _uniform_Time = glGetUniformLocation(_program->getID(), "iTime");
    _uniform_Resolution = glGetUniformLocation(_program->getID(), "iResolution");
    _uniform_Mouse = glGetUniformLocation(_program->getID(), "iMouse");

    return true;
}

bool ShadeApp::loadErrorShader() {
    cleanupShaders(false);

    _vertexShader = _builtinVertexShader;
    _fragmentShader = _builtinErrorShader;

    return relinkProgram();
}

bool ShadeApp::loadFragmentShader(const char* shaderFile) {
	_currentShaderFile = shaderFile;
	_currentShaderFileTimestamp = fwatch::ZERO_TIMESTAMP;
	fwatch::checkFileModified(shaderFile, &_currentShaderFileTimestamp);

    Shader* fragmentShader = new Shader;
    
    if(!fragmentShader->compile(GL_FRAGMENT_SHADER, shaderFile)) {
        delete fragmentShader;
        return loadErrorShader();
    }
  
    cleanupShaders(false);
    _vertexShader = _builtinVertexShader;
    _fragmentShader = fragmentShader;
    
    return relinkProgram();
}

/* GLFW specific window setup */
bool ShadeApp::setupGLFW(const char* title) {
	glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    _window = glfwCreateWindow(_windowWidth, _windowHeight + MENUBAR_HEIGHT, title, NULL, NULL);
    if (!_window)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(_window);

    if (gl3wInit()) {
        LOG_F(FATAL, "Failed to initialize OpenGL\n");
        return false;
    }

    if (!gl3wIsSupported(3, 2)) {
        LOG_F(FATAL, "OpenGL 3.2 not supported\n");
        return false;
    }

    LOG_F(INFO, "OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    return true;
}

int ShadeApp::runLoop() {
	double lastFilePollTime = glfwGetTime();
    while (!glfwWindowShouldClose(_window)) {
    	// Poll for file change
    	if(_currentShaderFile && glfwGetTime() - lastFilePollTime > 1.0) {
    		if (fwatch::checkFileModified(_currentShaderFile, &_currentShaderFileTimestamp)) {
    			loadFragmentShader(_currentShaderFile);
    		}
    		lastFilePollTime = glfwGetTime();
    	}

        ImGui_ImplGlfwGL3_NewFrame();

        ////////// BEGIN FRAME ///////////

        glClear(GL_COLOR_BUFFER_BIT);

        drawUI();

        glViewport(0,0,_windowWidth,_windowHeight);

        _program->use();

        if (_uniform_Time != -1) {
            glUniform1f(_uniform_Time, glfwGetTime());
        }
        if (_uniform_Resolution != -1) {
            glUniform2f(_uniform_Resolution, (float)_windowWidth, (float)_windowHeight);
        }
        if (_uniform_Mouse != -1) {
            double x, y;
            glfwGetCursorPos(_window, &x, &y);
            glUniform2f(_uniform_Mouse, (float)x, (float)y - MENUBAR_HEIGHT);
        }


        CHECK_GL(glBindVertexArray(_vao));
        CHECK_GL(glEnableVertexAttribArray(0));
        CHECK_GL(glEnableVertexAttribArray(1));
        
        CHECK_GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
        
        CHECK_GL(glDisableVertexAttribArray(0));
        CHECK_GL(glDisableVertexAttribArray(1));
        CHECK_GL(glBindVertexArray(0));

        //////////// END FRAME ///////////
        ImGui::Render();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}