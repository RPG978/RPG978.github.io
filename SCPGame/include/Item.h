#pragma once
#include <string>

// Possible color states for the spray bottle's mixture
enum class SprayColor {
    None,
    Red,
    Green,
    Blue,
    Black,
};

/**
 * @class Item
 * @brief Represents an object the player can collect or use during gameplay.
 *
 * Design Notes:
 * 
 *   - Items are lightweight data objects: name, description, and optional state.
 * 
 *   - Only the spray bottle uses the sprayColor field; all other items ignore it.
 * 
 *   - getDisplayName() provides dynamic inventory labeling (e.g., "spray bottle (green)").
 */
class Item {
public:
    /**
     * @brief Construct an empty item (used for fallback or placeholder cases).
     */
    Item() : name(""), description("") {}

    /**
     * @brief Construct an item with a name and optional description.
     */
    Item(const std::string& name, const std::string& desc = "")
        : name(name), description(desc) {}

    /**
     * @brief Retrieves the item's display name.
     */
    const std::string& getName() const { 
        return name; 
    }

    /**
     * @brief Placeholder for item behavior.
     * 
     * Most items have no direct "use" action; interactions are handled
     * through GameEngine and PuzzleEngine.
     */
    void use();

    /**
     * @brief Current color state for the spray bottle.
     *
     * Ignored for all other items.
     */
    SprayColor sprayColor = SprayColor::None;

    /**
     * @brief Returns the name shown in the player's inventory.
     *
     * For the spray bottle, this includes the current mixture color.
     *
     * For all other items, this returns the base name unchanged.
     */
    std::string getDisplayName() const {
        if (name == "spray bottle") {
            switch (sprayColor) {
            case SprayColor::Red:   return "spray bottle (red)";
            case SprayColor::Green: return "spray bottle (green)";
            case SprayColor::Blue:  return "spray bottle (blue)";
            case SprayColor::Black: return "spray bottle (black)";
            default:                return "spray bottle";
            }
        }
        return name;
    }

private:
    std::string name;           // Base name of the item
    std::string description;    // Optional flavor text
};
