#pragma once
#include <string>
#include <map>
#include "Room.h"
#include "DatabaseManager.h"

class Room;
class DatabaseManager;
class PuzzleEngine;


/**
 * @class Containment
 * @brief Handles all special behavior for the SCP?1048?A containment room.
 *
 * This subsystem owns the narrative and logic for Room 5, including:
 * 
 *   - Special LOOK behavior (barrier active, boss defeated, etc.)
 * 
 *   - Entry logic that checks lever configuration and triggers outcomes
 *
 * Design Notes:
 * 
 *   - PuzzleEngine delegates containment?specific events to this class.
 * 
 *   - This keeps GameEngine and PuzzleEngine free of room?specific logic.
 * 
 *   - Containment does not own any resources; it operates on engine state.
 */
class Containment {
public:
    /**
     * @brief Back?reference to the PuzzleEngine.
     *
     * Used for accessing shared state or utilities if needed.
     * Ownership remains with PuzzleEngine.
     */
    PuzzleEngine* engine = nullptr;

    /**
     * @brief Handle special LOOK behavior inside the containment room.
     *
     * Behavior varies depending on:
     * 
     *   - Whether the barrier is active
     * 
     *   - Whether the boss has been defeated
     * 
     *   - Whether the player has the required items
     *
     * @param room The room being examined.
     * @param db   DatabaseManager for checking runtime flags.
     * @return true if the look action was handled; false otherwise.
     */
    bool handleLook(Room* room, DatabaseManager* db);

    /**
     * @brief Handle logic when the player enters the containment room.
     *
     * This includes:
     * 
     *   - Checking lever configuration from the Maintenance Room
     * 
     *   - Triggering the failure sequence if incorrect
     * 
     *   - Activating the soundproof barrier if correct
     *
     * @param room    The containment room.
     * @param running Reference to the game loop flag (may end the game).
     * @param db      DatabaseManager for flags and state.
     * @param rooms   Map of all rooms for cross?room lookups.
     * @return true if the entry event was handled; false otherwise.
     */
    bool handleEntry(Room* room, bool& running, DatabaseManager* db, std::map<std::string, Room*>* rooms);
};
