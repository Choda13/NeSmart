# Architecture

How the code is organized and how a frame gets from the simulation to the screen. This document covers what exists
today: the window, the main loop, input, the coordinate system, the route, stations, the bus state machine, passengers,
the inspector and fines, the HUD, and the text drawing.

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
| `app` | `nesmart::app` | Window, main loop, input, frame limiter. Owns the other layers and wires them together. |

`simulation` never includes `graphics/` or `app/`; `graphics` never includes `app/`.

## 2. Classes

| Class | Layer | Purpose |
|---|---|---|
| `World.hpp` (constants) | simulation | The coordinate contract: world size, safe area size, safe area center. No behaviour. |
| `Route` | simulation | The closed route sampled into a polyline at startup, with cumulative distances, station positions and `positionAt` / `headingAt` by distance. |
| `Bus` | simulation | State machine (`Driving` / `StoppedAtStation`), distance along the route at constant speed, cached position, heading and facing, passenger count, inspector and fines total. |
| `Simulation` | simulation | Owns the `Route` and the `Bus`; `update(dt)` advances them. Forwards the input actions `addPassenger`, `removePassenger` and `addInspector`. |
| `Shader` | graphics | Compiles and links a vertex and fragment shader; sets uniforms by name. |
| `Texture` | graphics | An image on the GPU. Loads a file with `stb_image`, always RGBA, flipped on load. |
| `Mesh` | graphics | A shape on the GPU: one VAO and one VBO, uploaded once. Knows its own primitive mode. |
| `Font` | graphics | A glyph atlas built with FreeType: one GL texture plus per-glyph metrics and `width(text)`. |
| `Sprite` | graphics | A plain struct: texture, position, size, rotation, origin, flips, and `model()`. `makeSprite` builds one from a width and the texture's aspect ratio. |
| `SpriteRenderer` | graphics | Draws `Sprite`s with the sprite shader and a shared unit quad. `begin(projection)`, then `draw(sprite)`. |
| `TextRenderer` | graphics | Draws text with the text shader and one `Font`: `drawCentered`, `drawAt`, `width`, `height`. |
| `Camera` | graphics | Computes the world projection and the screen (HUD) projection from the screen size. Pure math. |
| `ResourceManager` | graphics | Loads every shader, texture and font once on construction; the rest of the code only looks them up by name. |
| `Scene` | graphics | Draws the world (background, route, stations with labels, bus) with `Camera::projection()`. |
| `Hud` | graphics | Draws the screen-space overlay with `Camera::screenProjection()`: doors card, passengers card, fines card, inspector icon, author credit. |
| `Renderer` | graphics | Owns `ResourceManager`, the `SpriteRenderer`, two `TextRenderer`s (regular and bold), `Scene` and `Hud`; receives the `Camera`. Sets the GL state and draws one frame: clear, `Scene`, `Hud`. |
| `Window` | app | Full-screen GLFW window, GL context, GLEW. Exposes its pixel size. |
| `FrameLimiter` | app | Holds the loop to 75 FPS and provides `dt`. Defines `Clock` as `std::chrono::steady_clock`. |
| `Application` | app | Main loop, input callbacks and the custom cursor. Owns `Window`, `FrameLimiter`, `Simulation`, `Camera` and `Renderer`. |

GL resources (`Shader`, `Texture`, `Mesh`, `Font`) are RAII: they release their GL objects in the destructor and cannot
be copied. `Camera`, `Sprite`, `World.hpp` and everything in `simulation` make no GL calls.

## 3. Ownership and startup

```
main()
 └─ Application
     ├─ Window            creates the window, GL context, GLEW
     ├─ FrameLimiter
     ├─ Simulation        declared before Renderer: the renderer builds the route mesh from its Route
     │   ├─ Route
     │   └─ Bus           declared after Route: its constructor reads the route
     ├─ Camera            projection from screen size + World.hpp
     └─ Renderer          needs the GL context, so it is declared after Window; gets the Camera and Simulation
         ├─ ResourceManager   shaders sprite, solid, text; textures background, circle, bus, doors_closed,
         │                    doors_opened, passengers_hud, fines_hud, inspection; fonts Roboto Regular and Bold
         ├─ SpriteRenderer    shared by Scene and Hud
         ├─ TextRenderer x2   regular (Scene, station labels) and bold (Hud)
         ├─ Scene             route ribbon mesh, station and bus sprites
         └─ Hud               card, inspector and door sprites
```

- Members are constructed in declaration order and destroyed in reverse. Because `m_window` is declared before
  `m_renderer`, the GL context exists while the renderer's GL objects are created and still exists while they are
  destroyed. No manual cleanup is needed.
- `Application`'s constructor also loads `assets/images/cursor.png` with `stb_image` and sets it as the GLFW cursor.
- Any constructor that cannot finish (missing shader file, compile error, missing image or font) throws
  `std::runtime_error`. It propagates to `main`, which prints the message and returns -1.

## 4. A frame

`Application::run()`:

```
while (!window.shouldClose())
    dt = limiter.delta()
    simulation.update(dt)
    renderer.draw()
    window.swapBuffers()
    window.pollEvents()      // input callbacks, see below
    limiter.tick()           // sleeps to hold 75 FPS (VSync is off)
```

Input callbacks are static functions in `Application` that reach the object through the GLFW window user pointer. They
only call `Simulation`:

| Input | Action |
|---|---|
| Escape | close the window |
| Left mouse button | `addPassenger()` |
| Right mouse button | `removePassenger()` |
| K | `addInspector()` |

`Renderer::draw()` clears the screen, then calls `Scene::draw` and `Hud::draw`. `Scene` and `Hud` get the `Simulation`
in their constructors and read it every frame, so `draw()` takes no arguments.

`Scene::draw`, in this order:

