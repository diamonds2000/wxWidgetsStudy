cmake_minimum_required(VERSION 3.10)
project(wxWidgetsStudy)


# Set C++ standard
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL is not found")
endif()

# compiler flag configuration
if(WIN32)
	if(MSVC_TOOLSET_VERSION EQUAL 120)
		set(WXGIDGETS_LIB_PREFIX "vc${MSVC_TOOLSET_VERSION}_x64")
	else()
		set(WXGIDGETS_LIB_PREFIX "vc${MSVC_TOOLSET_VERSION}x_x64")
	endif()

	message(STATUS "WXGIDGETS_LIB_PREFIX = ${WXGIDGETS_LIB_PREFIX}")
	
	# Add preprocessor definitions for Windows
    add_definitions(-DWXUSINGDLL -D__WXMSW__ -D__WXDEBUG__ -DwxUSE_UNICODE)
	
    # MSVC compiler flags for debugging
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /RTC1 /MDd")
    set(CMAKE_C_FLAGS_DEBUG "/Zi /Od /RTC1 /MDd")
    # Enable full debug info in PDB
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG /INCREMENTAL:NO")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/DEBUG /INCREMENTAL:NO")
    
    # MSVC compiler flags for release
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG /MD")
    set(CMAKE_C_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG /MD")
    # Release linker flags
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO /OPT:REF /OPT:ICF")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO /OPT:REF /OPT:ICF")
else()
    # GCC/Clang flags for debugging
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
    set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall")
    
    # GCC/Clang flags for release
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -Wall")
endif()

# SDK path configuration
if(WIN32)
	message(STATUS "MSVC_VERSION = ${MSVC_VERSION}.")
	message(STATUS "MSVC_TOOLSET_VERSION = ${MSVC_TOOLSET_VERSION}.")
	
    # Windows-specific wxWidgets configuration
    # Use environment variable WXWIDGETS or fallback to default path
    if(DEFINED ENV{WXWIDGETS})
        set(WXWIDGETS_ROOT "$ENV{WXWIDGETS}")
        message(STATUS "Using wxWidgets from environment variable: ${WXWIDGETS_ROOT}")
    else()
        set(WXWIDGETS_ROOT "D:/DevCode/wxWidgets")
        message(WARNING "WXWIDGETS environment variable not set. Using default: ${WXWIDGETS_ROOT}")
    endif()
    
    set(WXWIDGETS_LIB_DIR "${WXWIDGETS_ROOT}/lib/${WXGIDGETS_LIB_PREFIX}_dll")
    set(WXWIDGETS_INCLUDE_DIR "${WXWIDGETS_ROOT}/include")
    set(WXWIDGETS_SETUP_DIR "${WXWIDGETS_LIB_DIR}/mswud")
    
    # Add include directories for Windows
    include_directories(${WXWIDGETS_INCLUDE_DIR})
    include_directories(${WXWIDGETS_SETUP_DIR})
else()
    # Linux/Unix - try to find wxWidgets
    # First try the standard FindwxWidgets module
    find_package(wxWidgets COMPONENTS core base gl)
    
    if(wxWidgets_FOUND)
        # Use standard FindwxWidgets
        include(${wxWidgets_USE_FILE})
        add_definitions(-D__WXGTK__)
        add_definitions(-DwxUSE_UNICODE)
    else()
        # Fallback to pkg-config
        find_package(PkgConfig)
        if(PKG_CONFIG_FOUND)
            pkg_check_modules(WXWIDGETS REQUIRED wx_gtk3u_core-3.2 wx_baseu-3.2 wx_gtk3u_gl-3.2)
            
            # Add include directories for Linux
            include_directories(${WXWIDGETS_INCLUDE_DIRS})
            
            # Add compiler flags for Linux
            add_compile_options(${WXWIDGETS_CFLAGS_OTHER})
            
            # Add preprocessor definitions for Linux
            add_definitions(-D__WXGTK__)
        else()
            message(FATAL_ERROR "Could not find wxWidgets. Please install:\n"
                               "  Ubuntu/Debian: sudo apt-get install libwxgtk3.2-dev\n"
                               "  Fedora: sudo dnf install wxGTK3-devel\n"
                               "  Or set wxWidgets_ROOT_DIR manually")
        endif()
    endif()
endif()

# Add executable (WIN32 flag only on Windows)
if(WIN32)
    add_executable(wxWidgetDemo WIN32
        src/main.cpp
        src/MainFrame.cpp
        src/MainFrame.h
        src/CustomDialog.cpp
        src/CustomDialog.h
        src/DrawingPanel.cpp
        src/DrawingPanel.h
    )
else()
    add_executable(wxWidgetDemo
        src/main.cpp
        src/MainFrame.cpp
        src/MainFrame.h
        src/CustomDialog.cpp
        src/CustomDialog.h
        src/DrawingPanel.cpp
        src/DrawingPanel.h
    )
endif()

# Platform-specific linking
if(WIN32)
    # Link libraries for Windows
    
    target_link_libraries(wxWidgetDemo 
        ${WXWIDGETS_LIB_DIR}/wxmsw32ud_core.lib
        ${WXWIDGETS_LIB_DIR}/wxbase32ud.lib
        ${WXWIDGETS_LIB_DIR}/wxmsw32ud_gl.lib
        # Windows system libraries
        comctl32.lib
        rpcrt4.lib
        winmm.lib
        advapi32.lib
        wsock32.lib
        opengl32.lib
        glu32.lib
    )

else()
    # Link libraries for Linux
    if(wxWidgets_FOUND)
        # Use standard wxWidgets linking
        target_link_libraries(wxWidgetDemo ${wxWidgets_LIBRARIES} ${OPENGL_LIBRARIES})
    else()
        # Use pkg-config libraries
        target_link_libraries(wxWidgetDemo ${WXWIDGETS_LIBRARIES} ${OPENGL_LIBRARIES})
    endif()
endif()

# Set output directory
set_target_properties(wxWidgetDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy DLLs to output directory (Windows only)
if(WIN32)
    add_custom_command(TARGET wxWidgetDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WXWIDGETS_LIB_DIR}/wxmsw32ud_core_${WXGIDGETS_LIB_PREFIX}.dll"
        $<TARGET_FILE_DIR:wxWidgetDemo>
    )

    add_custom_command(TARGET wxWidgetDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WXWIDGETS_LIB_DIR}/wxbase32ud_${WXGIDGETS_LIB_PREFIX}.dll"
        $<TARGET_FILE_DIR:wxWidgetDemo>
    )

    add_custom_command(TARGET wxWidgetDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WXWIDGETS_LIB_DIR}/wxmsw32ud_gl_${WXGIDGETS_LIB_PREFIX}.dll"
        $<TARGET_FILE_DIR:wxWidgetDemo>
    )
endif()
