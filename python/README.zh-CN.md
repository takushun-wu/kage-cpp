# KAGE C++ Python 绑定中文说明

[English](README.md)

`kage_cpp` 是 KAGE 字形引擎核心的 Python 扩展。它提供部件数据管理、字形渲染、笔画拆解和检查接口，不包含 HTTP 服务、SQLite、Crow、librsvg 或 Cairo 服务端功能。

## 构建

Python 模块需要 CMake 3.18 或更高版本、支持 C++11 的编译器、所选 Python 解释器对应的开发文件，以及 pybind11 的 CMake 配置包。请确保解释器、开发文件、编译器和目标架构相互匹配。

### 独立构建核心模块

独立的 `python` 子项目只编译核心引擎，不需要配置服务器依赖。下面以 MSYS2 MinGW Python 3.11 为例；构建和导入时必须使用同一套 Python 及对应的 MinGW 运行时。

```powershell
& E:\msys64\mingw64\bin\cmake.exe `
  -S D:\kage-cpp\python -B D:\kage-cpp\build-python `
  -G "MinGW Makefiles" `
  -DPython3_EXECUTABLE=E:/msys64/mingw64/bin/python.exe `
  -DPython3_ROOT_DIR=E:/msys64/mingw64 `
  -DPython3_INCLUDE_DIR=E:/msys64/mingw64/include/python3.11 `
  -DPython3_LIBRARY=E:/msys64/mingw64/lib/libpython3.11.dll.a `
  -Dpybind11_DIR=E:/msys64/mingw64/lib/cmake/pybind11
& E:\msys64\mingw64\bin\cmake.exe --build D:\kage-cpp\build-python --target kage_cpp -j2
```

生成的扩展位于 `D:\kage-cpp\build-python\python`。在 PowerShell 中加入模块目录并启动对应解释器：

```powershell
$env:PYTHONPATH = "D:\kage-cpp\build-python\python;$env:PYTHONPATH"
& E:\msys64\mingw64\bin\python.exe
```

Python 版本变化时，需要相应更新 include 目录和 `libpython` 文件名。若使用新的空构建目录，CMake 通常可以自动查找这些路径；显式填写可避免找到另一套 Python 的开发文件。

### 使用项目根目录构建

项目根目录提供可选项 `KAGE_BUILD_PYTHON`，开启后可使用 `KagePython` 目标。Python 功能默认关闭；根项目的 Python 模块仅构建核心引擎，不需要服务器依赖。若构建目录曾配置过另一套 Python，先清除缓存中的 Python 查找结果：

```powershell
& E:\msys64\mingw64\bin\cmake.exe `
  -U "Python3_*" -U "_Python3_*" `
  -S D:\kage-cpp -B D:\kage-cpp\build `
  -DKAGE_BUILD_PYTHON=ON `
  -DPython3_ROOT_DIR=E:/msys64/mingw64 `
  -DPython3_EXECUTABLE=E:/msys64/mingw64/bin/python.exe `
  -DPython3_INCLUDE_DIR=E:/msys64/mingw64/include/python3.11 `
  -DPython3_LIBRARY=E:/msys64/mingw64/lib/libpython3.11.dll.a `
  -Dpybind11_DIR=E:/msys64/mingw64/lib/cmake/pybind11
& E:\msys64\mingw64\bin\cmake.exe --build D:\kage-cpp\build --target KagePython -j2
```

模块输出到 `D:\kage-cpp\build\python`。如果曾在该目录加载过 `.pyd`，先退出对应 Python 进程，再重新链接模块。

## 快速开始

```python
import kage_cpp

engine = kage_cpp.Engine(kage_cpp.FontType.MINCHO)
engine.set_buhin("u4e00", "1:0:0:20:100:180:100")

