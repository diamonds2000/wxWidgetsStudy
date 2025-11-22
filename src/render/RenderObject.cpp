#include "RenderObject.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <cmath>


RenderObject::RenderObject(const std::string& name)
    : m_name(name)
{
    // Constructor implementation (if needed)
}

RenderObject::~RenderObject()
{
    // Cleanup any GL resources
    cleanupVBO();
}

void RenderObject::setPosition(const PointDouble3D& position)
{
    m_position = position;
    for (PointDouble3D& v : m_vertices)
    {
        v = (v + position);
    }
}

void RenderObject::createDefaultNormal()
{
    m_normals.clear();

    // compute face normal for the triangle that contains this vertex
    for (size_t i = 0; i < m_vertices.size(); ++i)
    {
        size_t triStart = (i / 3) * 3;
        if (triStart + 2 < m_vertices.size())
        {
            PointDouble3D v0 = m_vertices[triStart + 0];
            PointDouble3D v1 = m_vertices[triStart + 1];
            PointDouble3D v2 = m_vertices[triStart + 2];

            double ux = v1.x - v0.x;
            double uy = v1.y - v0.y;
            double uz = v1.z - v0.z;

            double vx = v2.x - v0.x;
            double vy = v2.y - v0.y;
            double vz = v2.z - v0.z;

            double nx = uy * vz - uz * vy;
            double ny = uz * vx - ux * vz;
            double nz = ux * vy - uy * vx;

            double len = std::sqrt(nx * nx + ny * ny + nz * nz);
            if (len > 1e-12)
            {
                nx /= len; ny /= len; nz /= len;
            }
            glNormal3f(static_cast<float>(nx), static_cast<float>(ny), static_cast<float>(nz));
        }
        else
        {
            // fallback normal
            glNormal3f(0.0f, 0.0f, 1.0f);
        }
    }
}

void RenderObject::cleanupVBO()
{
    if (m_useVBO)
    {
        if (m_vboVertices)
        {
            glDeleteBuffers(1, &m_vboVertices);
            m_vboVertices = 0;
        }
        if (m_vboNormals)
        {
            glDeleteBuffers(1, &m_vboNormals);
            m_vboNormals = 0;
        }
        if (m_vboColors)
        {
            glDeleteBuffers(1, &m_vboColors);
            m_vboColors = 0;
        }

        m_vboCount = 0;
    }
}

void RenderObject::Render()
{
    // Enable smooth shading and color material so vertex colors interpolate across triangles
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    // Prefer VBO rendering for speed. Initialize VBOs once when possible.
    if (m_useVBO)
    {
        RenderWithVBO();
    }
    else
    {
        if (m_useClientArray)
        {
            // Client-side arrays fallback (faster than immediate mode)
            RenderWithClientArray();
        }
        else
        {
            // VBO init failed; fall back to immediate mode
            RenderWithImmediate();
        }
    }

    for (const std::shared_ptr<RenderObject>& child : m_children)
    {
        if (child)
        {
            child->Render();
        }
    }

    printf("RenderObject::Render()\n");
}

void RenderObject::RenderWithVBO()
{
    bool hasNormals = (m_normals.size() == m_vertices.size());
    bool hasPerVertexColors = (m_colors.size() == m_vertices.size());

    if (!hasNormals)
    {
        createDefaultNormal();
    }

    m_vboCount = m_vertices.size();

    std::vector<float> vertBuf;
    vertBuf.reserve(m_vboCount * 3);
    for (const auto &v : m_vertices)
    {
        vertBuf.push_back((float)v.x);
        vertBuf.push_back((float)v.y);
        vertBuf.push_back((float)v.z);
    }

    std::vector<float> normalBuf;
    normalBuf.reserve(m_vboCount * 3);
    for (const auto &n : m_normals)
    {
        normalBuf.push_back((float)n.x);
        normalBuf.push_back((float)n.y);
        normalBuf.push_back((float)n.z);
    }

    std::vector<float> colorBuf;
    if (hasPerVertexColors)
    {
        colorBuf.reserve(m_vboCount * 3);
        for (const auto &c : m_colors)
        {
            colorBuf.push_back((float)c.x);
            colorBuf.push_back((float)c.y);
            colorBuf.push_back((float)c.z);
        }
    }
    else
    {
        colorBuf.reserve(m_vboCount * 3);
        for (size_t i = 0; i < m_vboCount; ++i)
        {
            colorBuf.push_back((float)m_color.x);
            colorBuf.push_back((float)m_color.y);
            colorBuf.push_back((float)m_color.z);
        }
    }

    glGenBuffers(1, &m_vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
    glBufferData(GL_ARRAY_BUFFER, vertBuf.size() * sizeof(float), vertBuf.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
    glBufferData(GL_ARRAY_BUFFER, colorBuf.size() * sizeof(float), colorBuf.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normalBuf.size() * sizeof(float), normalBuf.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (m_vboVertices)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
        glVertexPointer(3, GL_FLOAT, 0, (void*)0);
    }

    if (m_vboNormals)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
        glNormalPointer(GL_FLOAT, 0, (void*)0);
    }

    if (m_vboColors)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
        glColorPointer(3, GL_FLOAT, 0, (void*)0);
    }

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_vboCount);

    if (m_vboColors)
    {
        glDisableClientState(GL_COLOR_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (m_vboNormals)
    {
        glDisableClientState(GL_NORMAL_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (m_vboVertices)
    {
        glDisableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void RenderObject::RenderWithClientArray()
{
    bool hasNormals = (m_normals.size() == m_vertices.size());
    bool hasPerVertexColors = (m_colors.size() == m_vertices.size());

    std::vector<float> vertBuf;
    vertBuf.reserve(m_vertices.size() * 3);
    for (const auto &v : m_vertices)
    {
        vertBuf.push_back((float)v.x);
        vertBuf.push_back((float)v.y);
        vertBuf.push_back((float)v.z);
    }

    std::vector<float> normalBuf;
    if (hasNormals)
    {
        normalBuf.reserve(m_normals.size() * 3);
        for (const auto &n : m_normals)
        {
            normalBuf.push_back((float)n.x);
            normalBuf.push_back((float)n.y);
            normalBuf.push_back((float)n.z);
        }
    }

    std::vector<float> colorBuf;
    if (hasPerVertexColors)
    {
        colorBuf.reserve(m_colors.size() * 3);
        for (const auto &c : m_colors)
        {
            colorBuf.push_back((float)c.x);
            colorBuf.push_back((float)c.y);
            colorBuf.push_back((float)c.z);
        }
    }
    else
    {
        colorBuf.reserve(m_vertices.size() * 3);
        for (size_t i = 0; i < m_vertices.size(); ++i)
        {
            colorBuf.push_back((float)m_color.x);
            colorBuf.push_back((float)m_color.y);
            colorBuf.push_back((float)m_color.z);
        }
    }

    if (hasNormals)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, normalBuf.data());
    }

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_FLOAT, 0, colorBuf.data());

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertBuf.data());

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_vertices.size());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    if (hasNormals) glDisableClientState(GL_NORMAL_ARRAY);
}

