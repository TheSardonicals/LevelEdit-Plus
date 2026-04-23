# LevelEdit ++
A new version of the level editor we made before, written using C++ instead. Incorporates IMGUI as the GUI backend

## Technical Overview

### 💻 Programming Language
**LevelEdit-Plus** is engineered using **C++**, prioritizing high-performance memory management and low-level system control. The choice of C++ allows the editor to handle complex level data structures and real-time rendering updates with minimal overhead, ensuring a responsive experience even when managing high entity counts.

### 📦 Frameworks & APIs
The project utilizes a modular stack of industry-standard libraries to provide a robust editing environment:

* **SDL2 / SDL3 (Simple DirectMedia Layer):** Acts as the foundational hardware abstraction layer. It manages window creation, 2D/3D rendering contexts, and handles cross-platform input from keyboards, mice, and controllers.
* **Dear ImGui:** Powers the editor’s interface. This immediate-mode graphical user interface (IMGUI) library provides a highly efficient, non-retained UI system, allowing for the creation of complex docking windows, property inspectors, and toolbars that sync perfectly with the application state.
* **nlohmann/json:** Utilized as the primary data interchange format. This header-only C++ library is used to serialize and deserialize level files and the **Template System**, providing a human-readable and portable way to save level geometry and entity configurations.

---

## ✨ Key Features

### 1. Precision Object Manipulation
LevelEdit-Plus provides a suite of professional-grade transform tools designed for granular control over level entities:
* **Multi-Select Engine:** Implements a selection marquee logic to group multiple objects for simultaneous translation, scaling, and rotation.
* **Coordinate Systems:** Support for both local and global transformation modes, allowing users to manipulate objects relative to their own axes or the world grid.
* **Dynamic Snapping:** A variable grid-snapping system that ensures pixel-perfect or unit-perfect placement, crucial for maintaining level alignment.

### 2. State Management & Workflow
To support a non-destructive design workflow, the editor includes standard productivity features:
* **Command-Pattern Undo/Redo:** A robust state-tracking system that allows designers to experiment freely, with the ability to revert or re-apply a long history of specific actions.
* **JSON-Based Template System:** Leveraging the `nlohmann/json` integration, users can save specific clusters of objects as reusable templates. These templates can be shared between users or reused across different project files.
* **Clipboard Operations:** Full support for copying, cutting, and pasting entities, significantly speeding up the replication of complex patterns or structural elements.

### 3. Integrated Toolset
The editor features a streamlined UI built on Dear ImGui, offering:
* **Property Inspectors:** Real-time editing of entity attributes, such as position coordinates, scale factors, and custom logic flags.
* **Layer Management:** Tools to organize and toggle the visibility of different level segments, reducing visual clutter during the design process.
* **Optimized Serialization:** A backend process that validates level data during the save operation to ensure compatibility with the target game engine and prevent data corruption.

<img src"./imgs/Screenshot1.png">
<img src"./imgs/Screenshot2.png">
<picture src"./imgs/Screenshot3.png">
