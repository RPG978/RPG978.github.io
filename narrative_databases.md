# Software Design and Engineering

## Purpose of the Enhancement
This enhancement builds on the same text‑based adventure game I have been evolving throughout the program. The original artifact, created in IT‑140, began as a small procedural Python script demonstrating basic programming constructs. Through earlier enhancements, it grew into a modular, object‑oriented C++ application.
For this category, I extended the system into a fully data‑driven engine by integrating SQLite and shifting core gameplay behavior such as rooms, puzzles, and interactions, out of hard coded logic and into structured database tables. This redesign transformed the project from a static game into a dynamic system capable of loading world data, puzzle definitions, and stateful interactions directly from a database.

## Enhancements Made
The Category Three enhancements focused on building a database‑driven architecture that cleanly separates content from logic. Instead of embedding room descriptions, exits, puzzle states, and item interactions directly in C++, the engine now loads all of this information from SQLite tables at runtime.
This shift required designing a schema that represents rooms, objects, puzzles, states, and conditions in a way the engine can interpret predictably. The game now supports dynamic puzzle definitions, room objects with behaviors, locked exits, and multi‑step interactions that are all defined externally. Updating or adding content no longer requires modifying C++ source code; developers can adjust the SQLite tables and the engine will reinterpret the world accordingly.
These enhancements demonstrate my ability to design scalable systems, integrate databases into application logic, and build engines that support extensible gameplay through data‑driven design.

## Skills Demonstrated

- __Database‑driven architecture:__ loading rooms, puzzles, items, and interactions from SQLite.

- __Schema design:__ representing game entities, states, and relationships.

- __Separation of content from logic:__ enabling scalable and maintainable gameplay.

- __Dynamic puzzle loading:__ allowing new puzzles to be added without modifying C++ code.

- __Structured state machines:__ puzzle progression and room behavior.

- __Deterministic command routing:__ database defined interactions.

- __Input validation and defensive programming:__ preventing invalid or unsafe states.

- __Use of industry tools:__ including SQLite, Visual Studio, Git, and debugging utilities.
 
- __Clear technical communication:__ documenting database structures and engine behavior.

## Challenges & Lessons Learned
Treating the game as a network of interacting systems required rethinking the original procedural logic and restructuring it into predictable, reusable components. I learned how to design a puzzle engine that cleanly separates puzzle logic from game logic, how to route commands deterministically, and how to manage state transitions without creating contradictions.
Debugging the system taught me to trace execution paths carefully, identify unreachable code, and reason about how state flows through the engine. Issues such as early returns, unordered checks, and inconsistent naming conventions revealed how fragile a system can become without clear architectural rules. Fixing these problems required deliberate sequencing, explicit state flags, and disciplined control flow.
Selecting appropriate data structures was another key learning experience. Lever states, spray colors, puzzle triggers, and room conditions each required different representations, and choosing between vectors, enums, booleans, and object based models meant evaluating clarity, scalability, and long term maintainability.
Integrating new mechanics, such as the mixer puzzle and spray‑and‑reveal system, tested the flexibility of the architecture. Each addition surfaced edge cases and invalid states that had to be resolved through careful reasoning and iterative refinement. These challenges ultimately strengthened the system and deepened my understanding of algorithmic design, data‑driven architecture, and software engineering principles.
Overall, this artifact demonstrates my ability to design scalable architectures, integrate databases into application logic, and build systems that are maintainable, extensible, and grounded in sound engineering practices.

[<- Back to C++ SCP Game Page](SCP_Artifact.md)
