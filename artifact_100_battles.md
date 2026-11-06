---
layout: default
---

# 100 Battles

## Artifact Summary

100 Battles is a single-player, turn-based RPG developed in modern C++ that challenges players to survive a 
gauntlet of one hundred battles through strategic character building, equipment selection, and risk management. 
Players begin each run by selecting from procedurally generated character options, drafting their starting 
equipment, and choosing how aggressively to progress through battle streaks. The game's core combat system 
revolves around momentum-based mechanics that reward offensive and defensive commitments while creating 
meaningful risk-versus-reward decisions throughout each run.

The project began as a small programming exercise focused on learning data structures, inline initialization, 
and character statistics. Early versions consisted of simple player and enemy structures that could attack, 
defend, and grow stronger over time. As new systems were introduced, including equipment, procedural content 
generation, and data loading, the project naturally expanded beyond its original scope. What started as a 
console-based experiment evolved into a fully playable game featuring a graphical interface, multiple game 
states, SQLite-backed content management, and a modular architecture consisting of more than thirty source files.

Today, 100 Battles serves as both a playable game and a demonstration of modern C++ development practices. 
The project incorporates object-oriented design, database integration, procedural generation, state-driven 
menu systems, combat algorithms, and graphical user interface development through SDL2 and Dear ImGui. 
While still actively developed, the project demonstrates the progression from foundational programming 
concepts to the design of a complete software system with interconnected gameplay, user interface, and 
data management subsystems.

What makes this project particularly meaningful is that it was not built to satisfy a course requirement or 
follow a predefined specification. Instead, each subsystem emerged from curiosity and experimentation. 
As new technical challenges appeared, the project expanded to accommodate them, transforming a simple 
combat simulator into a feature-rich RPG that continues to evolve alongside my skills as a software developer.
Ongoing development has further expanded the project into areas such as cross-platform development, CMake build 
automation, Docker containerization, and Linux deployment, providing valuable experience in software engineering 
practices beyond gameplay programming alone.

---

## Current Architecture

100 Battles is organized into a collection of specialized systems that separate gameplay logic, data management, 
combat processing, and user interface concerns. Rather than placing all functionality inside a single game loop, 
responsibilities are distributed across dedicated classes that communicate through the GameEngine.
At the center of the application is the GameEngine, which acts as the primary coordinator for gameplay flow. 
The GameEngine is responsible for creating and loading the game world, managing state transitions, routing player 
actions, generating store inventories, handling purchases, and controlling the progression between character 
selection, camp, battle preparation, combat, and post-combat rewards.

The CombatEngine manages turn-based encounters. It calculates damage, processes attack and defense actions, 
controls enemy turns, applies rewards, manages combat state transitions, and determines victory or defeat 
conditions. Combat mechanics are further supported by the EnemyAI subsystem, which selects enemy actions based 
on personality-driven behaviors.

Persistent game content is managed through the DatabaseManager, which loads enemies and items from a SQLite 
database into memory at runtime. This approach separates content from implementation, allowing game balancing 
and content updates without requiring direct code changes. The ItemFactory builds item objects from loaded data, 
creating weapon, armor, and accessory instances used throughout the game.

Character progression is handled through the Player system, which manages character statistics, equipment, 
leveling, experience gains, health recalculation, and stat allocation. Supporting utility systems such as the 
Random class provide centralized random number generation for gameplay and procedural content.

The user interface is implemented through SDL2 and Dear ImGui. Rather than using a single monolithic interface, 
each major game state is represented by a dedicated UI component. Separate interfaces exist for the Main Menu, 
Character Selection, Character Confirmation, Camp, Combat, Battle Selection, Character Overview, Debug Tools, 
and Death Summary screens. These elements are coordinated through the UIRouter, which directs rendering to the 
appropriate screen based on the current game state.

This architecture currently spans more than thirty source files and reflects the project's evolution from a 
simple combat experiment into a structured software project containing gameplay systems, database integration, 
procedural generation, graphical user interfaces, state management, and cross-platform build support.


---

## Code Review

---

## Code Excerpts

---

## Development Journal Highlights

---

## Future Roadmap 

---

## Repository

**[View Full Original C++ Project](https://github.com/RPG978/100_Battles.git)**

---

[<- Back to Portfolio](index.md)
