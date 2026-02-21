#include "Room.h"
#include "Item.h"
#include "Puzzle.h"

// Construct a room with a name, description, and no puzzle assigned yet.
Room::Room(const std::string& name, const std::string& description)
    : roomId(-1), name(name), description(description), puzzle(nullptr) {}

// Construct a room with a known database ID.
Room::Room(int id, const std::string& name, const std::string& description)
    : roomId(id), name(name), description(description), puzzle(nullptr)
{
}

// Look up an exit by direction. Returns nullptr if no such exit exists.
Room* Room::getExit(const std::string& direction) {
    auto it = exits.find(direction);
    if (it != exits.end()) {
        return it->second;
    }
    return nullptr;
}

// Retrieve a room object by its display name.
// This enables language interactions like "use console" or "use levers". 
// Returns nullptr if no matching object is present.
RoomObject* Room::getObjectByName(const std::string& name) const {
    for (RoomObject* obj : objects) {
        if (obj->getName() == name) {
            return obj;
        }
    }
    return nullptr;
}