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
    std::vector<PointDouble3D> vertices = m_vertices;
    vertices.resize(m_vertices.size());
    for (size_t i = 0; i < m_vertices.size(); ++i)
    {
        vertices[i] = m_vertices[i] + m_position;
    }

    glBegin(GL_TRIANGLES);
    {
        if (vertices.size() == m_colors.size())
        {
            for (size_t i = 0; i < vertices.size(); ++i)
            {
                PointFloat3D vertex = vertices[i];
                PointFloat3D color = m_colors[i];

                glColor3f(color.x, color.y, color.z);
                glVertex3f(vertex.x, vertex.y, vertex.z);

                if (m_normals.size() == vertices.size())
                {
                    PointFloat3D normal = m_normals[i];
                    glNormal3f(normal.x, normal.y, normal.z);
                }
            }
        }
        else
        {
            for (size_t i = 0; i < vertices.size(); ++i)
            {
                glColor3f(m_color.x, m_color.y, m_color.z);

                PointFloat3D vertex = vertices[i];
                glVertex3f(vertex.x, vertex.y, vertex.z);
                
                if (m_normals.size() == vertices.size())
                {
                    PointFloat3D normal = m_normals[i];
                    glNormal3f(normal.x, normal.y, normal.z);
                }

                // {
                //     // compute face normal for the triangle that contains this vertex
                //     size_t triStart = (i / 3) * 3;
                //     if (triStart + 2 < vertices.size())
                //     {
                //         PointDouble3D v0 = vertices[triStart];
                //         PointDouble3D v1 = vertices[triStart + 1];
                //         PointDouble3D v2 = vertices[triStart + 2];

                //         double ux = v1.x - v0.x;
                //         double uy = v1.y - v0.y;
                //         double uz = v1.z - v0.z;

                //         double vx = v2.x - v0.x;
                //         double vy = v2.y - v0.y;
                //         double vz = v2.z - v0.z;

                //         double nx = uy * vz - uz * vy;
                //         double ny = uz * vx - ux * vz;
                //         double nz = ux * vy - uy * vx;

                //         double len = std::sqrt(nx * nx + ny * ny + nz * nz);
                //         if (len > 1e-12)
                //         {
                //             nx /= len; ny /= len; nz /= len;
                //         }
                //         glNormal3f(static_cast<float>(nx), static_cast<float>(ny), static_cast<float>(nz));
                //     }
                //     else
                //     {
                //         // fallback normal
                //         glNormal3f(0.0f, 0.0f, 1.0f);
                //     }
                //}
            }
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
    if (m_vertices.empty())
        return false;

    for (const auto& child : m_children)
    {
        if (child)
        {
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
    }

    // Initialize min and max with the first vertex
    min = m_vertices[0];
    max = m_vertices[0];

    for (const auto& v : m_vertices)
    {
        if (v.x < min.x) min.x = v.x;
        if (v.y < min.y) min.y = v.y;
        if (v.z < min.z) min.z = v.z;

        if (v.x > max.x) max.x = v.x;
        if (v.y > max.y) max.y = v.y;
        if (v.z > max.z) max.z = v.z;
    }

    return true;
}