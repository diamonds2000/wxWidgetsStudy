#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <memory>
#include "RenderObject.h"

class SceneGraph 
{
public:
    SceneGraph();
    ~SceneGraph();

    void Render();
    void buildScene();

    void setupViewport(int width, int height);

private:
    void setup();
    void setLight();

    std::unique_ptr<RenderObject> m_rootObject;
    int m_width;
    int m_height;
};