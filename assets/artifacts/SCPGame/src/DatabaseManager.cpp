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

// Load directional exits between rooms. 
// Each row defines a one-way connection
void DatabaseManager::loadRoomConnections(std::map<std::string, Room*>& rooms)
{
    if (connection == nullptr) {
        std::cerr << "Database not connected. Cannot load room connections.\n";
        return;
    }

    const char* sql =
        "SELECT room_id, direction, target_room_id "
        "FROM room_connections;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(connection), sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare room_connections query: "
            << sqlite3_errmsg(reinterpret_cast<sqlite3*>(connection)) << "\n";
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int roomId = sqlite3_column_int(stmt, 0);

        const unsigned char* dirText = sqlite3_column_text(stmt, 1);
        const unsigned char* targetText = sqlite3_column_text(stmt, 2);

        std::string direction = dirText ? reinterpret_cast<const char*>(dirText) : "";
        int targetId = targetText ? sqlite3_column_int(stmt, 2) : -1;

        // Find the source and target rooms by ID
        // This uses a linear search because rooms are keyed by name 
        // and world size is still small.
        Room* source = nullptr;
        Room* target = nullptr;

        for (auto& pair : rooms) {
            if (pair.second->getId() == roomId){
                source = pair.second;
            }
            if (pair.second->getId() == targetId){
                target = pair.second;
            }
        }

        if (!source || !target) {
            std::cerr << "Warning: connection references unknown room_id(s): "
                << roomId << " -> " << targetId << "\n";
            continue;
        }

        // Add the exit
        source->addExit(direction, target);
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Error while stepping through room_connections: "
            << sqlite3_errmsg(reinterpret_cast<sqlite3*>(connection)) << "\n";
    }

    sqlite3_finalize(stmt);
}

// Load exits that start in a locked state.
void DatabaseManager::loadLockedExits(std::map<std::string, Room*>& rooms)
{
    if (connection == nullptr) {
        std::cerr << "Database not connected. Cannot load locked exits.\n";
        return;
    }

    const char* sql =
        "SELECT room_id, direction "
        "FROM locked_exits "
        "WHERE is_locked = 1;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(connection), sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare locked_exits query: "
            << sqlite3_errmsg(reinterpret_cast<sqlite3*>(connection)) << "\n";
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int roomId = sqlite3_column_int(stmt, 0);

        const unsigned char* dirText = sqlite3_column_text(stmt, 1);
        std::string direction = dirText ? reinterpret_cast<const char*>(dirText) : "";

        // Find the room by ID
        Room* room = nullptr;
        for (auto& pair : rooms) {
            if (pair.second->getId() == roomId) {
                room = pair.second;
                break;
            }
        }

        if (!room) {
            std::cerr << "Warning: locked exit references unknown room_id: "
                << roomId << "\n";
            continue;
        }

        room->lockExit(direction);
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Error while stepping through locked_exits: "
            << sqlite3_errmsg(reinterpret_cast<sqlite3*>(connection)) << "\n";
    }

    sqlite3_finalize(stmt);
}

// Load interactive objects
// Objects may reference direct puzzles or puzzle groups.
void DatabaseManager::loadRoomObjects(std::map<std::string, Room*>& rooms)
{
    const char* sql =
        "SELECT object_id, room_id, name, puzzle_id, puzzle_group "
        "FROM room_objects;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(connection), sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare room_objects query: "
            << sqlite3_errmsg(reinterpret_cast<sqlite3*>(connection)) << "\n";
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int objectId = sqlite3_column_int(stmt, 0);
        int roomId = sqlite3_column_int(stmt, 1);

        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        int puzzleId = sqlite3_column_type(stmt, 3) == SQLITE_NULL
            ? -1
            : sqlite3_column_int(stmt, 3);

        std::string puzzleGroup;
        if (sqlite3_column_type(stmt, 4) != SQLITE_NULL) {
            puzzleGroup = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        }

        // Find the room by ID
        Room* room = nullptr;
        for (auto& pair : rooms) {
            if (pair.second->getId() == roomId) {
                room = pair.second;
                break;
            }
        }

        if (!room) {
            std::cerr << "Warning: room_objects references unknown room_id: "
                << roomId << "\n";
            continue;
        }

        // Create the object
        RoomObject* obj = new RoomObject(objectId, name, puzzleId, puzzleGroup);

        // Attach it to the room
        room->addObject(obj);
    }

    sqlite3_finalize(stmt);
}

