# wxWidgets Installation Guide for Manual Setup

This guide will help you set up wxWidgets manually at `D:\DevCode\wxWidgets` to build the demo application.

## Manual Installation (Recommended for this project)

### Step 1: Download wxWidgets

1. Go to https://www.wxwidgets.org/downloads/
2. Download the latest stable release for Windows (e.g., wxWidgets-3.2.4.zip)
3. Extract the archive to `D:\DevCode\wxWidgets`

Your directory structure should look like:
```
D:\DevCode\wxWidgets\
├── build\
├── include\
├── lib\
├── src\
└── ...
```

### Step 2: Build wxWidgets

#### Option A: Using Visual Studio IDE

1. Open `D:\DevCode\wxWidgets\build\msw\wx_vc16.sln` (or the appropriate version for your VS)
2. Select "Release" configuration and "x64" platform
3. Build the entire solution (Build → Build Solution)
4. This will create libraries in `D:\DevCode\wxWidgets\lib\vc_x64_dll\`

#### Option B: Using Command Line

1. Open "Developer Command Prompt for VS" (or "x64 Native Tools Command Prompt")
2. Navigate to the build directory:
```cmd
cd D:\DevCode\wxWidgets\build\msw
```

3. Build release version:
```cmd
nmake /f makefile.vc BUILD=release RUNTIME_LIBS=dynamic SHARED=1
```

4. Optionally build debug version:
```cmd
nmake /f makefile.vc BUILD=debug RUNTIME_LIBS=dynamic SHARED=1
```

### Step 3: Verify Installation

Check that these directories exist:
- `D:\DevCode\wxWidgets\lib\vc_x64_dll\` (contains .lib and .dll files)
- `D:\DevCode\wxWidgets\include\` (contains header files)
- `D:\DevCode\wxWidgets\include\msw\` (contains Windows-specific headers)

## Method 3: Using MSYS2 (Alternative)

If you use MSYS2:

```bash
pacman -S mingw-w64-x86_64-wxwidgets3.2-msw
```

## Troubleshooting

### Common Issues

1. **CMake can't find wxWidgets**:
   - Make sure VCPKG_ROOT is set correctly
   - Use the correct toolchain file in CMake command
   - For manual installation, set wxWidgets_ROOT_DIR

2. **Build errors**:
   - Ensure you have Visual Studio 2017 or later
   - Check that all required components are installed
   - Try cleaning and rebuilding

3. **Runtime DLL errors**:
   - For vcpkg: DLLs are automatically handled
   - For manual builds: Copy DLLs from lib/vc_dll to your exe directory

### Verification

To verify your installation works, try building the demo:

```powershell
# Clone or navigate to the demo directory
cd wxWidgetDemo

# Build using the provided script
.\build.bat

# Or build manually
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release
```

If everything is set up correctly, you should get a successful build and be able to run the demo application.

## Next Steps

Once wxWidgets is installed successfully:

1. Build the demo application using the instructions in README.md
2. Explore the source code to understand wxWidgets concepts
3. Start developing your own wxWidgets applications!

## Additional Resources

- [wxWidgets Documentation](https://docs.wxwidgets.org/)
- [vcpkg Documentation](https://vcpkg.io/)
- [Visual Studio C++ Documentation](https://docs.microsoft.com/en-us/cpp/)
- [CMake Documentation](https://cmake.org/documentation/)