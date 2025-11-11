#include "CustomDialog.h"
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>

// Event table
wxBEGIN_EVENT_TABLE(CustomDialog, wxDialog)
    EVT_BUTTON(wxID_OK, CustomDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, CustomDialog::OnCancel)
wxEND_EVENT_TABLE()

CustomDialog::CustomDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Information Dialog", 
               wxDefaultPosition, wxSize(400, 250),
               wxDEFAULT_DIALOG_STYLE)
{
    // Create main sizer
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Create input panel
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    
    // Name input
    wxBoxSizer* nameSizer = new wxBoxSizer(wxHORIZONTAL);
    nameSizer->Add(new wxStaticText(panel, wxID_ANY, "Name:"), 0, 
                   wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    m_nameCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    nameSizer->Add(m_nameCtrl, 1, wxEXPAND);
    panelSizer->Add(nameSizer, 0, wxEXPAND | wxALL, 10);
    
    // Email input
    wxBoxSizer* emailSizer = new wxBoxSizer(wxHORIZONTAL);
    emailSizer->Add(new wxStaticText(panel, wxID_ANY, "Email:"), 0, 
                    wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    m_emailCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    emailSizer->Add(m_emailCtrl, 1, wxEXPAND);
    panelSizer->Add(emailSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    // Age input
    wxBoxSizer* ageSizer = new wxBoxSizer(wxHORIZONTAL);
    ageSizer->Add(new wxStaticText(panel, wxID_ANY, "Age:"), 0, 
                  wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    m_ageCtrl = new wxSpinCtrl(panel, wxID_ANY, wxEmptyString, 
                               wxDefaultPosition, wxSize(100, -1), 
                               wxSP_ARROW_KEYS, 1, 120, 25);
    ageSizer->Add(m_ageCtrl, 0);
    panelSizer->Add(ageSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    // Add some descriptive text
    wxStaticText* infoText = new wxStaticText(panel, wxID_ANY, 
        "This is a custom dialog with multiple input fields.\n"
        "Fill in your information and click OK.");
    panelSizer->Add(infoText, 0, wxEXPAND | wxALL, 10);
    
    panel->SetSizer(panelSizer);
    mainSizer->Add(panel, 1, wxEXPAND);
    
    // Create button sizer
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->AddStretchSpacer();
    buttonSizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxRIGHT, 5);
    buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"), 0);
    
    mainSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, 10);
    
    SetSizer(mainSizer);
    
    // Set focus to the first control
    m_nameCtrl->SetFocus();
    
    // Center the dialog
    Center();
}

wxString CustomDialog::GetName() const
{
    return m_nameCtrl->GetValue();
}

wxString CustomDialog::GetEmail() const
{
    return m_emailCtrl->GetValue();
}

int CustomDialog::GetAge() const
{
    return m_ageCtrl->GetValue();
}

void CustomDialog::OnOK(wxCommandEvent& event)
{
    // Validate input before accepting
    if (m_nameCtrl->GetValue().Trim().IsEmpty())
    {
        wxMessageBox("Please enter a name.", "Validation Error", 
                     wxOK | wxICON_WARNING, this);
        m_nameCtrl->SetFocus();
        return;
    }
    
    // if (m_emailCtrl->GetValue().Trim().IsEmpty() || 
    //     m_emailCtrl->GetValue().Find('@') == wxNOT_FOUND)
    // {
    //     wxMessageBox("Please enter a valid email address.", "Validation Error", 
    //                  wxOK | wxICON_WARNING, this);
    //     m_emailCtrl->SetFocus();
    //     return;
    // }
    
    // If validation passes, accept the dialog
    EndModal(wxID_OK);
}

void CustomDialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}