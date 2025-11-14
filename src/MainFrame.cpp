#include "MainFrame.h"
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/textdlg.h>
#include <wx/artprov.h>
#include <wx/colordlg.h>
#include <wx/numdlg.h>

// Event table
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_MENU(wxID_NEW, MainFrame::OnNew)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
    EVT_MENU(wxID_SAVE, MainFrame::OnSave)
    EVT_MENU(ID_Hello, MainFrame::OnHello)
    EVT_MENU(ID_ShowDialog, MainFrame::OnShowDialog)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_TREE_SEL_CHANGED(wxID_ANY, MainFrame::OnTreeItemSelected)
    EVT_LIST_ITEM_SELECTED(wxID_ANY, MainFrame::OnListItemSelected)
wxEND_EVENT_TABLE()

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "wxWidgets Demo Application", 
              wxDefaultPosition, wxSize(800, 600))
{
    // Set application icon (you can add your own icon file)
    // SetIcon(wxIcon(wxICON(sample))); // Commented out - icon resource not available

    // Create menu bar
    CreateMenuBar();
    
    // Create toolbar
    CreateToolBar();
    
    // Create status bar
    CreateStatusBar();
    
    // Create main controls
    //CreateControls();

    CreateDrawingView();

    // Center the frame on screen
    Center();
    
    // Set status text
    SetStatusText("Ready", 0);
}

void MainFrame::CreateMenuBar()
{
    wxMenuBar* menuBar = new wxMenuBar;
    
    // File menu
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_NEW, "&New\tCtrl-N", "Create a new document");
    fileMenu->Append(wxID_OPEN, "&Open\tCtrl-O", "Open an existing document");
    fileMenu->Append(wxID_SAVE, "&Save\tCtrl-S", "Save the current document");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt-F4", "Quit this program");
    
    // Edit menu
    wxMenu* editMenu = new wxMenu;
    editMenu->Append(wxID_UNDO, "&Undo\tCtrl-Z", "Undo the last action");
    editMenu->Append(wxID_REDO, "&Redo\tCtrl-Y", "Redo the last action");
    editMenu->AppendSeparator();
    editMenu->Append(wxID_CUT, "Cu&t\tCtrl-X", "Cut the selection");
    editMenu->Append(wxID_COPY, "&Copy\tCtrl-C", "Copy the selection");
    editMenu->Append(wxID_PASTE, "&Paste\tCtrl-V", "Paste from clipboard");
    
    // Demo menu
    wxMenu* demoMenu = new wxMenu;
    demoMenu->Append(ID_Hello, "&Hello\tCtrl-H", "Show a greeting");
    demoMenu->Append(ID_ShowDialog, "&Show Dialog\tCtrl-D", "Show a sample dialog");
    
    // Help menu
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");
    
    // Add menus to menu bar
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    menuBar->Append(demoMenu, "&Demo");
    menuBar->Append(helpMenu, "&Help");
    
    SetMenuBar(menuBar);
}

void MainFrame::CreateToolBar()
{
    wxToolBar* toolBar = wxFrame::CreateToolBar(wxTB_HORIZONTAL | wxTB_TEXT);
    
    toolBar->AddTool(wxID_NEW, "New", wxArtProvider::GetBitmap(wxART_NEW), "New file");
    toolBar->AddTool(wxID_OPEN, "Open", wxArtProvider::GetBitmap(wxART_FILE_OPEN), "Open file");
    toolBar->AddTool(wxID_SAVE, "Save", wxArtProvider::GetBitmap(wxART_FILE_SAVE), "Save file");
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_CUT, "Cut", wxArtProvider::GetBitmap(wxART_CUT), "Cut");
    toolBar->AddTool(wxID_COPY, "Copy", wxArtProvider::GetBitmap(wxART_COPY), "Copy");
    toolBar->AddTool(wxID_PASTE, "Paste", wxArtProvider::GetBitmap(wxART_PASTE), "Paste");
    toolBar->AddSeparator();
    toolBar->AddTool(ID_Hello, "Hello", wxArtProvider::GetBitmap(wxART_INFORMATION), "Say Hello");
    
    toolBar->Realize();
}

void MainFrame::CreateStatusBar()
{
    wxFrame::CreateStatusBar(2);
    SetStatusText("Ready", 0);
    SetStatusText("wxWidgets Demo", 1);
}

