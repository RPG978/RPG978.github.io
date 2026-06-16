package com.rpg978.reelchoice;

import android.content.Context;
import androidx.room.Database;
import androidx.room.Room;
import androidx.room.RoomDatabase;

/**
 * MovieDatabase
 *
 * The Room database for the entire application.
 * Defines:
 *   - which entities belong to the database (Movie.class)
 *   - the schema version (used for migrations)
 *   - the DAO access point (movieDao())
 *
 * This class is a singleton because:
 *   - Room databases are expensive to create
 *   - Only one instance should exist per app process
 *   - Multiple instances can cause threading and data corruption issues
 *
 * Room generates the actual database implementation at compile time.
 */
@Database(entities = {Movie.class}, version = 8, exportSchema = false)
public abstract class MovieDatabase extends RoomDatabase {

    // Singleton instance of the database.
    private static MovieDatabase INSTANCE;

    /**
     * Returns the single shared database instance.
     *
     * Uses applicationContext to avoid leaking an Activity.
     * fallbackToDestructiveMigration() ensures the app won't crash
     * if the schema changes during development - Room will rebuild
     * the database instead of requiring a migration.
     *
     */
    public static MovieDatabase getInstance(Context context) {
        if (INSTANCE == null) {
            INSTANCE = Room.databaseBuilder(context.getApplicationContext(),
                            MovieDatabase.class, "movie_db") // physical SQLite filename
                    .fallbackToDestructiveMigration() // rebuild DB on version change
                    .build();
        }
        return INSTANCE;
    }

    /**
     * Exposes the DAO to the Repository.
     * Room generates the implementation automatically.
     */
    public abstract MovieDao movieDao();
}
