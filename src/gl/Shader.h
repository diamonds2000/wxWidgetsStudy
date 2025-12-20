#pragma once

#include <GL/glew.h>
#include <string>

namespace glshader {

// Compile and link a minimal example shader program and return program id.
// Returns 0 on failure.
GLuint CreateSimpleProgram();

// Return a cached simple program instance, creating it on first call.
GLuint GetSimpleProgram();

} // namespace glshader
