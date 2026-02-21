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

bool PuzzleEngine::runModulePuzzle(Player& player, Room* room, bool& running)
{
    // Access chip is required to activate the module.
    if (!player.hasItem("access chip")) {
        MessageFormatter::info("The module flashes: 'Needs Access Chip'.");
        return false;
    }

    // Mark containment as cleared.
    room->bossDefeated = true;

    // Endgame sequence
    MessageFormatter::info("All lights in the containment room turn green.");
    MessageFormatter::info("After some time, a voice comes over the intercom:");
    MessageFormatter::info("\"D-Class personnel, return to the entrance. Today, you have earned your freedom.\"");
    MessageFormatter::info("You leave Site-24 alive, and as a free person.");
    MessageFormatter::info("You're Winner!!!");

    // End the game loop
    running = false;
    return true;
}

void PuzzleEngine::unlockExitsForPuzzle(int)
{
    // Unlock all exits in all rooms.
    if (!rooms) return;

    for (auto& pair : *rooms) {
        Room* room = pair.second;
        for (auto& exitPair : room->getLockedExits()) {
            room->unlockExit(exitPair.first);
        }
    }
}

bool PuzzleEngine::runSpecialLook(Room* room)
{
    // Allow containment subsystem to override look behavior.
    return containment.handleLook(room, db);
}

void PuzzleEngine::runContainmentEntry(Room* room, bool& running)
{
    // Trigger containment entry logic
    containment.handleEntry(room, running, db, rooms);
}

bool PuzzleEngine::runUseCommand(const std::string& target, Player& player, Room* room, bool& running)
{
    // Primary: object-based use
    if (RoomObject* obj = room->getObjectByName(target)) {
        return runPuzzleForObject(obj, player, room, running);
    }

    // Secondary: direct subsystem routing
    if (target == "levers") {
        return lever.run(room);
    }

    if (target == "spray" || target == "spray bottle") {
        return spray.run(player, room);
    }

    if (target == "access chip") {
        return runAccessChipRouting(room, player, running);
    }

    // Not handled
    return false;
}

bool PuzzleEngine::runAccessChipRouting(Room* room, Player& player, bool& running)
{
    // Using the access chip attempts to activate the module if present.
    for (auto* obj : room->getObjects()) {
        if (obj->getName() == "module") {
            return runPuzzleForObject(obj, player, room, running);
        }
    }

    // Chip is valid, but not useful in this room.
    MessageFormatter::info("That doesn't seem useful here.");
    return true;
}

bool PuzzleEngine::runDirectPuzzle(RoomObject* obj, Player& player, Room* room, int puzzleId, bool& running)
{
    const std::string& name = obj->getName();

    Puzzle* p = puzzles[puzzleId];
    if (!p) {
        MessageFormatter::error("Puzzle not found for object.");
        return false;
    }

    // Console requires keycard before puzzle can run.
    if (name == "console" && !player.hasItem("keycard")) {
        MessageFormatter::info("The console is locked. It needs a keycard.");
        return false;
    }

    bool solved = runPuzzle(*p, player);

    if (solved) {
        p->setSolved(true);
        this->db->savePuzzleState(*p);

        if (name == "console") {
            this->db->setFlag("console_unlocked", true);
            unlockExitsForPuzzle(puzzleId);
        }
    }

    return solved;
}

bool PuzzleEngine::runPuzzleGroup(RoomObject* obj, Player& player, Room* room, const std::string& group, bool& running)
{
    const std::string& name = obj->getName();

    // Console group requires keycard before any puzzle in the group can run.
    if (name == "console" && !player.hasItem("keycard")) {
        MessageFormatter::info("The console is locked. It needs a keycard.");
        return true;
    }

    // If the entire group was solved earlier, avoid re-running.
    if (this->db->isPuzzleGroupSolved(group)) {
        MessageFormatter::info("You've already solved this puzzle.");
        return true;
    }

    // Collect all puzzles in this group for this room.
    std::vector<Puzzle*> groupPuzzles;
    for (auto& pair : puzzles) {
        Puzzle* p = pair.second;
        if (p->getGroup() == group && p->getRoomId() == room->getId()) {
            groupPuzzles.push_back(p);
        }
    }

    if (groupPuzzles.empty()) {
        MessageFormatter::error("No puzzles found for this object.");
        return false;
    }

    Puzzle* selected = nullptr;

    // Special case: tablet_group must be consistent per game
    if (group == "tablet_group") {
        if (activeTabletPuzzleId == -1) {
            // First time: choose and cache
            selected = groupPuzzles[rand() % groupPuzzles.size()];
            activeTabletPuzzleId = selected->getId();
        }
        else {
            // Later uses: return cached puzzle
            selected = puzzles[activeTabletPuzzleId];
        }
    }
    else {
        // All other puzzle groups behave normally
        selected = groupPuzzles[rand() % groupPuzzles.size()];
    }

    MessageFormatter::info("You examine the " + name + "...");
    bool solved = runPuzzle(*selected, player);

    if (solved) {
        // Mark entire group as solved.
        this->db->setPuzzleGroupSolved(group, true);

        for (Puzzle* p : groupPuzzles) {
            p->setSolved(true);
            this->db->savePuzzleState(*p);
        }

        // Console unlocks facility-wide exits.
        if (name == "console") {
            this->db->setFlag("console_unlocked", true);
            unlockExitsForPuzzle(selected->getId());
        }
    }

    return solved;
}

void PuzzleEngine::initialize() {
    // Choose lever pattern and required spray color once per game.
    leverPatternRaw = db->getRandomLeverPattern();
    std::string rawColor = db->getRandomRequiredSprayColor();

    // Parse lever pattern into boolean vector.
    leverSolution = parsePattern(leverPatternRaw);
    requiredSprayColor = MixerPuzzle::toSprayColor(rawColor);

    // Insert the required spray color into the Maintenance Room description.
    std::string actualColor = getRequiredSprayColorLower();

    Room* maint = (*rooms)["Maintenance Room"];
    if (maint) { 
        std::string desc = maint->getDescription(); 
        
        const std::string placeholder = "{color}"; 
        size_t pos = desc.find(placeholder); 
        
        if (pos != std::string::npos) { 
            desc.replace(pos, placeholder.length(), actualColor); 
            maint->setDescription(desc); 
        } 
    }
}

std::string PuzzleEngine::getFormattedLeverPattern() const {
    return formatPattern(leverPatternRaw);
}

const std::vector<bool>& PuzzleEngine::getLeverSolution() const {
    return leverSolution;
}

std::string PuzzleEngine::getRequiredSprayColorLower() const {
    switch (requiredSprayColor) {
    case SprayColor::Red:   return "red";
    case SprayColor::Green: return "green";
    case SprayColor::Blue:  return "blue";
    case SprayColor::Black: return "black";
    default:                return "unknown";
    }
}