// Load items placed in rooms at the start of the game. 
// Items are looked up by ID and attached to the room.
void DatabaseManager::loadRoomItems(std::map<std::string, Room*>& rooms)
{
    const char* sql =
        "SELECT room_id, item_id "
        "FROM room_items;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2((sqlite3*)connection, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare room_items query: "
            << sqlite3_errmsg((sqlite3*)connection) << "\n";
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int roomId = sqlite3_column_int(stmt, 0);
        int itemId = sqlite3_column_int(stmt, 1);

        // Find room
        Room* room = nullptr;
        for (auto& pair : rooms) {
            if (pair.second->getId() == roomId) {
                room = pair.second;
                break;
            }
        }

        if (!room) continue;

        // Load item
        Item item = getItemById(itemId);
        room->addItem(item);
    }

    sqlite3_finalize(stmt);
}

// Load all puzzles from the database. 
// Puzzles are keyed by puzzle_id and later routed by PuzzleEngine.
std::map<int, Puzzle*> DatabaseManager::loadPuzzles()
{
    std::map<int, Puzzle*> result;

    const char* sql =
        "SELECT puzzle_id, puzzle_type, puzzle_group, name, room_id, trigger, "
        "prompt, solution_data, reward_data, repeatable "
        "FROM puzzles;";

    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(connection), sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare puzzles query: "
            << sqlite3_errmsg(reinterpret_cast<sqlite3*>(connection)) << "\n";
        return result;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string group = sqlite3_column_type(stmt, 2) == SQLITE_NULL ? "" :
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string name = sqlite3_column_type(stmt, 3) == SQLITE_NULL ? "" :
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        int roomId = sqlite3_column_type(stmt, 4) == SQLITE_NULL ? -1 :
            sqlite3_column_int(stmt, 4);
        std::string trigger = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        std::string prompt = sqlite3_column_type(stmt, 6) == SQLITE_NULL ? "" :
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        std::string solution = sqlite3_column_type(stmt, 7) == SQLITE_NULL ? "" :
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        std::string reward = sqlite3_column_type(stmt, 8) == SQLITE_NULL ? "" :
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        bool repeatable = sqlite3_column_int(stmt, 9) != 0;

        Puzzle* puzzle = new Puzzle(
            id, type, group, name, roomId, trigger,
            prompt, solution, reward, repeatable
        );

        result[id] = puzzle;
    }

    sqlite3_finalize(stmt);
    return result;
}

// Retrieve a boolean runtime flag
bool DatabaseManager::getFlag(const std::string& name)
{
    sqlite3* db = reinterpret_cast<sqlite3*>(connection);

    const char* sql = "SELECT flag_value FROM flags WHERE flag_name = ?;";
    sqlite3_stmt* stmt = nullptr;

    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    bool value = false;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        value = sqlite3_column_int(stmt, 0) != 0;
    }

    sqlite3_finalize(stmt);
    return value;
}

// Set or update a runtime flag using SQLite UPSERT semantics.
void DatabaseManager::setFlag(const std::string& name, bool value)
{
    sqlite3* db = reinterpret_cast<sqlite3*>(connection);

    const char* sql =
        "INSERT INTO flags (flag_name, flag_value) VALUES (?, ?) "
        "ON CONFLICT(flag_name) DO UPDATE SET flag_value = excluded.flag_value;";

    sqlite3_stmt* stmt = nullptr;

    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, value ? 1 : 0);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Check whether a puzzle group has been solved. 
// PuzzleEngine uses this to avoid re-running group puzzles.
bool DatabaseManager::isPuzzleGroupSolved(const std::string& group) {
    sqlite3* db = static_cast<sqlite3*>(connection);

    const char* sql = "SELECT solved FROM puzzle_groups WHERE group_name = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, group.c_str(), -1, SQLITE_TRANSIENT);

    bool solved = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        solved = sqlite3_column_int(stmt, 0) != 0;
    }

    sqlite3_finalize(stmt);
    return solved;
}

