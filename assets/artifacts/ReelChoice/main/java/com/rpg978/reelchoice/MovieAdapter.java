package com.rpg978.reelchoice;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;
import androidx.recyclerview.widget.RecyclerView;

/**
 * MovieAdapter
 *
 * RecyclerView adapter responsible for displaying Movie items.
 * Extends ListAdapter to leverage DiffUtil for efficient list updates:
 *   - Only changed rows are redrawn
 *   - Smooth animations when items move between lists
 *   - No manual notifyDataSetChanged() calls
 *
 * Responsibilities:
 *   - Bind Movie data to item views.
 *   - Forward click and long‑click events to the fragment.
 *   - Provide stable item access for swipe handlers.
 */
public class MovieAdapter extends ListAdapter<Movie, MovieAdapter.MovieViewHolder> {

    // Callback interface implemented by fragments to handle user actions.
    private final OnMovieClickListener listener;

    public interface OnMovieClickListener {
        void onClick(Movie movie); // Single tap -> edit
        void onLongClick(Movie movie); // Long press -> delete
    }

    public MovieAdapter(@NonNull OnMovieClickListener listener) {
        super(DIFF_CALLBACK);
        this.listener = listener;
    }

    /**
     * DiffUtil callback:
     * Determines how RecyclerView detects changes between old/new lists.
     *
     * areItemsTheSame:
     *   - Checks identity (same database row)
     *
     * areContentsTheSame:
     *   - Checks if the Movie's fields actually changed
     *   - Uses Movie.equals() for deep comparison
     */
    private static final DiffUtil.ItemCallback<Movie> DIFF_CALLBACK =
            new DiffUtil.ItemCallback<Movie>() {
                @Override
                public boolean areItemsTheSame(@NonNull Movie oldItem, @NonNull Movie newItem) {
                    return oldItem.id == newItem.id; // Same DB row
                }

                @Override
                public boolean areContentsTheSame(@NonNull Movie oldItem, @NonNull Movie newItem) {
                    return oldItem.equals(newItem); // Same content
                }
            };

    @NonNull
    @Override
    public MovieViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        Log.d("Adapter", "Creating new ViewHolder");

        // Inflate a single movie row layout.
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.item_movie, parent, false);
        return new MovieViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull MovieViewHolder holder, int position) {
        Movie movie = getItem(position);

        // Defensive fallback for null names.
        if (movie != null && movie.name != null) {
            holder.nameText.setText(movie.name);
        } else {
            holder.nameText.setText("Untitled");
        }

        // Forward click events to the fragment.
        holder.itemView.setOnClickListener(v -> listener.onClick(movie));

        // Long‑press -> delete confirmation in fragment.
        holder.itemView.setOnLongClickListener(v -> {
            listener.onLongClick(movie);
            return true;
        });
    }

    /**
     * Helper method used by swipe handlers in fragments.
     * Provides stable access to the Movie at a given adapter position.
     */
    public Movie getMovieAt(int position) {
        return getItem(position);
    }

    /**
     * ViewHolder:
     * Holds references to the views inside each row.
     * RecyclerView reuses these for performance.
     */
    static class MovieViewHolder extends RecyclerView.ViewHolder {
        TextView nameText;

        public MovieViewHolder(@NonNull View itemView) {
            super(itemView);
            nameText = itemView.findViewById(R.id.movieNameText);
        }
    }
}
