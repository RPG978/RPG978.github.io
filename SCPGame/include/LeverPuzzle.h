#pragma once
#include <vector>
#include <string>

class Room;

/**
 * @class LeverPuzzle
 * @brief Handles the interactive lever?setting puzzle found in specific rooms.
 *
 * Responsibilities:
 * 
 *   - Display the current lever configuration to the player
 * 
 *   - Prompt the player for new lever positions
 * 
 *   - Confirm and commit the new configuration back to the Room
 *
 * Design Notes:
 * 
 *   - LeverPuzzle does not validate the pattern against the solution;
 * 
 *     PuzzleEngine performs that check when needed.
 * 
 *   - This class focuses solely on the UX loop for setting levers.
 * 
 *   - Lever state is stored on the Room itself, keeping this class stateless.
 */
class LeverPuzzle {
public:
    /**
     * @brief Run the lever?setting interaction for the given room.
     *
     * Behavior:
     * 
     *   - Shows current lever states
     * 
     *   - Prompts for new positions (up/down)
     * 
     *   - Asks for confirmation before committing
     *
     * @param room The room whose lever states will be modified.
     * @return true if the interaction completed successfully.
     */
    bool run(Room* room);

private:
    /**
     * @brief Display the room's current lever configuration.
     */
    void printCurrentState(const std::vector<bool>& ls);

    /**
     * @brief Display the player's proposed configuration before confirmation.
     */
    void printSummary(const std::vector<bool>& newStates);
};
