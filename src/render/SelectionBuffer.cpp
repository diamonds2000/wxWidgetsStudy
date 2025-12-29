#include "SelectionBuffer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

SelectionBuffer::SelectionBuffer()
    : m_fbo(0)
    , m_width(0)
    , m_height(0)
{
}

SelectionBuffer::~SelectionBuffer()
{
    cleanup();
}

bool SelectionBuffer::init(GLuint fbo, int width, int height)
{
    m_fbo = fbo;
    m_width = width;
    m_height = height;

    return true;
}

void SelectionBuffer::resize(int width, int height)
{
    if (width == m_width && height == m_height) {
        return; // No change needed
    }

    m_width = width;
    m_height = height;
}

void SelectionBuffer::bind()
{
    if (m_fbo == 0) {
        std::cerr << "SelectionBuffer::bind - FBO not initialized" << std::endl;
        return;
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
}

void SelectionBuffer::unbind()
{
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glReadBuffer(GL_BACK);
}

unsigned int SelectionBuffer::readObjectID(int x, int y)
{
    if (m_fbo == 0) return 0;
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return 0;
    
    // Flip Y coordinate (OpenGL uses bottom-left origin)
    int flippedY = m_height - y - 1;
    
    bind();
    
    unsigned char pixel[3];
    glReadPixels(x, flippedY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    
    unbind();
    
    return colorToObjectID(pixel[0], pixel[1], pixel[2]);
}

void SelectionBuffer::objectIDToColor(unsigned int id, float color[3])
{
    // Convert ID to RGB (24-bit color space allows up to 16,777,216 unique objects)
    color[0] = ((id >> 16) & 0xFF) / 255.0f; // Red channel
    color[1] = ((id >> 8) & 0xFF) / 255.0f;  // Green channel
    color[2] = (id & 0xFF) / 255.0f;         // Blue channel
}

unsigned int SelectionBuffer::colorToObjectID(unsigned char r, unsigned char g, unsigned char b)
{
    // Convert RGB back to ID
    return (static_cast<unsigned int>(r) << 16) |
           (static_cast<unsigned int>(g) << 8) |
           static_cast<unsigned int>(b);
}

void SelectionBuffer::cleanup()
{
    m_width = 0;
    m_height = 0;
}

bool SelectionBuffer::saveToFile(const char* filename)
{
    if (m_fbo == 0 || !filename) {
        std::cerr << "SelectionBuffer::saveToFile - Invalid FBO or filename" << std::endl;
        return false;
    }

    // Save current framebuffer binding
    // Bind FBO and ensure all rendering is complete before reading
    bind();

    // Set pixel pack alignment to 1 for tightly packed data
    GLint previousAlignment = 4;
    glGetIntegerv(GL_PACK_ALIGNMENT, &previousAlignment);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    // Allocate buffer for pixel data (no padding needed with alignment=1)
    std::vector<unsigned char> pixels(m_width * 3 * m_height);

    // Read pixels from the FBO
    glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Restore previous alignment
    glPixelStorei(GL_PACK_ALIGNMENT, previousAlignment);

    unbind();

    // Check for OpenGL errors GLenum
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "SelectionBuffer::saveToFile - OpenGL error: " << err << std::endl;
    }
    
    // Determine file format by extension
    std::string fname(filename);

    // PPM format (simple, no dependencies)
    // Change extension to .ppm if needed
    std::string ppmFilename = fname;
    std::ofstream file(ppmFilename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "SelectionBuffer::saveToFile - Failed to open file: " << ppmFilename << std::endl;
        return false;
    }
    
    // Write PPM header
    file << "P6\n" << m_width << " " << m_height << "\n255\n";
    
    // Write pixel data (flip vertically for correct orientation)
    int rowSize = m_width * 3;
    for (int y = m_height - 1; y >= 0; --y)
    {
        file.write(reinterpret_cast<const char*>(&pixels[y * rowSize]), rowSize);
    }
    
    file.close();
    
    std::cout << "SelectionBuffer saved to: " << ppmFilename << " (" 
              << m_width << "x" << m_height << ")" << std::endl;
    
    return true;
}
