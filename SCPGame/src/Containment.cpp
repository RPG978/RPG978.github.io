#include "Containment.h"
#include "MessageFormatter.h"
#include "PuzzleEngine.h"

bool Containment::handleLook(Room* room, DatabaseManager* db)
{
    // Only trigger in Containment Room (Room 5)
    if (room->getId() != 5)
        return false;

    // Boss defeated
    if (db->getFlag("boss_defeated")) {
        MessageFormatter::info("You are in the containment room.");
        MessageFormatter::info("The boss lies defeated. The room is silent.");
        return true;
    }

    // Barrier active
    if (room->bossBarrierActive) {
        MessageFormatter::info("SCP-1048-A is behind a soundproof barrier. It appears to vibrate, but you hear nothing.");
        MessageFormatter::info("There is a module in the corner. 'Needs Access Chip' is blinking across the display.");
        MessageFormatter::info("You see:\n - module");
        return true;
    }

    // Fallback
    MessageFormatter::info("The room is dark. Something is wrong.");
    return true;
}

bool Containment::handleEntry(Room* room, bool& running, DatabaseManager* db, std::map<std::string, Room*>* rooms) {
    // Only trigger in Containment Room (Room 5)
    if (room->getId() != 5)
        return false;

    // We need the Maintenance Room to check lever states
    Room* maint = (*rooms)["Maintenance Room"]; 
    if (!maint) {
        MessageFormatter::error("Internal error: maintenance room not found.");
        return true;
    }

    const auto& ls = maint->getLeverStates();
    const auto& solution = engine->getLeverSolution();

    bool correct = (ls == solution);

    if (!correct) {
        MessageFormatter::info("You are in SCP-1048-A's containment room.");
        MessageFormatter::info("You failed to configure the levers correctly.");
        MessageFormatter::info("A lonely teddy bear stands in front of you. It is made of human ears.");
        MessageFormatter::info("You hear a high-pitched shriek.");
        MessageFormatter::info("Pain erupts from every orifice. Ears begin to grow all over your body.");
        MessageFormatter::info("The last thing you recognize is the voice over the intercom:");
        MessageFormatter::info("\"Test 4 - AC complete. Subject terminated.\"");
        MessageFormatter::info("You died. Game Over.");

        running = false;
        return true;
    }

    // Correct lever configuration: activate barrier
    room->bossBarrierActive = true;
    return true;
}
