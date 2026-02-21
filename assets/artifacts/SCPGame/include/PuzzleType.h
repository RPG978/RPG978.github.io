#pragma once

/**
 * @enum PuzzleType
 * @brief Enumerates all supported puzzle categories.
 *
 * This enum allows the PuzzleEngine to dispatch the correct
 * algorithm for each puzzle without relying on fragile string
 * comparisons. New puzzle types can be added here as the
 * engine evolves.
 */
enum class PuzzleType {
    Sequence,       // Number or symbol sequence completion
    WordScramble,   // Unscramble or rearrange characters
    LogicGate,      // Boolean logic evaluation puzzles
    MiniMaze,       // Graph traversal or pathfinding puzzles
    Pattern         // Pattern recognition or matching puzzles
};
