#pragma once

#include <GL/glew.h>
#include <string>

class Shader
{
public:
    static Shader* GetDefaultShader();

    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~Shader();

    void setCurrent();

    void setUniformVec3f(const char* name, GLfloat vec[3]);

    void setUniformMat4f(const char* name, GLfloat mat[16]);

    void DebugPrintUniforms();

private:
    GLuint m_program;
};

