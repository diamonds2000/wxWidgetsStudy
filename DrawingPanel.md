# DrawingPanel Documentation

## Overview

The `DrawingPanel` class is a custom wxWidgets panel that provides interactive drawing capabilities. It allows users to draw freehand strokes with configurable colors and pen widths, using buffered drawing for smooth performance.

## Class Hierarchy

```
wxPanel
└── DrawingPanel
```

## Features

- **Interactive Drawing**: Click and drag to draw freehand strokes
- **Customizable Appearance**: Set drawing color and pen width
- **Buffered Drawing**: Uses double buffering for smooth rendering and reduced flicker
- **Stroke Management**: Maintains a history of all drawing strokes
- **Clear Functionality**: Option to clear the entire drawing canvas

## Public Interface

### Constructor

```cpp
DrawingPanel(wxWindow* parent)
```

Creates a new drawing panel as a child of the specified parent window.

**Parameters:**
- `parent`: Parent window that will contain this drawing panel

### Drawing Controls

#### SetDrawingColor
```cpp
void SetDrawingColor(const wxColour& color)
```
Sets the color for new drawing strokes.

**Parameters:**
- `color`: The color to use for subsequent drawing operations

#### SetPenWidth
```cpp
void SetPenWidth(int width)
```
Sets the width/thickness of the drawing pen.

**Parameters:**
- `width`: Pen width in pixels (default: 2)

#### ClearDrawing
```cpp
void ClearDrawing()
```
Clears all drawing strokes from the panel and resets the canvas to a white background.

### Getters

#### GetDrawingColor
```cpp
wxColour GetDrawingColor() const
```
Returns the current drawing color.

**Returns:** Current drawing color as `wxColour`

#### GetPenWidth
```cpp
int GetPenWidth() const
```
Returns the current pen width.

**Returns:** Current pen width in pixels

## Data Structures

### DrawingStroke

A structure that represents a single drawing stroke:

```cpp
struct DrawingStroke
{
    std::vector<wxPoint> points;  // Points that make up the stroke
    wxColour color;               // Color of the stroke
    int width;                    // Width of the stroke
};
```

**Members:**
- `points`: Vector of `wxPoint` objects representing the path of the stroke
- `color`: Color of the stroke (default: black)
- `width`: Width of the stroke in pixels (default: 2)

## Implementation Details

### Event Handling

The DrawingPanel handles the following events:

- **EVT_PAINT**: Renders the drawing canvas and all strokes
- **EVT_LEFT_DOWN**: Starts a new drawing stroke
- **EVT_MOTION**: Continues drawing when mouse is dragged
- **EVT_LEFT_UP**: Completes the current drawing stroke
- **EVT_SIZE**: Reinitializes the drawing bitmap when panel is resized
- **EVT_ERASE_BACKGROUND**: Overridden to prevent flicker

### Drawing System

The panel uses a dual-drawing approach:

1. **Immediate Drawing**: While drawing, strokes are rendered directly to the screen for real-time feedback
2. **Buffered Drawing**: Completed strokes are stored in a bitmap buffer for efficient repainting

### Performance Optimizations

- **Custom Background Style**: Uses `wxBG_STYLE_CUSTOM` for better performance
- **Double Buffering**: Uses `wxAutoBufferedPaintDC` to eliminate flicker
- **Bitmap Caching**: Maintains a bitmap buffer to avoid redrawing all strokes on every paint event
- **Smart Repainting**: Only redraws the bitmap when necessary (after clearing or resizing)

## Usage Example

```cpp
// Create drawing panel
DrawingPanel* drawingPanel = new DrawingPanel(parentWindow);

// Configure drawing properties
drawingPanel->SetDrawingColor(*wxRED);
drawingPanel->SetPenWidth(5);

// Get current settings
wxColour currentColor = drawingPanel->GetDrawingColor();
int currentWidth = drawingPanel->GetPenWidth();

// Clear the drawing
drawingPanel->ClearDrawing();
```

## Integration

The DrawingPanel is typically integrated into a larger application as part of a drawing or sketching interface. It can be:

- Embedded in dialog boxes for quick sketches
- Used as the main canvas in drawing applications
- Combined with toolbars for color and width selection
- Integrated with save/load functionality for persistent drawings

## Thread Safety

This class is designed for use in the main GUI thread only and is not thread-safe. All drawing operations should be performed from the main thread.

## Dependencies

- **wxWidgets Core**: For basic panel functionality and event handling
- **wxWidgets Drawing Context**: For rendering operations (`wxDC`, `wxMemoryDC`, etc.)
- **Standard Library**: Uses `std::vector` for stroke point storage

## File Structure

- **Header**: `src/DrawingPanel.h` - Class declaration and interface
- **Implementation**: `src/DrawingPanel.cpp` - Complete implementation with event handling and drawing logic