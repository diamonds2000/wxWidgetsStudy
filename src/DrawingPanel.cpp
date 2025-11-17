#include "DrawingPanel.h"
#include <GL/gl.h>

// Event table
wxBEGIN_EVENT_TABLE(DrawingPanel, wxGLCanvas)
EVT_PAINT(DrawingPanel::OnPaint)
EVT_LEFT_DOWN(DrawingPanel::OnMouseDown)
EVT_MOTION(DrawingPanel::OnMouseMove)
EVT_LEFT_UP(DrawingPanel::OnMouseUp)
EVT_SIZE(DrawingPanel::OnSize)
wxEND_EVENT_TABLE()

DrawingPanel::DrawingPanel(wxWindow* parent)
    : wxGLCanvas(parent, wxID_ANY)
    , m_isDrawing(false)
    , m_currentColor(*wxBLACK)
    , m_currentWidth(2)
    , m_needsRedraw(true)
    , m_width(0)
    , m_height(0)
{
    // Create OpenGL context
    m_context = new wxGLContext(this);
    if (!m_context->IsOK()) {
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
    if (!m_context) return;

    // Set as the current OpenGL context
    SetCurrent(*m_context);

    // Set up basic OpenGL settings
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // White background
    glEnable(GL_LINE_SMOOTH);              // Anti-aliased lines
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(m_currentWidth);
    
    // Set up viewport
    SetupViewport();
}

void DrawingPanel::SetupViewport()
{
    if (!m_context) return;
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
    
    if (!m_context) return;
    
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
        if (m_context) {
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
    
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Redraw all saved strokes
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