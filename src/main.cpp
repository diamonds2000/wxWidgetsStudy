#include <wx/wx.h>
#include "MainFrame.h"
#include <iostream>
#include <cstdio>

#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #include <fcntl.h>
#endif

class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;
};

// Implement the application class
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
#ifdef _WIN32
    // Allocate a console for debugging (Windows only)
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
#endif
    
    printf("Starting wxWidgets application...\n");
    
    try {
        // Create the main application window
        printf("Creating main frame...\n");
        MainFrame* frame = new MainFrame();
        
        // Show the frame
        printf("Showing frame...\n");
        frame->Show(true);
        
        printf("Application started successfully!\n");
        // Return true to continue processing
        return true;
    } catch (const std::exception& e) {
        printf("Exception occurred: %s\n", e.what());
        return false;
    } catch (...) {
        printf("Unknown exception occurred!\n");
        return false;
    }
}