void MainFrame::CreateDrawingView()
{
    // Create main panel
    wxPanel* panel = new wxPanel(this);

    // Create the drawing panel
    m_drawingPanel = new DrawingPanel(panel);

    // Create a simple layout with just the drawing panel
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(m_drawingPanel, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(mainSizer);
}

void MainFrame::CreateControls()
{
    // Create main panel
    wxPanel* panel = new wxPanel(this);
    
    // Create splitter window
    m_splitter = new wxSplitterWindow(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                      wxSP_3D | wxSP_LIVE_UPDATE);
    
    // Create left panel with tree control
    wxPanel* leftPanel = new wxPanel(m_splitter);
    m_treeCtrl = new wxTreeCtrl(leftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                wxTR_DEFAULT_STYLE | wxTR_SINGLE);
    
    wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);
    leftSizer->Add(new wxStaticText(leftPanel, wxID_ANY, "Tree Control:"), 0, wxEXPAND | wxALL, 5);
    leftSizer->Add(m_treeCtrl, 1, wxEXPAND | wxALL, 5);
    leftPanel->SetSizer(leftSizer);
    
    // Create right panel with splitter for list and text
    wxSplitterWindow* rightSplitter = new wxSplitterWindow(m_splitter, wxID_ANY);
    
    // Create top right panel with list control
    wxPanel* topRightPanel = new wxPanel(rightSplitter);
    m_listCtrl = new wxListCtrl(topRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                wxLC_REPORT | wxLC_SINGLE_SEL);
    
    wxBoxSizer* topRightSizer = new wxBoxSizer(wxVERTICAL);
    topRightSizer->Add(new wxStaticText(topRightPanel, wxID_ANY, "List Control:"), 0, wxEXPAND | wxALL, 5);
    topRightSizer->Add(m_listCtrl, 1, wxEXPAND | wxALL, 5);
    topRightPanel->SetSizer(topRightSizer);
    
    // Create bottom right panel with text control
    wxPanel* bottomRightPanel = new wxPanel(rightSplitter);
    m_textCtrl = new wxTextCtrl(bottomRightPanel, wxID_ANY, "Welcome to wxWidgets Demo!\n\nThis is a multi-line text control.\nYou can type here...",
                                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    
    wxBoxSizer* bottomRightSizer = new wxBoxSizer(wxVERTICAL);
    bottomRightSizer->Add(new wxStaticText(bottomRightPanel, wxID_ANY, "Text Control:"), 0, wxEXPAND | wxALL, 5);
    bottomRightSizer->Add(m_textCtrl, 1, wxEXPAND | wxALL, 5);
    bottomRightPanel->SetSizer(bottomRightSizer);
    
    // Set up splitters
    rightSplitter->SplitHorizontally(topRightPanel, bottomRightPanel);
    rightSplitter->SetSashGravity(0.5);
    rightSplitter->SetMinimumPaneSize(100);
    
    m_splitter->SplitVertically(leftPanel, rightSplitter);
    m_splitter->SetSashGravity(0.3);
    m_splitter->SetMinimumPaneSize(150);
    
    // Main sizer
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(m_splitter, 1, wxEXPAND);
    panel->SetSizer(mainSizer);
    
    // Populate controls with sample data
    PopulateTreeCtrl();
    PopulateListCtrl();
}

void MainFrame::PopulateTreeCtrl()
{
    wxTreeItemId root = m_treeCtrl->AddRoot("Root Item");
    
    wxTreeItemId folder1 = m_treeCtrl->AppendItem(root, "Folder 1");
    m_treeCtrl->AppendItem(folder1, "Item 1.1");
    m_treeCtrl->AppendItem(folder1, "Item 1.2");
    m_treeCtrl->AppendItem(folder1, "Item 1.3");
    
    wxTreeItemId folder2 = m_treeCtrl->AppendItem(root, "Folder 2");
    m_treeCtrl->AppendItem(folder2, "Item 2.1");
    m_treeCtrl->AppendItem(folder2, "Item 2.2");
    
    wxTreeItemId folder3 = m_treeCtrl->AppendItem(root, "Folder 3");
    m_treeCtrl->AppendItem(folder3, "Item 3.1");
    
    m_treeCtrl->Expand(root);
}

void MainFrame::PopulateListCtrl()
{
    m_listCtrl->AppendColumn("Name", wxLIST_FORMAT_LEFT, 150);
    m_listCtrl->AppendColumn("Type", wxLIST_FORMAT_LEFT, 100);
    m_listCtrl->AppendColumn("Size", wxLIST_FORMAT_RIGHT, 80);
    m_listCtrl->AppendColumn("Date", wxLIST_FORMAT_LEFT, 120);
    
    long index = m_listCtrl->InsertItem(0, "Document1.txt");
    m_listCtrl->SetItem(index, 1, "Text File");
    m_listCtrl->SetItem(index, 2, "1.2 KB");
    m_listCtrl->SetItem(index, 3, "2024-01-15");
    
    index = m_listCtrl->InsertItem(1, "Image.png");
    m_listCtrl->SetItem(index, 1, "PNG Image");
    m_listCtrl->SetItem(index, 2, "45.7 KB");
    m_listCtrl->SetItem(index, 3, "2024-01-14");
    
    index = m_listCtrl->InsertItem(2, "Presentation.pdf");
    m_listCtrl->SetItem(index, 1, "PDF Document");
    m_listCtrl->SetItem(index, 2, "2.1 MB");
    m_listCtrl->SetItem(index, 3, "2024-01-13");
    
    index = m_listCtrl->InsertItem(3, "Spreadsheet.xlsx");
    m_listCtrl->SetItem(index, 1, "Excel File");
    m_listCtrl->SetItem(index, 2, "156 KB");
    m_listCtrl->SetItem(index, 3, "2024-01-12");
}

// Event handlers
void MainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets demo application.\n\n"
                 "It demonstrates various wxWidgets controls and features:\n"
                 "• Menu bar and toolbar\n"
                 "• Tree control, list control, and text control\n"
                 "• Splitter windows\n"
                 "• Status bar\n"
                 "• Dialog boxes\n\n"
                 "Built with wxWidgets " ,
                 "About wxWidgets Demo",
                 wxOK | wxICON_INFORMATION);
}

