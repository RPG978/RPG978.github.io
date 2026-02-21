#pragma once
#include <string>

/**
 * @class RoomObject
 * @brief Represents an interactive object placed inside a room.
 *
 * Responsibilities:
 * 
 *   - Store the object's name and database ID
 * 
 *   - Indicate which puzzle (if any) it triggers
 * 
 *   - Support puzzle groups (multi‑stage or multi‑object puzzles)
 *
 * Design Notes:
 * 
 *   - RoomObjects are lightweight metadata containers.
 * 
 *   - They do not own puzzles; PuzzleEngine resolves puzzle IDs/groups.
 * 
 *   - Ownership of RoomObject instances belongs to GameEngine/Room.
 */
class RoomObject {
public:
    /**
     * @brief Construct a room object with puzzle linkage metadata.
     *
     * @param id          Database ID for this object
     * @param name        Display name (e.g., "console", "lever panel")
     * @param puzzleId    Direct puzzle ID (0 or -1 if none)
     * @param puzzleGroup Optional puzzle group name ("" if none)
     */
    RoomObject(int id, const std::string& name, int puzzleId, const std::string& puzzleGroup)
        : id(id), name(name), puzzleId(puzzleId), puzzleGroup(puzzleGroup) {}

    /*************
     * ACCESSORS *
     ************/

    const std::string& getName() const { 
        return name; 
    }

    /**
     * @brief Return the direct puzzle ID associated with this object.
     *        A value of -1 indicates no direct puzzle.
     */
    int getPuzzleId() const { 
        return puzzleId; 
    }

    /**
     * @brief Return the puzzle group name, if any.
     *        Empty string means no group association.
     */
    const std::string& getPuzzleGroup() const { 
        return puzzleGroup; 
    }

private:
    int id;                     // Database ID for this object
    std::string name;           // Display name
    int puzzleId;               // Direct puzzle link
    std::string puzzleGroup;    // Group name for multi-object puzzles
};
