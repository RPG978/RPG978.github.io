package com.rpg978.reelchoice;

import androidx.room.Dao;
import androidx.room.Delete;
import androidx.room.Insert;
import androidx.room.Query;
import androidx.room.Update;
import androidx.lifecycle.LiveData;

import java.util.List;

/**
 * MovieDao
 *
 * Data Access Object for the Movie table.
 * Room generates the implementation at compile time.
 *
 * Responsibilities:
 *   - Define SQL operations (insert, update, delete, queries).
 *   - Provide LiveData streams for reactive UI updates.
 *   - Provide direct-return queries for one-shot operations (like RNG).
 *
 * This interface contains only method signatures and SQL annotations.
 * Room handles the actual database code automatically.
 */
@Dao
public interface MovieDao {

    /**
     * Insert a new Movie into the database.
     */
    @Insert
    void insert(Movie movie);

    /**
     * Update an existing Movie.
     * Room matches rows by primary key (Movie.id).
     */
    @Update
    void update(Movie movie);

    /**
     * Delete a Movie from the database.
     */
    @Delete
    void delete(Movie movie);

    /**
     * Reactive query for the two main lists.
     *
     * @param watched false -> "To Watch" list
     *                true  -> "Watched" list
     *
     * LiveData ensures:
     *   - UI updates automatically when the table changes.
     *   - Queries run off the main thread.
     *   - Fragments stay lifecycle-safe.
     *
     * Sorted alphabetically (case-insensitive) for clean UI presentation.
     */
    @Query("SELECT * FROM movies WHERE watched = :watched ORDER BY name COLLATE NOCASE ASC")
    LiveData<List<Movie>> getMoviesByWatched(boolean watched);

    /**
     * One-shot random movie query.
     *
     * This intentionally returns Movie (not LiveData<Movie>) because:
     *   - Randomness is an event, not a state.
     *   - LiveData would not re-run unless the table changed.
     *   - Repository executes this on a background thread.
     *
     * LIMIT 1 ensures exactly one result or null if no movies exist.
     */
    @Query("SELECT * FROM movies WHERE watched = 0 ORDER BY RANDOM() LIMIT 1")
    Movie getRandomMovie();
}


