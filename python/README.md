# KAGE C++ Python Binding Reference

[简体中文](README.zh-CN.md)

`kage_cpp` is a Python extension for the KAGE glyph-engine core. It provides component-data management, glyph rendering, stroke extraction, and validation. It does not include the HTTP server or depend on SQLite, Crow, librsvg, or Cairo.

## Building

Building the Python module requires CMake 3.18 or newer, a C++11 compiler, the development files for the selected Python interpreter, and pybind11's CMake package. Keep the interpreter, development files, compiler, and target architecture compatible.

### Build the standalone Python module

The standalone `python` subproject builds only the core engine and does not require server dependencies. Use the same Python installation when building and importing the extension. CMake can usually locate the Python development files and pybind11 package automatically; specify their paths explicitly if multiple installations are present.

From the repository root:

```powershell
cmake -S .\python -B .\build-python `
  -DPython3_EXECUTABLE=C:/path/to/python.exe `
  -Dpybind11_DIR=C:/path/to/pybind11/cmake
cmake --build .\build-python --target kage_cpp --parallel 2
```

The extension is written to `build-python/python`. Add that directory to `PYTHONPATH` and launch the interpreter used for the build:

```powershell
$env:PYTHONPATH = "$PWD\build-python\python;$env:PYTHONPATH"
python
```

When using MinGW on Windows, make sure its runtime DLLs are discoverable, for example by adding the MinGW `bin` directory to `PATH`. Python version, architecture, development files, and runtime dependencies must be compatible with the extension.

### Build from the repository root

The root CMake project provides the optional `KAGE_BUILD_PYTHON` option and the `KagePython` target. Python support is disabled by default. If the build directory has cached paths for another Python installation, use a fresh build directory or clear the cached Python search results before configuring again.

```powershell
cmake -S . -B .\build-python `
  -DKAGE_BUILD_PYTHON=ON `
  -DPython3_EXECUTABLE=C:/path/to/python.exe `
  -Dpybind11_DIR=C:/path/to/pybind11/cmake
cmake --build .\build-python --target KagePython --parallel 2
```

The extension is written to `build-python/python`. Exit any Python process that has imported the `.pyd` before rebuilding it on Windows.

## Quick start

```python
import kage_cpp

engine = kage_cpp.Engine(kage_cpp.FontType.MINCHO)
engine.set_buhin("u4e00", "1:0:0:20:100:180:100")

