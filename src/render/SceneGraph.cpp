#include "SceneGraph.h"
#include "RenderObject.h"
#include "Sphere.h"
#include "../gl/Shader.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <algorithm> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


const RenderMethod RENDER_METHOD = RENDER_VAO;


SceneGraph::SceneGraph()
    : m_width(0), m_height(0)
{
}

SceneGraph::~SceneGraph()
{
}

GLuint SceneGraph::getFBO()
{
    return m_fbo;
}

void SceneGraph::init(int width, int height)
{
    m_width = width;
    m_height = height;

    setup();

    float lightPos[3] = { 500.0f, 500.0f, 500.0f };
    setLight(lightPos);
}

void SceneGraph::setup()
{
    if (RENDER_METHOD == RENDER_VAO)
    {
        Shader::GetDefaultShader()->setCurrent();
    }

    glEnable(GL_LINE_SMOOTH);              // Anti-aliased lines
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // Enable depth testing for 3D objects
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Ensure polygons are filled (not wireframe) and disable face culling by default
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);

    // Multiple Rendering Target (MRT) FBO setup
    GLuint tex0, tex1;
    glGenTextures(1, &tex0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glGenTextures(1, &tex1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex0, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex1, 0);

    // Create and attach depth renderbuffer
    GLuint depthRbo;
    glGenRenderbuffers(1, &depthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRbo);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "MRT FBO creation failed!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneGraph::setupViewport(int width, int height)
{
    m_width = width;
    m_height = height;

    if (m_fbo != 0)
    {
        setupCamera();
    }
}

void SceneGraph::setLight(const float pos[3])
{
    GLfloat lightPos[] = { pos[0], pos[1], pos[2], 1.0f }; // positional

    if (RENDER_METHOD == RENDER_VAO)
    {
        GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f};

        Shader::GetDefaultShader()->setUniformVec3f("lightColor", lightColor);
        Shader::GetDefaultShader()->setUniformVec3f("lightPos", lightPos);
    }
    else
    {
        // Enable basic lighting
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_NORMALIZE); // normalize normals after transforms

        GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    }
}

void SceneGraph::setupCamera()
{
    GLfloat eyePos[3] = { (GLfloat)m_width / 2.0f, (GLfloat)m_height / 2.0f, 500.0f };
    //GLfloat targetPos[3] = { (GLfloat)m_width / 2.0f, (GLfloat)m_height / 2.0f, 0.0f };
    //GLfloat upVec[3] = { 0.0f, 1.0f, 0.0f };

    // Set camera to look at center of the panel
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Camera positioned above the screen, looking toward the panel center
    // gluLookAt((double)eyePos[0], (double)eyePos[1], (double)eyePos[2],
    //           (double)targetPos[0], (double)targetPos[1], (double)targetPos[2],
    //           (double)upVec[0], (double)upVec[1], (double)upVec[2]);

    if (RENDER_METHOD == RENDER_VAO)
    {
        Shader::GetDefaultShader()->setUniformVec3f("viewPos", eyePos);
    }
}

void SceneGraph::render(bool selectionMode)
{
    if (m_fbo == 0)
    {
        return;
    }

    // Bind FBO FIRST, then clear it
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    
    // Set clear color, then clear
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If we have a 3D object, render it with a simple perspective camera
    if (m_rootObject)
    {
        PointDouble3D min, max;
        m_rootObject->getVolume(min, max);

        double volume_sphere = 1.5 * std::max({ max.x - min.x, max.y - min.y, max.z - min.z });

        glViewport(0, 0, m_width, m_height);

        // Set perspective projection
        glMatrixMode(GL_PROJECTION);
        //glLoadIdentity();
        //double aspect = m_width > 0 && m_height > 0 ? (double)m_width / (double)m_height : 1.0;
        //gluPerspective(45.0, aspect, 1.0, 2000.0);
        //glOrtho(0, m_width, m_height, 0, -volume_sphere, volume_sphere); // Y-axis inverted for screen coordinates

        glm::mat4 projection = glm::ortho(0.0f, (float)m_width, (float)m_height, 0.0f, -(float)volume_sphere, (float)volume_sphere);
        GLfloat proj[16];
        memcpy(proj, &projection, sizeof(GLfloat) * 16);
        glLoadMatrixf(proj);

        //setupCamera();

        m_rootObject->Render();
    }

    // Flush OpenGL commands
    glFlush();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glReadBuffer(GL_BACK);
}

void SceneGraph::buildScene()
{
    m_rootObject = std::make_unique<RenderObject>("RootObject");
    m_rootObject->setObjectID(1); // Assign ID 1 to triangle
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
    mySphere->setObjectID(2); // Assign ID 2 to sphere
    mySphere->setColors({ PointDouble3D(0.8, 0.2, 0.2) }); // Reddish color
    mySphere->setPosition(PointDouble3D(100.0, 100.0, 0.0));
    m_rootObject->addChild(mySphere);

    m_rootObject->buildGraphicsResources();
}
