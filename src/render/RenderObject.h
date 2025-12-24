#pragma once
#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <string>
#include <memory>
#include "Point3D.h"


class RenderObject 
{
public:
    RenderObject(const std::string& name);
    virtual ~RenderObject();

    virtual void Render();
    virtual bool getVolume(PointDouble3D& min, PointDouble3D& max) const;

    virtual void buildGraphicsResources(); // e.g., VBOs, VAOs

    void addChild(const std::shared_ptr<RenderObject> child) { m_children.push_back(child); }
    void removeChild(const size_t i)
    {
        if (i < m_children.size())
        {
            m_children.erase(m_children.begin() + i);
        }
    }

    void setVertices(const std::vector<PointDouble3D>& vertices) { m_vertices = vertices; }
    void setNormals(const std::vector<PointDouble3D>& normals) { m_normals = normals; }
    void setTexCoords(const std::vector<PointDouble3D>& texCoords) { m_texCoords = texCoords; }
    void setColors(const std::vector<PointDouble3D>& colors) { m_colors = colors; }

    void setColors(const PointDouble3D& color)
    {
        m_color = color;
        m_colors.clear();
        m_colors.reserve(m_vertices.size());
        for (size_t i = 0; i < m_vertices.size(); ++i)
        {
            m_colors.push_back(color);
        }
    }

    void setPosition(const PointDouble3D& position);

    void createDefaultNormal();

protected:
    std::string m_name;

    std::vector<PointDouble3D> m_vertices;
    std::vector<PointDouble3D> m_normals;
    std::vector<PointDouble3D> m_texCoords;
    std::vector<PointDouble3D> m_colors;

    PointDouble3D m_color;
    PointDouble3D m_position;

    std::vector<std::shared_ptr<RenderObject>> m_children;

    // VBO support
    size_t m_vboCount = 0;
    bool m_useClientArray = false;
    bool m_useVBO = true; // can be toggled for systems without VBO support

    GLuint m_vao = 0;
    GLuint m_vbo = 0; // interleaved VBO (pos,norm,color)
    GLuint m_dispList = 0;

    GLuint createDispList(
        const std::vector<PointDouble3D>& vertices,
        const std::vector<PointDouble3D>& normals,
        const std::vector<PointDouble3D>& colors);

    GLuint createVBO(
        const std::vector<PointDouble3D>& vertices,
        const std::vector<PointDouble3D>& normals,
        const std::vector<PointDouble3D>& colors);

    GLuint createVAO(const GLuint vbo);

    void RenderWithImmediate();
    void RenderWithVBO();
    void RenderWithVAO();
    void cleanRenderResources();
};