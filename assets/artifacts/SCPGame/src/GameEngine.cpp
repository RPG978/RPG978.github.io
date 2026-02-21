#include "GameEngine.h"
#include "Player.h"
#include "Room.h"
#include "PuzzleEngine.h"
#include "DatabaseManager.h"
#include "MessageFormatter.h"
#include "Item.h"
#include "Puzzle.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>

GameEngine::GameEngine(DatabaseManager* db)
    : db(db), player(nullptr), puzzleEngine(nullptr)
{
    // Seed global RNG for any randomized puzzle behavior
    srand(static_cast<unsigned>(time(nullptr)));

    // Construct the puzzle engine and wire shared dependencies.
    puzzleEngine = new PuzzleEngine();
    puzzleEngine->db = db; // Shared database connection
}

Room* GameEngine::getRoomById(int id)
{
    // Linear search through loaded rooms.
    for (auto& pair : rooms) {
        Room* room = pair.second;
        if (room->getId() == id)
            return room;
    }
    return nullptr;
}

void GameEngine::run() {
    // Display the game banner and begin the interactive loop.
    MessageFormatter::banner();

    std::string input;
    while (running) {
        std::cout << "> ";
        std::getline(std::cin, input);

        // Delegate parsing and dispatching to the command handler.
        processCommand(input);
    }
}

void GameEngine::loadWorld() {

    // Reset any persistent runtime flags from previous sessions.
    this->db->resetRuntimeState();

    // Load all world data from the database into temporary maps. 
    // These are then transferred into the engine's persistent state. 
    auto rooms = this->db->loadRooms();
    this->db->loadRoomConnections(rooms);
    this->db->loadLockedExits(rooms);
    this->db->loadRoomObjects(rooms);
    this->db->loadRoomItems(rooms);

    // Store rooms in the engine's persistent map.
    this->rooms = rooms;

    // Load all puzzles and hand them to PuzzleEngine.
    puzzles = this->db->loadPuzzles();
    puzzleEngine->setRooms(&this->rooms);
    puzzleEngine->puzzles = puzzles;

    // Initialize dynamic puzzle state
    puzzleEngine->initialize();

    // Ensure rooms loaded correctly 
    if (rooms.empty()) { 
        throw std::runtime_error("World load failed: no rooms found in database."); 
    }

    // Find the starting room (ID = 1)
    Room* startingRoom = nullptr;
    for (auto& pair : rooms) {
        if (pair.second->getId() == 1) {
            startingRoom = pair.second;
            break;
        }
    }

    if (!startingRoom) { 
        throw std::runtime_error("World load failed: starting room (ID 1) not found."); 
    }

    // Initialize player in the starting room
    player = new Player(startingRoom);
}

// Parse the player's input and route to the appropriate handler.
void GameEngine::processCommand(const std::string& input) {
    CommandParser::Command cmd = parser.parse(input);

    if (cmd.verb == "go") {
        handleGoCommand(cmd.argument);
    }
    else if (cmd.verb == "look") {
        handleLookCommand();
    }
    else if (cmd.verb == "take") {
        handleTakeCommand(cmd.argument);
    }
    else if (cmd.verb == "use") {
        handleUseCommand(cmd.argument);
    }
    else if (cmd.verb == "inventory") {
        handleInventoryCommand();
    }
    else if (cmd.verb == "help") {
        handleHelpCommand();
    }
    else if (cmd.verb == "quit") {
        handleQuitCommand();
    }
    else {
        MessageFormatter::error("I don't understand that command. Type 'help' for options.");
    }
    std::cout << "\n"; // spacing rule
}

void GameEngine::handleGoCommand(const std::string& direction)
{
    Room* current = player->getCurrentRoom();
    Room* next = current->getExit(direction);

    // No exit in that direction
    if (!next) {
        MessageFormatter::error("You can't go that way.");
        return;
    }

    // Exit exists but is locked
    if (current->isExitLocked(direction)) {
        MessageFormatter::error("The door is locked.");
        return;
    }

    // Move the player
    player->moveTo(next);
    MessageFormatter::roomEntry(*next);

    // Special-case logic for containment room entry
    if (next->getId() == 5) {  // Containment Room ID
        puzzleEngine->runContainmentEntry(next, running);
    }
}

void GameEngine::handleLookCommand()
{
    Room* room = player->getCurrentRoom();

    // Let PuzzleEngine handle any dynamic/special look logic.
    if (puzzleEngine->runSpecialLook(room))
        return;

    // Default room description
    MessageFormatter::roomDescription(*room);

    // Show room objects
    const auto& objects = room->getObjects();
    if (!objects.empty()) {
        MessageFormatter::info("You see:");
        for (auto* obj : objects) {
            MessageFormatter::info(" - " + obj->getName());
        }
    }
}

void GameEngine::handleTakeCommand(const std::string& itemName) {
    Room* room = player->getCurrentRoom();

    // Stone tablets cannot be taken
    if (itemName == "stone tablets") {
        MessageFormatter::error("You can't take these with you.");
        return;
    }

    // Item not present in the room
    if (!room->hasItem(itemName)) {
        MessageFormatter::error("There is no " + itemName + " here.");
        return;
    }

    // Take the item
    Item item = room->takeItem(itemName);
    player->addItem(item);

    MessageFormatter::itemPickup(itemName);
}

void GameEngine::handleInventoryCommand() {
    MessageFormatter::inventory(*player);
}

void GameEngine::handleHelpCommand() {
    MessageFormatter::help();
}

void GameEngine::handleQuitCommand() {
    std::cout << "Are you sure you want to quit? (yes/no): ";

    std::string response;
    std::getline(std::cin, response);

    CommandParser::Command confirm = parser.parse(response);

    if (confirm.verb == "yes" || confirm.verb == "y") {
        std::cout << "Goodbye.\n";
        running = false;
    }
    else {
        MessageFormatter::info("Returning to game.");
    }
}

void GameEngine::handleUseCommand(const std::string& target)
{
    // Whitelist: Valid use targets
    static const std::unordered_set<std::string> validUseTargets = {
        "levers", "stone tablets", "spray", "spray bottle",
        "access chip", "console", "keycard", "mixer",
        "machine", "module"
    };

    if (!validUseTargets.count(target)) {
        MessageFormatter::error("I don't understand that.");
        return;
    }

    Room* room = player->getCurrentRoom();
    std::string effectiveTarget = target;

    // Special case: keycard acts as console in Control Room
    if (effectiveTarget == "keycard" && room->getName() == "Control Room") {
        effectiveTarget = "console";
    }

    // Route ALL use logic to PuzzleEngine
    if (puzzleEngine->runUseCommand(effectiveTarget, *player, room, running))
        return;

    // If PuzzleEngine didn't handle it:
    MessageFormatter::error("You can't use that here.");
}
