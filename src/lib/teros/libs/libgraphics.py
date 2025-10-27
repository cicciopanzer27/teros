"""
Ternary Graphics Library (libgraphics.so)

This module provides comprehensive graphics capabilities for the ternary operating system.
"""

from typing import List, Union, Optional, Dict, Any, Tuple
import math
from enum import Enum
from ..core.trit import Trit
from ..core.tritarray import TritArray


class TernaryColor(Enum):
    """Ternary color representation."""
    BLACK = -1
    GRAY = 0
    WHITE = 1


class TernaryShape(Enum):
    """Ternary shape types."""
    POINT = "point"
    LINE = "line"
    RECTANGLE = "rectangle"
    CIRCLE = "circle"
    TRIANGLE = "triangle"
    POLYGON = "polygon"


class TernaryCanvas:
    """
    Ternary Canvas - Main graphics surface.
    
    Provides a ternary-based canvas for drawing operations.
    """
    
    def __init__(self, width: int, height: int):
        """
        Initialize ternary canvas.
        
        Args:
            width: Canvas width in pixels
            height: Canvas height in pixels
        """
        self.width = width
        self.height = height
        
        # Initialize canvas with neutral gray (0)
        self.pixels = [[Trit(0) for _ in range(width)] for _ in range(height)]
        
        # Graphics state
        self.current_color = TernaryColor.WHITE
        self.current_thickness = 1
        self.current_style = "solid"
        
        # Drawing history for undo/redo
        self.history = []
        self.history_index = -1
    
    def set_color(self, color: TernaryColor) -> None:
        """
        Set current drawing color.
        
        Args:
            color: Ternary color to set
        """
        self.current_color = color
    
    def set_thickness(self, thickness: int) -> None:
        """
        Set current line thickness.
        
        Args:
            thickness: Line thickness in pixels
        """
        self.current_thickness = max(1, thickness)
    
    def set_style(self, style: str) -> None:
        """
        Set current drawing style.
        
        Args:
            style: Drawing style (solid, dashed, dotted)
        """
        self.current_style = style
    
    def draw_pixel(self, x: int, y: int, color: Optional[TernaryColor] = None) -> None:
        """
        Draw a single pixel.
        
        Args:
            x: X coordinate
            y: Y coordinate
            color: Color to draw (uses current color if None)
        """
        if 0 <= x < self.width and 0 <= y < self.height:
            draw_color = color or self.current_color
            self.pixels[y][x] = Trit(draw_color.value)
    
    def draw_line(self, x1: int, y1: int, x2: int, y2: int) -> None:
        """
        Draw a line between two points.
        
        Args:
            x1: Start X coordinate
            y1: Start Y coordinate
            x2: End X coordinate
            y2: End Y coordinate
        """
        # Bresenham's line algorithm adapted for ternary
        dx = abs(x2 - x1)
        dy = abs(y2 - y1)
        sx = 1 if x1 < x2 else -1
        sy = 1 if y1 < y2 else -1
        err = dx - dy
        
        x, y = x1, y1
        
        while True:
            self.draw_pixel(x, y)
            
            if x == x2 and y == y2:
                break
            
            e2 = 2 * err
            if e2 > -dy:
                err -= dy
                x += sx
            if e2 < dx:
                err += dx
                y += sy
    
    def draw_rectangle(self, x: int, y: int, width: int, height: int, filled: bool = False) -> None:
        """
        Draw a rectangle.
        
        Args:
            x: Top-left X coordinate
            y: Top-left Y coordinate
            width: Rectangle width
            height: Rectangle height
            filled: Whether to fill the rectangle
        """
        if filled:
            for py in range(y, y + height):
                for px in range(x, x + width):
                    self.draw_pixel(px, py)
        else:
            # Draw outline
            self.draw_line(x, y, x + width, y)
            self.draw_line(x + width, y, x + width, y + height)
            self.draw_line(x + width, y + height, x, y + height)
            self.draw_line(x, y + height, x, y)
    
    def draw_circle(self, center_x: int, center_y: int, radius: int, filled: bool = False) -> None:
        """
        Draw a circle.
        
        Args:
            center_x: Center X coordinate
            center_y: Center Y coordinate
            radius: Circle radius
            filled: Whether to fill the circle
        """
        if filled:
            for y in range(center_y - radius, center_y + radius + 1):
                for x in range(center_x - radius, center_x + radius + 1):
                    if (x - center_x) ** 2 + (y - center_y) ** 2 <= radius ** 2:
                        self.draw_pixel(x, y)
        else:
            # Draw outline using midpoint circle algorithm
            x = 0
            y = radius
            d = 1 - radius
            
            while x <= y:
                self.draw_pixel(center_x + x, center_y + y)
                self.draw_pixel(center_x - x, center_y + y)
                self.draw_pixel(center_x + x, center_y - y)
                self.draw_pixel(center_x - x, center_y - y)
                self.draw_pixel(center_x + y, center_y + x)
                self.draw_pixel(center_x - y, center_y + x)
                self.draw_pixel(center_x + y, center_y - x)
                self.draw_pixel(center_x - y, center_y - x)
                
                if d < 0:
                    d += 2 * x + 3
                else:
                    d += 2 * (x - y) + 5
                    y -= 1
                x += 1
    
    def draw_triangle(self, x1: int, y1: int, x2: int, y2: int, x3: int, y3: int, filled: bool = False) -> None:
        """
        Draw a triangle.
        
        Args:
            x1, y1: First vertex
            x2, y2: Second vertex
            x3, y3: Third vertex
            filled: Whether to fill the triangle
        """
        if filled:
            # Fill triangle using scanline algorithm
            vertices = [(x1, y1), (x2, y2), (x3, y3)]
            vertices.sort(key=lambda v: v[1])
            
            y_min, y_max = vertices[0][1], vertices[2][1]
            
            for y in range(y_min, y_max + 1):
                intersections = []
                
                for i in range(3):
                    p1 = vertices[i]
                    p2 = vertices[(i + 1) % 3]
                    
                    if p1[1] != p2[1]:
                        t = (y - p1[1]) / (p2[1] - p1[1])
                        if 0 <= t <= 1:
                            x = int(p1[0] + t * (p2[0] - p1[0]))
                            intersections.append(x)
                
                intersections.sort()
                for i in range(0, len(intersections), 2):
                    if i + 1 < len(intersections):
                        for x in range(intersections[i], intersections[i + 1] + 1):
                            self.draw_pixel(x, y)
        else:
            # Draw outline
            self.draw_line(x1, y1, x2, y2)
            self.draw_line(x2, y2, x3, y3)
            self.draw_line(x3, y3, x1, y1)
    
    def draw_polygon(self, points: List[Tuple[int, int]], filled: bool = False) -> None:
        """
        Draw a polygon.
        
        Args:
            points: List of (x, y) coordinate tuples
            filled: Whether to fill the polygon
        """
        if len(points) < 3:
            return
        
        if filled:
            # Fill polygon using scanline algorithm
            y_min = min(p[1] for p in points)
            y_max = max(p[1] for p in points)
            
            for y in range(y_min, y_max + 1):
                intersections = []
                
                for i in range(len(points)):
                    p1 = points[i]
                    p2 = points[(i + 1) % len(points)]
                    
                    if p1[1] != p2[1]:
                        t = (y - p1[1]) / (p2[1] - p1[1])
                        if 0 <= t <= 1:
                            x = int(p1[0] + t * (p2[0] - p1[0]))
                            intersections.append(x)
                
                intersections.sort()
                for i in range(0, len(intersections), 2):
                    if i + 1 < len(intersections):
                        for x in range(intersections[i], intersections[i + 1] + 1):
                            self.draw_pixel(x, y)
        else:
            # Draw outline
            for i in range(len(points)):
                p1 = points[i]
                p2 = points[(i + 1) % len(points)]
                self.draw_line(p1[0], p1[1], p2[0], p2[1])
    
    def clear(self, color: TernaryColor = TernaryColor.GRAY) -> None:
        """
        Clear the canvas with specified color.
        
        Args:
            color: Color to clear with
        """
        for y in range(self.height):
            for x in range(self.width):
                self.pixels[y][x] = Trit(color.value)
    
    def get_pixel(self, x: int, y: int) -> Optional[Trit]:
        """
        Get pixel value at coordinates.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            Trit value at coordinates or None if out of bounds
        """
        if 0 <= x < self.width and 0 <= y < self.height:
            return self.pixels[y][x]
        return None
    
    def save_state(self) -> None:
        """Save current canvas state to history."""
        state = {
            'pixels': [[trit.value for trit in row] for row in self.pixels],
            'color': self.current_color,
            'thickness': self.current_thickness,
            'style': self.current_style
        }
        
        # Remove any states after current index
        self.history = self.history[:self.history_index + 1]
        self.history.append(state)
        self.history_index += 1
    
    def undo(self) -> bool:
        """
        Undo last operation.
        
        Returns:
            True if undo was successful, False otherwise
        """
        if self.history_index > 0:
            self.history_index -= 1
            state = self.history[self.history_index]
            
            # Restore pixels
            for y in range(self.height):
                for x in range(self.width):
                    self.pixels[y][x] = Trit(state['pixels'][y][x])
            
            # Restore state
            self.current_color = state['color']
            self.current_thickness = state['thickness']
            self.current_style = state['style']
            
            return True
        return False
    
    def redo(self) -> bool:
        """
        Redo last undone operation.
        
        Returns:
            True if redo was successful, False otherwise
        """
        if self.history_index < len(self.history) - 1:
            self.history_index += 1
            state = self.history[self.history_index]
            
            # Restore pixels
            for y in range(self.height):
                for x in range(self.width):
                    self.pixels[y][x] = Trit(state['pixels'][y][x])
            
            # Restore state
            self.current_color = state['color']
            self.current_thickness = state['thickness']
            self.current_style = state['style']
            
            return True
        return False
    
    def to_ternary_array(self) -> TritArray:
        """
        Convert canvas to ternary array.
        
        Returns:
            TritArray representation of canvas
        """
        # Flatten pixels to 1D array
        flat_pixels = []
        for row in self.pixels:
            flat_pixels.extend([trit.value for trit in row])
        
        return TritArray(flat_pixels)
    
    def from_ternary_array(self, trit_array: TritArray, width: int, height: int) -> None:
        """
        Load canvas from ternary array.
        
        Args:
            trit_array: TritArray to load from
            width: Canvas width
            height: Canvas height
        """
        self.width = width
        self.height = height
        self.pixels = []
        
        for y in range(height):
            row = []
            for x in range(width):
                index = y * width + x
                if index < len(trit_array):
                    row.append(Trit(trit_array[index].value))
                else:
                    row.append(Trit(0))
            self.pixels.append(row)


