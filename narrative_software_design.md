# Software Design and Engineering

## Purpose of the Enhancement
The original artifact began as a simple text‑based adventure game created early in my academic program for IT‑140. That version, written in Python, was a single‑file procedural script intended to demonstrate basic control flow and user input. While it met the assignment requirements, it lacked structure, modularity, and long‑term maintainability. For this enhancement, I rebuilt the project in C++ and redesigned it around a clean, object‑oriented architecture. The goal was to transform a basic script into a scalable foundation capable of supporting future subsystems, data‑driven content, and more complex game mechanics.

## Enhancements Made
The enhanced version introduces a modular class design centered around components such as GameEngine, Room, Player, and Puzzle, each with clearly defined responsibilities. This separation of concerns replaces the tightly coupled logic of the original script and allows new features to be added without destabilizing existing behavior.
Puzzle logic is now isolated in a dedicated PuzzleEngine, enabling room‑specific interactions, dynamic puzzle states, and locked‑exit mechanics that were difficult to express cleanly in the Python version. Input handling was rewritten with defensive programming techniques to ensure predictable behavior and prevent invalid states. The redesign also includes clearer documentation and architectural reasoning, resulting in a codebase that resembles a lightweight game engine.

## Skills Demonstrated

- __Object‑oriented design:__ translating procedural logic into cohesive classes with single responsibilities.

- __Architectural thinking:__ designing subsystems that can evolve independently without breaking the overall structure.

- __Modern C++ practices:__ using header/source separation, encapsulation, and clean memory‑safe patterns.

- __Tooling proficiency:__ configuring Visual Studio projects, managing Git repositories, and debugging complex interactions.

- __Security‑aware development:__ validating input, preventing invalid states, and applying defensive programming techniques.

- __Technical communication:__ documenting class responsibilities and explaining design decisions clearly.

## Challenges & Lessons Learned
Rebuilding the game in C++ required rethinking the entire structure of the original program. Instead of directly translating Python logic, I had to identify the underlying responsibilities of each subsystem and design interfaces that supported clean, predictable behavior. Balancing abstraction with practicality was a recurring challenge—especially ensuring the architecture remained simple enough for the project’s scope while still demonstrating professional engineering discipline.

I also encountered real‑world tooling challenges, including Visual Studio project configuration, Git repository structure, and build artifacts. Solving these issues strengthened my understanding of how development environments, folder structures, and version control interact.

Incremental testing played a major role in validating the new architecture. As each subsystem was introduced—room navigation, puzzle triggers, locked exits, and data‑driven world loading—I verified that new features integrated cleanly and behaved as intended.

Finally, creating an initial UML class diagram helped guide the early design. As the project evolved, the UML naturally diverged from the original plan, reflecting a deeper understanding of the system’s needs. This experience reinforced the value of UML as a living design tool rather than a static document.

[<- Back to Portfolio](index.md)