// Mark a puzzle group as solved or unsolved. 
// Used when a group puzzle is completed.
void DatabaseManager::setPuzzleGroupSolved(const std::string& group, bool solved) {
    sqlite3* db = static_cast<sqlite3*>(connection);

    const char* sql =
        "INSERT INTO puzzle_groups (group_name, solved) VALUES (?, ?) "
        "ON CONFLICT(group_name) DO UPDATE SET solved = excluded.solved;";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return;

    sqlite3_bind_text(stmt, 1, group.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, solved ? 1 : 0);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Reset all puzzle and puzzle-group solved states. 
// Called at the start of each playthrough to ensure a clean session.
void DatabaseManager::resetRuntimeState() {
    sqlite3* conn = static_cast<sqlite3*>(connection);

    sqlite3_exec(conn, "UPDATE puzzles SET solved = 0;", nullptr, nullptr, nullptr);
    sqlite3_exec(conn, "UPDATE puzzle_groups SET solved = 0;", nullptr, nullptr, nullptr);
}

// Retrieve all exits unlocked by a specific puzzle. 
// This supports multi-exit unlocks if needed.
std::vector<ExitUnlock> DatabaseManager::getExitsUnlockedByPuzzle(int puzzleId)
{
    std::vector<ExitUnlock> results;

    const char* sql =
        "SELECT room_id, direction "
        "FROM puzzle_unlocks_exit "
        "WHERE puzzle_id = ?;";

    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(connection), sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare puzzle_unlocks_exit query: "
            << sqlite3_errmsg(reinterpret_cast<sqlite3*>(connection)) << "\n";
        return results;
    }

    sqlite3_bind_int(stmt, 1, puzzleId);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        ExitUnlock u;
        u.roomId = sqlite3_column_int(stmt, 0);

        const unsigned char* dirText = sqlite3_column_text(stmt, 1);
        u.direction = dirText ? reinterpret_cast<const char*>(dirText) : "";

        results.push_back(u);
    }

    sqlite3_finalize(stmt);
    return results;
}

// Load a single item definition by ID. 
// Used when populating room inventories.
Item DatabaseManager::getItemById(int itemId)
{
    if (!connection) {
        std::cerr << "Database not connected. Cannot load item.\n";
        return Item("");
    }

    const char* sql =
        "SELECT name, description FROM items WHERE item_id = ?;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2((sqlite3*)connection, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare item query: "
            << sqlite3_errmsg((sqlite3*)connection) << "\n";
        return Item("");
    }

    sqlite3_bind_int(stmt, 1, itemId);

    Item result("");

    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char* nameText = sqlite3_column_text(stmt, 0);
        const unsigned char* descText = sqlite3_column_text(stmt, 1);

        std::string name = nameText ? reinterpret_cast<const char*>(nameText) : "";
        std::string desc = descText ? reinterpret_cast<const char*>(descText) : "";

        result = Item(name, desc);
    }

    sqlite3_finalize(stmt);
    return result;
}

// Persist a puzzle's solved state to the database. 
// Called whenever a puzzle is successfully completed.
void DatabaseManager::savePuzzleState(const Puzzle& p)
{
    sqlite3* db = reinterpret_cast<sqlite3*>(connection);

    const char* sql =
        "UPDATE puzzles SET solved = ? WHERE puzzle_id = ?;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return;

    sqlite3_bind_int(stmt, 1, p.isSolved() ? 1 : 0);
    sqlite3_bind_int(stmt, 2, p.getId());

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Retrieve a random lever pattern for this playthrough. 
// Patterns are stored as comma-separated strings.
std::string DatabaseManager::getRandomLeverPattern()
{
    sqlite3* db = reinterpret_cast<sqlite3*>(connection);

    const char* sql =
        "SELECT pattern FROM lever_patterns ORDER BY RANDOM() LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;
    std::string result;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return result;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        if (text) result = reinterpret_cast<const char*>(text);
    }

    sqlite3_finalize(stmt);
    return result;
}

// Retrieve the required spray color for this playthrough. 
// Used by MixerPuzzle and dynamic room description injection.
std::string DatabaseManager::getRandomRequiredSprayColor() {
    sqlite3* db = reinterpret_cast<sqlite3*>(connection);

    sqlite3_stmt* stmt = nullptr;
    const char* sql = 
        "SELECT required_color FROM mixer_requirements ORDER BY RANDOM() LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return "BLUE"; // safe fallback
    }

    std::string color = "BLUE"; // default fallback

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        if (text) {
            color = reinterpret_cast<const char*>(text);
        }
    }

    sqlite3_finalize(stmt);
    return color;
}
