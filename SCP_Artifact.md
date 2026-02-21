---
layout: default
---

# C++ SCP Game

This artifact is a multi‑file C++ project consisting of several source, header, and data files.  
To keep this page readable and aligned with UI/UX expectations, only key excerpts are shown below.  
The full project is available here:

**[View Full Original C++ Project](assets/artifacts/SCPGame/)**

## Purpose of the Artifact

This project demonstrates my ability to design and implement a modular, object‑oriented C++ application.  
It includes a database‑driven world model, a puzzle subsystem, a command parser, and a dynamic room navigation system.  
I selected this artifact because it showcases my ability to work with multi‑file architectures, subsystem integration, and data‑driven game logic.


## Project Structure

```
SCPGame/
    main.cpp
    data/
        game.db
    include/
        CommandParser.h
        Containment.h
        DatabaseManager.h
        GameEngine.h
        Item.h
        LeverPuzzle.h
        MessageFormatter.h
        MixerPuzzle.h
        Player.h
        Puzzle.h
        PuzzleAlgorithms.h
        PuzzleEngine.h
        PuzzleType.h
        PuzzleUtils.h
        Room.h
        RoomObject.h
        SprayPuzzle.h
        sqlite3.h
    src/
        CommandParser.cpp
        Containment.cpp
        DatabaseManager.cpp
        GameEngine.cpp
        Item.cpp
        LeverPuzzle.cpp
        MessageFormatter.cpp
        MixerPuzzle.cpp
        Player.cpp
        Puzzle.cpp
        PuzzleAlgorithms.cpp
        PuzzleEngine.cpp
        PuzzleUtils.cpp
        Room.cpp
        SprayPuzzle.cpp
        sqlite3.c
```

## Representative Code Excerpts

### main.cpp

<details>
<summary><strong>Click to view excerpt</strong></summary>

<pre><code>
#include <iostream>
#include "GameEngine.h"

// Entry point for the SCP game engine.
// Sets up the database, constructs the engine, 
// loads the world, and starts the main loop 
int main() {
    try {
        // Initialize database connection
        // All world data, puzzles, and runtime flags originate here.
        DatabaseManager db("data/game.db");

        // Create the game engine with DB dependency
        // The engine coordinates world loading, command routing, and puzzle logic.
        GameEngine engine(&db);

        // Load world data and begin the game loop
        // Once loaded, the engine transitions into the interactive loop.
        engine.loadWorld();
        engine.run();
    }
    catch (const std::exception& ex) {
        // Catch any standard exceptions and report a fatal error.
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return 1;
    }
    catch (...) {
        // Catch-all for unexpected failures.
        std::cerr << "An unknown error occurred.\n";
        return 1;
    }
}

</code></pre>

</details>

<br>
---

### GameEngine.cpp

<details>
<summary><strong>Click to view excerpt</strong></summary>

<pre><code>
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

void GameEngine::loadWorld() {

    // Reset any persistent runtime flags from previous sessions.
    this->db->resetRuntimeState();

    // Load all world data from the database into temporary maps.
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

    // Initialize dynamic puzzle state.
    puzzleEngine->initialize();

    // Ensure rooms loaded correctly.
    if (rooms.empty()) {
        throw std::runtime_error("World load failed: no rooms found in database.");
    }

    // Find the starting room (ID = 1).
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

    // Initialize player in the starting room.
    player = new Player(startingRoom);
}

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
</code></pre>

</details>

<br>
---

### PuzzleEngine.cpp

<details>
<summary><strong>Click to view excerpt</strong></summary>

<pre><code>
#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include "PuzzleEngine.h"
#include "Puzzle.h"
#include "Player.h"
#include "MessageFormatter.h"
#include "Item.h"
#include "DatabaseManager.h"
#include "PuzzleUtils.h"

PuzzleEngine::PuzzleEngine() {
    // Wire puzzle subsystems back to this engine.
    spray.engine = this;
    containment.engine = this;
}

bool PuzzleEngine::runPuzzle(Puzzle& puzzle, Player& player) {

    // avoid re-running its logic and simply acknowledge completion.
    if (puzzle.isSolved()) {
        MessageFormatter::info("You've already solved this puzzle.");
        return true;
    }

    // Delegate puzzle execution to the algorithm dispatcher.
    bool solved = algorithms.run(puzzle, player);

    // Apply reward and mark solved if successful.
    if (solved) {
        puzzle.setSolved(true);
        giveReward(player, puzzle);
    }

    return solved;
}

void PuzzleEngine::giveReward(Player& player, Puzzle& puzzle) {
    // Rewards are simple item grants; empty reward means no item.
    const std::string rewardName = puzzle.getRewardData();
    if (rewardName.empty()) return;

    Item reward(rewardName);
    player.addItem(reward);

    // Reward messaging
    std::cout << "You received: " << reward.getName() << "\n";
}

