#include "RenderObject.h"
#include "SceneGraph.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include "../gl/Shader.h"
#include <cmath>


static void multiply4(const GLfloat a[16], const GLfloat b[16], GLfloat out[16])
{
    // out = a * b (column-major)
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += a[k * 4 + row] * b[col * 4 + k];
            }
            out[col * 4 + row] = sum;
        }
    }
}


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

void RenderObject::createDefaultNormal()
{
    m_normals.clear();

    if (m_vertices.size() == 3)
    {
        m_normals.push_back(PointDouble3D(0.0, 0.0, 1.0));
        m_normals.push_back(PointDouble3D(0.0, 0.0, 1.0));
        m_normals.push_back(PointDouble3D(0.0, 0.0, 1.0));
        return;
    }

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
    if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
    if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
}

void RenderObject::Render()
{
    // Enable smooth shading and color material so vertex colors interpolate across triangles
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef((GLfloat)m_position.x, (GLfloat)m_position.y, (GLfloat)m_position.z);

    // Prefer VBO rendering for speed. Initialize VBOs once when possible.
    switch (RENDER_METHOD)
    {
    case RENDER_VAO:
    {
        RenderWithVAO();
        break;
    }
    case RENDER_VBO:
        RenderWithVBO();
        break;
    case RENDER_CLIENT_ARRAY:
        RenderWithClientArray();
        break;
    case RENDER_IMMEDIATE:
    default:
        RenderWithImmediate();
        break;
    }

    for (const std::shared_ptr<RenderObject>& child : m_children)
    {
        if (child)
        {
            child->Render();
        }
    }

    glPopMatrix();

    printf("RenderObject::Render()\n");
}

void RenderObject::RenderWithVAO()
{
    if (m_vao == 0)
    {
        if (m_colors.size() != m_vertices.size())
        {
            m_colors.assign(m_vertices.size(), m_color);
        }

        m_vbo = createVBO(m_vertices, m_normals, m_colors);
        m_vao = createVAO(m_vbo);
    }

    GLfloat proj[16]; GLfloat model[16]; GLfloat mvp[16];
    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, model);
    multiply4(proj, model, mvp);
    Shader::GetDefaultShader()->setUniformMat4f("mvp", mvp);
    Shader::GetDefaultShader()->setUniformMat4f("model", model);

    if (m_vao)
    {
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_vertices.size());
        glBindVertexArray(0);
    }
}

void RenderObject::RenderWithVBO()
{
    if (m_vbo == 0)
    {
        m_vbo = createVBO(m_vertices, m_normals, m_colors);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    GLsizei stride = 9 * sizeof(float);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, stride, reinterpret_cast<void*>(0));

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, stride, reinterpret_cast<void*>(3 * sizeof(float)));

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_FLOAT, stride, reinterpret_cast<void*>(6 * sizeof(float)));

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)1);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

GLuint RenderObject::createVBO(
    const std::vector<PointDouble3D>& vertices, 
    const std::vector<PointDouble3D>& normals, 
    const std::vector<PointDouble3D>& colors)
{
    bool hasNormals = (normals.size() == vertices.size());
    //bool hasPerVertexColors = (colors.size() == vertices.size());

    if (!hasNormals)
    {
        createDefaultNormal();
    }

    size_t vertexCount = vertices.size();

    size_t bufferCount = vertexCount * 3 * 3;  //interleaved buffer
    std::vector<float> buffer;
    buffer.reserve(bufferCount);
    for (size_t i = 0; i < vertexCount; ++i)
    {
        const auto& v = vertices[i];
        const auto& n = normals[i];
        const auto& c = colors[i];

        // Vertex
        buffer.push_back((float)v.x);
        buffer.push_back((float)v.y);
        buffer.push_back((float)v.z);

        // Normal
        buffer.push_back((float)n.x);
        buffer.push_back((float)n.y);
        buffer.push_back((float)n.z);

        // Color
        buffer.push_back((float)c.x);
        buffer.push_back((float)c.y);
        buffer.push_back((float)c.z);
    }

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), buffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vbo;
}

GLuint RenderObject::createVAO(const GLuint vbo)
{
    GLuint vao = 0;
    // Create VAO/VBO (requires GL context/current)
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(float), interleaved.data(), GL_STATIC_DRAW);

    GLsizei stride = 9 * sizeof(float);
    // position at location 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0));
    // normal at location 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    // color at location 2
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vao;
}

