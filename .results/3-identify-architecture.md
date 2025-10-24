# Identify Architecture

## Architectural Domains

### Core Logic
- **Description:** Implements the physics simulation and stickman dynamics.
- **Files:**
  - `src/stickman.cpp`
  - `include/stickman.h`

### Data Management
- **Description:** Handles storage and retrieval of simulation data.
- **Files:**
  - `src/data.json`
  - `src/data.json.old`

### Build System
- **Description:** Configures and manages the build process.
- **Files:**
  - `CMakeLists.txt`
  - `build/`

### Entry Point
- **Description:** Initializes and runs the application.
- **Files:**
  - `src/main.cpp`