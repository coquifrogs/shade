#include "shader.h"

#include <vector>
#include <memory>
#include <stdint.h>

struct SourceFile {
	std::string filename;
	std::shared_ptr<uint8_t> data;
	uint32_t size;
};

SourceFile readWholeFile(const std::string& path) {
	SourceFile result = {"", nullptr, 0};
	uint32_t fileSize;

	FILE* f = fopen(path.c_str(), "rb");
	if(!f) { return result; }
	fseek(f, 0, SEEK_END);
	fileSize = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	uint8_t* buff = new uint8_t[fileSize];

	fread(buff, fileSize, 1, f);
	fclose(f);

	result.filename = path;
	result.data = std::shared_ptr<uint8_t>(buff);
	result.size = fileSize;
	return result;
}

bool Shader::compile(GLenum shaderType, GLint size, const GLchar* data, const char* filename) {
	CHECK_GL(_id = glCreateShader(shaderType));
	// If shader size is 0, pass null to indicate null-terminated (ie embedded shader)
	CHECK_GL(glShaderSource(_id, 1, &data, size == 0 ? NULL : &size));
	CHECK_GL(glCompileShader(_id));

	GLint isCompiled = 0;
	CHECK_GL(glGetShaderiv(_id, GL_COMPILE_STATUS, &isCompiled));
	if(isCompiled == GL_FALSE) {
		GLint logLength = 0;
		glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLength);

		// The logLength includes the NULL character
		std::vector<GLchar> errorLog(logLength);
		glGetShaderInfoLog(_id, logLength, &logLength, &errorLog[0]);
		RAW_LOG_F(ERROR, "Failed to compile shader '%s'", filename);
		RAW_LOG_F(ERROR, "%s", &errorLog[0]);

		glDeleteShader(_id);
		return false;
	}

	return true;
}

bool Shader::compile(GLenum shaderType, const std::string& sourceFile) {
	SourceFile source = readWholeFile(sourceFile);
	if(source.size == 0) {
		LOG_F(ERROR, "Couldn't read from file '%s'", sourceFile.c_str());
		return false;
	}

	return compile(shaderType, source.size, (const GLchar*)source.data.get(), source.filename.c_str());
}

void Program::bindAttribLocation(GLuint attribIndex, const GLchar* name) const {
	CHECK_GL(glBindAttribLocation(_id, attribIndex, name));
}

bool Program::link() const {
	CHECK_GL(glLinkProgram(_id));

	GLint isLinked;
	glGetProgramiv(_id, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		GLsizei logLength = 0;
		glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> errorLog(logLength);

		glGetProgramInfoLog(_id, logLength, &logLength, &errorLog[0]);

		LOG_F(ERROR, "%s", &errorLog[0]);

		return false;
	} 

	return true;
}

void Program::use() const {
	glUseProgram(_id);
}