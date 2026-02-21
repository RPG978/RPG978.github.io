#pragma once
#include <string>
#include <vector>

class Puzzle;
class Player;

/**
 * @class PuzzleAlgorithms
 * @brief Implements the concrete algorithms for each puzzle type.
 *
 * Responsibilities:
 * 
 *   - Execute the logic for all supported puzzle types
 *
 * Design Notes:
 * 
 *   - PuzzleAlgorithms contains only puzzle logic.
 * 
 *   - PuzzleEngine decides *when* to run a puzzle and handles rewards,
 *     state updates, and group logic.
 * 
 *   - This class is stateless; all puzzle data comes from the Puzzle object.
 */
class PuzzleAlgorithms {
public:
    /**
     * @brief Dispatch to the correct puzzle handler based on puzzle type.
     *
     * @param puzzle The puzzle to run.
     * @param player Reference to the player (used for inventory or input).
     * @return true if the puzzle was solved; false otherwise.
     */
    bool run(Puzzle& puzzle, Player& player);

private:
    /******************
    * PUZZLE HANDLERS *
    ******************/

    /**
     * @brief Run a numeric sequence puzzle (e.g., missing number).
     */
    bool runSequencePuzzle(Puzzle& puzzle, Player& player);

    /**
     * @brief Run a word scramble puzzle.
     *
     * Scrambled word is generated once and cached in the Puzzle object.
     */
    bool runWordScramblePuzzle(Puzzle& puzzle, Player& player);

    /**
     * @brief Placeholder for a logic gate puzzle.
     */
    bool runLogicGatePuzzle(Puzzle& puzzle, Player& player);

    /**
     * @brief Placeholder for a mini‑maze puzzle.
     */
    bool runMiniMazePuzzle(Puzzle& puzzle, Player& player);

    /**
     * @brief Placeholder for a pattern‑recognition puzzle.
     */
    bool runPatternPuzzle(Puzzle& puzzle, Player& player);

    /**********
    * HELPERS *
    **********/

    /**
     * @brief Parse a comma‑separated list of numbers into a vector.
     */
    std::vector<double> parseSequence(const std::string& data);

    /**
     * @brief Print a list of strings as a comma‑separated sequence.
     */
    void printCommaSeparated(const std::vector<std::string>& items);
    
    /**
     * @brief Format a floating‑point number with trimmed trailing zeros.
     */
    std::string formatNumber(double value);
};
