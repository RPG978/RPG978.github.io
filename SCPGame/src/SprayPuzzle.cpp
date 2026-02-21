#include "SprayPuzzle.h"
#include "Player.h"
#include "Item.h"
#include "MessageFormatter.h"
#include "PuzzleEngine.h"

bool SprayPuzzle::run(Player& player, Room* room)
{
    // Player must have a spray bottle
    Item* spray = player.getItem("spray bottle");
    if (!spray) {
        MessageFormatter::info("You don't have a spray bottle.");
        return false;
    }

    // If not in Maintenance Room
    if (room->getId() != 3) {
        MessageFormatter::info("You spray into the air aimlessly.");
        return false;
    }

    // Compare against PuzzleEngine's required spray color
    if (spray->sprayColor == engine->getRequiredSprayColor()) { 

        MessageFormatter::info(
            "You spray the canvas. The chemical reacts instantly.\n" 
            "Hidden text emerges beneath the pigment:\n\n"
        ); 
        
        MessageFormatter::info(engine->getFormattedLeverPattern()); 
        
        MessageFormatter::info(
            "\nThe chemical dries quickly and the canvas returns to normal."
        ); 
        
        return true; 
    } 
    
    // Wrong color 
    MessageFormatter::info("You spray the canvas, but nothing happens."); 
    return false;
}
