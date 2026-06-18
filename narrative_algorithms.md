# Algorithms and Data Structure

## Purpose of the Enhancement
This enhancement builds on the same text‑based adventure game used in the Software Design & Engineering category. The original artifact, created in IT‑140, was a single‑file Python script demonstrating basic control flow and simple game logic. After redesigning the project in C++ with modular classes, defensive programming, and structured input handling, I had a strong architectural foundation to introduce more advanced algorithmic systems.
The goal of this enhancement was to transform the game’s simple interactions into structured, data‑driven puzzle mechanics supported by clear algorithms and well‑chosen data structures.

## Enhancements Made
This enhancement focused on evolving the game from a linear, procedural script into a coordinated system of algorithmic subsystems. Instead of relying on scattered conditionals and ad‑hoc logic, puzzles now operate through structured state machines, deterministic command‑parsing pathways, and consistent data representations.
I introduced multi‑step puzzle systems—including the lever puzzle, reagent mixer, spray‑and‑reveal mechanic, and boss‑room gating logic—each designed around predictable control flow and explicit state transitions. These improvements not only strengthened maintainability but also made gameplay smoother and more intuitive.
The result is a cohesive algorithmic framework capable of supporting diverse puzzle types, reliable state management, and extensible gameplay.

## Skills Demonstrated
- __Structured state machines:__  managing puzzle progression and preventing invalid transitions.

- __Deterministic command parsing:__ ensuring interactions occur in the correct order and cannot be bypassed.

- __Thoughtful data structure selection:__ choosing between vectors, enums, booleans, and object‑based models based on clarity and scalability.

- __Algorithmic problem‑solving:__ applied to item handling, state transitions, navigation, gating logic, and retry systems.

- __Modular subsystem design:__ enabling puzzles to evolve independently without breaking the engine.

- __Security‑aware logic:__ including input validation and prevention of unsafe or undefined states.

- __Clear technical communication:__ documenting puzzle behavior, room interactions, and design decisions.

- __Use of industry tools:__ Visual Studio, Git, and debugging utilities to validate algorithmic behavior.
  
## Challenges & Lessons Learned
Enhancing this artifact required rethinking the original script as a system of interconnected algorithms rather than a sequence of linear events. One of the first challenges was untangling the procedural logic inherited from the IT‑140 version and reshaping it into structured, reusable components. This meant replacing “just make it work” code with predictable data structures, state machines, and well‑defined control flow.

Managing state across multiple systems proved equally challenging. Early in development, the command parser and handleUseCommand function revealed how fragile the system could become when loosely ordered checks or early returns interfered with intended logic. Fixing this required careful sequencing, explicit state flags, and a deterministic approach to command handling. These refinements not only solved immediate bugs but also clarified the underlying algorithmic patterns the engine needed.

Selecting appropriate data structures was another key learning experience. Lever states, spray colors, and room conditions each required different representations, and choosing between vectors, enums, booleans, and object‑based models meant evaluating tradeoffs in clarity, scalability, and complexity. Each decision pushed the system closer to a cohesive algorithmic framework rather than a collection of isolated features.

Finally, integrating new mechanics such as the mixer puzzle, spray‑and‑reveal system, and boss‑room gating logic tested the flexibility of the architecture. Each addition surfaced edge cases, invalid states, and ordering issues that had to be resolved through careful reasoning and iterative refinement. These challenges ultimately strengthened the design, ensuring that puzzles could be triggered, retried, and resolved through consistent and reliable algorithms.

[<- Back to C++ SCP Game Page](SCP_Artifact.md)
