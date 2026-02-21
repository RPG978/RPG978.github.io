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

