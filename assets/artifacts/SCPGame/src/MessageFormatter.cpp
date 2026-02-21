#include "MessageFormatter.h"
#include "Room.h"
#include "Player.h"
#include "Item.h"
#include "Puzzle.h"
#include <iostream>

// Display the game banner and opening narrative.
void MessageFormatter::banner() {
    std::cout << "=============================================\n";
    std::cout << "  SCP Foundation: Secure, Contain, Protect.\n";
    std::cout << "=============================================\n\n";

    std::cout << "You have been escorted into a dark room by armed guards. They leave, locking the door.\n";
    std::cout << "After a few moments the lights click on and you hear a voice over the intercom:\n\n";
    std::cout << "\"D-class personnel, you have now entered the containment site for SCP-1048-A.\n";
    std::cout << "Today, you will demonstrate the new containment procedures.\n";
    std::cout << "A series of puzzles have been developed to ensure limited access to the subject.\n";
    std::cout << "Be sure not to enter the containment area before ensuring the protocol has been implemented\n";
    std::cout << "Following successful interaction with the SCP, you will be given a full pardon.\"\n\n";

    std::cout << "Type 'help' for a list of commands.\n\n";

}

// Announce room entry
void MessageFormatter::roomEntry(const Room& room) {
    std::cout << "You enter the " << room.getName() << ".\n";
}

// Print the room's description and any visible items.
void MessageFormatter::roomDescription(const Room& room) {
    std::cout << room.getDescription() << "\n";

    const auto& items = room.getItems();
    if (!items.empty()) {
        std::cout << "You see:\n";
        for (const auto& item : items) {
            std::cout << " - " << item.getName() << "\n";
        }
    }
}

// Display the player's inventory
void MessageFormatter::inventory(const Player& player) {
    const auto& items = player.getInventory();

    if (items.empty()) {
        std::cout << "Inventory: empty\n";
        return;
    }

    std::cout << "Inventory:\n";
    for (const auto& item : items) {
        std::cout << " - " << item.getDisplayName() << "\n";
    }
}

// Standardized pickup message.
void MessageFormatter::itemPickup(const std::string& itemName) {
    std::cout << "You pick up the " << itemName << ".\n";
}

// Used when the player attempts to take an item they already have.
void MessageFormatter::itemAlreadyOwned(const std::string& itemName) {
    std::cout << "You already have the " << itemName << ".\n";
}

// Error messages are prefixed for clarity and consistency.
void MessageFormatter::error(const std::string& message) {
    std::cout << "Error: " << message << "\n";
}

// Generic informational output.
void MessageFormatter::info(const std::string& message) {
    std::cout << message << "\n";
}

// Display available commands.
void MessageFormatter::help() {
    std::cout << "Available commands:\n";
    std::cout << "  look            - Examine your surroundings\n";
    std::cout << "  go <direction>  - Move north, south, east, or west\n";
    std::cout << "  take <item>     - Pick up an item in the room\n";
    std::cout << "  use <target>    - Use an item or interact with something\n";
    std::cout << "  inventory       - View items you are carrying\n";
    std::cout << "  help            - Show this help message\n";
    std::cout << "  quit            - Exit the game\n";
}

