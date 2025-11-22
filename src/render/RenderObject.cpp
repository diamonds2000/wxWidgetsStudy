#include "RenderObject.h"
#include <GL/gl.h>


RenderObject::RenderObject(const std::string& name)
    : m_name(name)
{
    // Constructor implementation (if needed)
}

RenderObject::~RenderObject()
{
    // Destructor implementation (if needed)
}

void RenderObject::Render()
{
    // Build transformed vertex list (apply object position)
    std::vector<PointDouble3D> vertices;
    vertices.reserve(m_vertices.size());
    for (const auto& v : m_vertices)
    {
        vertices.push_back(v + m_position);
    }

    bool hasNormals = (m_normals.size() == vertices.size());
    bool hasPerVertexColors = (m_colors.size() == vertices.size());

    // Enable smooth shading and color material so vertex colors interpolate across triangles
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    glBegin(GL_TRIANGLES);
    if (hasPerVertexColors)
    {
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            if (hasNormals)
            {
                PointFloat3D normal = m_normals[i];
                glNormal3f(normal.x, normal.y, normal.z);
            }

            PointFloat3D color = m_colors[i];
            glColor3f(color.x, color.y, color.z);

            PointFloat3D vertex = vertices[i];
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    else
    {
        // single color for all vertices
        PointFloat3D useColor = m_color;
        glColor3f(useColor.x, useColor.y, useColor.z);

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            if (hasNormals)
            {
                PointFloat3D normal = m_normals[i];
                glNormal3f(normal.x, normal.y, normal.z);
            }

            PointFloat3D vertex = vertices[i];
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
    }
    glEnd();

    for (const std::shared_ptr<RenderObject>& child : m_children)
    {
        if (child)
        {
            child->Render();
        }
    }

    printf("RenderObject::Render()\n");
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