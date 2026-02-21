#include "MixerPuzzle.h"
#include "Player.h"
#include "Item.h"
#include "CommandParser.h"
#include "MessageFormatter.h"
#include <algorithm>
#include <iostream>

// Convert a dial's numeric setting into a readable label.
std::string MixerPuzzle::dialName(int v) {
    switch (v) {
    case 1: return "low";
    case 2: return "medium";
    case 3: return "high";
    }
    return "unknown";
}

// Determine the resulting mixture color based on the sum of all dial values.
std::string MixerPuzzle::getMixtureColor(const MixerState& m) {
    int sum = m.dialA + m.dialB + m.dialC;

    if (sum <= 4) return "RED";
    if (sum <= 6) return "GREEN";
    if (sum <= 8) return "BLUE";
    return "BLACK";
}

// Convert a mixture color string into the internal SprayColor enum.
SprayColor MixerPuzzle::toSprayColor(const std::string& color) {
    if (color == "RED") return SprayColor::Red;
    if (color == "GREEN") return SprayColor::Green;
    if (color == "BLUE") return SprayColor::Blue;
    return SprayColor::Black;
}

bool MixerPuzzle::run(Player& player) {
    // Local mixer state, each dial starts at "low"
    MixerState state;
    std::string raw;
    CommandParser parser;

    while (true) {
        // Display current mixer configuration.
        MessageFormatter::info("Reagent Mixer - Calibration Mode\n");
        MessageFormatter::info("Dial A: " + dialName(state.dialA));
        MessageFormatter::info("Dial B: " + dialName(state.dialB));
        MessageFormatter::info("Dial C: " + dialName(state.dialC));

        // Show the resulting mixture color based on current settings.
        std::string color = getMixtureColor(state);
        MessageFormatter::info("\nCurrent mixture: " + color);
        MessageFormatter::info("Produce? (yes/no)");

        // Handle the player's decision to produce or adjust.
        std::getline(std::cin, raw);
        auto cmd = parser.parse(raw);

        // If the player chooses to produce the mixture
        if (cmd.verb == "yes") {
            Item* spray = player.getItem("spray bottle");
            SprayColor newColor = toSprayColor(color);

            // Lowercase version for display.
            std::string lowerColor = color;
            std::transform(lowerColor.begin(), lowerColor.end(), lowerColor.begin(), ::tolower);

            // If the player does not yet have a spray bottle, create one.
            if (!spray) {
                Item newSpray("spray bottle");
                newSpray.sprayColor = newColor;
                player.addItem(newSpray);

                MessageFormatter::info(
                    "The machine filled the bottle with a " + lowerColor +
                    " liquid.\nReceived spray bottle (" + lowerColor + ")"
                );
            }
            else {
                // Otherwise, overwrite the existing bottle's contents.
                spray->sprayColor = newColor;

                MessageFormatter::info(
                    "The machine filled the bottle with a " + lowerColor + " liquid."
                );
            }

            // Mixer interaction complete.
            return true;
        }

        // Otherwise, the player wants to adjust a dial.
        MessageFormatter::info("Adjust which dial? (A/B/C)");
        std::getline(std::cin, raw);
        cmd = parser.parse(raw);

        // Extract the dial letter
        char dial = cmd.verb.empty() ? ' ' : cmd.verb[0];

        // Prompt for the new dial setting.
        MessageFormatter::info("To which volume? (low/medium/high)");
        std::getline(std::cin, raw);
        cmd = parser.parse(raw);

        // Map text input to numeric dial values.
        int value = 1;
        if (cmd.verb == "medium") value = 2;
        else if (cmd.verb == "high") value = 3;

        // Apply the change to the selected dial.
        if (dial == 'a') state.dialA = value;
        else if (dial == 'b') state.dialB = value;
        else if (dial == 'c') state.dialC = value;
        // Invalid dial letters are silently ignored; the loop continues.
    }
}
