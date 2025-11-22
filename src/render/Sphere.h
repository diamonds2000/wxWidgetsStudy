#pragma once

#include "RenderObject.h"

class Sphere : public RenderObject
{
public:
    // radius: sphere radius
    // slices: subdivisions around the Z axis (like longitude)
    // stacks: subdivisions from top to bottom (like latitude)
    Sphere(const std::string& name, double radius, int slices, int stacks);
    virtual ~Sphere();

private:
    // Rebuild mesh with new parameters
    void Build(double radius, int slices, int stacks);

    virtual bool getVolume(PointDouble3D& min, PointDouble3D& max) const;

protected:
    double m_radius;
    int m_slices;
    int m_stacks;
};
