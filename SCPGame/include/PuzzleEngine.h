#pragma once
#include <map>
#include <string>
#include "RoomObject.h"
#include "Room.h"
#include "MixerPuzzle.h"
#include "SprayPuzzle.h"
#include "Containment.h"
#include "PuzzleAlgorithms.h"
#include "LeverPuzzle.h"

class Puzzle;
class Player;
class DatabaseManager;

/**
 * @class PuzzleEngine
 * @brief Central coordinator for puzzle execution, dispatching, and rewards.
 *
 * Responsibilities:
 * 
 *   - Run puzzles based on type, trigger, or object interaction
 * 
 *   - Delegate puzzle logic to PuzzleAlgorithms, MixerPuzzle, SprayPuzzle, etc.
 * 
 *   - Manage puzzle groups (e.g., tablet group)
 * 
 *   - Unlock exits and apply rewards after puzzle completion
 * 
 *   - Handle special room logic (containment entry, special LOOK)
 *
 * Design Notes:
 * 
 *   - PuzzleEngine does not load puzzles; DatabaseManager provides them.
 * 
 *   - PuzzleEngine is the single source of truth for puzzle routing.
 * 
 *   - Subsystems (MixerPuzzle, LeverPuzzle, SprayPuzzle, Containment)
 *     are owned here and reused across interactions.
 */
class PuzzleEngine {
public:
    PuzzleEngine();

    /**
     * @brief Database interface (non‑owning).
     *        Must be assigned by GameEngine before use.
     */
    DatabaseManager* db = nullptr;

    /**
     * @brief All puzzles loaded from the database, keyed by puzzle_id.
     *        Ownership belongs to GameEngine; PuzzleEngine only references them.
     */
    std::map<int, Puzzle*> puzzles;

    /**
     * @brief Pointer to the world’s room map.
     *        Set by GameEngine after world loading.
     */
    std::map<std::string, Room*>* rooms = nullptr;

    /*******************
    * PUZZLE EXECUTION *
    *******************/

    /**
     * @brief Run a puzzle directly.
     * @return true if solved.
     */
    bool runPuzzle(Puzzle& puzzle, Player& player);

    /**
     * @brief Apply puzzle rewards (items, flags, exit unlocks).
     */
    void giveReward(Player& player, Puzzle& puzzle);

    /********************************
    * OBJECT AND ROOM BASED ROUTING *
    ********************************/

    /**
     * @brief Run a puzzle triggered by interacting with a RoomObject.
     */
    bool runPuzzleForObject(RoomObject* obj, Player& player, Room* room, bool& running);

    /**
     * @brief Run a module puzzle (e.g., mixer, spray, lever panel).
     */
    bool runModulePuzzle(Player& player, Room* room, bool& running);

    /**
     * @brief Unlock exits associated with a solved puzzle.
     */
    void unlockExitsForPuzzle(int puzzleId);

    /****************************
    * SPECIAL CONTAINMENT LOGIC *
    ****************************/

    /**
     * @brief Handle containment room entry logic.
     */
    void runContainmentEntry(Room* room, bool& running);

    /**
     * @brief Handle special LOOK behavior in containment.
     */
    bool runSpecialLook(Room* room);

    /******************
    * COMMAND ROUTING *
    ******************/

    /**
     * @brief Handle "use <target>" commands that may trigger puzzles.
     */
    bool runUseCommand(const std::string& target, Player& player, Room* room, bool& running);

    /**
     * @brief Handle access‑chip routing puzzle (console interaction).
     */
    bool runAccessChipRouting(Room* room, Player& player, bool& running);

    /**
     * @brief Run a puzzle directly by ID (used by objects with direct puzzle links).
     */
    bool runDirectPuzzle(RoomObject* obj, Player& player, Room* room, int puzzleId, bool& running);

    /**
     * @brief Run a puzzle group (e.g., tablet group).
     */
    bool runPuzzleGroup(RoomObject* obj, Player& player, Room* room, const std::string& group, bool& running);
    
    /***********************
    * LEVER & SPRAY CONFIG *
    ***********************/

    /**
     * @brief Initialize lever solution and required spray color for this session.
     */
    void initialize();

    /**
     * @brief Raw lever pattern string from the database
     */
    std::string leverPatternRaw;
    
    /**
     * @brief Return the lever pattern formatted for display.
     */
    std::string getFormattedLeverPattern() const;

    /**
     * @brief Return the lever solution as a vector of booleans.
     */
    const std::vector<bool>& getLeverSolution() const;

    /**
     * @brief Required spray color
     */
    SprayColor getRequiredSprayColor() const { 
        return requiredSprayColor; 
    }

    /**
     * @brief Lowercase string version of required spray color.
     */
    std::string getRequiredSprayColorLower() const;

    /**
     * @brief Assign the room map after world loading.
     */
    void setRooms(std::map<std::string, Room*>* r) { 
        rooms = r; 
    }

    /**
     * @brief Tracks which tablet puzzle is currently active (group puzzle).
     */
    int activeTabletPuzzleId = -1;

private:
    // Subsystems owned by PuzzleEngine
    MixerPuzzle mixer;
    SprayPuzzle spray;
    Containment containment;
    PuzzleAlgorithms algorithms;
    LeverPuzzle lever;

    // Session‑specific configuration
    std::vector<bool> leverSolution;
    SprayColor requiredSprayColor;
};
