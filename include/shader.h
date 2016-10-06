#pragma once

#include <string>
#include <loguru/loguru.hpp>
#include <GL/gl3w.h>

#if CHECK_OPENGL
#if __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

inline void __checkGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    if(err != GL_NO_ERROR) {
        
        do {
            
            if(gluErrorString) {
                LOG_F(ERROR, "GL 0x%08x error at %s:%i: %s - %s", err, fname, line, stmt, gluErrorString(err));
            } else {
                LOG_F(ERROR, "GL 0x%08x error at %s:%i: %s", err, fname, line, stmt);
            }
            err = glGetError();
        } while (err != GL_NO_ERROR);
    }
}

#define CHECK_GL(stmt) \
    do { \
        stmt; \
        __checkGLError(#stmt, __FILE__, __LINE__); \
    } while (0)
#else
#define CHECK_GL(stmt) stmt
#endif

class Shader {
public:
	~Shader() { if(_id != 0) CHECK_GL(glDeleteShader(_id)); }
	bool compile(GLenum shaderType, const std::string& sourceFile);
	bool compile(GLenum shaderType, GLint size, const GLchar* data, const char* filename);
	GLuint getID() const {
		return _id;
	}
private:
	GLuint _id;
};

class Program {
public:
	Program(const Shader* vertex, const Shader* fragment):_vertex(vertex), _fragment(fragment) {
		CHECK_GL(_id = glCreateProgram());
		CHECK_GL(glAttachShader(_id, vertex->getID()));
		CHECK_GL(glAttachShader(_id, fragment->getID()));
	}

	~Program() { if(_id != 0) CHECK_GL(glDeleteProgram(_id)); }

	GLuint getID() const {
		return _id;
	}

	const Shader& getVertexShader() const {
		return *_vertex;
	}

	const Shader& getFragmentShader() const {
		return *_fragment;
	}

	void bindAttribLocation(GLuint attribIndex, const GLchar* name) const;

	bool link() const;

	void use() const;

private:
	const Shader* _vertex;
	const Shader* _fragment;
	GLuint _id;
};
