# Interactive Physics Sandbox 📦

A lightweight, real-time 2D physics engine prototype built in C++ using SFML. This project demonstrates the bridge between user input and rigid-body dynamics, focusing on tactile feedback and stable collision resolution.

Instead of a simple "snap-to-cursor" drag mechanic, this engine calculates object displacement during interaction to preserve momentum. This allows users to "flick" and throw objects across the simulation with realistic kinetic transfer.

## 🎥 Demo
![ezgif com-crop](https://github.com/user-attachments/assets/ffac3789-16c5-4063-beb8-0ad24de3a382)



## ✨ Features
* **Momentum-Based Interaction:** Objects track mouse velocity while grabbed, injecting kinetic energy back into the simulation upon release.
* **Impulse Resolution:** Realistic bouncing and energy dissipation using configurable coefficients of restitution.
* **AABB Collision & Response:** Efficient Axis-Aligned Bounding Box detection with immediate positional correction to prevent object "sinking."
* **Dynamic Environment:** Includes gravity, air resistance, and floor friction (drag) to simulate a natural environment.
* **Real-time Debugging:** Toggleable velocity vectors and a dynamic legend to monitor simulation parameters.

## 🚀 Quick Start

### Prerequisites
* **Compiler:** C++17 or higher
* **Library:** [SFML 2.5+](https://www.sfml-dev.org/)
* **Assets:** Ensure `hello-kitty.png` and `arial.ttf` are in the executable directory.

### Building & Running
```bash
# Clone the repository
git clone [https://github.com/l4aaa/Interactive-Physics-Sandbox.git](https://github.com/l4aaa/Interactive-Physics-Sandbox.git)
cd Interactive-Physics-Sandbox

# Compile (example using g++)
g++ -c main.cpp
g++ main.o -o physics-app -lsfml-graphics -lsfml-window -lsfml-system

# Run
./physics-app
```

## 🕹️ Controls
* **Left Mouse Click:** Grab and drag objects.
* **Right Mouse Click:** Spawn a new physics object at the cursor.
* **Mouse Wheel:** Adjust "Bounciness" ($0.0$ to $1.5$) in real-time.
* **[V] Key:** Toggle velocity text overlays.
* **[L] Key:** Toggle the UI legend.
* **[R] Key:** Reset the simulation.

---

## 🛠️ Technical Summary

The simulation loop implements a discrete-time integration approach to handle motion and constraints:

### 1. Integration & Force Application
The engine uses a semi-implicit Euler integration. Gravity is applied to the vertical velocity, while air resistance acts as a constant damping factor:
$$V_{new} = (V_{old} + G \cdot \Delta t) \cdot \text{airResistance}$$
$$P_{new} = P_{old} + V_{new} \cdot \Delta t$$

### 2. Interaction Physics
When an object is grabbed, its velocity is not zeroed out. Instead, it is derived from the displacement between the current mouse position and the position in the previous frame:
$$V = \frac{P_{current} - P_{previous}}{\Delta t}$$
This ensures that when the mouse button is released, the object maintains the "throw" velocity.

### 3. Collision Handling
The engine utilizes **AABB (Axis-Aligned Bounding Box)** logic. When two objects overlap, the engine calculates the penetration depth on both axes and resolves the collision:
* **Positional Correction:** Objects are moved apart by half the overlap distance to resolve the intersection immediately.
* **Impulse Exchange:** For simplicity and stability, velocities are swapped on the axis of collision and scaled by the `bounceDamping` coefficient to simulate energy loss.

---

## 📄 License
Distributed under the MIT License. See `LICENSE` for more information.
