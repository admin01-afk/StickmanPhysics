# Copilot Instructions

## Overview
This document provides structured guidance for AI tools to generate features aligned with the StickmanPhysics project’s architecture and style. It is based on observed patterns and conventions in the codebase.

---

## 1. Tech Stack
- **Programming Language(s):** C++
- **Primary Framework:** None explicitly mentioned
- **Build System:** CMake
- **Data Format:** JSON

---

## 2. File Categories

### Source Files
- `src/main.cpp`: Entry point for the application
- `src/stickman.cpp`: Logic for stickman physics

### Header Files
- `include/stickman.h`: Header file for stickman logic

### Build Files
- `build/`: Contains build artifacts and intermediate files

### Data Files
- `src/data.json`: Stores simulation data
- `src/data.json.old`: Backup of simulation data

### Configuration Files
- `CMakeLists.txt`: Build configuration

### Documentation
- `README.md`: Project overview and instructions

---

## 3. Architecture

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

---

## 4. Domain Deep Dive

### Core Domain: Physics Simulation

#### Key Concepts
- **Rigid Body Dynamics:** Simulating the motion of the stickman as a collection of rigid bodies.
- **Collision Detection:** Ensuring the stickman interacts realistically with its environment.

#### Implementation Details
- **Stickman Logic:**
  - File: `src/stickman.cpp`
  - Description: Contains the core logic for simulating the stickman’s movements and interactions.
- **Data Storage:**
  - File: `src/data.json`
  - Description: Stores the initial configuration and state of the simulation.

#### Observed Patterns
- **Data-Driven Design:** Simulation parameters are stored in JSON files for flexibility.
- **Modular Code:** Separation of concerns between simulation logic and data storage.

---

## 5. Style Guide

### General Conventions
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

---

## 6. Build Instructions

### Prerequisites
- **C++ Compiler:** Ensure a modern C++ compiler (e.g., GCC) is installed.
- **CMake:** Install CMake for build configuration.

### Steps
1. **Navigate to the Build Directory:**
   ```bash
   cd build
   ```
2. **Run CMake:**
   ```bash
   cmake ..
   ```
3. **Build the Project:**
   ```bash
   make
   ```
4. **Run the Application:**
   ```bash
   ./stickman
   ```

### Notes
- Ensure the `data.json` file is present in the `src/` directory for the simulation to run correctly.
- Use the `data.json.old` file as a backup if needed.