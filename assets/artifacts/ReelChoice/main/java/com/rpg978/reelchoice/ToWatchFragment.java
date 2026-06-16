package com.rpg978.reelchoice;

import android.app.AlertDialog;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.ItemTouchHelper;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * ToWatchFragment
 *
 * Displays the list of movies the user still wants to watch.
 * This fragment observes LiveData from the shared MovieViewModel and updates
 * its RecyclerView automatically whenever the underlying Room table changes.
 *
 * Responsibilities:
 *   - Display the "To Watch" list using MovieAdapter.
 *   - Allow editing and deleting via click/long‑press.
 *   - Handle swipe gestures to move movies -> Watched.
 *   - Never access the database directly - all actions go through the ViewModel.
 */
public class ToWatchFragment extends Fragment {

    private RecyclerView recyclerView;
    private MovieAdapter adapter;
    private MovieViewModel viewModel;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        // Inflate the shared list layout used by both fragments.
        View view = inflater.inflate(R.layout.fragment_movie_list, container, false);

        // Obtain the shared ViewModel from the Activity.
        // Ensures both fragments operate on the same movie data.
        viewModel = new ViewModelProvider(requireActivity()).get(MovieViewModel.class);

        // RecyclerView setup: vertical list of movies.
        recyclerView = view.findViewById(R.id.recyclerViewFragment);
        recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));

        // Adapter handles click and long-click actions.
        adapter = new MovieAdapter(new MovieAdapter.OnMovieClickListener() {

            @Override
            public void onClick(Movie movie) {
                // Inline rename dialog.
                showEditDialog(movie);
            }

            @Override
            public void onLongClick(Movie movie) {
                // Confirm deletion before removing from DB.
                new AlertDialog.Builder(requireContext())
                        .setTitle("Delete Movie")
                        .setMessage("Are you sure you want to delete \"" + movie.name + "\"?")
                        .setPositiveButton("Delete", (dialog, which) -> viewModel.deleteMovie(movie))
                        .setNegativeButton("Cancel", null)
                        .show();
            }
        });

        recyclerView.setAdapter(adapter);

        // Observe the LiveData list for "To Watch" movies.
        // Whenever Room updates the table, the adapter receives a new list.
        viewModel.getToWatchMovies().observe(getViewLifecycleOwner(), movies -> {
            Log.d("ToWatchFragment", "Observed " + movies.size() + " movies");
            adapter.submitList(movies); // DiffUtil handles animations and updates
        });

        // Enable swipe to mark as watched.
        attachSwipeHandler();

        return view;
    }

    /**
     * Configures swipe gestures for this fragment.
     *
     * To Watch list only supports RIGHT swipe:
     *   RIGHT -> mark movie as "Watched"
     *
     * The fragment creates a new Movie object with the same ID but watched = true.
     * ViewModel handles the update -> Repository -> Room.
     */
    private void attachSwipeHandler() {
        ItemTouchHelper.SimpleCallback swipeCallback = new ItemTouchHelper.SimpleCallback(
                0,
                ItemTouchHelper.RIGHT   // Only allow RIGHT swipe in this fragment
        ) {
            @Override
            public boolean onMove(@NonNull RecyclerView recyclerView,
                                  @NonNull RecyclerView.ViewHolder viewHolder,
                                  @NonNull RecyclerView.ViewHolder target) {
                return false; // No drag‑and‑drop reordering
            }

            @Override
            public void onSwiped(@NonNull RecyclerView.ViewHolder viewHolder, int direction) {
                int position = viewHolder.getAdapterPosition();
                if (position == RecyclerView.NO_POSITION) return;

                Movie oldMovie = adapter.getMovieAt(position);

                // Create updated copy with watched = true.
                Movie updated = new Movie(oldMovie.name);
                updated.id = oldMovie.id;
                updated.watched = true;

                // Push update through ViewModel -> Repository -> Room.
                viewModel.updateMovie(updated);
            }
        };

        new ItemTouchHelper(swipeCallback).attachToRecyclerView(recyclerView);
    }

    /**
     * Shows a simple dialog allowing the user to rename a movie.
     * Updates are sent through the ViewModel to keep MVVM boundaries clean.
     */
    private void showEditDialog(Movie movie) {
        EditText input = new EditText(getContext());
        input.setText(movie.name);

        new AlertDialog.Builder(requireContext())
                .setTitle("Edit Movie")
                .setView(input)
                .setPositiveButton("Save", (dialog, which) -> {
                    String newName = input.getText().toString().trim();
                    if (!newName.isEmpty()) {
                        movie.name = newName;
                        viewModel.updateMovie(movie);
                    }
                })
                .setNegativeButton("Cancel", null)
                .show();
    }
}
