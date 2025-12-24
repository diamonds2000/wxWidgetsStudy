#include <GL/glew.h>
#include "DrawingPanel.h"
#include "gl/Shader.h"
#include <GL/glu.h>
#include "render/SceneGraph.h"

// Request a GL canvas with a depth buffer and double buffering
static int s_gl_attribs[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 24, 0 };

// Event table
wxBEGIN_EVENT_TABLE(DrawingPanel, wxGLCanvas)
EVT_PAINT(DrawingPanel::OnPaint)
EVT_LEFT_DOWN(DrawingPanel::OnMouseDown)
EVT_MOTION(DrawingPanel::OnMouseMove)
EVT_LEFT_UP(DrawingPanel::OnMouseUp)
EVT_SIZE(DrawingPanel::OnSize)
//EVT_TIMER(-1, DrawingPanel::OnTimer)
wxEND_EVENT_TABLE()

DrawingPanel::DrawingPanel(wxWindow* parent)
    : wxGLCanvas(parent, wxID_ANY, s_gl_attribs, wxDefaultPosition, wxDefaultSize, 0)
    , m_isDrawing(false)
    , m_needsRedraw(true)
    , m_width(0)
    , m_height(0)
{
    // Create OpenGL context
    m_context = new wxGLContext(this);
    if (!m_context) {
        wxMessageBox("Failed to create OpenGL context!");
        delete m_context;
        m_context = nullptr;
    }

    // Get initial size
    wxSize size = GetSize();
    m_width = size.x > 0 ? size.x : 640;
    m_height = size.y > 0 ? size.y : 480;

    // Initialize OpenGL when the panel is shown
    Bind(wxEVT_SHOW, [this](wxShowEvent& event) {
        if (event.IsShown()) {
            InitializeOpenGL();
        }
    });

    m_Timer = new wxTimer(this);
}

DrawingPanel::~DrawingPanel()
{
    delete m_Timer;
    delete m_context;
}

void DrawingPanel::ClearDrawing()
{
    m_needsRedraw = true;
    Refresh();
}

void DrawingPanel::InitializeOpenGL()
{
    if (!m_context || !IsShownOnScreen()) return; // Ensure the context exists and the window is fully shown

    // Set as the current OpenGL context
    SetCurrent(*m_context);

    if (glewInit() != GLEW_OK) 
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }

    // Create a simple shared shader program (example). This does not change
    // existing rendering paths yet — it provides a program you can use for
    // incremental migration to modern GL.
    static Shader* s_shader = nullptr;
    if (!s_shader) {
        s_shader = Shader::GetDefaultShader();
        if (s_shader) {
            std::cerr << "Created simple shader program: " << s_shader << std::endl;
        }
    }

    m_sceneGraph = std::make_unique<SceneGraph>();
    m_sceneGraph->init();
    m_sceneGraph->setupViewport(m_width, m_height);
    m_sceneGraph->buildScene();

    //m_Timer->Start(32); // Approx. 30 FPS
}

void DrawingPanel::OnPaint(wxPaintEvent& event)
{
    wxPaintDC(this); // Required for wxGLCanvas
    
    if (!m_context || !IsShownOnScreen()) return;
    
    SetCurrent(*m_context);
    
    // Initialize OpenGL if not already done
    static bool initialized = false;
    if (!initialized) {
        InitializeOpenGL();
        initialized = true;
    }

    m_sceneGraph->render();
    SwapBuffers(); // Swap front and back buffers
}

void DrawingPanel::OnSize(wxSizeEvent& event)
{
    wxSize size = event.GetSize();
    m_width = size.x;
    m_height = size.y;
    
    if (m_width > 0 && m_height > 0) 
    {
		if (m_context && IsShownOnScreen() && m_sceneGraph)
        {
            //SetCurrent(*m_context);
            m_sceneGraph->setupViewport(m_width, m_height);
        }
        m_needsRedraw = true;
        Refresh();
    }
    
    event.Skip();
}

void DrawingPanel::OnMouseDown(wxMouseEvent& event)
{
}

void DrawingPanel::OnMouseMove(wxMouseEvent& event)
{
}

void DrawingPanel::OnMouseUp(wxMouseEvent& event)
{
}

void DrawingPanel::RedrawAll()
{
    m_needsRedraw = true;
    Refresh();
}

void DrawingPanel::OnTimer(wxTimerEvent& event)
{
    static float angle = 0.0f;
    angle += 1.0f;
    if (angle >= 360.0f) angle -= 360.0f;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRotatef(angle, 0.0f, 1.0f, 0.0f); // Rotate scene for demonstration
    
    GLfloat model[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, model);
    glPopMatrix();

    float lightPos[3] = { 500.0f, 500.0f, 500.0f };
    for (int i = 0; i < 3; ++i)
        lightPos[i] = model[i * 4 + 0] * lightPos[0] +
                      model[i * 4 + 1] * lightPos[1] +
                      model[i * 4 + 2] * lightPos[2] +
                      model[i * 4 + 3] * 1.0f;
    
    m_sceneGraph->setLight(lightPos);

    Refresh();
}


