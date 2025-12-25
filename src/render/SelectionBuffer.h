#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>

/**
 * SelectionBuffer manages an off-screen framebuffer for object selection.
 * Objects are rendered with unique color IDs, which can be read back
 * to determine which object is at a given pixel coordinate.
 */
class SelectionBuffer
{
public:
    SelectionBuffer();
    ~SelectionBuffer();

    // Initialize the FBO with given dimensions
    bool init(int width, int height);
    
    // Resize the selection buffer
    void resize(int width, int height);
    
    // Bind this FBO for rendering (off-screen)
    void bind();
    
    // Unbind and return to default framebuffer
    void unbind();
    
    // Clear the selection buffer
    void clear();
    
    // Read the object ID at given screen coordinates
    // Returns 0 if no object, otherwise the object ID
    unsigned int readObjectID(int x, int y);
    
    // Convert an object ID to an RGB color for rendering
    static void objectIDToColor(unsigned int id, float color[3]);
    
    // Convert RGB color back to object ID
    static unsigned int colorToObjectID(unsigned char r, unsigned char g, unsigned char b);
    
    // Save the selection buffer to an image file for debugging
    // Supports .ppm (no dependencies) and .png (requires stb_image_write)
    bool saveToFile(const char* filename);
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    bool isValid() const { return m_fbo != 0; }

private:
    void cleanup();
    
    GLuint m_fbo;              // Framebuffer object
    GLuint m_prefbo;            // Previous framebuffer binding
    GLuint m_colorTexture;     // Color attachment (stores object IDs as colors)
    GLuint m_depthRenderbuffer; // Depth attachment for proper depth testing
    
    int m_width;
    int m_height;
};
