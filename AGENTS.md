# AGENTS.md: NeSmart

NeSmart is a small university assignment: a 2D OpenGL simulation of a bus driving around a closed route of stations. The bus moves at constant speed along the route, stops at each station for 10 seconds, and passengers and an inspector are handled via mouse clicks and the keyboard. **Keep it small. Do not over-engineer.**

## 1. Architecture Limits
- **No ECS.** Use plain C++ classes with `std::vector` / `std::array`.
- **No abstract factories, no deep inheritance.** Concrete classes only (`Bus`, `Station`, `Application`, etc.).
- **No physics engines.** The route is curved, so sample it into a polyline at startup and move the bus at constant speed along it (by distance travelled). No per-frame curve evaluation.
- **Simple state machine only.** Example: `Driving` / `StoppedAtStation` (10 s timer). Doors are open only while stopped.
- **No new dependencies without asking first.** Allowed: GLFW, GLEW, OpenGL, FreeType, and `stb_image` for textures and the cursor image.
- **No new layers or directory changes** without asking first.

## 2. Layers

Three layers, one namespace each: `nesmart::app`, `nesmart::graphics`, `nesmart::simulation`.

- `simulation`: pure logic with no OpenGL or GLFW includes. Route, stations, bus state machine, passengers, inspector, fines. Advanced with `dt` and plain input calls from the app layer.
- `graphics`: OpenGL only. Shaders, textures, meshes, text, image loading, and drawing from a read-only view of the simulation state.
- `app`: window and main loop. `Window` (GLFW/GLEW init, full screen, context), `Application` (main loop, input callbacks, cursor, Escape), `Timer`, `FrameLimiter` (75 FPS). Owns the simulation and the renderer and wires input to the simulation.

Dependency direction: `app` -> `graphics` -> `simulation`. `simulation` includes nothing from the other two layers.

## 3. Technical Requirements
- **OpenGL 3.3 core profile only:** shaders, VAOs, VBOs, uniforms. Never use legacy fixed-pipeline calls (`glBegin`, `glEnd`, `glMatrixMode`, etc.).
- **Window:** full-screen at the monitor's native video mode, exits immediately on Escape.
- **Frame rate:** strict 75 FPS. VSync is off (`glfwSwapInterval(0)`) and `FrameLimiter::tick()` does the capping at the end of each frame. Use its delta time for `dt`.
- **Language:** C++20, MSVC.
- **GL resources** (VAO, VBO, shader program, texture) are wrapped in small classes that release them in the destructor (RAII). No raw `new`/`delete`; prefer `std::vector`, `std::array`, `std::unique_ptr`.
- **Errors:** constructors that cannot complete throw `std::runtime_error`; `main` catches, prints the message, and returns -1. No manual cleanup paths.

## 4. File Placement

Header and source for a class live side by side in the layer directory.

- Code: `src/<layer>/Name.hpp` and `src/<layer>/Name.cpp`
- Layers: `app`, `graphics`, `simulation`
- Entry point: `src/main.cpp`
- Assets: `assets/<asset_type>/` (`assets/images`, `assets/shaders`, `assets/fonts`)
- Docs: `docs/` at the repo root (`Architecture.md`, `StyleGuide.md`)
- File names are `PascalCase`, matching the class name (`Bus.hpp`, `Bus.cpp`).
- `src/` is on the include path. Include with the layer prefix: `#include "graphics/Shader.hpp"`. Never use `../` relative includes.
- Use `#pragma once` in headers.
- Layer rule: files in `simulation/` must not include anything prefixed `graphics/` or `app/`; files in `graphics/` must not include `app/`.

## 5. Naming and Formatting
Full rules are in `docs/StyleGuide.md`. Summary:
- Classes/structs/enums: `PascalCase`; always `enum class`
- Functions and local variables: `camelCase`
- Member variables: `m_camelCase`; statics: `s_camelCase`
- Constants: `UPPER_SNAKE_CASE` (`constexpr`, no macros)
- Namespaces: `snake_case` per layer: `nesmart::app`, `nesmart::rendering`, `nesmart::simulation`
- Accessors have no `get` prefix (`delta()`); booleans read as questions (`isAlive()`)
- Allman braces, always braces, 4 spaces, 120 columns. `.clang-format` in the repo root is the authority.
- `.cpp` include order: own header, C std, C++ std, third-party (`GL/glew.h` before `GLFW/glfw3.h`), project headers.

## 6. Working Rules
- **Do not generate or modify CMake files.** The project uses a native Visual Studio `.vcxproj` managed by the user.
- **Do not try to build or run the project.** The user compiles in Visual Studio. Verify your work by re-reading includes, signatures, and namespaces.
- **List every new file you create** at the end of your response so the user can add it to the `.vcxproj` and `.vcxproj.filters`.
- **Make minimal, targeted changes.** Do not refactor or reformat unrelated code, and do not fix clang-tidy warnings in code you are not otherwise changing.
- When implementing a **new class**, provide the complete `.hpp` and `.cpp`. When **modifying** existing code, edit the files directly and do not reprint unchanged files.
- If a requirement is ambiguous or conflicts with these rules, ask before proceeding.
- Assume GLFW, GLEW, and OpenGL are already linked in the MSVC environment.
