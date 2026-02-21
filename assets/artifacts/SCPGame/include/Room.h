#pragma once
#include <string>
#include <map>
#include <vector>
#include "Item.h"
#include "RoomObject.h"

class Puzzle;

/**
 * @class Room
 * @brief Represents a location in the game world that the player can enter.
 *
 * Responsibilities:
 * 
 *   - Store the room’s name, description, and ID
 * 
 *   - Maintain directional exits to other rooms
 * 
 *   - Hold items and interactive objects
 * 
 *   - Track locked exits and lever states (if applicable)
 * 
 *   - Optionally reference a puzzle associated with the room
 *
 * Design Notes:
 * 
 *   - Rooms do not own puzzles; PuzzleEngine manages puzzle lifetimes.
 * 
 *   - Items are stored by value (lightweight).
 * 
 *   - RoomObjects are stored as raw pointers; ownership belongs to GameEngine.
 * 
 *   - Lever states are room‑specific and used by the lever puzzle subsystem.
 */
class Room {
public:

    /**************
    * CONSTRUCTON *
    **************/

    Room(const std::string& name, const std::string& description);
    Room(int id, const std::string& name, const std::string& description);

    /******************
    * BASIC ACCESSORS *
    ******************/

    const std::string& getName() const {
        return name;
    }

    const std::string& getDescription() const {
        return description;
    }

    void setDescription(const std::string& desc) {
        description = desc;
    }

    int getId() const { 
        return roomId; 
    }
    
    /********
    * EXITS *
    ********/

    /**
     * @brief Retrieves the Room connected in the given direction.
     */
    Room* getExit(const std::string& direction);

    /**
     * @brief Adds a directional exit to another room.
     */
    void addExit(const std::string& direction, Room* target) {
        exits[direction] = target;
    }

    /**
     * @brief Check or modify locked exit state.
     */
    bool isExitLocked(const std::string& direction) const {
        auto it = lockedExits.find(direction);
        return it != lockedExits.end() && it->second;
    }

    void lockExit(const std::string& direction) {
        lockedExits[direction] = true;
    }

    void unlockExit(const std::string& direction) {
        lockedExits[direction] = false;
    }

    const std::map<std::string, bool>& getLockedExits() const {
        return lockedExits;
    }

    /*********************
    * PUZZLE ASSOCIATION *
    *********************/

    bool hasPuzzle() const { 
        return puzzle != nullptr; 
    }

    void setPuzzle(Puzzle* p) { 
        puzzle = p; 
    }

    Puzzle* getPuzzle() const { 
        return puzzle; 
    }

    /********
    * ITEMS *
    ********/

    void addItem(const Item& item) {
        items.push_back(item);
    }

    // Checks whether the player currently has an item with the given name. 
    // Used for gating puzzle interactions and validating player actions.
    bool hasItem(const std::string& name) const {
        for (const auto& item : items) {
            if (item.getName() == name) return true;
        }
        return false;
    }

    Item takeItem(const std::string& name) {
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].getName() == name) {
                Item found = items[i];
                items.erase(items.begin() + i);
                return found;
            }
        }
        return Item(""); // placeholder empty item
    }

    const std::vector<Item>& getItems() const {
        return items;
    }

    /***************
    * ROOM OBJECTS *
    ***************/

    void addObject(RoomObject* obj) {
        objects.push_back(obj);
    }

    const std::vector<RoomObject*>& getObjects() const {
        return objects;
    }

    // Debug helper
    RoomObject* getObjectByName(const std::string& name) const;

    /***************
    * LEVER STATES *
    ***************/

    void setLeverStates(const std::vector<bool>& states) { 
        leverStates = states; 
    }

    const std::vector<bool>& getLeverStates() const { 
        return leverStates; 
    }

    /*******************
     * BOSS ROOM FLAGS *
     ******************/

    bool bossBarrierActive = false;
    bool bossDefeated = false;

private:
    int roomId;

    std::string name;                     // The room's display name.
    std::string description;              // Text describing the room.

    std::map<std::string, Room*> exits;   // Directional exits to other rooms.
    std::map<std::string, bool> lockedExits;

    std::vector<Item> items;              // Items present in the room.
    std::vector<RoomObject*> objects;

    Puzzle* puzzle = nullptr;             // nullptr if no puzzle in this room
    std::vector<bool> leverStates;        // true = up, false = down
};
