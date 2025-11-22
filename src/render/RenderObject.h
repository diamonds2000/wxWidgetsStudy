#pragma once

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
    }

    void setPosition(const PointDouble3D& position)
    {
        m_position = position;
    }

    virtual bool getVolume(PointDouble3D& min, PointDouble3D& max) const;

protected:
    std::string m_name;

    std::vector<PointDouble3D> m_vertices;
    std::vector<PointDouble3D> m_normals;
    std::vector<PointDouble3D> m_texCoords;
    std::vector<PointDouble3D> m_colors;

    PointDouble3D m_color;
    PointDouble3D m_position;

    std::vector<std::shared_ptr<RenderObject>> m_children;
};