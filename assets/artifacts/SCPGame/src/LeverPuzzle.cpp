#include "LeverPuzzle.h"
#include "Room.h"
#include "MessageFormatter.h"
#include <iostream>

// Display the player's current lever configuration.
void LeverPuzzle::printCurrentState(const std::vector<bool>& ls)
{
    std::cout << "The levers are currently set to: ";
    for (int i = 0; i < ls.size(); i++) {
        std::cout << (ls[i] ? "up" : "down");
        if (i < ls.size() - 1) std::cout << ", ";
    }
    std::cout << ".\n";
}

// Summarize the player's proposed configuration before confirmation.
void LeverPuzzle::printSummary(const std::vector<bool>& newStates)
{
    std::cout << "\nLever positions (";
    for (int i = 0; i < 4; i++) {
        std::cout << (newStates[i] ? "up" : "down");
        if (i < 3) std::cout << ",";
    }
    std::cout << ")\n";
}

bool LeverPuzzle::run(Room* room)
{
    // Ensure this room actually has levers
    if (room->getLeverStates().empty()) {
        MessageFormatter::error("There are no levers here.");
        return false;
    }

    // Show the player the current lever state before prompting changes.
    const auto& ls = room->getLeverStates();
    printCurrentState(ls);

    // Temporary buffer for the player's new configuration.
    std::vector<bool> newStates(4);

    while (true) {
        // Collect input for each lever individually.
        for (int i = 0; i < 4; i++) {
            while (true) {
                std::cout << "Lever " << (i + 1) << " position (up/down): ";
                std::string input;
                std::getline(std::cin, input);

                if (input == "up") {
                    newStates[i] = true;
                    break;
                }
                if (input == "down") {
                    newStates[i] = false;
                    break;
                }

                // Strict "up/down" validation.
                std::cout << "Invalid input. Please type 'up' or 'down'.\n";
            }
        }

        // Show the player what they entered before committing.
        printSummary(newStates);

        // Confirmation
        std::cout << "Is this correct? (yes/no): ";
        std::string confirm;
        std::getline(std::cin, confirm);

        if (confirm == "yes") {
            // Commit the new configuration to the room.
            room->setLeverStates(newStates);
            std::cout << "You set the levers into place.\n";
            return true;
        }

        if (confirm == "no") {
            std::cout << "Let's try again.\n\n";
            continue;
        }

        // Fallback for malformed confirmation input.
        std::cout << "Invalid response. Please type 'yes' or 'no'.\n\n";
    }

    // Unreachable, but kept for structural completeness.
    return true;
}
