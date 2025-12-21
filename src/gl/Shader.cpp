#include "Shader.h"
#include <iostream>


static const char* simple_vert = 
R"GLSL(#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;

uniform mat4 mvp;
uniform mat4 model;

out vec3 vNormal;
out vec3 vColor;
out vec3 vFragPos;

void main() {
    vColor = vec3(aColor.x, aColor.y, aColor.z);
    gl_Position = mvp * vec4(aPos, 1.0);
    vNormal = aNormal;
    vFragPos = vec3(model * vec4(aPos, 1.0));
}
)GLSL";

static const char* simple_frag = 
R"GLSL(#version 330 core
in vec3 vNormal;
in vec3 vColor;
in vec3 vFragPos;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightPos;

out vec4 FragColor;
void main() {
    vec3 lightDir = normalize(lightPos - vFragPos);
    float diff = max(dot(normalize(vNormal), lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, normalize(vNormal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (diffuse + ambient + specular) * vColor;

    FragColor = vec4(result, 1.0);
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

Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSrc.c_str());
    if (!vs) 
    {
        m_program = 0;
        return;
    }
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSrc.c_str());
    if (!fs) 
    {
        glDeleteShader(vs);
        m_program = 0;
        return;
    }

    m_program = linkProgram(vs, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Shader::setCurrent()
{
    glUseProgram(m_program);
}

void Shader::setUniformVec3f(const char* name, GLfloat vec[3])
{
    GLint loc = glGetUniformLocation(m_program, name);
    if (loc >= 0)
    {
        glUniform3f(loc, vec[0], vec[1], vec[2]);
    }
    else
    {
        std::cerr << "Warning: uniform '" << name << "' not found in program " << m_program << std::endl;
    }
}

void Shader::setUniformMat4f(const char* name, GLfloat mat[16])
{
    GLint loc = glGetUniformLocation(m_program, name);
    if (loc >= 0)
    {
        glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
    }
    else
    {
        std::cerr << "Warning: uniform '" << name << "' not found in program " << m_program << std::endl;
    }
}

void Shader::DebugPrintUniforms()
{
    if (m_program == 0) {
        std::cerr << "DebugPrintUniforms: Invalid program (0)" << std::endl;
        return;
    }

    GLint numUniforms = 0;
    glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &numUniforms);
    std::cerr << "\n=== Shader Program " << m_program << " ===" << std::endl;
    std::cerr << "Active Uniforms: " << numUniforms << std::endl;

    for (GLint i = 0; i < numUniforms; ++i) {
        GLchar name[256];
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = 0;

        glGetActiveUniform(m_program, (GLuint)i, sizeof(name) - 1, &length, &size, &type, name);
        name[length] = '\0';

        GLint location = glGetUniformLocation(m_program, name);
        std::cerr << "  [" << i << "] " << name << " (type=" << type << ", size=" << size << ", location=" << location << ")" << std::endl;
    }
    std::cerr << "=======================\n" << std::endl;
}

Shader* Shader::GetDefaultShader()
{
    static Shader* s_shader = nullptr;
    if (s_shader == nullptr)
        s_shader = new Shader(simple_vert, simple_frag);
    return s_shader;
}
