# Interactive Physics Sandbox 📦
A lightweight, real-time 2D physics engine prototype built in C++ using SFML.

This project focuses on the bridge between user input and rigid-body dynamics. Instead of a simple "snap-to-cursor" drag mechanic, this engine calculates the displacement of objects during interaction to preserve momentum, allowing you to "flick" and throw objects across the simulation.

## ✨ Features
* **Momentum-Based Interaction:** Objects track mouse velocity while grabbed, injecting kinetic energy back into the simulation upon release.
* **Impulse Resolution:** Realistic bouncing and energy dissipation using coefficients of restitution (bounciness).
* **Positional Correction:** Prevents objects from "sinking" into floors or walls by resolving overlaps immediately.
* **AABB Collision:** Efficient Axis-Aligned Bounding Box detection for stable performance.



## 🚀 Quick Start

### Prerequisites
* **Compiler:** C++17 or higher
* **Library:** [SFML 2.5+](https://www.sfml-dev.org/)

### Building
```bash
# Clone the repository
git clone [https://github.com/l4aaa/Interactive-Physics-Sandbox.git](https://github.com/l4aaa/Interactive-Physics-Sandbox.git)

# Navigate to directory
cd Interactive-Physics-Sandbox

# Compile (example using g++)
g++ -c main.cpp
g++ main.o -o physics-app -lsfml-graphics -lsfml-window -lsfml-system

# Run
./physics-app
```

## 🕹️ Controls
* **Left Mouse Click:** Grab an object.
* **Mouse Move:** Drag to build momentum.
* **Release Click:** Throw the object.
* **R Key:** Reset the simulation.

## 🛠️ The Physics Logic
The simulation follows a standard physics step:
1. **Integration:** Gravity is applied to velocity, and velocity is applied to position ($P = P + V \cdot \Delta t$).
2. **Input Handling:** If grabbed, velocity is derived from the mouse delta: $V = (P_{current} - P_{previous}) / \Delta t$.
3. **Collision Detection:** Checks for intersections between the box and environment boundaries.
4. **Resolution:** If a collision occurs, the velocity is reflected based on a "bounciness" factor and the object is snapped to the surface.



---

## 📄 License
Distributed under the MIT License. See `LICENSE` for more information.
