# libluna Framework

libluna is a portable C++ framework for game development that leverages modern
C++17 features.

## Supported Platforms

- Linux and Windows
- Nintendo 64 (via libdragon)
- Nintendo DS, Nintendo 3DS, and Nintendo Switch (via devkitPro)

## Rendering Backends

libluna supports multiple rendering backends depending on your platform and
requirements:

- **OpenGL**: Full 3D rendering support (desktop platforms)
- **SDL2 Renderer**: Hardware-accelerated 2D rendering (desktop platforms)
- **N64 OpenGL**: OpenGL 1.1-like rendering for Nintendo 64 (via libdragon)

## Platform Detection Macros

Use these preprocessor macros to write platform-specific code:

### Platform Detection

- `_WIN32` - Windows platform
- `__linux__` - Linux platform
- `__SWITCH__` - Nintendo Switch
- `__3DS__` - Nintendo 3DS
- `NDS` - Nintendo DS
- `N64` - Nintendo 64

### Build Configuration

- `NDEBUG` - Release build (non-debug)

### Feature Detection

- `LUNA_WINDOW_SDL2` - SDL2 windowing/input enabled
- `LUNA_WINDOW_GLFW` - GLFW windowing/input enabled
- `LUNA_WINDOW_EGL` - EGL windowing/input enabled
- `LUNA_RENDERER_SDL2` - SDL2 rendering enabled
- `LUNA_RENDERER_OPENGL` - OpenGL rendering enabled
- `LUNA_RENDERER_N64_GL` - N64 OpenGL rendering enabled
- `LUNA_AUDIO_SDL2` - SDL2 audio enabled
- `LUNA_IMGUI` - ImGui support enabled
- `LUNA_STD_THREAD` - std::thread available

SDL2, GLFW and EGL are mutually exclusive.

## Coding Style and Conventions

### File Organization

- Header files use `.hpp` extension, implementation files use `.cpp` extension
- Header files must start with `#pragma once` as include guard
- Always include `<libluna/config.h>` first in headers when configuration macros
  are needed
- Group includes in this order: config header, related header, standard library
  headers, third-party headers, libluna headers
- Sort includes alphabetically within each group

### Naming Conventions

#### Classes and Structures

- Use **PascalCase** for class and struct names: `Application`, `AudioManager`,
  `ColorRgb32`
- Use descriptive names that clearly indicate purpose

#### Functions and Methods

- Use **camelCase** for function and method names: `setTimeScale()`,
  `getCanvasBySdlWindowId()`
- Use verb-based names for methods: `init()`, `update()`, `create()`, `get()`,
  `set()`
- Boolean functions should use predicative names: `hasCanvas()`, `isDebuggerOpen()`

#### Variables and Member Variables

- Use **camelCase** for local variables and function parameters: `deltaTime`,
  `elapsedTime`
- **Private member variables** use `m` prefix with PascalCase: `mCanvases`,
  `mTimeScale`, `mImageLoader`
- **Static/global variables** use `g` prefix with PascalCase: `gSingletonApp`
- **Constants** use `k` prefix with PascalCase: `kPi`

#### Namespaces

- Use **PascalCase** for namespaces: `Luna`, `Luna::Audio`, `Luna::Filesystem`
- Prefer nested namespaces over long compound names
- Use `using namespace` declarations in implementation files, not headers

#### Template Parameters

- Use single **uppercase letters** for simple template parameters: `T`, `U`
- Use descriptive **PascalCase** names for complex template parameters

#### Macros and Defines

- Use **UPPER_SNAKE_CASE** for macros: `LUNA_WINDOW_SDL2`, `LUNA_THREADED_CANVAS`

### Code Formatting

#### Braces and Indentation

- Use **2 spaces** for indentation (no tabs)
- Opening braces on the same line for functions, classes, and control structures
- Always use braces for single-statement blocks in control structures

#### Spacing

- Space after control keywords: `if (`, `while (`, `for (`
- No space between function name and parentheses: `function()`
- Space around binary operators: `x + y`, `a == b`
- Space after commas: `function(a, b, c)`

#### Line Length

- Keep lines reasonably short (typically under 80-100 characters)
- Break long parameter lists across multiple lines with proper indentation

### Memory Management

#### Dynamic memory

- Attach asterisk (*) and ampersand (&) to the type: `Type* ptr`, `Type& ref`
- Prefer raw pointers over smart pointers (minimize dynamic memory allocation)
- Prefer static arrays or pools over dynamic memory allocation

#### Resource Management

- Use RAII principles for resource management
- Implement proper constructors/destructors with `= default` when appropriate
- Use move semantics with `std::move()` where beneficial

### Class Design

#### Access Control

- Order class members: public, protected, private
- Group related methods together
- Use `///@{` and `///@}` comments to group related getter/setter pairs

#### Constructors and Destructors

- Provide explicit constructors to prevent implicit conversions
- Use constructor delegation when appropriate
- Make destructors virtual for base classes: `virtual ~Application();`
- Use `= default` and `= delete` where appropriate

#### Const Correctness

- Mark methods `const` when they don't modify object state
- Use `const` references for parameters when objects won't be modified
- Use `const` for variables that shouldn't change
- Put `const` before the type: `const Type& param`

### Documentation

#### Doxygen Comments

- Use `/**` style comments for public API documentation
- Use `@brief` for brief descriptions
- Use `@param` for parameter descriptions
- Use `@return` for return value descriptions
- Document all public methods and classes
- Use `@ingroup` and `@defgroup` for logical groupings
- Include code examples in class documentation using `@code` blocks

#### Inline Comments

- Use `//` for inline comments
- Comment complex algorithms and non-obvious code
- Avoid obvious comments that just repeat the code

### Error Handling

- Avoid throwing exceptions
- Use logging functions: `logInfo()`, `logWarn()`, `logError()`
- Validate parameters and handle edge cases appropriately

### Type Safety

- Avoid implicit conversions (e.g. from `float` to `int` or `double` to `float`)
- Avoid implicit conversion from `size_t` to another type
- Use `static_cast` for explicit conversions
- Prefer strongly-typed enums over plain enums

### Variable Scope

- Avoid shadowing (i.e. a variable in an inner scope has the same name as a
  variable in outer scope)
- Declare variables as close to their first use as possible
- Prefer initialization over assignment

### Platform-Specific Code

- Use appropriate `#ifdef` guards for platform-specific code
- Keep platform-specific code isolated and well-documented
- Provide fallbacks or alternative implementations where possible

### Template Usage

- Use templates judiciously for type safety and performance
- Provide explicit template instantiations in implementation files when needed
- Use SFINAE and concepts (C++17 compatible) for template constraints

### Performance Considerations

- Pass large objects by const reference rather than by value
- Use move semantics for expensive-to-copy objects
- Consider memory layout for frequently-used classes
- Use `inline` for small, frequently-called functions
