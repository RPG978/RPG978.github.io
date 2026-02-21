#pragma once
#include <string>
#include <vector>
#include "Item.h"
#include "Room.h"

/**
 * @class Player
 * @brief Represents the player character and manages movement and inventory.
 */
class Player {
public:

    /**
     * @brief Constructs a Player starting in the given room.
     */
    Player(Room* startingRoom);

    /**
     * @brief Adds an item to the player's inventory.
     */
    void addItem(const Item& item) {
        inventory.push_back(item);
    }

    /**
     * @brief Checks whether the player possesses an item by name.
     */
    bool hasItem(const std::string& name) const {
        for (const auto& item : inventory) {
            if (item.getName() == name) return true;
        }
        return false;
    }

    /**
     * @brief Gets the room the player is currently in.
     */
    Room* getCurrentRoom() const { 
        return currentRoom; 
    }

    /**
     * @brief Gets a read-only reference to the player's inventory.
     */
    const std::vector<Item>& getInventory() const { 
        return inventory; 
    }

    /**
     * @brief Directly sets the player's current room (stub movement).
     */
    void moveTo(Room* room) { 
        currentRoom = room; 
    }

    // Returns a pointer to an item in the player's inventory by name. 
    // Used when puzzles or interactions need to modify the actual item object.
    Item* getItem(const std::string& name) {
        for (auto& item : inventory) {
            if (item.getName() == name) {
                return &item;
            }
        }
        return nullptr;
    }

private:
    Room* currentRoom;            // Pointer to the room the player is currently in.
    std::vector<Item> inventory;  // Items the player has collected.
};
