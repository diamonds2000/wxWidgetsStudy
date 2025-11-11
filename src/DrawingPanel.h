#ifndef DRAWINGPANEL_H
#define DRAWINGPANEL_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/dcbuffer.h>
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

class DrawingPanel : public wxPanel
{
public:
    DrawingPanel(wxWindow* parent);

    // Drawing controls
    void SetDrawingColor(const wxColour& color) { m_currentColor = color; }
    void SetPenWidth(int width) { m_currentWidth = width; }
    void ClearDrawing();

    // Getters
    wxColour GetDrawingColor() const { return m_currentColor; }
    int GetPenWidth() const { return m_currentWidth; }

private:
    // Event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event) { /* Do nothing to reduce flicker */ }

    // Drawing state
    bool m_isDrawing;
    wxPoint m_lastPoint;
    wxColour m_currentColor;
    int m_currentWidth;

    // Drawing data
    std::vector<DrawingStroke> m_strokes;
    DrawingStroke m_currentStroke;

    // Buffered drawing
    wxBitmap m_bitmap;
    bool m_needsRedraw;

    void InitializeDrawing();
    void DrawStroke(wxDC& dc, const DrawingStroke& stroke);
    void RedrawAll();

    DECLARE_EVENT_TABLE()
};

#endif // DRAWINGPANEL_H