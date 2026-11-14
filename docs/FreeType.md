# FreeType: Setup and Text Rendering

How to add FreeType (static, prebuilt binaries) to NeSmart and use it to draw text. FreeType is on the allowed
dependency list in `AGENTS.md`, so no extra approval is needed. The files go under `NeSmart-2D/external/freetype/` and
are committed, like GLM and `stb_image`.

## 1. Get the binaries

Use the community prebuilt Windows binaries: <https://github.com/ubawurinna/freetype-windows-binaries>
(download the ZIP of the latest release, or clone it). It contains:

```
include/                      <- headers (ft2build.h and the freetype/ folder)
release static/vs2015-2022/win64/freetype.lib
release static/vs2015-2022/win32/freetype.lib
debug static/vs2015-2022/win64/freetype.lib
debug static/vs2015-2022/win32/freetype.lib
...
```

Folder names can differ slightly between releases. Check what you actually downloaded. This project's toolset (`v145`)
is ABI-compatible with the `vs2015-2022` libs.

## 2. Copy into the repo

Only copy what you need (static libs, x64 first; add Win32 only if you build it):

```
NeSmart-2D/external/freetype/
├── include/                  <- contents of the downloaded include/ (ft2build.h, freetype/)
└── lib/
    └── x64/
        ├── Debug/freetype.lib
        └── Release/freetype.lib
```

Also copy the FreeType license (`FTL.TXT` / `LICENSE.TXT` from the download) to `external/freetype/` and keep it
committed.

Check that `.gitignore` does not exclude `*.lib`. If it does, add an exception
(`!NeSmart-2D/external/freetype/lib/**/*.lib`), otherwise the libs will not be committed.

## 3. Visual Studio project settings

Open **Project -> Properties** and set **Configuration: All Configurations**, **Platform: All Platforms** for the first
item, then set the per-configuration items separately.

1. **C/C++ -> General -> Additional Include Directories**: append
   `$(ProjectDir)external/freetype/include`
2. **Linker -> General -> Additional Library Directories** (Platform: x64):
   - Debug: `$(ProjectDir)external/freetype/lib/x64/Debug`
   - Release: `$(ProjectDir)external/freetype/lib/x64/Release`
3. **Linker -> Input -> Additional Dependencies**: append `freetype.lib`

Notes:

- Use `$(ProjectDir)`. The existing `.vcxproj` has `$(ProjecdDir)` (typo) in the GLM and stb include entries. That macro
  is undefined and expands to nothing, so those paths only work by accident, through the relative path. Correct it when
  you are in there, but do not copy the typo.
- If the linker reports `LNK2038: mismatch detected for 'RuntimeLibrary'`, the lib was built with a different CRT
  setting than the project (`/MT` vs `/MD`, or Debug vs Release). Match the Debug lib to Debug and the Release lib to
  Release. If it still fails, switch **C/C++ -> Code Generation -> Runtime Library** to the one the lib expects.
- Expected harmless linker warnings with the prebuilt static lib:
  - `LNK4099: PDB 'freetype.pdb' was not found`: the lib references a debug-info file that is not shipped. Ignore it,
    or add `4099` under **Linker -> Debugging -> Ignore specific warnings**.
  - `LNK4098: defaultlib 'LIBCMT' conflicts with use of other libs`: the lib was built with `/MT` while the project
    uses `/MD`. It links and runs fine because FreeType frees its own allocations. To silence it, add `LIBCMT` under
    **Linker -> Input -> Ignore Specific Default Libraries**.
- No DLL is needed, since the linking is static.
- Do not forget to add the new `src/graphics/*.cpp` and shader files to `.vcxproj` / `.vcxproj.filters`, as usual.

## 4. Sanity check

Add this temporarily, for example at the start of `main`, build, then remove it:

```cpp
#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library library;
if (FT_Init_FreeType(&library) != 0)
{
    throw std::runtime_error("FreeType init failed");
}
FT_Done_FreeType(library);
```

If this compiles, links and runs, the setup is done. `ft2build.h` plus `FT_FREETYPE_H` is the documented way to include
FreeType; do not include `freetype/freetype.h` directly.

## 5. Pick a font

`assets/fonts/` is empty. Put a `.ttf` there, for example `assets/fonts/Roboto-Regular.ttf` or DejaVu Sans. Use an
openly licensed font and commit the license next to it. Do not copy fonts from `C:\Windows\Fonts`.

If the UI needs Serbian Latin letters (č, ć, š, ž, đ), make sure the font has them (Roboto and DejaVu do), and include
them in the glyph range (see below).

## 6. Text rendering design (fits the existing layers)

`Font.hpp` / `Font.cpp` already exist in `src/graphics/` as empty stubs, so this fits the planned architecture. FreeType
is OpenGL-adjacent, so it stays in `graphics`. `simulation` never sees it.

### `Font` class (RAII)