class TernaryTextRenderer:
    """
    Ternary Text Renderer - Text rendering for ternary graphics.
    
    Provides text rendering capabilities using ternary fonts.
    """
    
    def __init__(self):
        """Initialize ternary text renderer."""
        # Simple 5x7 ternary font
        self.font = {
            'A': [
                [0, 1, 1, 1, 0],
                [1, 0, 0, 0, 1],
                [1, 1, 1, 1, 1],
                [1, 0, 0, 0, 1],
                [1, 0, 0, 0, 1],
                [1, 0, 0, 0, 1],
                [0, 0, 0, 0, 0]
            ],
            'B': [
                [1, 1, 1, 1, 0],
                [1, 0, 0, 0, 1],
                [1, 1, 1, 1, 0],
                [1, 0, 0, 0, 1],
                [1, 0, 0, 0, 1],
                [1, 1, 1, 1, 0],
                [0, 0, 0, 0, 0]
            ],
            'C': [
                [0, 1, 1, 1, 0],
                [1, 0, 0, 0, 1],
                [1, 0, 0, 0, 0],
                [1, 0, 0, 0, 0],
                [1, 0, 0, 0, 1],
                [0, 1, 1, 1, 0],
                [0, 0, 0, 0, 0]
            ],
            ' ': [
                [0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0],
                [0, 0, 0, 0, 0]
            ]
        }
        
        self.char_width = 5
        self.char_height = 7
    
    def render_text(self, canvas: TernaryCanvas, text: str, x: int, y: int, 
                   color: Optional[TernaryColor] = None) -> None:
        """
        Render text on canvas.
        
        Args:
            canvas: Canvas to render on
            text: Text to render
            x: X position
            y: Y position
            color: Text color (uses current color if None)
        """
        current_x = x
        
        for char in text.upper():
            if char in self.font:
                char_data = self.font[char]
                
                for char_y in range(self.char_height):
                    for char_x in range(self.char_width):
                        if char_data[char_y][char_x] == 1:
                            canvas.draw_pixel(current_x + char_x, y + char_y, color)
                
                current_x += self.char_width + 1  # Add spacing between characters
    
    def get_text_width(self, text: str) -> int:
        """
        Get text width in pixels.
        
        Args:
            text: Text to measure
            
        Returns:
            Text width in pixels
        """
        return len(text) * (self.char_width + 1) - 1
    
    def get_text_height(self) -> int:
        """
        Get text height in pixels.
        
        Returns:
            Text height in pixels
        """
        return self.char_height


