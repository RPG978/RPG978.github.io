#pragma once
#include <string>
#include "Item.h"   // for SprayColor

class Player;
class CommandParser;

/**
 * @class MixerPuzzle
 * @brief Handles the reagent mixer interaction used to produce spray colors.
 *
 * Responsibilities:
 * 
 *   - Present the mixer interface (three adjustable dials)
 * 
 *   - Compute the resulting mixture color based on dial settings
 * 
 *   - Fill or update the player's spray bottle with the chosen mixture
 *
 * Design Notes:
 * 
 *   - MixerPuzzle is a self-contained UX loop; it does not validate
 *     whether the produced color is correct for any puzzle.
 * 
 *   - PuzzleEngine handles all higher-level logic (e.g., checking
 *     required spray color for the containment barrier).
 * 
 *   - Mixer state is local to the interaction and not persisted.
 */
class MixerPuzzle {
public:
    /**
     * @brief Run the mixer interaction for the player.
     *
     * Behavior:
     * 
     *   - Shows current dial settings
     * 
     *   - Allows the player to adjust dials (A/B/C)
     * 
     *   - Computes the resulting mixture color
     * 
     *   - Fills or updates the player's spray bottle
     *
     * @return true when the interaction completes successfully.
     */
    bool run(Player& player);

    /**
     * @brief Convert a mixture color string (e.g., "RED") into a SprayColor enum.
     */
    static SprayColor toSprayColor(const std::string& color);

private:
    /**
     * @struct MixerState
     * @brief Represents the current dial settings for the mixer.
     */
    struct MixerState {
        int dialA = 1;
        int dialB = 1;
        int dialC = 1;
    };

    /**
     * @brief Convert a dial value (1–3) into a human-readable label.
     */
    std::string dialName(int v);

    /**
     * @brief Compute the resulting mixture color based on dial settings.
     *
     * The sum of the three dials determines the color:
     * 
     *   <= 4  - RED
     * 
     *   <= 6  - GREEN
     * 
     *   <= 8  - BLUE
     * 
     *   >  8  - BLACK
     */
    std::string getMixtureColor(const MixerState& m);
};
