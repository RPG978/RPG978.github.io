#pragma once
#include <map>
#include <string>

#include "CommandParser.h"
#include "Room.h" 
#include "Player.h" 
#include "PuzzleEngine.h" 
#include "DatabaseManager.h"


/**
 * @class GameEngine
 * @brief Central coordinator for world loading, command routing, and the main loop.
 *
 * Responsibilities:
 * 
 *   - Initialize subsystems (Player, PuzzleEngine, world data)
 * 
 *   - Load all rooms, objects, items, and puzzles from the database
 * 
 *   - Run the main game loop and process player input
 * 
 *   - Delegate puzzle logic to PuzzleEngine
 * 
 *   - Manage navigation, inventory, and general game commands
 *
 * Design Notes:
 * 
 *   - GameEngine owns the high?level flow; it does not implement puzzle logic.
 * 
 *   - All room?specific or puzzle?specific behavior is delegated outward.
 * 
 *   - The engine maintains a simple, predictable command?handling pipeline.
 */
class GameEngine {
public:
    /**
     * @brief Construct the engine with an existing DatabaseManager.
     *
     * GameEngine does not own the DatabaseManager; it assumes the caller
     * manages its lifetime.
     */
    GameEngine(DatabaseManager* db);
    
    /**
     * @brief Run the main game loop until the player quits or the game ends.
     */
    void run();

    /**
     * @brief Load all world data from the database.
     *
     * This includes:
     * 
     *   - Rooms
     * 
     *   - Room connections
     * 
     *   - Locked exits
     * 
     *   - Room objects
     * 
     *   - Room items
     * 
     *   - Puzzles
     *
     * PuzzleEngine is initialized after world data is loaded.
     */
    void loadWorld();

    /**
     * @brief Parse and execute a single player command.
     */
    void processCommand(const std::string& cmd);

    /**
     * @brief Look up a room by its database ID.
     * @return Pointer to the room, or nullptr if not found.
     */
    Room* getRoomById(int id);


private:
    /*************
    * CORE STATE *
    *************/

    Player* player;                     // The player character
    std::map<std::string, Room*> rooms; // All rooms loaded from DB
    std::map<int, Puzzle*> puzzles;     // Puzzles loaded from DB (passed to PuzzleEngine)
    PuzzleEngine* puzzleEngine;         // Handles all puzzle logic
    DatabaseManager* db;                // Database interface
    CommandParser parser;               // Parses raw player input into structured commands.
    bool running = true;                // Main loop flag

    /*******************
    * COMMAND HANDLERS *
    *******************/

    /**
     * @brief Move the player to an adjacent room, if possible.
     */
    void handleGoCommand(const std::string& direction);

    /**
     * @brief Display the current room's description and visible items/objects.
     */
    void handleLookCommand();

    /**
     * @brief Attempt to pick up an item from the room.
     */
    void handleTakeCommand(const std::string& itemName);

    /**
     * @brief Interact with an object or item in the room.
     */
    void handleUseCommand(const std::string& target);

    /**
     * @brief Display the player's inventory.
     */
    void handleInventoryCommand();

    /**
     * @brief Display available commands.
     */
    void handleHelpCommand();

    /**
     * @brief End the game loop.
     */
    void handleQuitCommand();
};