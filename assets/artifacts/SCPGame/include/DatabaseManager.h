#pragma once
#include <map>
#include <string>
#include <vector>
#include "Item.h"

class Room;
class Puzzle;

/**
 * @struct ExitUnlock
 * @brief Represents an exit unlocked by solving a puzzle.
 *
 * Used by PuzzleEngine to update room navigation after puzzle completion.
 */
struct ExitUnlock {
    int roomId;
    std::string direction;
};

/**
 * @class DatabaseManager
 * @brief Centralized interface for all SQLite-backed game data.
 *
 * Responsibilities:
 * 
 *   - Own and manage the SQLite connection
 * 
 *   - Load world data (rooms, exits, objects, items, puzzles)
 * 
 *   - Persist puzzle and puzzle-group solved state
 * 
 *   - Provide runtime flags for cross-room logic
 * 
 *   - Supply randomized data (lever patterns, spray colors)
 *
 * Design Notes:
 * 
 *   - DatabaseManager isolates all SQL logic from the rest of the engine.
 * 
 *   - GameEngine and PuzzleEngine operate entirely on in-memory objects.
 * 
 *   - The database connection is intentional to avoid
 *     leaking SQLite types into headers.
 * 
 *   - All returned Room* and Puzzle* objects are heap-allocated and owned
 *     by the caller.
 */
class DatabaseManager {
public:
    /**
     * @brief Open a SQLite database at the given path.
     *        Connection remains active for the lifetime of the manager.
     */
    DatabaseManager(const std::string& dbPath);
    /**
     * @brief Close the SQLite connection and release resources.
     */
    ~DatabaseManager();

    /****************
    * WORLD LOADING *
    ****************/

    /**
     * @brief Load all rooms from the database.
     * @return Map keyed by room name, each containing a Room*.
     */
    std::map<std::string, Room*> loadRooms();

    /**
     * @brief Load all puzzles from the database.
     * @return Map keyed by puzzle_id, each containing a Puzzle*.
     */
    std::map<int, Puzzle*> loadPuzzles();
    
    /**
     * @brief Load directional exits between rooms.
     * @param rooms Map of rooms previously loaded by loadRooms().
     */
    void loadRoomConnections(std::map<std::string, Room*>& rooms);

    /**
     * @brief Load which exits begin in a locked state.
     */
    void loadLockedExits(std::map<std::string, Room*>& rooms);

    /**
     * @brief Load interactive objects (console, levers, tablets, etc.)
     *        and attach them to their respective rooms.
     */
    void loadRoomObjects(std::map<std::string, Room*>& rooms);

    /**
     * @brief Load items placed in rooms at the start of the game.
     */
    void loadRoomItems(std::map<std::string, Room*>& rooms);

    /***********************
    * PUZZLE STATE & FLAGS *
    ***********************/

    /**
     * @brief Save a puzzle's solved state back to the database.
     */
    void savePuzzleState(const Puzzle& p);

    /**
     * @brief Retrieve a boolean runtime flag.
     *        Used for cross-room logic (e.g., console_unlocked).
     */
    bool getFlag(const std::string& name);

    /**
     * @brief Set or update a runtime flag.
     */
    void setFlag(const std::string& name, bool value);

    /**
     * @brief Check whether a puzzle group has been solved.
     */
    bool isPuzzleGroupSolved(const std::string& group);

    /**
     * @brief Mark a puzzle group as solved or unsolved.
     */
    void setPuzzleGroupSolved(const std::string& group, bool solved);

    /**
     * @brief Reset all puzzle and puzzle-group solved states.
     *        Called at the start of each playthrough.
     */
    void resetRuntimeState();

    /**********************
    * ITEM & EXIT HELPERS *
    **********************/

    /**
     * @brief Load a single item definition by ID.
     */
    Item getItemById(int itemId);

    /**
     * @brief Retrieve all exits unlocked by solving a specific puzzle.
     */
    std::vector<ExitUnlock> getExitsUnlockedByPuzzle(int puzzleId);
    
    /************************
    * RANDOMIZATION HELPERS *
    ************************/

    /**
     * @brief Retrieve a random lever pattern for this playthrough.
     *        Patterns are stored as comma-separated "1,0,1,1" strings.
     */
    std::string getRandomLeverPattern();

    /**
     * @brief Retrieve the required spray color for this playthrough.
     */
    std::string getRandomRequiredSprayColor();

private:
    /**
     * @brief SQLite connection pointer (sqlite3*).
     *
     * Stored as void* to avoid exposing SQLite headers in the public API.
     */
    void* connection;
};


