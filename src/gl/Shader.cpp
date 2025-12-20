#include "Shader.h"
#include <iostream>

namespace {

static const char* simple_vert = 
R"GLSL(#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;

uniform mat4 uMVP;

out vec3 vColor;

void main() {
    vColor = aColor;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)GLSL";

static const char* simple_frag = 
R"GLSL(#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(vColor, 1.0);
}
)GLSL";

static GLuint compileShader(GLenum type, const char* src)
{
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0; glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        std::string log(len>0?len:1, '\0');
        glGetShaderInfoLog(sh, len, nullptr, &log[0]);
        std::cerr << "Shader compile error: " << log << std::endl;
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

static GLuint linkProgram(GLuint v, GLuint f)
{
    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    GLint ok = 0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0; glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
        std::string log(len>0?len:1, '\0');
        glGetProgramInfoLog(p, len, nullptr, &log[0]);
        std::cerr << "Program link error: " << log << std::endl;
        glDeleteProgram(p);
        return 0;
    }
    return p;
}

} // anonymous

namespace glshader {

GLuint CreateSimpleProgram()
{
    // Compile shaders
    GLuint vs = compileShader(GL_VERTEX_SHADER, simple_vert);
    if (!vs) return 0;
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, simple_frag);
    if (!fs) { glDeleteShader(vs); return 0; }

    GLuint prog = linkProgram(vs, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return prog;
}

} // namespace glshader

namespace glshader {

GLuint GetSimpleProgram()
{
    static GLuint s_prog = 0;
    if (s_prog == 0)
        s_prog = CreateSimpleProgram();
    return s_prog;
}

} // namespace glshader
