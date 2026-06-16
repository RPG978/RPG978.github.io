package com.rpg978.reelchoice;

import android.app.Application;

import androidx.annotation.NonNull;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;

import java.util.List;
import java.util.function.Consumer;

/**
 * MovieViewModel
 *
 * Acts as the bridge between UI (Activity/Fragments) and the data layer (Repository).
 * Holds UI-related data in a lifecycle-safe way and survives configuration changes.
 *
 * Responsibilities:
 *  - Expose LiveData streams for the two movie lists.
 *  - Forward user actions (add/update/delete) to the Repository.
 *  - Provide one-shot operations (like RNG) without exposing threading to the UI.
 */
public class MovieViewModel extends AndroidViewModel {

    // Single source for all movie-related data operations.
    private final MovieRepository repository;

    // LiveData streams observed by the fragments.
    // These automatically update when the underlying Room tables change.
    private final LiveData<List<Movie>> toWatchMovies;
    private final LiveData<List<Movie>> watchedMovies;

    public MovieViewModel(@NonNull Application application) {
        super(application);

        // Repository handles all DB access and background threading.
        repository = new MovieRepository(application);

        // LiveData is obtained once and kept for the lifetime of the ViewModel.
        // Fragments observe these to update their RecyclerViews automatically.
        toWatchMovies = repository.getToWatchMovies();
        watchedMovies = repository.getWatchedMovies();
    }

    // Expose the "To Watch" list to the UI.
    public LiveData<List<Movie>> getToWatchMovies() {
        return toWatchMovies;
    }

    // Expose the "Watched" list to the UI.
    public LiveData<List<Movie>> getWatchedMovies() {
        return watchedMovies;
    }

    // UI requests to add a movie -> forwarded to Repository.
    // ViewModel never touches the database directly.
    public void addMovie(String name) {
        repository.addMovie(name);
    }

    // UI requests to update a movie (rename, toggle watched, etc.).
    public void updateMovie(Movie movie) {
        repository.updateMovie(movie);
    }

    // UI requests to delete a movie.
    public void deleteMovie(Movie movie) {
        repository.deleteMovie(movie);
    }

    /**
     * One-shot random movie request.
     *
     * The UI provides a callback, and the ViewModel
     * ensures the Repository runs the query off the main thread.
     */
    public void getRandomMovie(Consumer<Movie> callback) {
        repository.getRandomMovie(callback);
    }
}
