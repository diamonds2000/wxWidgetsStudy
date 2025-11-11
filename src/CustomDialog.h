#ifndef CUSTOMDIALOG_H
#define CUSTOMDIALOG_H

#include <wx/wx.h>
#include <wx/dialog.h>

class wxSpinCtrl;
class wxTextCtrl;

class CustomDialog : public wxDialog
{
public:
    CustomDialog(wxWindow* parent);
    
    // Getters for the input values
    wxString GetName() const;
    wxString GetEmail() const;
    int GetAge() const;

private:
    // Event handlers
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    
    // Controls
    wxTextCtrl* m_nameCtrl;
    wxTextCtrl* m_emailCtrl;
    wxSpinCtrl* m_ageCtrl;
    
    DECLARE_EVENT_TABLE()
};

#endif // CUSTOMDIALOG_H