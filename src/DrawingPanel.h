#ifndef DRAWINGPANEL_H
#define DRAWINGPANEL_H

#include <wx/wx.h>
#include <wx/glcanvas.h>
//#include <wx/glutils.h>
#include <vector>

// Structure to represent a drawing stroke
struct DrawingStroke
{
    std::vector<wxPoint> points;
    wxColour color;
    int width;

    DrawingStroke(const wxColour& col = *wxBLACK, int w = 2)
        : color(col), width(w) {
    }
};

class DrawingPanel : public wxGLCanvas
{
public:
    DrawingPanel(wxWindow* parent);
    ~DrawingPanel();

    // Drawing controls
    void SetDrawingColor(const wxColour& color);
    void SetPenWidth(int width) { m_currentWidth = width; }
    void ClearDrawing();

    // Getters
    wxColour GetDrawingColor() const { return m_currentColor; }
    int GetPenWidth() const { return m_currentWidth; }

private:
    // OpenGL context
    wxGLContext* m_context;
    
    // Event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);

    // Drawing state
    bool m_isDrawing;
    wxPoint m_lastPoint;
    wxColour m_currentColor;
    int m_currentWidth;

    // Drawing data
    std::vector<DrawingStroke> m_strokes;
    DrawingStroke m_currentStroke;

    // OpenGL state
    bool m_needsRedraw;
    int m_width, m_height;

    // OpenGL initialization and rendering
    void InitializeOpenGL();
    void Render();
    void SetupViewport();
    void DrawStroke(const DrawingStroke& stroke);
    void RedrawAll();

    DECLARE_EVENT_TABLE()
};

#endif // DRAWINGPANEL_H