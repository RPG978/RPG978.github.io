#include "Player.h"
#include "Room.h"
#include "Item.h"

// Initialize the player in the given starting room. 
Player::Player(Room* startingRoom)
    : currentRoom(startingRoom) {}
