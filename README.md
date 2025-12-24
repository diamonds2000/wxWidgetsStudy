# wxWidgets Demo Application

This is a comprehensive demo application showcasing various wxWidgets features and controls.

## Features

- **Main Window**: Professional application window with menu bar, toolbar, and status bar
- **Tree Control**: Hierarchical data display with expandable nodes
- **List Control**: Multi-column list view with sample data
- **Text Control**: Multi-line text editor with file operations
- **Splitter Windows**: Resizable panes for flexible layout
- **Menu System**: Complete menu structure with keyboard shortcuts
- **Toolbar**: Quick access buttons with icons
- **Dialog Boxes**: Message boxes, file dialogs, and text entry dialogs
- **Event Handling**: Interactive controls with status updates

## Prerequisites

Before building this application, you need to set a few environment variables:
- WXWIDGETS to the wxWidgets installation location.
- GLEW_ROOT to the glew installation location.
- GLM_ROOT to the glm installation location.


## Building the Application

### Using CMake (Recommended)

1. Create a build directory:
```powershell
mkdir build
cd build
```

2. Configure the project:
```powershell
cmake .. -G "Visual Studio 12 2013 Win64"
```

3. Build the application:
```powershell
cmake --build . --config Release
```

4. Run the application:
```powershell
.\bin\Release\wxWidgetDemo.exe
```

### Using Visual Studio

If you prefer to use Visual Studio directly:

1. Open Visual Studio
2. Select "Open a local folder" and choose the wxWidgetDemo directory
3. Configure CMake settings in Visual Studio
4. Build and run the project

## Project Structure

```
wxWidgetDemo/
├── CMakeLists.txt          # CMake build configuration
├── README.md              # This file
├── build.bat              # Windows build script
└── src/
    ├── main.cpp           # Application entry point
    ├── MainFrame.h        # Main window header
    └── MainFrame.cpp      # Main window implementation
```

## Usage

Once the application is running, you can:

1. **File Operations**: Use File menu to create new documents, open/save text files
2. **Tree Navigation**: Click on tree items to see selection events in action
3. **List Interaction**: Select items in the list control to view details
4. **Text Editing**: Type in the text control area
5. **Demo Features**: Use the Demo menu to try message boxes and input dialogs
6. **Keyboard Shortcuts**: All menu items have keyboard shortcuts (Ctrl+N, Ctrl+O, etc.)

## Troubleshooting

### Common Issues

1. **wxWidgets not found**: Ensure wxWidgets is properly installed and CMAKE_TOOLCHAIN_FILE is set correctly if using vcpkg
2. **Build errors**: Make sure you have a compatible compiler (Visual Studio 2017 or later recommended for Windows)
3. **Runtime errors**: Ensure all necessary DLLs are in the PATH or copy them to the executable directory

### Debug vs Release

- For development, use Debug configuration: `cmake --build . --config Debug`
- For distribution, use Release configuration: `cmake --build . --config Release`

## Extending the Demo

This demo serves as a foundation for wxWidgets applications. You can extend it by:

- Adding more controls (wxGrid, wxPropertyGrid, wxRibbonBar, etc.)
- Implementing custom drawing with wxPaintDC
- Adding networking capabilities with wxSocket
- Integrating multimedia features with wxMediaCtrl
- Creating custom dialogs and windows

## License

This demo application is provided as an educational example. Feel free to use and modify it for your own projects.

## Resources

- [wxWidgets Documentation](https://docs.wxwidgets.org/)
- [wxWidgets Tutorials](https://www.wxwidgets.org/docs/tutorials/)
- [wxWidgets Samples](https://github.com/wxWidgets/wxWidgets/tree/master/samples)