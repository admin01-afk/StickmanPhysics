# Styleguide Generation

## Coding Conventions

### General
- Use consistent indentation (4 spaces).
- Follow C++ naming conventions (e.g., `CamelCase` for classes, `snake_case` for variables).

### File Organization
- Place header files in the `include/` directory.
- Place implementation files in the `src/` directory.

### Comments
- Use `//` for single-line comments.
- Use `/* */` for multi-line comments.

### Specific Guidelines
- **Header Files:**
  - Use include guards to prevent multiple inclusions.
  - Example:
    ```cpp
    #ifndef STICKMAN_H
    #define STICKMAN_H
    // ...
    #endif
    ```
- **Data Files:**
  - Store simulation data in JSON format.
  - Maintain a backup file for critical data.

### Build System
- Use CMake for build configuration.
- Keep build artifacts in the `build/` directory.