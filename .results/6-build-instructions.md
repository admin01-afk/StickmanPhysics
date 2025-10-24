# Build Instructions

## Overview
This document provides instructions for building and running the StickmanPhysics project.

## Prerequisites
- **C++ Compiler:** Ensure a modern C++ compiler (e.g., GCC) is installed.
- **CMake:** Install CMake for build configuration.

## Steps
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

## Notes
- Ensure the `data.json` file is present in the `src/` directory for the simulation to run correctly.
- Use the `data.json.old` file as a backup if needed.