class TernaryGraphics:
    """
    Ternary Graphics - Main graphics library.
    
    Provides comprehensive graphics capabilities for ternary systems.
    """
    
    def __init__(self):
        """Initialize ternary graphics library."""
        self.canvas = None
        self.text_renderer = TernaryTextRenderer()
        
        # Graphics state
        self.current_color = TernaryColor.WHITE
        self.current_thickness = 1
        self.current_style = "solid"
        
        # Performance optimization
        self.lookup_tables = self._build_lookup_tables()
    
    def _build_lookup_tables(self) -> Dict[str, Any]:
        """Build lookup tables for performance optimization."""
        return {
            'sin_table': [math.sin(math.radians(i)) for i in range(360)],
            'cos_table': [math.cos(math.radians(i)) for i in range(360)],
            'sqrt_table': [math.sqrt(i) for i in range(1000)]
        }
    
    def create_canvas(self, width: int, height: int) -> TernaryCanvas:
        """
        Create a new canvas.
        
        Args:
            width: Canvas width
            height: Canvas height
            
        Returns:
            New TernaryCanvas instance
        """
        self.canvas = TernaryCanvas(width, height)
        return self.canvas
    
    def set_color(self, color: TernaryColor) -> None:
        """
        Set current drawing color.
        
        Args:
            color: Ternary color to set
        """
        self.current_color = color
        if self.canvas:
            self.canvas.set_color(color)
    
    def set_thickness(self, thickness: int) -> None:
        """
        Set current line thickness.
        
        Args:
            thickness: Line thickness in pixels
        """
        self.current_thickness = thickness
        if self.canvas:
            self.canvas.set_thickness(thickness)
    
    def set_style(self, style: str) -> None:
        """
        Set current drawing style.
        
        Args:
            style: Drawing style (solid, dashed, dotted)
        """
        self.current_style = style
        if self.canvas:
            self.canvas.set_style(style)
    
    def draw_point(self, x: int, y: int) -> None:
        """
        Draw a point.
        
        Args:
            x: X coordinate
            y: Y coordinate
        """
        if self.canvas:
            self.canvas.draw_pixel(x, y)
    
    def draw_line(self, x1: int, y1: int, x2: int, y2: int) -> None:
        """
        Draw a line.
        
        Args:
            x1: Start X coordinate
            y1: Start Y coordinate
            x2: End X coordinate
            y2: End Y coordinate
        """
        if self.canvas:
            self.canvas.draw_line(x1, y1, x2, y2)
    
    def draw_rectangle(self, x: int, y: int, width: int, height: int, filled: bool = False) -> None:
        """
        Draw a rectangle.
        
        Args:
            x: Top-left X coordinate
            y: Top-left Y coordinate
            width: Rectangle width
            height: Rectangle height
            filled: Whether to fill the rectangle
        """
        if self.canvas:
            self.canvas.draw_rectangle(x, y, width, height, filled)
    
    def draw_circle(self, center_x: int, center_y: int, radius: int, filled: bool = False) -> None:
        """
        Draw a circle.
        
        Args:
            center_x: Center X coordinate
            center_y: Center Y coordinate
            radius: Circle radius
            filled: Whether to fill the circle
        """
        if self.canvas:
            self.canvas.draw_circle(center_x, center_y, radius, filled)
    
    def draw_triangle(self, x1: int, y1: int, x2: int, y2: int, x3: int, y3: int, filled: bool = False) -> None:
        """
        Draw a triangle.
        
        Args:
            x1, y1: First vertex
            x2, y2: Second vertex
            x3, y3: Third vertex
            filled: Whether to fill the triangle
        """
        if self.canvas:
            self.canvas.draw_triangle(x1, y1, x2, y2, x3, y3, filled)
    
    def draw_polygon(self, points: List[Tuple[int, int]], filled: bool = False) -> None:
        """
        Draw a polygon.
        
        Args:
            points: List of (x, y) coordinate tuples
            filled: Whether to fill the polygon
        """
        if self.canvas:
            self.canvas.draw_polygon(points, filled)
    
    def draw_text(self, text: str, x: int, y: int, color: Optional[TernaryColor] = None) -> None:
        """
        Draw text.
        
        Args:
            text: Text to draw
            x: X position
            y: Y position
            color: Text color (uses current color if None)
        """
        if self.canvas:
            self.text_renderer.render_text(self.canvas, text, x, y, color)
    
    def clear(self, color: TernaryColor = TernaryColor.GRAY) -> None:
        """
        Clear the canvas.
        
        Args:
            color: Color to clear with
        """
        if self.canvas:
            self.canvas.clear(color)
    
    def save_canvas(self, filename: str) -> bool:
        """
        Save canvas to file.
        
        Args:
            filename: Output filename
            
        Returns:
            True if successful, False otherwise
        """
        if not self.canvas:
            return False
        
        try:
            with open(filename, 'w') as f:
                f.write("# TEROS Graphics Canvas\n")
                f.write(f"# Width: {self.canvas.width}, Height: {self.canvas.height}\n")
                f.write("# Format: Ternary pixel values (-1, 0, 1)\n")
                f.write("\n")
                
                for y in range(self.canvas.height):
                    row = []
                    for x in range(self.canvas.width):
                        pixel = self.canvas.get_pixel(x, y)
                        if pixel:
                            row.append(str(pixel.value))
                        else:
                            row.append("0")
                    f.write(" ".join(row) + "\n")
            
            return True
        except Exception as e:
            print(f"Failed to save canvas: {e}")
            return False
    
    def load_canvas(self, filename: str) -> bool:
        """
        Load canvas from file.
        
        Args:
            filename: Input filename
            
        Returns:
            True if successful, False otherwise
        """
        try:
            with open(filename, 'r') as f:
                lines = f.readlines()
            
            # Skip header lines
            data_lines = [line for line in lines if not line.startswith('#') and line.strip()]
            
            if not data_lines:
                return False
            
            height = len(data_lines)
            width = len(data_lines[0].split())
            
            # Create new canvas
            self.canvas = TernaryCanvas(width, height)
            
            # Load pixel data
            for y, line in enumerate(data_lines):
                values = line.split()
                for x, value in enumerate(values):
                    try:
                        pixel_value = int(value)
                        if pixel_value in [-1, 0, 1]:
                            self.canvas.pixels[y][x] = Trit(pixel_value)
                    except ValueError:
                        self.canvas.pixels[y][x] = Trit(0)
            
            return True
        except Exception as e:
            print(f"Failed to load canvas: {e}")
            return False
    
    def get_canvas_info(self) -> Dict[str, Any]:
        """
        Get canvas information.
        
        Returns:
            Dictionary with canvas information
        """
        if not self.canvas:
            return {}
        
        return {
            'width': self.canvas.width,
            'height': self.canvas.height,
            'current_color': self.current_color.value,
            'current_thickness': self.current_thickness,
            'current_style': self.current_style,
            'history_size': len(self.canvas.history),
            'history_index': self.canvas.history_index
        }
    
    def optimize_performance(self) -> None:
        """Optimize graphics performance."""
        # Rebuild lookup tables
        self.lookup_tables = self._build_lookup_tables()
        
        # Optimize canvas if it exists
        if self.canvas:
            # Limit history size to prevent memory issues
            if len(self.canvas.history) > 100:
                self.canvas.history = self.canvas.history[-50:]
                self.canvas.history_index = len(self.canvas.history) - 1
    
    def benchmark(self, operations: int = 1000) -> Dict[str, float]:
        """
        Benchmark graphics operations.
        
        Args:
            operations: Number of operations to perform
            
        Returns:
            Dictionary with benchmark results
        """
        if not self.canvas:
            return {}
        
        import time
        
        # Benchmark drawing operations
        start_time = time.time()
        
        for i in range(operations):
            self.draw_line(i % self.canvas.width, i % self.canvas.height, 
                          (i + 10) % self.canvas.width, (i + 10) % self.canvas.height)
        
        end_time = time.time()
        
        return {
            'operations': operations,
            'total_time': end_time - start_time,
            'ops_per_second': operations / (end_time - start_time),
            'avg_time_per_op': (end_time - start_time) / operations
        }


