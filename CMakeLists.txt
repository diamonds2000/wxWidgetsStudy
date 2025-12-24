cmake_minimum_required(VERSION 3.10)
project(wxWidgetsStudy)


# Set C++ standard
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(NOT WIN32)
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL is not found")
endif()

# SDK path configuration
if(WIN32)
    # Windows - wxWidgets will be configured manually.
    if(DEFINED ENV{WXWIDGETS})
        set(WXWIDGETS_ROOT "$ENV{WXWIDGETS}")
        message(STATUS "Using wxWidgets from environment variable: WXWIDGETS_ROOT")
    endif()

    if(MSVC_TOOLSET_VERSION EQUAL 120)
        set(WXWIDGETS_LIB_PREFIX "vc${MSVC_TOOLSET_VERSION}_x64")
    else()
        set(WXWIDGETS_LIB_PREFIX "vc${MSVC_TOOLSET_VERSION}x_x64")
    endif()
    
    set(WXWIDGETS_LIB_DIR "${WXWIDGETS_ROOT}/lib/${WXWIDGETS_LIB_PREFIX}_dll")
    set(WXWIDGETS_INCLUDE_DIR "${WXWIDGETS_ROOT}/include")
    set(WXWIDGETS_VC_INCLUDE_DIR "${WXWIDGETS_ROOT}/include/msvc")

    message(STATUS "WXWIDGETS_LIB_PREFIX = ${WXWIDGETS_LIB_PREFIX}")
    message(STATUS "MSVC_VERSION = ${MSVC_VERSION}.")
    message(STATUS "MSVC_TOOLSET_VERSION = ${MSVC_TOOLSET_VERSION}.")

    if(DEFINED ENV{GLEW_ROOT})
        set(GLEW_ROOT "$ENV{GLEW_ROOT}")
        message(STATUS "Using glew from environment variable: GLEW_ROOT")
    endif()

    set(GLEW_INCLUDE_DIR "${GLEW_ROOT}/include")
    set(GLEW_LIB_DIR "${GLEW_ROOT}/lib/Release/x64")

    if(DEFINED ENV{GLM_ROOT})
        set(GLM_ROOT "$ENV{GLM_ROOT}")
        message(STATUS "Using glm from environment variable: GLM_ROOT")
    endif()

    set(GLM_INCLUDE_DIR "${GLM_ROOT}")
else()
    # Linux/Unix - try to find wxWidgets
    # First try the standard FindwxWidgets module
    find_package(wxWidgets COMPONENTS core base gl)

    find_package(GLEW REQUIRED)
    if(NOT GLEW_FOUND)
        message(FATAL_ERROR "GLEW is not found")
    endif()

    find_package(glm REQUIRED)
    if(NOT glm_FOUND)
        message(FATAL_ERROR "GLM is not found")
    endif()

    include_directories(${GLEW_INCLUDE_DIRS})
    
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


if(WIN32)
    set(WIN32_FLAG WIN32)
else()
    set(WIN32_FLAG "")
endif()

# compiler flag configuration
if(WIN32)
    # empty
else()
    # GCC/Clang flags for debugging
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
    set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall")
    
    # GCC/Clang flags for release
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -Wall")
endif()

add_executable(wxWidgetDemo ${WIN32_FLAG}
        src/main.cpp
        src/MainFrame.cpp
        src/MainFrame.h
        src/CustomDialog.cpp
        src/CustomDialog.h
        src/DrawingPanel.cpp
        src/DrawingPanel.h
        src/render/SceneGraph.cpp
        src/render/SceneGraph.h
        src/render/Point3D.h
        src/render/RenderObject.cpp
        src/render/Sphere.cpp
        src/render/RenderObject.h
        src/gl/Shader.cpp
        src/gl/Shader.h
)

# Platform-specific linking
if(WIN32)
    target_compile_definitions(wxWidgetDemo PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _UNICODE
        UNICODE
        wxMSVC_VERSION_AUTO
        wxUSE_UNICODE
        WXUSINGDLL 
        __WXMSW__ 
        __WXDEBUG__ 
    )

    target_compile_options(wxWidgetDemo PRIVATE
        $<$<CONFIG:Debug>:/Zi /Od /RTC1 /MDd>
        $<$<CONFIG:Release>:/O2 /Ob2 /DNDEBUG /MD>
    )

    target_link_options(wxWidgetDemo PRIVATE
        $<$<CONFIG:Debug>:/DEBUG /INCREMENTAL:NO>
        $<$<CONFIG:Release>:/INCREMENTAL:NO /OPT:REF /OPT:ICF>
    )

    set_property(TARGET wxWidgetDemo PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )

    # Add include directories for Windows
    target_include_directories(wxWidgetDemo PRIVATE
        ${WXWIDGETS_INCLUDE_DIR}
        ${WXWIDGETS_VC_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIR}
        ${GLM_INCLUDE_DIR}
    )

    # Link libraries for Windows
    target_link_directories(wxWidgetDemo PRIVATE
        ${WXWIDGETS_LIB_DIR}
        ${GLEW_LIB_DIR}
    )

    target_link_libraries(wxWidgetDemo 
        # Windows system libraries
        comctl32.lib
        rpcrt4.lib
        winmm.lib
        advapi32.lib
        wsock32.lib
        opengl32.lib
        glu32.lib
        glew32.lib
    )

else()
    # Link libraries for Linux
    if(wxWidgets_FOUND)
        # Use standard wxWidgets linking
        target_link_libraries(wxWidgetDemo ${wxWidgets_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glm::glm)
    else()
        # Use pkg-config libraries
        target_link_libraries(wxWidgetDemo ${WXWIDGETS_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glm::glm)
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
        "${WXWIDGETS_LIB_DIR}/wxmsw32ud_core_${WXWIDGETS_LIB_PREFIX}.dll"
        $<TARGET_FILE_DIR:wxWidgetDemo>
    )

    add_custom_command(TARGET wxWidgetDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WXWIDGETS_LIB_DIR}/wxbase32ud_${WXWIDGETS_LIB_PREFIX}.dll"
        $<TARGET_FILE_DIR:wxWidgetDemo>
    )

    add_custom_command(TARGET wxWidgetDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WXWIDGETS_LIB_DIR}/wxmsw32ud_gl_${WXWIDGETS_LIB_PREFIX}.dll"
        $<TARGET_FILE_DIR:wxWidgetDemo>
    )
endif()