svg = engine.render_svg("u4e00")
expanded_strokes = engine.extract("u4e00")
status = engine.check("u4e00")
print(status == kage_cpp.CheckGlyphState.OK)
```

`Engine` uses Mincho by default, and a `size` of `0` selects the engine's default size. `render`, `extract`, and `check` operate on component names; their corresponding `*_data` methods accept KAGE data strings directly.

## Engine API

### Component library

- `set_buhin(name, data)`: Add a local component or replace the existing entry.
- `set_buhin_strokes(name, strokes)`: Add or replace a component from a list of `Stroke` objects.
- `push_buhin(name, data)`: Insert only if the name does not already exist; existing entries are not overwritten.
- `push_buhin_strokes(name, strokes)`: Perform the same insert-only operation from a list of `Stroke` objects.
- `search_buhin(name)`: Return the component's strokes. If it is not in the local library and a database callback is configured, the callback is queried.
- `delete_buhin(name)`: Delete a local component. Returns `True` if it was found and deleted, or `False` if the local library did not contain it.
- `export_buhins()`: Return the entire local library as `{name: KAGE_data_string}`, sorted by name.

`delete_buhin` removes only the local entry. A later lookup may still retrieve a component with the same name from the database callback. `export_buhins` contains only entries stored locally in the `Engine`; it does not enumerate an external database or call the callback.

### Font, callback, and missing-glyph settings

- `set_font(font, size=0)`: Change the font and size.
- `set_db_search_callback(callback)`: Set a component lookup callback. It receives a component name and returns a KAGE data string, or `None` when not found. Pass `None` to clear the callback.
- `set_notdef_glyph(data)`: Set the fallback glyph data used when a component is missing.
- `set_notdef_glyph_strokes(strokes)`: Set the fallback glyph from a list of strokes.

The callback is invoked only when the local library has no matching component. Preloading frequently used components with `set_buhin` can reduce callback calls.

### Rendering, extraction, and validation

- `render(name)` / `render_data(data)`: Return the rendered outlines as a `Canva`.
- `render_data_separated(data)`: Return outlines grouped by each original input-data line. Strokes expanded from a component reference remain grouped under that reference line. This is useful for per-stroke selection and editing.
- `render_svg(name)` / `render_data_svg(data)`: Return an SVG string.
- `render_sfd(name)` / `render_data_sfd(data)`: Return an SFD string.
- `extract(name)` / `extract_data(data)`: Expand component references and return a list of strokes.
- `check(name)` / `check_data(data)`: Validate a glyph and return a `CheckGlyphState`.

`CheckGlyphState` has three values: `OK`, `BUHIN_NOTFOUND`, and `LOOPBACK_DETECTED`.

## Data Types and Module Functions

### Stroke and enums

The readable and writable `Stroke` fields are `type`, `start`, `end`, `p1` through `p4`, `type_opt`, `start_opt`, `end_opt`, `buhin`, `s1`, `s2`, `special1`, and `special2`. The `p1` through `p4`, `s1`, and `s2` fields are `Point` objects. Which fields are used depends on the stroke type.

Use enum members in Python as `kage_cpp.EnumClass.MEMBER`:

- `FontType`: `DEFAULT`, `MINCHO`, `GOTHIC`.
- `CheckGlyphState`: `OK`, `BUHIN_NOTFOUND` (component not found), `LOOPBACK_DETECTED` (circular reference detected).
- `StrokeType`: `SPECIAL` (special instruction, 0), `STRAIGHT` (straight line, 1), `CURVE` (quadratic curve, 2), `BENDING` (polyline, 3), `BENDING_ROUND` (rounded bend, 4), `BEZIER` (cubic curve, 6), `VCURVE` (vertical sweep, 7), `REFERENCE` (component reference, 99).
- `SpecialInt1`: `FLIP_UPDOWN` (flip vertically, 97), `FLIP_LEFTRIGHT` (flip horizontally, 98), `ROTATE` (rotate, 99).
- `SpecialInt2`: `ROTATE_90` (rotate 90 degrees, 1), `ROTATE_180` (rotate 180 degrees, 2), `ROTATE_270` (rotate 270 degrees, 3).
- `StartType`: `OPEN` (open, 0), `CONNECTING_H` (horizontal connection, 2), `UPPER_LEFT_CORNER` (upper-left corner, 12), `UPPER_RIGHT_CORNER` (upper-right corner, 22), `CONNECTING_V` (vertical connection, 32), `THIN` (pointed, 7), `ROOFED_THIN` (roofed thin, 27), `INTERNAL_FOR_BENDING` (internal bend, 6), `INTERNAL_PLANE` (internal plane, 1), `CONNECT_THIN` (thin connection, 37), `CONNECTING_MANUAL` (manual connection, 39).
- `EndType`: `OPEN` (open, 0), `CONNECTING_H` (horizontal connection, 2), `CONNECTING_V` (vertical connection, 32), `LOWER_LEFT_CORNER` (lower-left corner, 13), `LOWER_RIGHT_CORNER` (lower-right corner, 23), `LOWER_LEFT_ZH_OLD` (legacy lower-left fold, 313), `LOWER_LEFT_ZH_NEW` (new lower-left fold, 413), `TURN_LEFT` (left hook, 4), `LOWER_RIGHT_HT` (lower-right corner for HT, 24), `TURN_UPWARDS` (upward hook, 5), `LEFT_SWEEP` (left sweep tip, 7), `STOP` (dot, 1), `INTERNAL_LAST_FILLED_CIRCLE` (internal final filled circle, 1), `INTERNAL_TURN_LEFT` (internal left turn, 14), `INTERNAL_TURN_UPWARDS` (internal upward turn, 15).

Use `StartType` and `EndType` for stroke start and end styles. Special transforms use `SpecialInt1` and `SpecialInt2`. These enums can be used directly when creating or modifying a `Stroke`.

For example:

```python
stroke = kage_cpp.Stroke()
stroke.type = kage_cpp.StrokeType.STRAIGHT
stroke.start = kage_cpp.StartType.OPEN
stroke.end = kage_cpp.EndType.OPEN
stroke.p1 = kage_cpp.Point(20, 100)
stroke.p2 = kage_cpp.Point(180, 100)
engine.set_buhin_strokes("horizontal", [stroke])
```

`Stroke`'s `repr` shows applicable fields and displays start, end, and transform enums as `ENUM_NAME(raw_value)`, for example `start=UPPER_LEFT_CORNER(12)`. `type_opt`, `start_opt`, and `end_opt` are numeric adjustment parameters and remain numbers.

### Other types

- `Point(x, y)`: A two-dimensional coordinate with readable and writable `x` and `y` fields.
- `BoundingData`: A bounding box with `min_x`, `min_y`, `max_x`, and `max_y` fields.
- `Contour`: A contour. `points`, `point_types`, and `closed` are read-only properties; `size()` returns the number of points.
- `Canva`: A render result. `contours()` returns a list of contours, `size()` returns the contour count, and `clear()` removes all contents.

The `repr` of these objects shows their fields or a concise summary, for example `Point(x=20, y=100)` and `Canva(contours=3)`.

### Module-level functions

- `parse(data)`: Parse a KAGE data string into a list of `Stroke` objects.
- `serialize(strokes)`: Serialize a list of strokes as a KAGE data string.
- `bounding_box(strokes)`: Calculate a bounding box and return `BoundingData`.
- `stretch(destination, source, position, minimum, maximum)`: Run the engine's coordinate-stretch calculation.
- `canva_to_svg(canva)` / `canva_to_sfd(canva)`: Convert a `Canva` to an SVG or SFD string.

`parse` skips stroke entries that cannot be parsed rather than raising an error for each entry. `check_data` validates the parsed glyph and its component references; it cannot report malformed entries already skipped by `parse`. Validate the raw input in Python first when strict format validation is required.

## Multithreading

Public operations on the same `Engine` instance are protected by a recursive mutex. Rendering, extraction, and validation release the Python GIL while the C++ engine runs, so multiple threads can call the same instance; calls on that instance are serialized by its mutex. For parallel processing of multiple glyphs, create a separate `Engine` for each worker and load the required components into each instance.

The GIL is reacquired when a Python database callback is invoked. The caller is responsible for protecting any Python data accessed by the callback according to its concurrency model.

## Troubleshooting

- **`Python3 Development.Module` cannot be found**: Install the development headers and library for the selected interpreter. With MSYS2, use Python, the compiler, and libraries from the same `mingw64` environment.
- **CMake finds the wrong Python version**: The build directory may have cached a previous interpreter. Reconfigure with a fresh build directory or clear the cached Python search results using `-U "Python3_*" -U "_Python3_*"`.
- **`import kage_cpp` fails**: Add the build output's `python` subdirectory to `PYTHONPATH` and use a Python version and architecture compatible with the module. If using MinGW, also make its runtime DLLs available on `PATH`. Building and importing with the same Python installation is the most reliable setup.
- **The `.pyd` cannot be overwritten during linking**: Exit any Python process that has imported the module, then build again.
