# Architecture

How the code is organized and how a frame gets from the simulation to the screen. This document covers what exists
today: the window, the main loop, the coordinate system, the route, stations and bus, and the text drawing. Passengers,
the inspector and the HUD will plug into the same structure (see [Extending](#8-extending)).

Rules and limits live in `AGENTS.md`; naming and formatting live in `style-guide.md`. This file explains the design.
`Architecture-current.drawio` is a diagram of the structure below.

## 1. Layers

Three layers, one namespace each. Dependencies point one way only.

```
app  ──►  graphics  ──►  simulation
```

| Layer | Namespace | Responsibility |
|---|---|---|
| `simulation` | `nesmart::simulation` | Pure game logic and the world's coordinate constants. No OpenGL, no GLFW. |
| `graphics` | `nesmart::graphics` | Everything OpenGL: shaders, textures, meshes, fonts, camera, drawing. |
| `app` | `nesmart::app` | Window, main loop, frame limiter. Owns the other layers and wires them together. |

`simulation` never includes `graphics/` or `app/`; `graphics` never includes `app/`.

## 2. Classes

| Class | Layer | Purpose |
|---|---|---|
| `World.hpp` (constants) | simulation | The coordinate contract: world size, safe area size, safe area center. No behaviour. |
| `Route` | simulation | The closed route sampled into a polyline at startup, with cumulative distances, station positions and `positionAt` / `headingAt` by distance. |
| `Bus` | simulation | Distance travelled along the route at constant speed, plus the cached position, heading and facing. |
| `Simulation` | simulation | Owns the `Route` and the `Bus`; `update(dt)` advances them. |
| `Shader` | graphics | Compiles and links a vertex and fragment shader; sets uniforms by name. |
| `Texture` | graphics | An image on the GPU. Loads a file with `stb_image`, always RGBA, flipped on load. |
| `Mesh` | graphics | A shape on the GPU: one VAO and one VBO, uploaded once. Knows its own primitive mode. |
| `Font` | graphics | A glyph atlas built with FreeType: one GL texture plus per-glyph metrics and `width(text)`. |
| `Camera` | graphics | Computes the world-to-screen projection matrix from the screen size. Pure math. |
| `Renderer` | graphics | The only class that draws. Owns all shaders, textures, meshes, the font and the camera, and decides the draw order. |
| `Window` | app | Full-screen GLFW window, GL context, GLEW. Exposes its pixel size. |
| `Timer` | app | Small clock helpers used by `FrameLimiter`. |
| `FrameLimiter` | app | Holds the loop to 75 FPS and provides `dt`. |
| `Application` | app | Main loop and input callbacks. Owns `Window`, `FrameLimiter`, `Simulation` and `Renderer`. |

GL resources (`Shader`, `Texture`, `Mesh`, `Font`) are RAII: they release their GL objects in the destructor and cannot
be copied. `Camera`, `World.hpp` and everything in `simulation` make no GL calls.

## 3. Ownership and startup

```
main()
 └─ Application
     ├─ Window            creates the window, GL context, GLEW
     ├─ FrameLimiter
     ├─ Simulation        declared before Renderer: the renderer builds the route mesh from its Route
     │   ├─ Route
     │   └─ Bus
     └─ Renderer          needs the GL context, so it is declared after Window
         ├─ Shader x3     sprite, solid, text (assets/shaders)
         ├─ Camera        projection from screen size + World.hpp
         ├─ Mesh x2       unit quad, route ribbon
         ├─ Texture x3    background, circle, bus (assets/images)
         └─ Font          Roboto Regular (assets/fonts)
```

- Members are constructed in declaration order and destroyed in reverse. Because `m_window` is declared before
  `m_renderer`, the GL context exists while the renderer's GL objects are created and still exists while they are
  destroyed. No manual cleanup is needed.
- Any constructor that cannot finish (missing shader file, compile error, missing image or font) throws
  `std::runtime_error`. It propagates to `main`, which prints the message and returns -1.

## 4. A frame

`Application::run()`:

```
while (!window.shouldClose())
    dt = limiter.delta()
    simulation.update(dt)
    renderer.draw(simulation)
    window.swapBuffers()
    window.pollEvents()      // key callback: Escape closes the window
    limiter.tick()           // sleeps to hold 75 FPS (VSync is off)
```

`Renderer::draw(const Simulation&)`, in this order:

1. Clear the screen.
2. `drawBackground()`: sprite shader, unit quad scaled to the world size.
3. `drawRoute()`: solid shader, the route ribbon mesh.
4. `drawStations()`: a circle sprite per station, then its number as text.
5. `drawBus(bus)`: sprite shader, positioned, rotated and mirrored from the bus state.

Each draw function selects its shader, sets `uProjection`, binds its texture and builds its own `uModel`. Blending is
enabled once in the constructor so transparent areas of sprites and glyph edges work.

## 5. Coordinate system

All game objects are positioned in **world space**, defined in `simulation/World.hpp`:

- Units are background image pixels.
- Origin is the top-left corner, x points right, **y points down**.
- The world is the background image: `WORLD_WIDTH` x `WORLD_HEIGHT`.
- The **safe area** (`SAFE_WIDTH` x `SAFE_HEIGHT`, centered at `SAFE_CENTER_X/Y`) is the region every screen must show.
  Route and stations must stay inside it.

| Space | Used for | Range |
|---|---|---|
| Mesh space | vertex positions in a `Mesh` (unit quad is 0..1) | local |
| World space | everything the game places | 0..WORLD_WIDTH, 0..WORLD_HEIGHT |
| Clip / NDC | what the GPU consumes | -1..1 |

`uModel` takes mesh space to world space (position, size, rotation of one sprite). `uProjection` takes world space to
clip space. The route ribbon is built directly in world space, so its `uModel` is the identity.

### Camera

`Camera` computes the projection once in its constructor (the window never resizes):

```
scale      = min(screenW / SAFE_WIDTH, screenH / SAFE_HEIGHT)   // pixels per world unit
halfWidth  = screenW  / (2 * scale)                              // visible half-extents in world units
halfHeight = screenH  / (2 * scale)
projection = ortho(cx - halfWidth, cx + halfWidth, cy + halfHeight, cy - halfHeight)
```

- The safe area is always fully visible, and the scale is uniform, so circles stay round.
- The limiting axis shows exactly the safe area; the other axis shows extra background. A wider screen reveals more on
  the left and right, a taller one more above and below.
- `bottom` is passed larger than `top` on purpose: that is what makes world y point down.
- The viewport is the whole window (`glViewport` in `Window`); there is no letterboxing.

### Background coverage

The background must be large enough that the extra area never runs past the image. With a 3000 x 3000 image, a
2300 x 1680 safe area centered at (1500, 1450):

- Aspect ratios from about 5:4 up to about 1.78:1 (16:9) are fully covered.
- Wider screens (for example 21:9) show the clear color on the left and right, because the image is only 3000 px wide.

To support wider screens, use a wider image or a narrower safe area. Keep `WORLD_WIDTH` and `WORLD_HEIGHT` equal to the
image size; if they differ the background is stretched.

### Textures and UVs

`Texture` calls `stbi_set_flip_vertically_on_load(true)`, so UV (0, 0) is the **bottom-left** of the image (OpenGL
convention). Because world y points down, the unit quad's UVs are flipped to match:

| Corner | Position (x, y) | UV (s, t) |
|---|---|---|
| top-left | 0, 0 | 0, 1 |
| top-right | 1, 0 | 1, 1 |
| bottom-left | 0, 1 | 0, 0 |
| bottom-right | 1, 1 | 1, 0 |

The quad is drawn as a `GL_TRIANGLE_STRIP`, so its vertices are in that order. If an image ever appears upside down,
check this table first.

## 6. Route and bus

- `Route` samples its curve into a polyline once at startup and stores the cumulative distance at each point. Looking
  up a position or heading by distance is a search in that table, so no curve is evaluated per frame. It also places
  `STATION_COUNT` stations along the route and stores each one's position and distance.
- `Bus` only stores how far it has travelled (`m_distance`). `update(dt, route)` adds `SPEED * dt`, wraps at the route's
  total length with `fmod`, and caches the position, heading and facing for that distance.
- The heading is only updated when it differs from the shown one by more than `ROTATION_THRESHOLD`, so the sprite does
  not jitter along small bends.
- There is no state machine yet: the bus never stops at stations.

## 7. Shaders and text

Three shader pairs in `assets/shaders/`, all taking `uProjection` and `uModel` and all using a mesh in the unit quad's
layout or the ribbon's layout:

| Shader | Mesh layout | Other uniforms | Used for |
|---|---|---|---|
| `sprite` | `{2, 2}`: `aPos`, `aUV` | `uTexture` | background, station circles, bus |
| `solid` | `{2}`: `aPos` | `uColor` | the route ribbon |
| `text` | `{2, 2}`: `aPos`, `aUV` | `uTexture`, `uColor`, `uUVOffset`, `uUVSize` | station numbers |

Text: `Font` renders its glyphs once into a single-channel atlas texture. `Renderer::drawTextCentered` draws one quad per
glyph with the unit quad mesh, picking the glyph's part of the atlas with `uUVOffset` / `uUVSize`. The fragment shader
uses the atlas value as alpha. Glyph sizes are in pixels and placed in world units, so one font pixel is one world
unit.

## 8. Extending

- **A new sprite (passenger, inspector, icon):** load a `Texture`, then in `Renderer` build a `uModel` (translate,
  optionally rotate, then scale) and draw the same unit-quad `Mesh`. No new mesh or shader is needed.
- **A new shape:** build a vertex array, create a `Mesh` with the right primitive mode (`GL_TRIANGLE_FAN`,
  `GL_LINE_LOOP`, ...), and draw it from `Renderer`.
- **Simulation state:** `Bus`, `Route` and the rest live in `simulation` and hold only game state (distance, state,
  timer, counts). They never hold GL objects. `Application` owns a `Simulation`, calls `simulation.update(dt)` each
  frame, and passes a `const` view of it to `Renderer::draw`, which reads positions and builds each model matrix.
- **HUD:** draw last, anchored to the camera's visible bounds (to be added to `Camera`) so items sit at the screen
  corners on any aspect ratio, not at the safe area's corners.

## 9. Build notes

- `src/`, `external/glm`, `external/stb` and the FreeType headers must be on the include path, and `freetype.lib` must
  be linked. `stb_image.h` is included as `<stb_image.h>`, and `STB_IMAGE_IMPLEMENTATION` is defined only in
  `Texture.cpp`.
- Include `GL/glew.h` before `GLFW/glfw3.h`. In `.cpp` files, put the file's own header first; it pulls in GLEW before
  any GLFW include.
- Asset paths in code are relative (`assets/shaders/...`, `assets/images/...`, `assets/fonts/...`), so the process's
  working directory must be the project folder. In Visual Studio set Project Properties, Debugging, Working Directory
  to `$(ProjectDir)`.