void MainFrame::OnNew(wxCommandEvent& event)
{
    m_textCtrl->Clear();
    SetStatusText("New document created", 0);
}

void MainFrame::OnOpen(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, "Open file", "", "",
                               "Text files (*.txt)|*.txt|All files (*.*)|*.*",
                               wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;
    
    wxString filename = openFileDialog.GetPath();
    if (m_textCtrl->LoadFile(filename))
    {
        SetStatusText("File opened: " + filename, 0);
    }
    else
    {
        wxMessageBox("Could not open file: " + filename, "Error", wxOK | wxICON_ERROR);
    }
}

void MainFrame::OnSave(wxCommandEvent& event)
{
    wxFileDialog saveFileDialog(this, "Save file", "", "",
                               "Text files (*.txt)|*.txt|All files (*.*)|*.*",
                               wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;
    
    wxString filename = saveFileDialog.GetPath();
    if (m_textCtrl->SaveFile(filename))
    {
        SetStatusText("File saved: " + filename, 0);
    }
    else
    {
        wxMessageBox("Could not save file: " + filename, "Error", wxOK | wxICON_ERROR);
    }
}

void MainFrame::OnHello(wxCommandEvent& event)
{
    wxMessageBox("Hello from wxWidgets!", "Greeting", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnShowDialog(wxCommandEvent& event)
{
    CustomDialog dialog(this);
    
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString name = dialog.GetName();
        wxString email = dialog.GetEmail();
        int age = dialog.GetAge();
        
        wxString message = wxString::Format(
            "Information Received:\n\n"
            "Name: %s\n"
            "Email: %s\n"
            "Age: %d years old\n\n"
            "Thank you for using the custom dialog!",
            name, email, age);
            
        wxMessageBox(message, "Custom Dialog Result", wxOK | wxICON_INFORMATION);
    }
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto())
    {
        int answer = wxMessageBox("Do you really want to close the application?", "Confirm Exit",
                                 wxYES_NO | wxICON_QUESTION);
        if (answer == wxNO)
        {
            event.Veto();
            return;
        }
    }
    
    Destroy();
}

void MainFrame::OnTreeItemSelected(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    if (item.IsOk())
    {
        wxString itemText = m_treeCtrl->GetItemText(item);
        SetStatusText("Tree item selected: " + itemText, 0);
        
        // Update text control with information about selected item
        m_textCtrl->SetValue("Selected tree item: " + itemText + "\n\n"
                            "This demonstrates tree control selection events.\n"
                            "You can expand/collapse nodes and select different items.");
    }
}

void MainFrame::OnListItemSelected(wxListEvent& event)
{
    long selectedIndex = event.GetIndex();
    if (selectedIndex != -1)
    {
        wxString itemText = m_listCtrl->GetItemText(selectedIndex);
        SetStatusText("List item selected: " + itemText, 0);
        
        // Get all column data
        wxString info = "Selected list item details:\n\n";
        info += "Name: " + m_listCtrl->GetItemText(selectedIndex, 0) + "\n";
        info += "Type: " + m_listCtrl->GetItemText(selectedIndex, 1) + "\n";
        info += "Size: " + m_listCtrl->GetItemText(selectedIndex, 2) + "\n";
        info += "Date: " + m_listCtrl->GetItemText(selectedIndex, 3) + "\n";
        info += "\nThis demonstrates list control selection events.";
        
        m_textCtrl->SetValue(info);
    }
}