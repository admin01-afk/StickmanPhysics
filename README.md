# StickmanPhysics

A physics-based 2D stickman simulation using Box2D and Raylib. The project implements a ragdoll physics system with interactive controls and debug visualization.

## Features

- Physics-based ragdoll stickman with realistic joint constraints
- Interactive mouse dragging of body parts
- Camera movement and zoom
- Debug visualization mode(f3)
- Event-driven architecture for clean component communication

## Dependencies

- Box2D (v2.4.x) - Physics engine
- Raylib - Graphics and input handling
- CMake - Build system

## Building

1. Clone the repository
```bash
git clone https://github.com/admin01-afk/StickmanPhysics.git
cd StickmanPhysics
```

2. Create and navigate to build directory
```bash
mkdir build
cd build
```

3. Build the project
```bash
cmake ..
make
```

Or use the provided VS Code task:
- Press `Ctrl/Cmd + Shift + B` or run the "build stickman" task

## Controls

- **WASD** - Move camera
- **Q/E** - Zoom camera in/out
- **F3** - Toggle debug visualization
- **P** - Pause/unpause physics simulation
- **Left Mouse Button** - Drag stickman body parts
- **ESC** - Exit application

## Debug Features

When debug mode is enabled (F3):
- Grid overlay showing world space
- Mouse position in both screen and world coordinates
- Physics body outlines and joint connections

## Architecture

The project is structured around several key components:

- Input processing with event emission
- Box2D physics simulation
- Raylib-based rendering
- Event bus for cross-component communication
- Centralized shared data management

## Future Development

- Implementation of a dual stickman system:
  - Animation-driven skeleton for pose control
  - Physics-based ragdoll that follows animations
  - Realistic physical responses to impacts and forces
  - Weighted force application for balance control

## License

[Insert License Information]

## Contributing

[Insert Contribution Guidelines]