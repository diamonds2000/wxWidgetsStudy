#!/bin/bash

# Simple build script for Linux
echo "Building wxWidgets Demo on Linux..."

# Check if wxWidgets is installed
if ! pkg-config --exists wx_gtk3u_core-3.2; then
    echo "Error: wxWidgets not found!"
    echo "Please install wxWidgets development packages:"
    echo "  Ubuntu/Debian: sudo apt-get install libwxgtk3.2-dev"
    echo "  Fedora/CentOS: sudo dnf install wxGTK3-devel"
    echo "  Arch Linux: sudo pacman -S wxwidgets-gtk3"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure and build
echo "Configuring with CMake..."
cmake ..

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

echo "Building..."
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo ""
    echo "Build successful!"
    echo "Run with: ./bin/wxWidgetDemo"
else
    echo "Build failed!"
    exit 1
fi