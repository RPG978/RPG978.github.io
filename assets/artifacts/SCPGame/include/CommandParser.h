#pragma once
#include <string>

/**
 * @class CommandParser
 * @brief Converts raw player input into a structured command.
 *
 * Responsibilities:
 * 
 *   - Normalize and tokenize player input
 * 
 *   - Extract the command verb and optional argument
 * 
 *   - Provide a simple, predictable interface for GameEngine
 *
 * Design Notes:
 * 
 *   - Parsing is intentionally lightweight; all command semantics
 *     (movement, item use, puzzle routing) belong in GameEngine.
 * 
 *   - The parser only interprets text.
 * 
 *   - Commands are returned as a simple struct for clarity and flexibility.
 */
class CommandParser {
public:

    /**
     * @struct Command
     * @brief Represents a parsed player command.
     *
     * verb:
     *   The action requested by the player (e.g., "go", "take", "use").
     *
     * argument:
     *   The target of the action (e.g., "north", "keycard", "console").
     *   May be empty if the command has no argument.
     */
    struct Command {
        std::string verb;
        std::string argument;
    };

    /**
     * @brief Parse a raw input string into a Command.
     *
     * Behavior:
     * 
     *   - Input is normalized to lowercase.
     * 
     *   - Leading/trailing whitespace is trimmed.
     * 
     *   - The first token becomes the verb.
     * 
     *   - Remaining text becomes the argument (may be empty).
     *
     * @param input Raw text entered by the player.
     * @return A Command containing the parsed verb and argument.
     */
    Command parse(const std::string& input) const;
};