bool PuzzleEngine::runPuzzleForObject(RoomObject* obj, Player& player, Room* room, bool& running)
{
    const std::string& name = obj->getName();

    // Route special-case objects to their dedicated puzzle handlers.
    if (name == "machine") {
        return mixer.run(player);
    }
    if (name == "levers") {
        return lever.run(room);
    }
    if (name == "module") {
        return runModulePuzzle(player, room, running);
    }

    // Direct puzzle binding via puzzle_id
    int puzzleId = obj->getPuzzleId();
    if (puzzleId >= 0)
        return runDirectPuzzle(obj, player, room, puzzleId, running);

    // Group-based puzzle selection
    const std::string& group = obj->getPuzzleGroup();
    if (!group.empty())
        return runPuzzleGroup(obj, player, room, group, running);

    // Object has no puzzle logic associated with it.
    return false;
}
</code></pre>

</details>

<br>
---

### PuzzleAlgorithms.cpp

<details>
<summary><strong>Click to view excerpt</strong></summary>

<pre><code>
#include "PuzzleAlgorithms.h"
#include "Puzzle.h"
#include "Player.h"
#include "MessageFormatter.h"
#include "Room.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>

// Dispatch puzzle execution based on puzzle type.
bool PuzzleAlgorithms::run(Puzzle& puzzle, Player& player)
{
    const std::string type = puzzle.getType();

    if (type == "sequence") {
        return runSequencePuzzle(puzzle, player);
    }
    if (type == "scramble") {
        return runWordScramblePuzzle(puzzle, player);
    }
    if (type == "logic") {
        return runLogicGatePuzzle(puzzle, player);
    }
    if (type == "maze") {
        return runMiniMazePuzzle(puzzle, player);
    }
    if (type == "pattern") {
        return runPatternPuzzle(puzzle, player);
    }

    // Unknown puzzle types indicate a data or configuration error.
    MessageFormatter::error("Unknown puzzle type: " + type);
    return false;
}

// Displays a numeric sequence with one missing value. 
// Player must supply the missing number.
bool PuzzleAlgorithms::runSequencePuzzle(Puzzle& puzzle, Player& player)
{
    // Parse comma-separated numbers from the puzzle's solution data.
    std::vector<double> seq = parseSequence(puzzle.getSolutionData());

    // Randomly choose which element to hide.
    int missingIndex = rand() % seq.size();
    double correctAnswer = seq[missingIndex];

    // Build the display sequence with a blank placeholder.
    std::vector<std::string> display;
    for (int i = 0; i < seq.size(); i++) {
        if (i == missingIndex)
            display.push_back("_");
        else
            display.push_back(formatNumber(seq[i]));
    }

    std::cout << "The console displays a sequence: ";
    printCommaSeparated(display);

    // Player input
    std::string answer;
    std::getline(std::cin, answer);

    if (answer == formatNumber(correctAnswer)) {
        std::cout << "Correct. The console unlocks.\n";
        return true;
    }

    std::cout << "Incorrect. The console resets.\n";
    return false;
}
</code></pre>

</details>

<br>
---

### Room.cpp

<details>
<summary><strong>Click to view excerpt</strong></summary>

<pre><code>
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
</code></pre>

</details>

<br>
---

### DatabaseManager.cpp

<details>
<summary><strong>Click to view excerpt</strong></summary>

<pre><code>
#include "DatabaseManager.h"
#include <sqlite3.h>
#include <iostream>
#include "Room.h"
#include "Puzzle.h"

// Establish a SQLite connection for all world?loading and state?tracking.
DatabaseManager::DatabaseManager(const std::string& dbPath)
    : connection(nullptr)
{
    int result = sqlite3_open(dbPath.c_str(), reinterpret_cast<sqlite3**>(&connection));

    if (result != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(reinterpret_cast<sqlite3*>(connection)) << "\n";
        connection = nullptr;
    }
}

// Cleanly close the SQLite connection when the manager is destroyed.
DatabaseManager::~DatabaseManager()
{
    if (connection != nullptr) {
        sqlite3_close(reinterpret_cast<sqlite3*>(connection));
        connection = nullptr;
    }
}

// Load all rooms from the database and construct Room objects. 
// Rooms are keyed by name for natural-language lookup, but later 
// subsystems search by ID.
std::map<std::string, Room*> DatabaseManager::loadRooms()
{
    std::map<std::string, Room*> rooms;

    if (connection == nullptr) {
        std::cerr << "Database not connected. Cannot load rooms.\n";
        return rooms;
    }

    const char* sql = "SELECT room_id, name, description FROM rooms;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(connection), sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare room query: "
            << sqlite3_errmsg(reinterpret_cast<sqlite3*>(connection)) << "\n";
        return rooms;
    }

    // Step through each row
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int roomId = sqlite3_column_int(stmt, 0);
        const unsigned char* nameText = sqlite3_column_text(stmt, 1);
        const unsigned char* descText = sqlite3_column_text(stmt, 2);

        std::string name = nameText ? reinterpret_cast<const char*>(nameText) : "";
        std::string description = descText ? reinterpret_cast<const char*>(descText) : "";

        // Create the Room object from DB fields.
        Room* room = new Room(roomId, name, description);

        // Maintenance Room initializes lever states at load time.
        if (room->getName() == "Maintenance Room") {
            room->setLeverStates({ false, false, false, false });
        }

        // Insert into map keyed by room name
        rooms[name] = room;
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Error while stepping through rooms: "
            << sqlite3_errmsg(reinterpret_cast<sqlite3*>(connection)) << "\n";
    }

    sqlite3_finalize(stmt);
    return rooms;
}
</code></pre>

</details>

---

[<- Back to Portfolio](index.md)
