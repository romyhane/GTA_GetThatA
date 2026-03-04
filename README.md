# GTA: Get That A - Co-op Text Adventure Game 🎮

**An advanced object-oriented C++ console game developed as a 2nd-year B.Sc. Computer Science academic project at The Academic College of Tel-Aviv, Yaffo.**

## 📖 Overview
"Get That A" is a collaborative, multi-level console text adventure where two players (Romy and Tomer) must work together to solve dynamic riddles, navigate dark rooms, outsmart the patrolling "Fail Monster", and defeat the semester to get an 'A'.

The project demonstrates strong command of **C++**, **Data Structures**, **Algorithms (BFS, Bresenham's)**, and **Object-Oriented Programming (OOP)** principles.

## 🚀 Key Technical Features
* **Dynamic Level Loading (`<filesystem>` API):** The game engine dynamically scans the root directory for `.screen` files, allowing it to load an infinite number of levels automatically without hardcoded limits.
* **Spatial Coupling Algorithm:** Switches and doors are mapped dynamically at runtime. Using Euclidean distance calculations, switches automatically link to the nearest door, separating map design from hardcoded logic.
* **Co-op Physics & Object Interaction:** Implemented complex push mechanics where heavy obstacles require the combined force of both players (calculated side-by-side) or the use of in-game Springs.
* **Custom Memory & State Management:** Utilized polymorphic classes and careful memory management to handle dynamic entities (Bombs, Riddles, Smart/Latching Switches, Monsters) across varying game cycles.
* **Enhanced UI/UX via Windows Console API:** * Full multi-color rendering using `SetConsoleTextAttribute`.
    * Unicode support for graphic symbols (e.g., ♥ for lives).
    * Dynamic "Fog of War" (Dark Room) using line-of-sight algorithms.
* **Command-Line Automation & Testing:** The engine supports advanced execution via CLI arguments (`-save`, `-load`, `-silent`) for tracking keystrokes, automated playback testing, and headless execution for instant QA validation.

## 🕹️ Gameplay Mechanics
* **Asymmetric Co-op:** Players must work together to hold *Smart Switches* open for each other and combine strength to push heavy objects.
* **The "Fail" Monster:** A patrolling enemy algorithm that causes instant game-over upon contact.
* **Dynamic Riddles:** Riddles are pooled from a `.txt` file and assigned cyclically to map tiles, ensuring robust level design flexibility.

## 🛠️ Tech Stack
* **Language:** C++17
* **Environment:** Visual Studio 2022
* **Libraries:** Standard Template Library (STL), `<filesystem>`, `<windows.h>` (Console API)

## 👩‍💻 Developers
* **Romy Hane** * **Tomer Kill**
