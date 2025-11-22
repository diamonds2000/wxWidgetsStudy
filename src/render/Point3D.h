#pragma once

// Define the float version first so the double version can access members
class PointFloat3D
{
public:
    explicit PointFloat3D(float fx = 0.f, float fy = 0.f, float fz = 0.f)
        : x(fx), y(fy), z(fz) {}

    // Conversion from double values (no defaults to avoid ambiguity with the float constructor)
    explicit PointFloat3D(double dx, double dy, double dz)
        : x((float)dx), y((float)dy), z((float)dz) {}

    PointFloat3D(const PointFloat3D& pf)
        : x(pf.x), y(pf.y), z(pf.z) {}

    // conversion from double-version (defined later)
    PointFloat3D(const class PointDouble3D& pd);

    PointFloat3D& operator=(const PointFloat3D& pf)
    {
        x = pf.x;
        y = pf.y;
        z = pf.z;
        return *this;
    }

    PointFloat3D& operator=(const class PointDouble3D& pd);

    PointFloat3D operator+(const PointFloat3D& pf) const
    {
        return PointFloat3D(x + pf.x, y + pf.y, z + pf.z);
    }

    PointFloat3D operator-(const PointFloat3D& pf) const
    {
        return PointFloat3D(x - pf.x, y - pf.y, z - pf.z);
    }

    // conversion to double-version will be defined after PointDouble3D
    class PointDouble3D toDoule() const;
    operator class PointDouble3D() const;

public:
    float x;
    float y;
    float z;
};


class PointDouble3D
{
public:
    explicit PointDouble3D(double dx = 0.0, double dy = 0.0, double dz = 0.0)
        : x(dx), y(dy), z(dz) {}

    // Conversion from float values (no defaults to avoid ambiguity with the double constructor)
    explicit PointDouble3D(float fx, float fy, float fz)
        : x((double)fx), y((double)fy), z((double)fz) {}

    PointDouble3D(const PointDouble3D& pd)
        : x(pd.x), y(pd.y), z(pd.z) {}

    PointDouble3D(const PointFloat3D& pf)
        : x((double)pf.x), y((double)pf.y), z((double)pf.z) {}

    PointDouble3D& operator=(const PointDouble3D& pd)
    {
        x = pd.x;
        y = pd.y;
        z = pd.z;
        return *this;
    }

    PointDouble3D& operator=(const PointFloat3D& pf)
    {
        x = (double)pf.x;
        y = (double)pf.y;
        z = (double)pf.z;
        return *this;
    }

    PointDouble3D operator+(const PointDouble3D& pd) const
    {
        return PointDouble3D(x + pd.x, y + pd.y, z + pd.z);
    }

    PointDouble3D operator-(const PointDouble3D& pd) const
    {
        return PointDouble3D(x - pd.x, y - pd.y, z - pd.z);
    }

    // Named conversion method
    PointFloat3D toFloat() const
    {
        return PointFloat3D((float)x, (float)y, (float)z);
    }

    // Implicit conversion operator
    operator PointFloat3D() const
    {
        return toFloat();
    }

public:
    double x;
    double y;
    double z;
};

// Now define the remaining PointFloat3D methods that depend on PointDouble3D
inline PointFloat3D::PointFloat3D(const PointDouble3D& pd)
    : x((float)pd.x), y((float)pd.y), z((float)pd.z) {}

inline PointFloat3D& PointFloat3D::operator=(const PointDouble3D& pd)
{
    x = (float)pd.x;
    y = (float)pd.y;
    z = (float)pd.z;
    return *this;
}

inline PointDouble3D PointFloat3D::toDoule() const
{
    return PointDouble3D((double)x, (double)y, (double)z);
}

inline PointFloat3D::operator PointDouble3D() const
{
    return toDoule();
}