# Global graphics instance
_graphics = TernaryGraphics()


def get_graphics() -> TernaryGraphics:
    """Get global graphics instance."""
    return _graphics


def create_canvas(width: int, height: int) -> TernaryCanvas:
    """Create a new canvas."""
    return _graphics.create_canvas(width, height)


def draw_line(x1: int, y1: int, x2: int, y2: int) -> None:
    """Draw a line."""
    _graphics.draw_line(x1, y1, x2, y2)


def draw_rectangle(x: int, y: int, width: int, height: int, filled: bool = False) -> None:
    """Draw a rectangle."""
    _graphics.draw_rectangle(x, y, width, height, filled)


def draw_circle(center_x: int, center_y: int, radius: int, filled: bool = False) -> None:
    """Draw a circle."""
    _graphics.draw_circle(center_x, center_y, radius, filled)


def draw_text(text: str, x: int, y: int, color: Optional[TernaryColor] = None) -> None:
    """Draw text."""
    _graphics.draw_text(text, x, y, color)


def set_color(color: TernaryColor) -> None:
    """Set current drawing color."""
    _graphics.set_color(color)


def clear(color: TernaryColor = TernaryColor.GRAY) -> None:
    """Clear the canvas."""
    _graphics.clear(color)


if __name__ == "__main__":
    # Demo graphics library
    print("=== TEROS Graphics Library Demo ===")
    
    # Create canvas
    canvas = create_canvas(80, 24)
    print(f"Created canvas: {canvas.width}x{canvas.height}")
    
    # Set colors
    set_color(TernaryColor.WHITE)
    
    # Draw some shapes
    draw_rectangle(10, 5, 20, 10, filled=True)
    draw_circle(50, 10, 8, filled=False)
    draw_line(0, 0, 79, 23)
    
    # Draw text
    draw_text("TEROS", 30, 15)
    
    # Save canvas
    if _graphics.save_canvas("demo_canvas.txt"):
        print("Canvas saved to demo_canvas.txt")
    
    print("Graphics library demo completed!")
