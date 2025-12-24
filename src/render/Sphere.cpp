#include "Sphere.h"
#include "SceneGraph.h"
#include <cmath>
#include <GL/glew.h>
#include "../gl/Shader.h"
#include <vector>


static void multiply4(const GLfloat a[16], const GLfloat b[16], GLfloat out[16])
{
    // out = a * b (column-major)
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += a[k*4 + row] * b[col*4 + k];
            }
            out[col*4 + row] = sum;
        }
    }
}

static const double PI = 3.14159265358979323846;

Sphere::Sphere(const std::string& name, double radius, int slices, int stacks)
    : RenderObject(name)
    , m_radius(radius)
    , m_slices(slices)
    , m_stacks(stacks)
{
    Build(radius, slices, stacks);
}

Sphere::~Sphere()
{
    // cleanup GL resources if any
    //if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
    //if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
}

// void Sphere::Render()
// {
//     GLuint prog = glshader::GetSimpleProgram();
//     if (!prog)
//     {
//         // fallback to base Render (immediate/client arrays)
//         RenderObject::Render();
//         return;
//     }

//     // compute current MVP from fixed-function matrices so the shader matches existing transforms
//     GLfloat proj[16]; GLfloat model[16]; GLfloat mvp[16];
//     glGetFloatv(GL_PROJECTION_MATRIX, proj);
//     glGetFloatv(GL_MODELVIEW_MATRIX, model);
//     model[12] = (GLfloat)m_position.x;
//     model[13] = (GLfloat)m_position.y;
//     model[14] = (GLfloat)m_position.z;
//     multiply4(proj, model, mvp);

//     glUseProgram(prog);
//     GLint loc = glGetUniformLocation(prog, "uMVP");
//     if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, mvp);

//     if (m_vao)
//     {
//         glBindVertexArray(m_vao);
//         glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_vertices.size());
//         glBindVertexArray(0);
//     }
//     else
//     {
//         // fallback
//         RenderObject::Render();
//     }

//     glUseProgram(0);

//     // render children
//     for (const auto& child : m_children)
//         if (child) child->Render();
// }

void Sphere::Build(double radius, int slices, int stacks)
{
    m_vertices.clear();
    m_normals.clear();
    m_colors.clear();

    if (slices < 3) slices = 3;
    if (stacks < 2) stacks = 2;

    // Generate vertices using latitude/longitude (stacks/slices)
    for (int i = 0; i < stacks; ++i)
    {
        double phi1 = PI * (double(i) / stacks - 0.5);
        double phi2 = PI * (double(i + 1) / stacks - 0.5);

        double y1 = sin(phi1);
        double y2 = sin(phi2);

        double r1 = cos(phi1);
        double r2 = cos(phi2);

        for (int j = 0; j <= slices; ++j)
        {
            double theta = 2.0 * PI * double(j) / slices;
            double x1 = r1 * cos(theta);
            double z1 = r1 * sin(theta);

            double x2 = r2 * cos(theta);
            double z2 = r2 * sin(theta);

            // First vertex (phi1, theta)
            PointDouble3D v1(x1 * radius, y1 * radius, z1 * radius);
            PointDouble3D n1(x1, y1, z1);

            // Second vertex (phi2, theta)
            PointDouble3D v2(x2 * radius, y2 * radius, z2 * radius);
            PointDouble3D n2(x2, y2, z2);

            // For triangle strip style, we add vertices in pairs; RenderObject::Render expects triangles,
            // so we'll push triangles below when iterating quads. For simplicity, store strip vertices now.
            m_vertices.push_back(v1);
            m_normals.push_back(n1);

            m_vertices.push_back(v2);
            m_normals.push_back(n2);
        }
    }

    // Convert triangle-strip-like storage into triangles for RenderObject (which draws triangles sequentially)
    // Current storage has pairs [v1,v2] repeated; we need to form triangles from adjacent pairs.
    std::vector<PointDouble3D> triVerts;
    std::vector<PointDouble3D> triNormals;
    std::vector<PointDouble3D> triColors;

    int vertsPerStack = (slices + 1) * 2;
    for (int s = 0; s < stacks; ++s)
    {
        int base = s * vertsPerStack;
        for (int j = 0; j < slices; ++j)
        {
            // indices in the strip
            int i0 = base + j * 2;
            int i1 = i0 + 1;
            int i2 = i0 + 2;
            int i3 = i0 + 3;

            // triangle 1: v0, v2, v1
            triVerts.push_back(m_vertices[i0]);
            triVerts.push_back(m_vertices[i2]);
            triVerts.push_back(m_vertices[i1]);

            triNormals.push_back(m_normals[i0]);
            triNormals.push_back(m_normals[i2]);
            triNormals.push_back(m_normals[i1]);

            // triangle 2: v2, v3, v1
            triVerts.push_back(m_vertices[i2]);
            triVerts.push_back(m_vertices[i3]);
            triVerts.push_back(m_vertices[i1]);

            triNormals.push_back(m_normals[i2]);
            triNormals.push_back(m_normals[i3]);
            triNormals.push_back(m_normals[i1]);
        }
    }

    // Replace buffers with triangle lists
    m_vertices.swap(triVerts);
    m_normals.swap(triNormals);

    // generate a simple per-vertex color if none provided: top red -> bottom blue
    m_colors.clear();
    m_colors.reserve(m_vertices.size());
    for (const auto &vv : m_vertices)
    {
        // double t = 0.5 * (vv.y / radius + 1.0);
        // if (t < 0.0) t = 0.0; 
        // if (t > 1.0) t = 1.0;
        // m_colors.emplace_back(t, 0.0, 1.0 - t);
        m_colors.emplace_back(0.8, 0.2, 0.2);
    }

    // Build interleaved buffer: pos(3), normal(3), color(3)
    // std::vector<float> interleaved;
    // interleaved.reserve(m_vertices.size() * 9);
    // for (size_t i = 0; i < m_vertices.size(); ++i)
    // {
    //     const auto &p = m_vertices[i];
    //     const auto &n = m_normals[i];
    //     const auto &c = m_colors[i];
    //     interleaved.push_back((float)p.x);
    //     interleaved.push_back((float)p.y);
    //     interleaved.push_back((float)p.z);
    //     interleaved.push_back((float)n.x);
    //     interleaved.push_back((float)n.y);
    //     interleaved.push_back((float)n.z);
    //     interleaved.push_back((float)c.x);
    //     interleaved.push_back((float)c.y);
    //     interleaved.push_back((float)c.z);
    // }

    // // Create VAO/VBO (requires GL context/current)
    // if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
    // if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }

    // glGenVertexArrays(1, &m_vao);
    // glBindVertexArray(m_vao);

    // glGenBuffers(1, &m_vbo);
    // glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(float), interleaved.data(), GL_STATIC_DRAW);

    // GLsizei stride = 9 * sizeof(float);
    // // position at location 0
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0));
    // // normal at location 1
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    // // color at location 2
    // glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool Sphere::getVolume(PointDouble3D& min, PointDouble3D& max) const
{
    min = PointDouble3D(-m_radius + m_position.x, -m_radius + m_position.y, -m_radius + m_position.z);
    max = PointDouble3D(m_radius + m_position.x, m_radius + m_position.y, m_radius + m_position.z);
    return true;
}