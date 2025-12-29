#include "SelectionBuffer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

SelectionBuffer::SelectionBuffer()
    : m_fbo(0)
    , m_prefbo(0)
    , m_colorTexture(0)
    , m_depthRenderbuffer(0)
    , m_width(0)
    , m_height(0)
{
}

SelectionBuffer::~SelectionBuffer()
{
    cleanup();
}

bool SelectionBuffer::init(int width, int height)
{
    if (width <= 0 || height <= 0) {
        std::cerr << "SelectionBuffer::init - Invalid dimensions: " 
                  << width << "x" << height << std::endl;
        return false;
    }
    
    cleanup(); // Clean up any existing resources
    
    m_width = width;
    m_height = height;
    
    // Generate framebuffer
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    
    // Create color texture for storing object IDs as colors
    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Attach color texture to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);
    
    // Create depth renderbuffer (needed for proper depth testing during selection)
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
    
    // Attach depth renderbuffer to FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);
    
    // Check framebuffer completeness
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "SelectionBuffer::init - Framebuffer not complete: " << status << std::endl;
        cleanup();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    std::cout << "SelectionBuffer initialized: " << m_width << "x" << m_height << std::endl;
    return true;
}

void SelectionBuffer::resize(int width, int height)
{
    if (width == m_width && height == m_height) {
        return; // No change needed
    }
    
    init(width, height);
}

void SelectionBuffer::bind()
{
    if (m_fbo == 0) {
        std::cerr << "SelectionBuffer::bind - FBO not initialized" << std::endl;
        return;
    }

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&m_prefbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
}

void SelectionBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_prefbo);
}

void SelectionBuffer::clear()
{
    if (m_fbo == 0) return;
    
    bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black = no selection (ID 0)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    unbind();
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
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    
    if (m_colorTexture != 0) {
        glDeleteTextures(1, &m_colorTexture);
        m_colorTexture = 0;
    }
    
    if (m_depthRenderbuffer != 0) {
        glDeleteRenderbuffers(1, &m_depthRenderbuffer);
        m_depthRenderbuffer = 0;
    }
    
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
    GLint previousFBO = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousFBO);

    // Bind FBO and ensure all rendering is complete before reading
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFinish();  // Wait for all GL commands to complete

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

    glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);

    // Check for OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "SelectionBuffer::saveToFile - OpenGL error: " << err << std::endl;
        return false;
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
