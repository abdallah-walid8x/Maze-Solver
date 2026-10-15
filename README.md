# 🤖 Maze-Solver

An **Arduino-powered autonomous embedded vehicle system** designed to accurately track lines, autonomously navigate complex grid networks using exploration algorithms, and log a digital representation of the track for shortest path optimizations.

---

## 🚀 Key Features

* **Advanced Line Following:** Implements smooth differential speed adjustments utilizing a 5-sensor array layout to handle subtle drift corrections and track adjustments on the fly.
* **Autonomous Junction Navigation:** Dynamically detects intersections, dead ends, cross-junctions, and T-junctions, bypassing momentary momentum and blind spots.
* **Algorithmic Path Exploration:** Utilizes the programmatic **Left-Hand-On-Wall rule** to consistently map out and resolve grid-style layouts.
* **Internal Memory Representation:** Tracks every turn logic choice (`'L'`, `'S'`, `'R'`, `'B'`) inside an array buffer (`pathMemory`) to map the layout for future optimization calculations.

---

## 🔌 Hardware Configuration & Pin Layout

The robot architecture integrates an L298N (or similar) H-Bridge Dual DC Motor Driver paired alongside a 5-channel Infrared (IR) reflective tracking array sensor.

### 🏎️ Motor Control Pins

| Component Pin | Arduino Pin | Description |
| :--- | :---: | :--- |
| **ENA** | `13` | PWM Speed Control - Left Motor |
| **IN1** | `12` | Direction Pin - Left Motor |
| **IN2** | `11` | Direction Pin - Left Motor |
| **IN3** | `10` | Direction Pin - Right Motor |
| **IN4** | `9` | Direction Pin - Right Motor |
| **ENB** | `8` | PWM Speed Control - Right Motor |

### 🛰️ 5-Sensor Tracking Array Layout
> **Note:** The array is configured for an active-low environment (`0` = Black Line, `1` = White Floor).

* **Far Left Sensor:** Pin `31`
* **Left Sensor:** Pin `33`
* **Center Sensor:** Pin `35`
* **Right Sensor:** Pin `37`
* **Far Right Sensor:** Pin `39`

---

## 🛠️ Software & Control Logic Flow

1. **Junction Detection (`handleMazeJunction`):** When the outermost sensors trigger, the vehicle dampens its momentum, steps forward briefly to position its pivot axis over the line intersection, registers available paths, and executes turning clearance logic to prevent sensor lock freezes.
2. **Dead-End Resolution (`handleDeadEnd`):** If all sensors lose tracking (all-white floor state), the system registers a backtrack event (`'B'`) and executes a controlled 180-degree pivot until the center sensor locks back onto the tracking line.
3. **Internal Path Memory:** All junction decisions are pushed sequentially to an internal buffer array:
   * `L` → Left Turn
   * `S` → Straight Execution
   * `R` → Right Turn
   * `B` → Backtrack / U-Turn

---

## 💻 Technical Configurations

* **Cruise Speed:** `70` (PWM Duty Cycle for optimal straight line traction)
* **Sharp Turn Speed:** `140` (PWM Duty Cycle for powerful axis pivoting)
* **Turn Clearance Window:** Dynamic tracking overrides (`delay(150)`) are embedded to ensure physical sensor hardware rotates completely past the intersecting line before validating line-reacquisition checks.

---

## 📦 Hardware Components Used

* **Microcontroller:** Arduino Mega 2560
* **Motor Driver:** L298N Dual H-Bridge DC Motor Driver Module
* **Sensor Array:** 5-Channel Infrared (IR) Reflective Line Tracking Sensor Module
* **Actuators:** 2 x High-Torque Geared DC Motors
* **Power Source:** Li-ion Rechargeable Battery Pack
* **Chassis:** Robot Smart Car Chassis Platform (including wheels and caster)

---

## 👥 Contributors

Special thanks to the project team members who made this system possible:

* **Abdallah Mahmoud - 900221058 - abdallah_walid8@aucegypt.edu** : Software Architecture
* **Hamdy Moussa - 900221818 - hamdy04@aucegypt.edu** : Hardware Schematics
* **Youssef Tantawy - 900221189 - yousseftantawy@aucegypt.edu** : Software Architecture
* **Ismail Zidane - 900211327 - ismailzidan@aucegypt.edu** : Hardware Schematics

Team 7 - ECNG 4504 - Fall 2026
