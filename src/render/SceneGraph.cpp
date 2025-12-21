#include "SceneGraph.h"
#include "RenderObject.h"
#include "Sphere.h"
#include "../gl/Shader.h"
#include <cassert>
#include <algorithm> 


const RenderMethod RENDER_METHOD = RENDER_VAO;

SceneGraph::SceneGraph()
    : m_width(0), m_height(0)
{
}

SceneGraph::~SceneGraph()
{
}

void SceneGraph::init()
{
    setup();
    setLight();
}

void SceneGraph::setupViewport(int width, int height)
{
    m_width = width;
    m_height = height;
}

void SceneGraph::setLight()
{
    GLfloat lightPos[] = { 5000.0f, -5000.0f, 5000.0f, 1.0f }; // positional

    if (RENDER_METHOD == RENDER_VAO)
    {
        GLfloat lightColor[] = { 1.0f, 1.0f, 0.2f};

        Shader::GetDefaultShader()->setUniformVec3f("lightColor", lightColor);
        Shader::GetDefaultShader()->setUniformVec3f("lightPos", lightPos);
    }
    else
    {
        GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    }
}

void SceneGraph::setup()
{
    Shader::GetDefaultShader()->setCurrent();

    glEnable(GL_LINE_SMOOTH);              // Anti-aliased lines
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // Enable depth testing for 3D objects
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Ensure polygons are filled (not wireframe) and disable face culling by default
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);

    // Enable basic lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE); // normalize normals after transforms
}

void SceneGraph::render()
{
    // Clear color and depth buffers for 3D rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f); // light grey background

    // If we have a 3D object, render it with a simple perspective camera
    if (m_rootObject)
    {
        PointDouble3D min, max;
        m_rootObject->getVolume(min, max);

        double volume_sphere = 1.5 * std::max({ max.x - min.x, max.y - min.y, max.z - min.z });

        glViewport(0, 0, m_width, m_height);

        // Set perspective projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        //double aspect = m_width > 0 && m_height > 0 ? (double)m_width / (double)m_height : 1.0;
        //gluPerspective(45.0, aspect, 1.0, 2000.0);
        glOrtho(0, m_width, m_height, 0, -volume_sphere, volume_sphere); // Y-axis inverted for screen coordinates

        // Set camera to look at center of the panel
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        // Camera positioned above the screen, looking toward the panel center
        // gluLookAt((double)m_width / 2.0, (double)m_height / 2.0, 500.0,
        //           (double)m_width / 2.0, (double)m_height / 2.0, 0.0,
        //           0.0, 1.0, 0.0);

        // Render the 3D scene
        m_rootObject->Render();
    }

    // Flush OpenGL commands
    glFlush();
}

void SceneGraph::buildScene()
{
    m_rootObject = std::make_unique<RenderObject>("RootObject");
    m_rootObject->setVertices({
        PointDouble3D(0.0, 0.0, 0.0),
        PointDouble3D(0.0, 200.0, 0.0),
        PointDouble3D(200.0, 200.0, 0.0)
        });

    m_rootObject->setColors({
        PointDouble3D(1.0, 0.0, 0.0), // Red
        PointDouble3D(0.0, 1.0, 0.0), // Green
        PointDouble3D(0.0, 0.0, 1.0)  // Blue
        });

    std::shared_ptr<Sphere> mySphere = std::make_shared<Sphere>("unit_sphere", 100.0 /* radius */, 32 /* slices */, 16 /* stacks */);
    mySphere->setColors({ PointDouble3D(0.8, 0.2, 0.2) }); // Reddish color
    mySphere->setPosition(PointDouble3D(100.0, 100.0, 0.0));
    m_rootObject->addChild(mySphere);
}
