# Interactive Physics Sandbox 📦

A lightweight, real-time 2D physics engine prototype built in C++ using SFML. This project demonstrates the bridge between user input and rigid-body dynamics, focusing on tactile feedback, stable collision resolution, and performance scalability.

Instead of a simple "snap-to-cursor" drag mechanic, this engine calculates object displacement during interaction to preserve momentum. This allows users to "flick" and throw objects across the simulation with realistic linear and angular kinetic transfer.

## ✨ Features
* **Angular Physics:** Objects now support rotation, angular velocity, and rotational inertia. Throwing a box with a "flick" will cause it to spin realistically.
* **OBB Collision & SAT:** Precise **Oriented Bounding Box** detection using the **Separating Axis Theorem (SAT)**. Collisions are accurate regardless of the object's rotation.
* **Grid-Based Broad-phase:** Optimized collision loop using spatial partitioning (100x100 pixel grid). This significantly boosts performance when handling a large number of objects.
* **Sleeping State:** Inactive objects (linear/angular velocity below a threshold) enter a "sleeping" state (grayed out) to save CPU cycles. They wake up automatically upon interaction or collision.
* **Visual Debugger:** Real-time visualization of contact points (red), collision normals (blue), AABBs (green), and the spatial grid.
* **Momentum-Based Interaction:** Objects track mouse velocity while grabbed, injecting kinetic energy back into the simulation upon release.

## 🚀 Quick Start

### Prerequisites
* **Compiler:** C++17 or higher
* **Library:** [SFML 2.5+](https://www.sfml-dev.org/)
* **Assets:** Ensure `hello-kitty.png` and `arial.ttf` are in the executable directory.

### Building & Running
```bash
# Clone the repository
git clone https://github.com/l4aaa/Interactive-Physics-Sandbox.git
cd Interactive-Physics-Sandbox

# Compile using the provided Makefile
make

# Run
./physics-app
```

## 🕹️ Controls
* **Left Mouse Click:** Grab and drag objects (maintains click offset).
* **Right Mouse Click:** Spawn a new rotating box at the cursor.
* **Mouse Wheel:** Adjust "Bounciness" ($0.0$ to $1.5$) in real-time.
* **[D] Key:** Toggle Visual Debugger (Grid, AABBs, Contact Points).
* **[V] Key:** Toggle velocity text overlays.
* **[L] Key:** Toggle the UI legend.
* **[R] Key:** Reset the simulation.

---

## 🛠️ Technical Summary

The simulation loop implements a discrete-time integration approach to handle motion and constraints:

### 1. Integration & Force Application
The engine uses semi-implicit Euler integration for both linear and angular motion:
$$V_{new} = (V_{old} + G \cdot \Delta t) \cdot \text{airResistance}$$
$$\omega_{new} = \omega_{old} \cdot \text{airResistance}$$
$$P_{new} = P_{old} + V_{new} \cdot \Delta t$$
$$\theta_{new} = \theta_{old} + \omega_{new} \cdot \Delta t$$

### 2. Collision Pipeline
The engine follows a two-stage pipeline:
1.  **Broad-phase (Grid):** Objects are mapped to a spatial grid. Only objects in the same or adjacent cells are tested for narrow-phase collisions ($O(N)$ expected complexity).
2.  **Narrow-phase (SAT):** The Separating Axis Theorem finds the axis of minimum penetration. If an overlap exists, it calculates the **Collision Normal**, **Penetration Depth**, and **Contact Point**.

### 3. Impulse Resolution
A robust impulse-based system resolves collisions by exchanging momentum between bodies at the contact point ($r$):
*   **Positional Correction:** Objects are moved apart by the penetration depth to resolve intersections immediately.
*   **Angular Impulse:** The change in velocity accounts for rotational inertia ($I$):
    $$j = \frac{-(1 + e) \cdot V_{rel} \cdot n}{\frac{1}{M_a} + \frac{1}{M_b} + \frac{(r_a \times n)^2}{I_a} + \frac{(r_b \times n)^2}{I_b}}$$
    This impulse ($j$) is then applied to both linear and angular velocities.

---

## 📄 License
Distributed under the MIT License. See `LICENSE` for more information.
