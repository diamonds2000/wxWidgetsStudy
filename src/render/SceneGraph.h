#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <memory>
#include "RenderObject.h"


enum RenderMethod
{
    RENDER_IMMEDIATE,
    RENDER_CLIENT_ARRAY,
    RENDER_VBO,
    RENDER_VAO
};

extern const RenderMethod RENDER_METHOD;

class SceneGraph 
{
public:
    SceneGraph();
    ~SceneGraph();

    void init();
    void render();
    void buildScene();

    void setupViewport(int width, int height);

private:
    void setup();
    void setLight();

    std::unique_ptr<RenderObject> m_rootObject;
    int m_width;
    int m_height;
};