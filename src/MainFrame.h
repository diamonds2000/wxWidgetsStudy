#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/toolbar.h>
#include <wx/statusbr.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/splitter.h>
#include "CustomDialog.h"
#include "DrawingPanel.h"

class MainFrame : public wxFrame
{
public:
    MainFrame();

private:
    // Event handlers
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnHello(wxCommandEvent& event);
    void OnShowDialog(wxCommandEvent& event);
    void OnTreeItemSelected(wxTreeEvent& event);
    void OnListItemSelected(wxListEvent& event);
    void OnClearDrawing(wxCommandEvent& event);
    void OnSetPenColor(wxCommandEvent& event);
    void OnSetPenWidth(wxCommandEvent& event);

    // Helper methods
    void CreateMenuBar();
    void CreateToolBar();
    void CreateStatusBar();
    void CreateControls();
    void CreateDrawingView();
    void PopulateTreeCtrl();
    void PopulateListCtrl();

    // Controls
    wxSplitterWindow* m_splitter;
    wxTreeCtrl* m_treeCtrl;
    wxListCtrl* m_listCtrl;
    wxTextCtrl* m_textCtrl;
    DrawingPanel* m_drawingPanel;
    
    // Menu and toolbar IDs
    enum
    {
        ID_Hello = 1000,
        ID_ShowDialog = 1001,
        ID_ClearDrawing = 1002,
        ID_SetPenColor = 1003,
        ID_SetPenWidth = 1004
    };

    DECLARE_EVENT_TABLE()
};

#endif // MAINFRAME_H