svg = engine.render_svg("u4e00")
expanded_strokes = engine.extract("u4e00")
status = engine.check("u4e00")
print(status == kage_cpp.CheckGlyphState.OK)
```

`Engine` 默认使用明朝体，`size` 默认是 `0`，表示使用引擎默认字号。`render`、`extract` 和 `check` 按部件名操作；对应的 `*_data` 方法直接接收 KAGE 数据字符串。

## Engine 接口

### 部件库

- `set_buhin(name, data)`：新增或替换本地部件。
- `set_buhin_strokes(name, strokes)`：以 `Stroke` 列表新增或替换部件。
- `push_buhin(name, data)`：仅在名称尚不存在时插入；已有条目不会被覆盖。
- `push_buhin_strokes(name, strokes)`：以 `Stroke` 列表执行相同的插入操作。
- `search_buhin(name)`：返回笔画列表。本地库未命中时，如果设置了数据库回调，会继续调用回调。
- `delete_buhin(name)`：删除本地部件；返回 `True` 表示找到并删除，`False` 表示本地库中没有该名称。
- `export_buhins()`：一次性返回本地部件库，格式为 `{名称: KAGE数据字符串}`，按名称排序。

`delete_buhin` 只删除本地条目。若配置了数据库回调，后续查询同名部件仍可能从回调取得数据。`export_buhins` 只包含 Engine 内存中的本地条目，不会枚举外部数据库或调用回调。

### 字体、回调和缺省字形

- `set_font(font, size=0)`：更改字体和字号。
- `set_db_search_callback(callback)`：设置部件查询回调。回调接收部件名，返回 KAGE 数据字符串；未找到时可返回 `None`。传入 `None` 可清除回调。
- `set_notdef_glyph(data)`：设置未找到部件时使用的缺省字形数据。
- `set_notdef_glyph_strokes(strokes)`：以笔画列表设置缺省字形。

回调只在本地库查询未命中时触发。预先使用 `set_buhin` 加载常用部件可减少回调调用。

### 渲染、拆解和检查

- `render(name)` / `render_data(data)`：返回 `Canva` 轮廓。
- `render_data_separated(data)`：按输入数据的原始行分组，返回每行对应的轮廓列表；部件引用展开后的多个笔画仍归在引用行下，适合逐笔画选取或编辑。
- `render_svg(name)` / `render_data_svg(data)`：返回 SVG 字符串。
- `render_sfd(name)` / `render_data_sfd(data)`：返回 SFD 字符串。
- `extract(name)` / `extract_data(data)`：展开引用部件，返回笔画列表。
- `check(name)` / `check_data(data)`：检查字形，返回 `CheckGlyphState`。

`CheckGlyphState` 包含 `OK`、`BUHIN_NOTFOUND` 和 `LOOPBACK_DETECTED`。

## 数据类型与模块函数

### Stroke 和枚举

`Stroke` 可读写字段包括 `type`、`start`、`end`、`p1` 至 `p4`、`type_opt`、`start_opt`、`end_opt`、`buhin`、`s1`、`s2`、`special1` 和 `special2`。其中 `p1` 至 `p4` 和 `s1`、`s2` 都是 `Point`；不同笔画类型只使用其中一部分字段。

枚举值如下，Python 中通过 `kage_cpp.枚举类.成员名` 使用：

- `FontType`：`DEFAULT`、`MINCHO`（明朝体）、`GOTHIC`（黑体）。
- `CheckGlyphState`：`OK`、`BUHIN_NOTFOUND`（部件未找到）、`LOOPBACK_DETECTED`（检测到循环引用）。
- `StrokeType`：`SPECIAL`（特殊指令，0）、`STRAIGHT`（直线，1）、`CURVE`（二次曲线，2）、`BENDING`（折线，3）、`BENDING_ROUND`（折弯，4）、`BEZIER`（三次曲线，6）、`VCURVE`（竖撇，7）、`REFERENCE`（部件引用，99）。
- `SpecialInt1`：`FLIP_UPDOWN`（上下翻转，97）、`FLIP_LEFTRIGHT`（左右翻转，98）、`ROTATE`（旋转，99）。
- `SpecialInt2`：`ROTATE_90`（旋转 90 度，1）、`ROTATE_180`（旋转 180 度，2）、`ROTATE_270`（旋转 270 度，3）。
- `StartType`：`OPEN`（开放，0）、`CONNECTING_H`（横向连接，2）、`UPPER_LEFT_CORNER`（左上角，12）、`UPPER_RIGHT_CORNER`（右上角，22）、`CONNECTING_V`（竖向连接，32）、`THIN`（尖端，7）、`ROOFED_THIN`（横捺角，27）、`INTERNAL_FOR_BENDING`（内部折线用，6）、`INTERNAL_PLANE`（内部平面，1）、`CONNECT_THIN`（细笔连接，37）、`CONNECTING_MANUAL`（手动连接，39）。
- `EndType`：`OPEN`（开放，0）、`CONNECTING_H`（横向连接，2）、`CONNECTING_V`（竖向连接，32）、`LOWER_LEFT_CORNER`（左下角，13）、`LOWER_RIGHT_CORNER`（右下角，23）、`LOWER_LEFT_ZH_OLD`（旧式左下折，313）、`LOWER_LEFT_ZH_NEW`（新式左下折，413）、`TURN_LEFT`（左转钩，4）、`LOWER_RIGHT_HT`（台标右下角，24）、`TURN_UPWARDS`（向上弯钩，5）、`LEFT_SWEEP`（左撇尖，7）、`STOP`（点，1）、`INTERNAL_LAST_FILLED_CIRCLE`（内部末尾实心圆，1）、`INTERNAL_TURN_LEFT`（内部左转，14）、`INTERNAL_TURN_UPWARDS`（内部向上转，15）。

起笔和收笔分别使用 `StartType`、`EndType`；特殊变换使用 `SpecialInt1` 和 `SpecialInt2`。这些枚举可直接用于创建和修改 `Stroke`。

例如：

```python
stroke = kage_cpp.Stroke()
stroke.type = kage_cpp.StrokeType.STRAIGHT
stroke.start = kage_cpp.StartType.OPEN
stroke.end = kage_cpp.EndType.OPEN
stroke.p1 = kage_cpp.Point(20, 100)
stroke.p2 = kage_cpp.Point(180, 100)
engine.set_buhin_strokes("horizontal", [stroke])
```

`Stroke` 的 `repr` 会按类型显示有效字段，并以“枚举名(原始数值)”显示起笔、收笔和特殊变换，例如 `start=UPPER_LEFT_CORNER(12)`。`type_opt`、`start_opt` 和 `end_opt` 是数值调节参数，仍以数字显示。

### 其他类型

- `Point(x, y)`：二维坐标，可读写 `x`、`y`。
- `BoundingData`：边界框，字段为 `min_x`、`min_y`、`max_x`、`max_y`。
- `Contour`：轮廓；`points`、`point_types`、`closed` 是只读属性，`size()` 返回点数。
- `Canva`：渲染结果；`contours()` 返回轮廓列表，`size()` 返回轮廓数，`clear()` 清空内容。

这些对象的 `repr` 会显示字段或数量摘要，例如 `Point(x=20, y=100)`、`Canva(contours=3)`。

### 模块级函数

- `parse(data)`：将 KAGE 数据字符串解析为 `Stroke` 列表。
- `serialize(strokes)`：将笔画列表序列化为 KAGE 数据字符串。
- `bounding_box(strokes)`：计算笔画边界框，返回 `BoundingData`。
- `stretch(destination, source, position, minimum, maximum)`：调用引擎的坐标拉伸计算。
- `canva_to_svg(canva)` / `canva_to_sfd(canva)`：将 `Canva` 转为 SVG 或 SFD 字符串。

`parse` 会忽略无法解析的笔画条目，不会逐条抛出错误。`check_data` 检查的是解析后的字形及部件引用，不能检出已被 `parse` 跳过的格式错误条目；需要严格校验输入格式时，应在 Python 层先检查原始数据。

## 多线程

同一个 `Engine` 实例的公开操作由递归互斥锁保护。渲染、拆解和检查在执行 C++ 引擎时会释放 Python GIL，因此不同线程可调用同一实例，但这些调用会在该实例的锁上依次执行。若需要并行处理多个字形，可为工作线程各自创建 `Engine`，并为每个实例加载所需部件。

Python 数据库回调在引擎需要时重新获取 GIL。回调自身访问的 Python 数据仍应由调用方按其并发模型保护。

## 常见问题

- **找不到 `Python3 Development.Module`**：确认安装了所选解释器对应的开发头文件和库；MSYS2 应使用同一 `mingw64` 环境中的 Python、编译器和库。
- **配置时发现了错误的 Python 版本**：构建目录可能缓存了之前的解释器。使用上面的 `-U "Python3_*" -U "_Python3_*"` 参数重新配置，或换一个新的构建目录。
- **`import kage_cpp` 失败**：将构建输出的 `python` 子目录加入 `PYTHONPATH`，并确保 Python 版本、架构和运行时依赖与模块兼容。使用构建模块时的同一套 Python 通常最稳妥；MinGW 构建还需确保 MinGW 运行时 DLL 可被找到。
- **链接时提示无法覆盖 `.pyd`**：先退出已导入该模块的 Python 进程，再重新构建。
