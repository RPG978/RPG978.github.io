#pragma once
#include <string>
#include "Item.h"      // for SprayColor
#include "Room.h"      // for Room


class Player;
class PuzzleEngine;

/**
 * @class SprayPuzzle
 * @brief Handles the spray‑bottle interaction used to reveal hidden patterns.
 *
 * Responsibilities:
 * 
 *   - Validate that the player has a spray bottle
 * 
 *   - Check the current room (only works in the Maintenance Room)
 * 
 *   - React based on the bottle’s mixture color
 * 
 *   - Display the hidden lever pattern when the correct color is used
 *
 * Design Notes:
 * 
 *   - SprayPuzzle contains no puzzle‑solving logic; it is purely an interaction.
 * 
 *   - PuzzleEngine provides the required spray color and lever solution.
 * 
 *   - The engine pointer is assigned by PuzzleEngine during initialization.
 */
class SprayPuzzle {
public:
    /**
     * @brief Run the spray interaction for the given room.
     *
     * @param player The player using the spray bottle.
     * @param room   The room where the action occurs.
     * @return true if the spray interaction reveals something meaningful.
     */
    bool run(Player& player, Room* room);

    /**
     * @brief Back‑reference to PuzzleEngine for accessing session data
     *        (required spray color, lever solution, etc.).
     *
     * Non‑owning pointer; set by PuzzleEngine.
     */
    PuzzleEngine* engine = nullptr;
};
