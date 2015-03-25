#include "GLHelper.h"
#include <stdexcept>
#include <fstream>
#include <iostream>
using namespace std;

inline void getGLMat4(const algebra3::mat4& M, GLfloat arr[16]) {
	const int R[] = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3};
	const int C[] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};
	for (int i = 0; i < 16; ++i) {
		arr[i] = M[R[i]][C[i]];
	}
}

inline void getGLMat3(const algebra3::mat3& M, GLfloat arr[9]) {
	const int R[] = {0, 1, 2, 0, 1, 2, 0, 1, 2};
	const int C[] = {0, 0, 0, 1, 1, 1, 2, 2, 2};
	for (int i = 0; i < 9; ++i) {
		arr[i] = M[R[i]][C[i]];
	}
}

void die() {
	system("pause");
	exit(1);
}

void dieOnInvalidIndex(int k, const string& err) {
	if (k < 0) {
		std::cout << "Index " << k << " invalid on " << err << std::endl;
		die();
	}
}

void dieOnGLError(const string& errMessage) {
	GLenum errID = GL_NO_ERROR;
	errID = glGetError();
	if (errID != GL_NO_ERROR) {
		char errorMessage[1024];
		cout << "GL Error " << errID << ": " << errMessage << endl;
		die();
	}
}

void setUniformInt(GLuint program, const string& uniformName, int v) {
	int loc = glGetUniformLocation(program, uniformName.c_str());
	dieOnInvalidIndex(loc, uniformName);
	glUniform1i(loc, v);
	dieOnGLError("glUniform1i on " + uniformName);
}

void setUniformFloat(GLuint program, const string& uniformName, float v) {
	int loc = glGetUniformLocation(program, uniformName.c_str());
	dieOnInvalidIndex(loc, uniformName);
	glUniform1f(loc, v);
	dieOnGLError("glUniform1f on " + uniformName);
}

void setUniformVec2(GLuint program, const string& uniformName, const algebra3::vec2& v) {
	int loc = glGetUniformLocation(program, uniformName.c_str());
	dieOnInvalidIndex(loc, uniformName);
	glUniform2f(loc, v[0], v[1]);
	dieOnGLError("glUniform2f on " + uniformName);
}

void setUniformVec3(GLuint program, const string& uniformName, const algebra3::vec3& v) {
	int loc = glGetUniformLocation(program, uniformName.c_str());
	dieOnInvalidIndex(loc, uniformName);
	glUniform3f(loc, v[0], v[1], v[2]);
	dieOnGLError("glUniform3f on " + uniformName);
}

void setUniformVec4(GLuint program, const string& uniformName, const algebra3::vec4& v) {
	int loc = glGetUniformLocation(program, uniformName.c_str());
	dieOnInvalidIndex(loc, uniformName);
	glUniform4f(loc, v[0], v[1], v[2], v[3]);
	dieOnGLError("glUniform4f on " + uniformName);
}

void setUniformMat3(GLuint program, const string& uniformName, const algebra3::mat3& v) {
	int loc = glGetUniformLocation(program, uniformName.c_str());
	dieOnInvalidIndex(loc, uniformName);
	float glMat[9]; getGLMat3(v, glMat);
	glUniformMatrix3fv(loc, 1, GL_FALSE, glMat);
	dieOnGLError("glUniformMatrix3fv on " + uniformName);
}

void setUniformMat4(GLuint program, const string& uniformName, const algebra3::mat4& v) {
	int loc = glGetUniformLocation(program, uniformName.c_str());
	dieOnInvalidIndex(loc, uniformName);
	float glMat[16]; getGLMat4(v, glMat);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glMat);
	dieOnGLError("glUniformMatrix4fv on " + uniformName);
}

string getCodeFromSource(const string& fName) {
	ifstream fin(fName);
	if (!fin.is_open()) {
		cout << "The shader source file could not be found" << endl;
		die();
	}
	string temp;
	string finalOut = "";
	while (true) {
		getline(fin, temp);
		finalOut.append(temp);
		if (fin.eof())
			break;
		else
			finalOut.append("\n");
	}
	fin.close();
	return finalOut;
}

GLuint createShaderProgram(const string& vShaderFilename, const string& fShaderFilename) {
	string vShaderSource = getCodeFromSource(vShaderFilename);
	string fShaderSource = getCodeFromSource(fShaderFilename);
	const char * str = NULL;

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	dieOnGLError("glCreateShader for GL_VERTEX_SHADER");
	str = vShaderSource.c_str();
	glShaderSource(vShader, 1, &str, NULL);
	glCompileShader(vShader);
	dieOnGLError("glCompileShader for vShader");

	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	dieOnGLError("glCreateShader for GL_FRAGMENT_SHADER");
	str = fShaderSource.c_str();
	glShaderSource(fShader, 1, &str, NULL);
	glCompileShader(fShader);
	dieOnGLError("glCompileShader for fShader");

	GLuint program = glCreateProgram();
	dieOnGLError("glCreateProgram");
	
	glAttachShader(program, vShader);
	dieOnGLError("glAttachShader for vShader");
	glAttachShader(program, fShader);
	dieOnGLError("glAttachShader for fShader");
	
	glLinkProgram(program);
	dieOnGLError("glLinkProgram");
	GLint status = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLchar buff[1024];
		int len;
		glGetProgramInfoLog(program, 1023, &len, buff);
		cout << "Program could not be linked. Error Log: " << endl << buff << endl;
		die();
	}

	glUseProgram(program);
	dieOnGLError("glUseProgram");
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE) {
		GLchar buff[1024];
		int len;
		glGetProgramInfoLog(program, 1023, &len, buff);
		cout << "Program could not be validated. Error Log: " << endl << buff << endl;
		die();
	}

	return program;
}
