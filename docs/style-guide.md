# NeSmart Style Guide

Coding style and naming conventions for the **NeSmart** project. Consistency matters more than any single rule: when in doubt, match the surrounding code.

Formatting is handled by `.clang-format` at the repo root (Visual Studio: Ctrl+K, Ctrl+D). This guide covers what the formatter cannot.

---

## 1. Quick Reference

| Element | Convention | Example |
| :--- | :--- | :--- |
| Directories | `snake_case` | `src/rendering/` |
| Files | `PascalCase`, matches the class | `Bus.hpp`, `Bus.cpp` |
| Namespaces | `snake_case` | `nesmart::rendering` |
| Classes, structs, enums | `PascalCase` | `class BusStation;` |
| Enum values | `PascalCase` | `BusState::Driving` |
| Methods and functions | `camelCase` | `void updatePosition();` |
| Local variables, parameters | `camelCase` | `double elapsedTime` |
| Member variables | `m_camelCase` | `float m_speed;` |
| Static variables | `s_camelCase` | `static int s_count;` |
| Constants | `UPPER_SNAKE_CASE` | `constexpr int MAX_PASSENGERS = 50;` |
| Macros | `UPPER_SNAKE_CASE` (avoid macros) | `GLEW_STATIC` |

---

## 2. Files and Directories

- Code lives in `src/<layer>/Name.hpp` and `src/<layer>/Name.cpp`, side by side.
- Layers: `app`, `rendering`, `simulation`. The entry point is `src/main.cpp`.
- `src/` is on the include path. Include with the layer prefix: `#include "rendering/Shader.hpp"`. Never use `../` includes.
- One primary class per file pair; the file name matches the class name.
- Assets live in `assets/<asset_type>/` (`images`, `shaders`, `fonts`).
- Use `#pragma once` in every header.

### Layer rule

Dependency direction: `app` -> `rendering` -> `simulation`.

- `simulation/` includes nothing from `rendering/` or `app/`, and no OpenGL or GLFW headers.
- `rendering/` does not include anything from `app/`.
- Only `app/` talks to GLFW. Only `rendering/` makes OpenGL calls (documented exception: `glViewport` in `Window`).

Quick check: `grep -rE "rendering/|app/" src/simulation`

---

## 3. Types and Namespaces

- Namespaces mirror the layer: `nesmart::app`, `nesmart::rendering`, `nesmart::simulation`.
- Use `enum class`, never plain `enum`:
  ```cpp
  enum class BusState
  {
      AtStation,
      Driving,
      Broken
  };
  ```
- Use `struct` for plain data with no invariants, `class` otherwise.

---

## 4. Variables and Constants

- Prefer descriptive names (`elapsedTime`, not `t`).
- Member variables use the `m_` prefix; statics use `s_`.
- Pointer and reference symbols attach to the type: `Player* player`, `const std::string& name`.
- Use `constexpr` for constants. Avoid macros unless a library requires one.
- Use `nullptr`, never `NULL` or `0`, for pointers.
- Declare variables where they are first used, and initialize them.

---

## 5. Functions and Classes

- Names are `camelCase`, typically verbs: `updatePhysics()`, `draw()`.
- **Accessors have no `get` prefix:** `delta()`, `width()`, `name()`. Setters use `set`: `setTargetFps()`.
- **Booleans read as questions:** `isAlive()`, `hasTicket()`, `shouldClose()`.
- Mark methods `const` if they do not modify the object.
- Mark single-argument constructors `explicit`.
- Mark overrides `override`; never repeat `virtual` on them.
- Delete copy operations for classes that own a resource (windows, GL objects).
- Keep one-line functions (trivial getters, setters) in the class body. Put anything with real logic in the `.cpp`.
- Prefer RAII and exceptions over manual cleanup and error codes. Constructors that cannot complete throw `std::runtime_error`.
- Prefer `std::unique_ptr` for ownership. No raw `new` / `delete`.

---

## 6. Includes

In a `.cpp`, order includes in these groups, separated by blank lines:

1. The corresponding `.hpp`
2. C standard library (`<cmath>`)
3. C++ standard library (`<vector>`, `<string>`)
4. Third-party libraries (`<GL/glew.h>` before `<GLFW/glfw3.h>`)
5. Other NeSmart headers

```cpp
#include "app/Window.hpp"

#include <cmath>

#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "simulation/Bus.hpp"
```

Rules:

- `GL/glew.h` must always come before `GLFW/glfw3.h`.
- Headers must not include GL or GLFW headers. Forward-declare instead (`struct GLFWwindow;`), so the glew-before-glfw rule can never be broken from a header.
- Headers include only what their declarations need. Heavy includes go in the `.cpp`.
- Every header must compile on its own.

---

## 7. Formatting

- Allman braces (opening brace on its own line). Always use braces, even for one-line bodies.
  ```cpp
  if (isAlive())
  {
      updatePhysics();
  }
  ```
- 4 spaces, no tabs. Soft limit of 120 columns.
- Enforced by `.clang-format`. Do not hand-format against it.

---

## 8. Comments

- Comment *why*, not *what*.
- Use `//` comments. Document non-obvious behavior on the declaration in the header (units, ownership, thread assumptions, "only meaningful as a difference").
- Use English for code and comments. User-facing strings (error messages, UI text) may be Serbian.
