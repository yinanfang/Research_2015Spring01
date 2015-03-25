#ifndef _GLHELPER_H_
#define _GLHELPER_H_

#include <GL/glew.h>
#include <string>
#include "algebra3.h"

void die();
void dieOnGLError(const string& errMessage);
void dieOnInvalidIndex(int k, const std::string& err);

inline void* BUFFER_OFFSET(int i) {
	return ((void*)i);
}

void setUniformInt(GLuint program, const std::string& uniformName, int v);
void setUniformFloat(GLuint program, const std::string& uniformName, float v);
void setUniformVec2(GLuint program, const std::string& uniformName, const algebra3::vec2& v);
void setUniformVec3(GLuint program, const std::string& uniformName, const algebra3::vec3& v);
void setUniformVec4(GLuint program, const std::string& uniformName, const algebra3::vec4& v);
void setUniformMat3(GLuint program, const std::string& uniformName, const algebra3::mat3& v);
void setUniformMat4(GLuint program, const std::string& uniformName, const algebra3::mat4& v);

GLuint createShaderProgram(const std::string& vShaderFilename, const std::string& fShaderFilename);

#endif
