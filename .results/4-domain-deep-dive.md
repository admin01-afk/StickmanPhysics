# Domain Deep Dive

## Core Domain: Physics Simulation

### Key Concepts
- **Rigid Body Dynamics:** Simulating the motion of the stickman as a collection of rigid bodies.
- **Collision Detection:** Ensuring the stickman interacts realistically with its environment.

### Implementation Details
- **Stickman Logic:**
  - File: `src/stickman.cpp`
  - Description: Contains the core logic for simulating the stickman’s movements and interactions.
- **Data Storage:**
  - File: `src/data.json`
  - Description: Stores the initial configuration and state of the simulation.

### Observed Patterns
- **Data-Driven Design:** Simulation parameters are stored in JSON files for flexibility.
- **Modular Code:** Separation of concerns between simulation logic and data storage.