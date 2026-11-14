# Architecture

How the code is organized and how a frame gets from the simulation to the screen. This document covers what exists
today: the window, the main loop, the coordinate system and the background image. Route, stations, bus and HUD will
plug into the same structure (see [Extending](#extending)).

Rules and limits live in `AGENTS.md`; naming and formatting live in `style-guide.md`. This file explains the design.

## 1. Layers

Three layers, one namespace each. Dependencies point one way only.

```
app  ──►  graphics  ──►  simulation
```

| Layer | Namespace | Responsibility |
|---|---|---|
| `simulation` | `nesmart::simulation` | Pure game logic and the world's coordinate constants. No OpenGL, no GLFW. |
| `graphics` | `nesmart::graphics` | Everything OpenGL: shaders, textures, meshes, camera, drawing. |
| `app` | `nesmart::app` | Window, main loop, frame limiter. Owns the other layers and wires them together. |

`simulation` never includes `graphics/` or `app/`; `graphics` never includes `app/`.

## 2. Classes

| Class | Layer | Purpose |
|---|---|---|
| `World.hpp` (constants) | simulation | The coordinate contract: world size, safe area size, safe area center. No behaviour. |
| `Shader` | graphics | Compiles and links a vertex and fragment shader; sets uniforms by name. |
| `Texture` | graphics | An image on the GPU. Loads a file with `stb_image`, always RGBA, flipped on load. |
| `Mesh` | graphics | A shape on the GPU: one VAO and one VBO, uploaded once. Knows its own primitive mode. |
| `Camera` | graphics | Computes the world-to-screen projection matrix from the screen size. Pure math. |
| `Renderer` | graphics | The only class that draws. Owns the shader, camera, meshes and textures and decides the draw order. |
| `Window` | app | Full-screen GLFW window, GL context, GLEW. Exposes its pixel size. |
| `FrameLimiter` | app | Holds the loop to 75 FPS and provides `dt`. |
| `Application` | app | Main loop and input callbacks. Owns `Window`, `FrameLimiter` and `Renderer`. |

GL resources (`Shader`, `Texture`, `Mesh`) are RAII: they release their GL objects in the destructor and cannot be
copied. `Camera` and `World.hpp` make no GL calls.

## 3. Ownership and startup

```
main()
 └─ Application
     ├─ Window            creates the window, GL context, GLEW
     ├─ FrameLimiter
     └─ Renderer          needs the GL context, so it is declared after Window
         ├─ Shader        assets/shaders/sprite.vert, sprite.frag
         ├─ Camera        projection from screen size + World.hpp
         ├─ Mesh          unit quad
         └─ Texture       assets/images/background.png
```

- Members are constructed in declaration order and destroyed in reverse. Because `m_window` is declared before
  `m_renderer`, the GL context exists while the renderer's GL objects are created and still exists while they are
  destroyed. No manual cleanup is needed.
- Any constructor that cannot finish (missing shader file, compile error, missing image) throws
  `std::runtime_error`. It propagates to `main`, which prints the message and returns -1.

## 4. A frame

`Application::run()`:

```
while (!window.shouldClose())
    dt = limiter.delta()
    renderer.draw()
    window.swapBuffers()
    window.pollEvents()      // key callback: Escape closes the window
    limiter.tick()           // sleeps to hold 75 FPS (VSync is off)
```

`Renderer::draw()`:

1. Clear the screen.
2. `shader.use()`, then set `uProjection` (from `Camera`) and `uTexture` (texture unit 0).
3. `drawBackground()`: set `uModel` to a scale of the world size, bind the background texture, draw the unit quad.

On the GPU the vertex shader computes `uProjection * uModel * vec4(aPos, 0, 1)`. The fragment shader samples the
texture and keeps its alpha; blending is enabled so transparent areas of later sprites work.

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
clip space.

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

## 6. Shaders

`assets/shaders/sprite.vert` and `sprite.frag` draw any textured quad.

| Item | Kind | Meaning |
|---|---|---|
| `aPos` (location 0) | vertex input, vec2 | position in mesh space |
| `aUV` (location 1) | vertex input, vec2 | texture coordinate |
| `uProjection` | uniform, mat4 | world to clip space (`Camera`) |
| `uModel` | uniform, mat4 | mesh space to world space (this sprite) |
| `uTexture` | uniform, sampler2D | texture unit the image is bound to |

These match `Mesh` built with the attribute layout `{2, 2}`.

## 7. Assets and the working directory

Asset paths in code are relative (`assets/shaders/...`, `assets/images/...`), so the process's working directory must be
the project folder. In Visual Studio set Project Properties, Debugging, Working Directory to `$(ProjectDir)`.

## 8. Extending

- **A new sprite (bus, icon):** load a `Texture`, then in `Renderer` build a `uModel` (translate, optionally rotate,
  then scale) and draw the same unit-quad `Mesh`. No new mesh or shader is needed.
- **A new shape (station circle, route line):** build a vertex array, create a `Mesh` with the right primitive mode
  (`GL_TRIANGLE_FAN`, `GL_LINE_LOOP`, ...), and draw it from `Renderer`.
- **Simulation state:** `Bus`, `Route` and the rest live in `simulation` and hold only game state (distance, state,
  timer, counts). They never hold GL objects. `Application` owns a `Simulation`, calls `simulation.update(dt)` each
  frame, and passes a `const` view of it to `Renderer::draw`, which reads positions and builds each model matrix.
- **HUD:** draw last, anchored to the camera's visible bounds (to be added to `Camera`) so items sit at the screen
  corners on any aspect ratio, not at the safe area's corners.

## 9. Build notes

- `src/`, `external/glm` and `external/stb` must be on the include path. `stb_image.h` is included as
  `<stb_image.h>`, and `STB_IMAGE_IMPLEMENTATION` is defined only in `Texture.cpp`.
- Include `GL/glew.h` before `GLFW/glfw3.h`. In `.cpp` files, put the file's own header first; it pulls in GLEW before
  any GLFW include.
