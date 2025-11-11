# Cross-Platform Build Instructions

This wxWidgets demo can be built on both Windows and Linux.

## Linux Build

### Prerequisites

Install wxWidgets development packages:

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install libwxgtk3.2-dev cmake build-essential
```

**Fedora/CentOS:**
```bash
sudo dnf install wxGTK3-devel cmake gcc-c++
```

**Arch Linux:**
```bash
sudo pacman -S wxwidgets-gtk3 cmake base-devel
```

### Building

#### Option 1: Use the build script
```bash
chmod +x build_linux.sh
./build_linux.sh
```

#### Option 2: Manual build
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

#### Run the application
```bash
./bin/wxWidgetDemo
```

## Windows Build

### Prerequisites
- wxWidgets installed at `D:\DevCode\wxWidgets`
- CMake
- Visual Studio 2019 or later

### Building
```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

#### Run the application
```cmd
.\bin\wxWidgetDemo.exe
```

## Platform Differences

### Windows
- Uses WIN32 subsystem (no console window by default)
- Links against Windows-specific libraries
- Automatically copies required DLLs
- Debug console can be allocated programmatically

### Linux
- Uses GTK3 backend
- Links against system wxWidgets libraries
- Standard console output available
- Uses pkg-config to find wxWidgets

## Cross-Platform Features

The application includes:
- **Menu Bar**: File, Edit, Demo, Help menus
- **Tool Bar**: Common action buttons
- **Status Bar**: Multi-pane status display
- **Tree Control**: Hierarchical data view
- **List Control**: Multi-column data display
- **Text Control**: Multi-line text editor
- **Custom Dialog**: Multi-field input form
- **Splitter Windows**: Resizable layout panels

All features work identically on both platforms thanks to wxWidgets' cross-platform design.