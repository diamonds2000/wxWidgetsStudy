#include "Sphere.h"
#include <cmath>

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
}

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
}

bool Sphere::getVolume(PointDouble3D& min, PointDouble3D& max) const
{
    min = PointDouble3D(-m_radius + m_position.x, -m_radius + m_position.y, -m_radius + m_position.z);
    max = PointDouble3D(m_radius + m_position.x, m_radius + m_position.y, m_radius + m_position.z);
    return true;
}