- Constructor `Font(const std::string& path, unsigned int pixelHeight)`; throws `std::runtime_error` on any failure.
- Owns `FT_Library` and `FT_Face` only during construction. Once the atlas is built, call `FT_Done_Face` and
  `FT_Done_FreeType`. The font is then just a texture plus metrics, so the destructor only releases GL objects.
- Builds **one atlas texture** (`GL_R8`, one channel) holding all glyphs, rather than one texture per glyph.
- Stores a small `std::array`/`std::vector` of per-glyph data: UV rectangle in the atlas, size in pixels, bearing
  (`bitmap_left`, `bitmap_top`) and advance.
- API sketch: `bind(unit)`, `const Glyph& glyph(char32_t)`, `float width(const std::string&)` (for centring),
  `float lineHeight()`.

### Building the atlas

```cpp
FT_Init_FreeType(&library);
FT_New_Face(library, path.c_str(), 0, &face);
FT_Set_Pixel_Sizes(face, 0, pixelHeight);

glPixelStorei(GL_UNPACK_ALIGNMENT, 1);          // glyph bitmaps are 1 byte per pixel, rows are NOT 4-byte aligned

for each character c in the range (ASCII 32..126, plus č ć š ž đ if needed)
{
    FT_Load_Char(face, c, FT_LOAD_RENDER);       // renders an 8-bit coverage bitmap into face->glyph->bitmap
    // copy face->glyph->bitmap.buffer into the atlas at the current pen position (respect bitmap.pitch)
    // record: atlas x/y, bitmap.width/rows, bitmap_left, bitmap_top, advance.x >> 6
}
```

Practical points:

- Forgetting `glPixelStorei(GL_UNPACK_ALIGNMENT, 1)` is the classic cause of skewed or garbage glyphs.
- Pack glyphs left to right in rows with 1 px padding between them to avoid bleeding when sampling with
  `GL_LINEAR`. Allocate the atlas with `glTexImage2D(..., GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr)` and fill
  it with `glTexSubImage2D`, or build it in a `std::vector<unsigned char>` first and upload once (simpler).
- Use `GL_CLAMP_TO_EDGE` and `GL_LINEAR` (or `GL_NEAREST` for a crisp pixel look).
- FreeType bitmaps are top row first, while `Texture` flips images on load so UV (0, 0) is bottom-left. Pick one
  convention for the atlas and be consistent. Easiest: keep the atlas top-down and flip V when computing glyph UVs.
- `advance.x` is in 1/64 pixel units, so shift by 6 (`>> 6`) to get pixels.
- Fonts render at one pixel size. Create a second `Font` for a different size instead of scaling a big one. Scaling
  blurs.

### Drawing text

- New shader pair `assets/shaders/text.vert` / `text.frag` (or reuse `sprite.vert`, which already takes
  `uProjection`, `uModel`, `aPos`, `aUV`). The fragment shader treats the red channel as coverage:

```glsl
#version 330 core
in vec2 vUV;
uniform sampler2D uTexture;   // the font atlas (GL_R8)
uniform vec4 uColor;
out vec4 fragColor;
void main()
{
    fragColor = vec4(uColor.rgb, uColor.a * texture(uTexture, vUV).r);
}
```

- Blending must be on (`GL_SRC_ALPHA`, `GL_ONE_MINUS_SRC_ALPHA`). The renderer already enables it for sprites.
- `Renderer` gets `drawText(const Font&, const std::string&, glm::vec2 pos, glm::vec4 color)`. For each character it
  places a quad at
  `x + bearingX`, `y - (height - bearingY)` (baseline-relative, y up, which matches the camera), sized to the glyph, with
  the glyph's UV rectangle. It then advances `x` by the glyph's advance.
- Simplest approach for this project: reuse the unit-quad mesh and set `uModel` plus a UV offset/scale uniform per
  glyph (one draw call per character). A text string here is tiny (timers, fines, station names), so there is no need
  for batching. If you later want one draw call per string, build a dynamic VBO per string.
- Text uses the same `Camera` projection as everything else, so positions are in world units. If the HUD should be
  pixel-exact, use the same world-to-pixel scale for the font size and positions.

### Where the text shows up

Per the project spec, the UI text is things like the 10 s stop timer, the fine amount and the passenger/inspector state.
`Renderer` reads that from a read-only view of the simulation and calls `drawText`. The `simulation` layer exposes plain
numbers and states and never formats pixels.

## 7. Checklist

- [ ] Binaries copied to `external/freetype/` (`include/`, `lib/x64/Debug`, `lib/x64/Release`, license)
- [ ] `.gitignore` allows the `.lib` files
- [ ] Include directory, library directories and `freetype.lib` set for **Debug|x64** and **Release|x64**
- [ ] Sanity check builds and runs, then is removed
- [ ] Font and its license in `assets/fonts/`
- [ ] `Font` class, `text.vert`, `text.frag` added to the project (`.vcxproj` and `.filters`)
- [ ] `Renderer::drawText` implemented and a test string appears on screen
