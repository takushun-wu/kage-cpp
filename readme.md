# kage-cpp

[简体中文](README.zh-CN.md)

A C++11 implementation of the [KAGE ("shadow") engine](https://github.com/kurgm/kage-engine), based on [ge9's modified KAGE engine](https://github.com/ge9/kage-engine-2). It generates glyph outlines from KAGE stroke data and reusable component references. The project is licensed under GPL-3.0; see [LICENSE](LICENSE).

## Features

- Fits curved strokes with cubic Bezier curves instead of approximating them with segmented polylines.
- Improves serif details and hooks on vertical, sweep, and horizontal strokes.
- Fixes negative adjustment rendering and rotation/flip handling in the Gothic renderer.
- Provides per-stroke separated rendering for selection and editing workflows.
- Exports rendered outlines to SVG and FontForge SFD.

## Build the C++ target

The project requires CMake 3.15 or newer and a C++11 compiler.

```powershell
cmake -S . -B build
cmake --build build --target KageCppTest --parallel 2
```

## Python binding

The optional `kage_cpp` pybind11 module exposes the glyph-engine core. It does not include the HTTP server and does not require SQLite, Crow, librsvg, or Cairo. The Python target is disabled by default.

Building the Python module requires CMake 3.18 or newer, a C++11 compiler, the development headers for the selected Python interpreter, and pybind11's CMake package. Use matching Python, compiler, and target architectures.

```powershell
cmake -S . -B build-python `
  -DKAGE_BUILD_PYTHON=ON `
  -DPython3_EXECUTABLE=C:/path/to/python.exe `
  -Dpybind11_DIR=C:/path/to/pybind11/cmake
cmake --build build-python --target KagePython --parallel 2
```

The extension is written to `build-python/python`. On Windows, a MinGW build also needs its runtime DLLs to be discoverable, for example by adding the MinGW `bin` directory to `PATH` or placing the required DLLs beside the `.pyd`. Import the module with the same Python version and architecture used to build it:

```powershell
$env:PYTHONPATH = "$PWD\build-python\python;$env:PYTHONPATH"
python -c "import kage_cpp; print(kage_cpp)"
```

Example:

```python
import kage_cpp

engine = kage_cpp.Engine(kage_cpp.FontType.MINCHO)
engine.set_buhin("u4e00", "1:0:0:20:100:180:100")

svg = engine.render_svg("u4e00")
strokes = engine.extract("u4e00")
status = engine.check("u4e00")
```

`Engine` also provides local component storage, lookup, deletion and export, glyph validation, stroke extraction, and `render_data_separated` for rendering outlines grouped by input data line. Module-level helpers include `parse`, `serialize`, bounding-box calculation, and SVG/SFD export. See the [Python binding reference](python/README.md) for the full API.

## License and third-party code

The project is distributed under GPL-3.0. Glyph data generated from GlyphWiki is subject to GlyphWiki's own terms; see its [copyright and license information](https://zhs.glyphwiki.org/wiki/GlyphWiki:%e8%91%97%e4%bd%9c%e6%9d%83%e4%b8%8e%e8%ae%b8%e5%8f%af%e5%8d%8f%e8%ae%ae). The repository includes [utfcpp](https://github.com/nemtrif/utfcpp).