1. Background: sprite shader, unit quad scaled to the world size.
2. Route: solid shader, the route ribbon mesh.
3. Stations: a circle sprite per station, then its number as text.
4. Bus: sprite shader, positioned, rotated and mirrored from the bus state.

`Hud::draw` uses the screen projection (units are screen pixels, origin top-left, y down). Sprites first: the doors card
(top-left; the closed image while the bus moves, the open image while stopped), the passengers card and the fines card
(top-right, stacked), and the inspector icon (bottom-left, only while an inspector is aboard). Then text with the bold
font: the doors state, `count / BUS_CAPACITY`, the fines total, and the author credit (bottom-right).

`SpriteRenderer` and `TextRenderer` each take the projection in `begin()`, which also selects their shader, so a change
of projection or shader is a new `begin()` call. Each sprite builds its `uModel` from `Sprite::model()`. Blending is
enabled once in the `Renderer` constructor so transparent areas of sprites and glyph edges work.

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
| Screen space | the HUD | 0..screen width, 0..screen height, y down |
| Clip / NDC | what the GPU consumes | -1..1 |

`uModel` takes mesh space to world (or screen) space (position, size, rotation of one sprite). `uProjection` takes that
space to clip space: `Camera::projection()` for the world, `Camera::screenProjection()` for the HUD. The route ribbon
is built directly in world space, so its `uModel` is the identity.

### Camera

`Camera` computes both projections once in its constructor (the window never resizes). The world projection:

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
- The screen projection maps screen pixels directly (origin top-left, y down) and ignores the world and the safe area.
  `screenSize()` gives the size so HUD items can be anchored to the corners.

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
  `STATION_COUNT` (10) stations along the route and stores each one's position and distance.
- `Bus` stores how far it has travelled (`m_distance`) and the next station. While `Driving`, `update(dt, route)` adds
  `SPEED * dt`, wraps at the route's total length with `fmod`, and caches the position, heading and facing for that
  distance. It starts at station 0 and heads for station 1.
- The heading is only updated when it differs from the shown one by more than `ROTATION_THRESHOLD`, so the sprite does
  not jitter along small bends.
- State machine: `Driving` and `StoppedAtStation`. When the next station is within one step, the bus snaps onto it and
  stops for `STOP_DURATION` (10 s), counting a `float m_stopTimer` down with `dt`. When it reaches zero the bus drives on
  and targets the following station. Doors are open exactly while stopped (`isMoving()` drives the HUD).
- Passengers: `addPassenger` and `removePassenger` work only while stopped. Adding is capped at `BUS_CAPACITY` (50).
- Inspector: `addInspector` works only while stopped and with no inspector aboard. The inspector counts as a passenger
  and boards even when the bus is full. When the bus arrives at the next station the inspector leaves, and a random
  number of the other passengers (0 to count - 1) is added to `finesTotal`. `removePassenger` never removes the
  inspector.

## 7. Shaders and text

Three shader pairs in `assets/shaders/`, all taking `uProjection` and `uModel` and all using a mesh in the unit quad's
layout or the ribbon's layout:

| Shader | Mesh layout | Other uniforms | Used for |
|---|---|---|---|
| `sprite` | `{2, 2}`: `aPos`, `aUV` | `uTexture` | background, station circles, bus, HUD cards and icons |
| `solid` | `{2}`: `aPos` | `uColor` | the route ribbon |
| `text` | `{2, 2}`: `aPos`, `aUV` | `uTexture`, `uColor`, `uUVOffset`, `uUVSize` | station numbers, HUD text |

Text: `Font` renders its glyphs once into a single-channel atlas texture. `TextRenderer` draws one quad per glyph with its
own unit quad mesh (`drawCentered` for station numbers, `drawAt` for left-aligned HUD text with a scale), picking the
glyph's part of the atlas with `uUVOffset` / `uUVSize`. The fragment shader uses the atlas value as alpha. Glyph sizes
are in pixels and placed in the current projection's units: one font pixel is one world unit in the `Scene` and one
screen pixel in the `Hud`.

## 8. Extending

- **A new sprite:** add the PNG to `assets/images/`, register it in `ResourceManager`'s constructor, build a `Sprite`
  with `makeSprite` in `Scene` (world) or `Hud` (screen), and draw it with `SpriteRenderer`. No new mesh or shader is
  needed.
- **A new shape:** build a vertex array, create a `Mesh` with the right primitive mode (`GL_TRIANGLE_FAN`,
  `GL_LINE_LOOP`, ...), and draw it from `Scene`, like the route ribbon.
- **Simulation state:** `Bus`, `Route` and the rest live in `simulation` and hold only game state (distance, state,
  timer, counts). They never hold GL objects. `Application` owns a `Simulation`, calls `simulation.update(dt)` each
  frame, and forwards input to it. `Scene` and `Hud` keep a `const` reference to it and read it in `draw()`.
- **HUD items:** anchor them to the screen corners with `Camera::screenSize()` and draw them with the screen
  projection, so they sit at the screen corners on any aspect ratio, not at the safe area's corners.

## 9. Build notes

- `src/`, `external/glm`, `external/stb` and the FreeType headers must be on the include path, and `freetype.lib` must
  be linked. `stb_image.h` is included as `<stb_image.h>`, and `STB_IMAGE_IMPLEMENTATION` is defined only in
  `Texture.cpp`.
- Include `GL/glew.h` before `GLFW/glfw3.h`. In `.cpp` files, put the file's own header first; it pulls in GLEW before
  any GLFW include.
- Asset paths in code are relative (`assets/shaders/...`, `assets/images/...`, `assets/fonts/...`), so the process's
  working directory must be the project folder. In Visual Studio set Project Properties, Debugging, Working Directory
  to `$(ProjectDir)`.
