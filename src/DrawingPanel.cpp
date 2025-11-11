#include "DrawingPanel.h"
#include <wx/dcclient.h>

// Event table
wxBEGIN_EVENT_TABLE(DrawingPanel, wxPanel)
EVT_PAINT(DrawingPanel::OnPaint)
EVT_LEFT_DOWN(DrawingPanel::OnMouseDown)
EVT_MOTION(DrawingPanel::OnMouseMove)
EVT_LEFT_UP(DrawingPanel::OnMouseUp)
EVT_SIZE(DrawingPanel::OnSize)
EVT_ERASE_BACKGROUND(DrawingPanel::OnEraseBackground)
wxEND_EVENT_TABLE()

DrawingPanel::DrawingPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
    , m_isDrawing(false)
    , m_currentColor(*wxBLACK)
    , m_currentWidth(2)
    , m_needsRedraw(true)
{
    SetBackgroundColour(*wxWHITE);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // For better performance

    // Initialize the drawing bitmap
    InitializeDrawing();
}

void DrawingPanel::InitializeDrawing()
{
    wxSize size = GetSize();
    if (size.x > 0 && size.y > 0)
    {
        m_bitmap = wxBitmap(size.x, size.y);

        // Clear the bitmap with white background
        wxMemoryDC memDC(m_bitmap);
        memDC.SetBackground(*wxWHITE_BRUSH);
        memDC.Clear();

        m_needsRedraw = true;
    }
}

void DrawingPanel::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);

    if (!m_bitmap.IsOk())
    {
        InitializeDrawing();
        return;
    }

    if (m_needsRedraw)
    {
        RedrawAll();
        m_needsRedraw = false;
    }

    // Draw the bitmap to screen
    dc.DrawBitmap(m_bitmap, 0, 0);

    // If currently drawing, draw the current stroke
    if (m_isDrawing && !m_currentStroke.points.empty())
    {
        DrawStroke(dc, m_currentStroke);
    }
}

void DrawingPanel::OnMouseDown(wxMouseEvent& event)
{
    if (event.LeftIsDown())
    {
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
    if (m_isDrawing && event.LeftIsDown())
    {
        wxPoint currentPoint = event.GetPosition();
        m_currentStroke.points.push_back(currentPoint);

        // Draw line from last point to current point
        wxClientDC dc(this);
        dc.SetPen(wxPen(m_currentColor, m_currentWidth, wxPENSTYLE_SOLID));
        dc.DrawLine(m_lastPoint, currentPoint);

        // Also draw to the bitmap
        wxMemoryDC memDC(m_bitmap);
        memDC.SetPen(wxPen(m_currentColor, m_currentWidth, wxPENSTYLE_SOLID));
        memDC.DrawLine(m_lastPoint, currentPoint);

        m_lastPoint = currentPoint;
    }
}

void DrawingPanel::OnMouseUp(wxMouseEvent& event)
{
    if (m_isDrawing)
    {
        m_isDrawing = false;

        if (HasCapture())
            ReleaseMouse();

        // Save the completed stroke
        if (!m_currentStroke.points.empty())
        {
            m_strokes.push_back(m_currentStroke);
        }

        m_currentStroke.points.clear();

        // Force a repaint
        Refresh();
    }
}

void DrawingPanel::OnSize(wxSizeEvent& event)
{
    // Recreate bitmap when panel is resized
    InitializeDrawing();
    event.Skip();
}

void DrawingPanel::DrawStroke(wxDC& dc, const DrawingStroke& stroke)
{
    if (stroke.points.size() < 2)
        return;

    dc.SetPen(wxPen(stroke.color, stroke.width, wxPENSTYLE_SOLID));

    for (size_t i = 1; i < stroke.points.size(); ++i)
    {
        dc.DrawLine(stroke.points[i - 1], stroke.points[i]);
    }
}

void DrawingPanel::RedrawAll()
{
    if (!m_bitmap.IsOk())
        return;

    wxMemoryDC memDC(m_bitmap);

    // Clear background
    memDC.SetBackground(*wxGREEN_BRUSH);
    memDC.Clear();

    // Draw all strokes
    for (const auto& stroke : m_strokes)
    {
        DrawStroke(memDC, stroke);
    }
}

void DrawingPanel::ClearDrawing()
{
    m_strokes.clear();
    m_currentStroke.points.clear();
    m_needsRedraw = true;

    // Clear the bitmap
    if (m_bitmap.IsOk())
    {
        wxMemoryDC memDC(m_bitmap);
        memDC.SetBackground(*wxWHITE_BRUSH);
        memDC.Clear();
    }

    Refresh();
}