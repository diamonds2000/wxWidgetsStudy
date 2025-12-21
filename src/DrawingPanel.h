#ifndef DRAWINGPANEL_H
#define DRAWINGPANEL_H

#include <GL/glew.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>
//#include <wx/glutils.h>
#include <vector>
#include <memory>


class SceneGraph;

class DrawingPanel : public wxGLCanvas
{
public:
    DrawingPanel(wxWindow* parent);
    ~DrawingPanel();

    // Drawing controls
    void SetDrawingColor(const wxColour& color);
    void ClearDrawing();

private:
    // OpenGL context
    wxGLContext* m_context;
    
    // Event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnTimer(wxTimerEvent& event);

    // Drawing state
    bool m_isDrawing;
    std::unique_ptr<SceneGraph> m_sceneGraph;

    // OpenGL state
    bool m_needsRedraw;
    int m_width, m_height;

    wxTimer* m_Timer;

    // OpenGL initialization and rendering
    void InitializeOpenGL();
    void SetupViewport();
    void RedrawAll();

    DECLARE_EVENT_TABLE()
};

#endif // DRAWINGPANEL_H