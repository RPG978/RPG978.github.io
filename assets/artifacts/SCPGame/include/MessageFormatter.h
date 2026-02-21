#pragma once
#include <string>

class Room;
class Player;

/**
 * @class MessageFormatter
 * @brief Centralized presentation layer for all player-facing text.
 *
 * Responsibilities:
 * 
 *   - Provide consistent formatting for narrative, status, and UI messages
 * 
 *   - Keep output logic separate from game logic (GameEngine / PuzzleEngine)
 * 
 *   - Maintain a single, unified “voice” for the game
 *
 * Design Notes:
 * 
 *   - All methods are static because MessageFormatter has no state.
 * 
 *   - This class does not interpret commands or modify game data.
 * 
 *   - It simply prints text in a consistent, readable style.
 */
class MessageFormatter {
public:
    /**
     * @brief Display the opening banner and introductory narrative.
     */
    static void banner();

    /**
     * @brief Announce that the player has entered a room.
     */
    static void roomEntry(const Room& room);

    /**
     * @brief Print the room's description and visible items.
     *
     * Objects are handled separately by GameEngine::handleLookCommand().
     */
    static void roomDescription(const Room& room);

    /**
     * @brief Display the player's inventory in a clean, readable format.
     */
    static void inventory(const Player& player);

    /**
     * @brief Notify the player that they picked up an item.
     */
    static void itemPickup(const std::string& itemName);

    /**
     * @brief Notify the player that they already possess the item.
     */
    static void itemAlreadyOwned(const std::string& itemName);

    /**
     * @brief Display an error message with a consistent prefix.
     */
    static void error(const std::string& message);

    /**
     * @brief Display a general informational message.
     */
    static void info(const std::string& message);

    /**
     * @brief Display the list of available commands.
     */
    static void help();
};
