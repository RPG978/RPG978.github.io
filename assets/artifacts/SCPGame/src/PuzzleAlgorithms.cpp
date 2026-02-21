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

// Scrambles a word once per game session. Player must unscramble it.
bool PuzzleAlgorithms::runWordScramblePuzzle(Puzzle& puzzle, Player&)
{
    std::string word = puzzle.getSolutionData();

    if (word.empty()) {
        std::cout << "Puzzle error: no word provided.\n";
        return false;
    }

    // Generate the scramble only once per playthrough.
    if (puzzle.getScrambledWord().empty()) { 
        std::string scrambled = word; 
        
        std::random_device rd; 
        std::mt19937 g(rd()); 
        std::shuffle(scrambled.begin(), scrambled.end(), g); 
        
        puzzle.setScrambledWord(scrambled); 
    }

    // Prompt comes from the database, allowing per-puzzle flavor text.
    std::cout << puzzle.getPrompt() << " " << puzzle.getScrambledWord() << "\n";
    std::cout << "Your answer: ";

    std::string answer;
    std::getline(std::cin, answer);

    if (answer == word) {
        std::cout << "The letters glow. You hear a click as a hidden compartment in the table opens.\n";
        return true;
    }

    std::cout << "Nothing happens.\n";
    return false;
}

// Placeholder implementations for future puzzle types. 
// These return false until fully implemented.
bool PuzzleAlgorithms::runLogicGatePuzzle(Puzzle&, Player&)
{
    return false;
}

bool PuzzleAlgorithms::runMiniMazePuzzle(Puzzle&, Player&)
{
    return false;
}

bool PuzzleAlgorithms::runPatternPuzzle(Puzzle&, Player&)
{
    return false;
}

/******************
* UTILITY FUNCTIONS
******************/

// Parse comma-separated numeric values
std::vector<double> PuzzleAlgorithms::parseSequence(const std::string& data)
{
    std::vector<double> seq;
    std::stringstream ss(data);
    std::string token;

    while (std::getline(ss, token, ',')) {
        seq.push_back(std::stod(token));
    }

    return seq;
}

// Print a vector of strings as a comma-separated list.
void PuzzleAlgorithms::printCommaSeparated(const std::vector<std::string>& items)
{
    for (size_t i = 0; i < items.size(); i++) {
        std::cout << items[i];
        if (i < items.size() - 1)
            std::cout << ", ";
    }
    std::cout << "\n";
}

// Format a floating-point number by trimming trailing zeros.
std::string PuzzleAlgorithms::formatNumber(double value)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(6) << value;
    std::string s = out.str();

    // Remove trailing zeros.
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);

    // Remove trailing decimal point if present.
    if (!s.empty() && s.back() == '.') {
        s.pop_back();
    }

    return s;
}
