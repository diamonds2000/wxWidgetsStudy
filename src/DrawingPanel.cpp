#include <GL/glew.h>
#include "DrawingPanel.h"
#include <GL/glu.h>
#include<algorithm> 

// Request a GL canvas with a depth buffer and double buffering
static int s_gl_attribs[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 24, 0 };
#include "render/RenderObject.h"
#include "render/Sphere.h"

// Event table
wxBEGIN_EVENT_TABLE(DrawingPanel, wxGLCanvas)
EVT_PAINT(DrawingPanel::OnPaint)
EVT_LEFT_DOWN(DrawingPanel::OnMouseDown)
EVT_MOTION(DrawingPanel::OnMouseMove)
EVT_LEFT_UP(DrawingPanel::OnMouseUp)
EVT_SIZE(DrawingPanel::OnSize)
wxEND_EVENT_TABLE()

DrawingPanel::DrawingPanel(wxWindow* parent)
    : wxGLCanvas(parent, wxID_ANY, s_gl_attribs, wxDefaultPosition, wxDefaultSize, 0)
    , m_isDrawing(false)
    , m_currentColor(*wxBLACK)
    , m_currentWidth(2)
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
}

DrawingPanel::~DrawingPanel()
{
    delete m_context;
}

void DrawingPanel::SetDrawingColor(const wxColour& color)
{
    m_currentColor = color;
}

void DrawingPanel::ClearDrawing()
{
    m_strokes.clear();
    m_currentStroke.points.clear();
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

    // Set up basic OpenGL settings
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // White background
    glEnable(GL_LINE_SMOOTH);              // Anti-aliased lines
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(m_currentWidth);

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

    // Set up a default light (positional) in world coordinates
    setLight();

    // Set up viewport
    SetupViewport();

    buildScene();
}

void DrawingPanel::SetupViewport()
{
    if (!m_context || !IsShownOnScreen()) return; // Ensure the context exists and the window is fully shown

    SetCurrent(*m_context);

    glViewport(0, 0, m_width, m_height);

    // Set up orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_width, m_height, 0, -1, 1); // Y-axis inverted for screen coordinates

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
    
    Render();
    SwapBuffers(); // Swap front and back buffers
}

void DrawingPanel::OnSize(wxSizeEvent& event)
{
    wxSize size = event.GetSize();
    m_width = size.x;
    m_height = size.y;
    
    if (m_width > 0 && m_height > 0) {
        if (m_context && IsShownOnScreen()) 
        {
            SetCurrent(*m_context);
            SetupViewport();
        }
        m_needsRedraw = true;
        Refresh();
    }
    
    event.Skip();
}

void DrawingPanel::OnMouseDown(wxMouseEvent& event)
{
    if (event.LeftIsDown()) {
        m_isDrawing = true;
        m_lastPoint = event.GetPosition();
        
        // Start a new stroke
        m_currentStroke = DrawingStroke(m_currentColor, m_currentWidth);
        m_currentStroke.points.push_back(m_lastPoint);
        
        CaptureMouse();
    }
}

void DrawingPanel::OnMouseMove(wxMouseEvent& event)
{
    if (m_isDrawing && event.LeftIsDown()) {
        wxPoint currentPoint = event.GetPosition();
        m_currentStroke.points.push_back(currentPoint);
        
        // Force a repaint to show the current stroke
        Refresh();
        
        m_lastPoint = currentPoint;
    }
}

void DrawingPanel::OnMouseUp(wxMouseEvent& event)
{
    if (m_isDrawing) {
        m_isDrawing = false;
        
        if (HasCapture())
            ReleaseMouse();
        
        // Save the completed stroke
        if (!m_currentStroke.points.empty()) {
            m_strokes.push_back(m_currentStroke);
        }
        
        m_currentStroke.points.clear();
        
        // Force a repaint
        Refresh();
    }
}

void DrawingPanel::Render()
{
    if (!m_context) return;

    // Clear color and depth buffers for 3D rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If we have a 3D object, render it with a simple perspective camera
    if (m_renderObject)
    {
        PointDouble3D min, max;
        m_renderObject->getVolume(min, max);

        double volume_sphere = 1.5 * std::max({ max.x - min.x, max.y - min.y, max.z - min.z });

        glViewport(0, 0, m_width, m_height);

        // Set perspective projection
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        //double aspect = m_width > 0 && m_height > 0 ? (double)m_width / (double)m_height : 1.0;
        //gluPerspective(45.0, aspect, 1.0, 2000.0);
        glOrtho(0, m_width, m_height, 0, -volume_sphere, volume_sphere); // Y-axis inverted for screen coordinates

        // Set camera to look at center of the panel
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        // Camera positioned above the screen, looking toward the panel center
        // gluLookAt((double)m_width / 2.0, (double)m_height / 2.0, 500.0,
        //           (double)m_width / 2.0, (double)m_height / 2.0, 0.0,
        //           0.0, 1.0, 0.0);

        // Render the 3D scene
        m_renderObject->Render();

        // Restore modelview and projection
        glPopMatrix(); // MODELVIEW
        glMatrixMode(GL_PROJECTION);
        glPopMatrix(); // PROJECTION
        glMatrixMode(GL_MODELVIEW);
    }

    // Now set up orthographic projection for 2D overlays and drawing
    SetupViewport();

    // Redraw all saved strokes (2D overlay)
    for (const auto& stroke : m_strokes) {
        DrawStroke(stroke);
    }

    // Draw the current stroke if we're drawing
    if (m_isDrawing && !m_currentStroke.points.empty()) {
        DrawStroke(m_currentStroke);
    }

    // Flush OpenGL commands
    glFlush();
}

void DrawingPanel::DrawStroke(const DrawingStroke& stroke)
{
    if (stroke.points.size() < 2) return;
    
    // Set line color and width
    glColor3ub(stroke.color.Red(), stroke.color.Green(), stroke.color.Blue());
    glLineWidth(stroke.width);
    
    // Draw the stroke as a line strip
    glBegin(GL_LINE_STRIP);
    for (const auto& point : stroke.points) {
        glVertex2i(point.x, point.y);
    }
    glEnd();
}

void DrawingPanel::RedrawAll()
{
    m_needsRedraw = true;
    Refresh();
}

void DrawingPanel::setLight()
{
    GLfloat lightAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiffuse[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightPos[]      = { (GLfloat)m_width/2.0f, (GLfloat)m_height/2.0f, 500.0f, 1.0f }; // positional

    glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void DrawingPanel::buildScene()
{
    m_renderObject = std::make_unique<RenderObject>("RootObject");
    m_renderObject->setVertices({
        PointDouble3D(100.0, 100.0, 0.0),
        PointDouble3D(100.0, 200.0, 0.0),
        PointDouble3D(200.0, 200.0, 0.0)
    });

    m_renderObject->setColors({
        PointDouble3D(1.0, 0.0, 0.0), // Red
        PointDouble3D(0.0, 1.0, 0.0), // Green
        PointDouble3D(0.0, 0.0, 1.0)  // Blue
    });
    std::shared_ptr<Sphere> mySphere = std::make_shared<Sphere> ("unit_sphere", 100.0 /* radius */, 32 /* slices */, 16 /* stacks */);
    mySphere->setColors({ PointDouble3D(0.8, 0.2, 0.2) }); // Reddish color
    mySphere->setPosition(PointDouble3D(100.0, 100.0, 0.0));
    m_renderObject->addChild(mySphere);
}

