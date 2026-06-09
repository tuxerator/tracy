# Project Rules

## Project Structure

### Folders
- `include/`: All `.h` files, mirrored tree under `include/`
- `src/`: All `.cpp` files, mirrored tree under `src/`
- `3rd_party/`: Vendored headers-only (`glm/`)
- `Exercises/`: Exercise PDFs
- `resources/`: Reference PDFs

### Module layout under `include/` and `src/`
```
app/        GUI, viewer, scene presets
core/       Math-adjacent rendering basics (ray, color, constants, random)
scene/      Camera, scene container, hit record
geometry/   Primitives (sphere, triangle, plane) + abstract base
shading/    Materials, textures, lights
render/     Renderer + integrators (direct, whitted, path tracing)
accel/      Acceleration structures (bbox, BVH, octree)
io/         Image output, OBJ loading
```

## Build

### Commands
```bash
cmake --preset debug                           # configure (build/debug/)
cmake --build --preset debug                   # build
cmake --preset release                         # configure (build/release/)
cmake --build --preset release                 # release build
```

### Lauch (post-build)
```bash
./build/debug/tracy
```

### Build system details
- CMake 3.30+, generator: Ninja, C++20 (`CMAKE_CXX_STANDARD 20`)
- Dependencies: Qt6 (Widgets), Threads, GLM (vendored)
- Compile def: `PROJECT_SOURCE_DIR` for asset paths
- No tests, no CTest, no test framework configured
- No CI, no linters, no formatters configured
- `.vscode/tasks.json`: configure + build tasks only

### If you add new source files
Update both `SOURCES` and `HEADERS` lists in `CMakeLists.txt`.

## Code Style

### Include guards
`#pragma once` — every header.

### Indentation
4 spaces. No tabs. (Note: `app/` module files historically use 2-space; new code must use 4-space to match the majority.)

### Naming conventions

| Element | Convention | Example |
|---------|-----------|---------|
| Files/dirs | `snake_case` | `cpu_viewer.h`, `direct_lighting_integrator.cpp` |
| Classes & structs | PascalCase | `HitRecord`, `Sphere`, `DirectLightingIntegrator` |
| Enums | `enum class` PascalCase | `MaterialType { Lambert, Mirror, Dielectric, Emissive, Unknown }` |
| Methods & functions | camelCase | `generateRay()`, `intersect()`, `setFaceNormal()` |
| Member variables | `m_` prefix | `m_center`, `m_imageWidth`, `m_material` |
| Parameters | camelCase | `sampleX`, `rec`, `outwardNormal` |
| Constants | `k` prefix | `constants::kPi`, `constants::kEpsilon` |
| Type aliases | PascalCase | `using Color = glm::dvec3;` |

### Include order

**Headers** (`*.h`):
```
#pragma once

<standard/library includes>

"project/module.h"
```

**Sources** (`*.cpp`):
```
"corresponding/module.h"       // 1. Corresponding header first

<standard library includes>    // 2. Standard library

<Qt includes>                  // 3. Qt headers — when present

"project/module.h"             // 4. Other project headers
```

Groups separated by blank lines. Within groups, roughly alphabetical.

### Namespace usage
Minimal. Keep code in global namespace. Use `namespace constants { ... }` for numeric constants. Use anonymous `namespace { ... }` in `.cpp` for file-local helpers. No `using namespace std;` or `using namespace glm;`.

### Comments
`//` exclusively. Block comments use a `//` prefix on each line. Avoid `/* */`.

### Class structure
```
class ClassName : public Base {
public:
    ClassName() = delete;
    explicit ClassName(Params...);
    ~ClassName() override;

    Type method();
    Type getter() const;

private:
    Type m_member;
};
```

- Access specifiers (`public:`, `private:`, `protected:`) at same indent as `class`
- `explicit` on single-argument constructors
- `override` on all overrides
- Virtual destructors: `virtual ~ClassName() = default;`
- Pure virtual: `virtual Type method() const = 0;`

### Memory & pointers
- `std::shared_ptr<T>` + `std::make_shared<T>()` for most ownership
- `std::dynamic_pointer_cast<T>()` for downcasting
- Raw pointers only for Qt widget ownership (parent-child model)
- `nullptr` (not `NULL` or `0`)

### Const correctness
- Getters marked `const`
- Non-trivial parameters: `const Type&`
- Return-by-const-ref where appropriate: `const std::shared_ptr<Material>& material() const`

### Error handling
- Return-value-based signaling (`bool`, null pointer checks)
- Early return for guards
- Qt `qWarning()` for diagnostics
- No exceptions, no `assert()` (in current code)

### Qt patterns
- Qt6 (Widgets)
- Widget hierarchy: `QMainWindow`, `QWidget`
- Signal/slot connections via lambdas
- `QOverload<int>::of(&QSpinBox::valueChanged)` for overloaded signals
- `Q_UNUSED(var)` for unused parameters

### Initialization
- Constructor member initializer lists
- In-class default member initializers: `int m_width = 0;`
- `std::move()` for `shared_ptr` in constructors: `m_material(std::move(material))`

### Struct vs class
- `struct` for POD-like data holders (`HitRecord`, `MaterialSample`, `SceneSetup`)
- `class` for everything with methods/encapsulation

# Caveman

Respond terse like smart caveman. All technical substance stay. Only fluff die.

## Persistence

ACTIVE EVERY RESPONSE AND THINKING. No revert after many turns. No filler drift. Still active if unsure. Off only: "stop caveman" / "normal mode".

Default: **full**. Switch: `/caveman lite|full|ultra`.

## Rules

- Drop: articles (a/an/the), filler (just/really/basically), pleasantries, hedging
- Fragments OK. Short synonyms. Technical terms exact. Code unchanged.
- Pattern: [thing] [action] [reason]. [next step].
- Not: "Sure! I'd be happy to help you with that."
- Yes: "Bug in auth middleware. Fix:"

Switch level: /caveman lite|full|ultra|wenyan
Stop: "stop caveman" or "normal mode"

Auto-Clarity: drop caveman for security warnings, irreversible actions, user confused. Resume after.

Boundaries: code/commits/PRs written normal.