void RenderObject::RenderWithImmediate()
{
    bool hasNormals = (m_normals.size() == m_vertices.size());
    bool hasPerVertexColors = (m_colors.size() == m_vertices.size());
    
    glBegin(GL_TRIANGLES);
    if (hasPerVertexColors)
    {
        for (size_t i = 0; i < m_vertices.size(); ++i)
        {
            if (hasNormals)
            {
                PointFloat3D normal(m_normals[i]);
                glNormal3f(normal.x, normal.y, normal.z);
            }

            PointFloat3D color(m_colors[i]);
            glColor3f(color.x, color.y, color.z);
            PointFloat3D vertex(m_vertices[i]);
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    else
    {
        PointFloat3D useColor(m_color);
        glColor3f(useColor.x, useColor.y, useColor.z);
        for (size_t i = 0; i < m_vertices.size(); ++i)
        {
            if (hasNormals)
            {
                PointFloat3D normal(m_normals[i]);
                glNormal3f(normal.x, normal.y, normal.z);
            }
            PointFloat3D vertex(m_vertices[i]);
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    glEnd();
}

bool RenderObject::getVolume(PointDouble3D& min, PointDouble3D& max) const
{
    // If we have no local vertices, try to derive volume from children
    if (m_vertices.empty())
    {
        bool found = false;
        for (const auto& child : m_children)
        {
            if (!child) continue;
            PointDouble3D childMin, childMax;
            if (child->getVolume(childMin, childMax))
            {
                if (!found)
                {
                    min = childMin;
                    max = childMax;
                    found = true;
                }
                else
                {
                    if (childMin.x < min.x) min.x = childMin.x;
                    if (childMin.y < min.y) min.y = childMin.y;
                    if (childMin.z < min.z) min.z = childMin.z;

                    if (childMax.x > max.x) max.x = childMax.x;
                    if (childMax.y > max.y) max.y = childMax.y;
                    if (childMax.z > max.z) max.z = childMax.z;
                }
            }
        }
        return found;
    }

    // Initialize min and max with the first local vertex
    min = m_vertices[0];
    max = m_vertices[0];
    for (size_t i = 1; i < m_vertices.size(); ++i)
    {
        const auto& v = m_vertices[i];
        if (v.x < min.x) min.x = v.x;
        if (v.y < min.y) min.y = v.y;
        if (v.z < min.z) min.z = v.z;

        if (v.x > max.x) max.x = v.x;
        if (v.y > max.y) max.y = v.y;
        if (v.z > max.z) max.z = v.z;
    }

    // Expand by children's volumes
    for (const auto& child : m_children)
    {
        if (!child) continue;
        PointDouble3D childMin, childMax;
        if (child->getVolume(childMin, childMax))
        {
            if (childMin.x < min.x) min.x = childMin.x;
            if (childMin.y < min.y) min.y = childMin.y;
            if (childMin.z < min.z) min.z = childMin.z;

            if (childMax.x > max.x) max.x = childMax.x;
            if (childMax.y > max.y) max.y = childMax.y;
            if (childMax.z > max.z) max.z = childMax.z;
        }
    }

    return true;
}