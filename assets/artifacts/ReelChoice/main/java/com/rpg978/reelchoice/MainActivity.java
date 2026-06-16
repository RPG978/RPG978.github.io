package com.rpg978.reelchoice;

import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.ViewModelProvider;
import androidx.viewpager2.widget.ViewPager2;

import com.google.android.material.tabs.TabLayout;
import com.google.android.material.tabs.TabLayoutMediator;

public class MainActivity extends AppCompatActivity {

    private EditText editTextMovie;
    private TextView textViewRandom;
    private MovieViewModel viewModel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Obtain the shared ViewModel for the entire activity.
        // This ViewModel survives configuration changes and is shared with both fragments.
        viewModel = new ViewModelProvider(this).get(MovieViewModel.class);

        // Cache UI references for user input and actions.
        editTextMovie = findViewById(R.id.editTextMovie);
        textViewRandom = findViewById(R.id.textViewRandom);
        Button buttonAdd = findViewById(R.id.buttonAdd);
        Button buttonRandom = findViewById(R.id.buttonRandom);

        // Add-movie button: pushes a new movie name into the ViewModel.
        // ViewModel -> Repository -> Room handles persistence.
        buttonAdd.setOnClickListener(v -> {
            String name = editTextMovie.getText().toString().trim();
            if (!name.isEmpty()) {
                viewModel.addMovie(name); // triggers DB insert on background thread
                editTextMovie.setText(""); // clear input field after submission
            }
        });

        // Random-movie button: requests a one-shot random movie from the ViewModel.
        // This bypasses LiveData because randomness is an event, not a state.
        buttonRandom.setOnClickListener(v -> {
            viewModel.getRandomMovie(movie -> {
                runOnUiThread(() -> {
                    if (movie != null) {
                        textViewRandom.setText("🎲 " + movie.name);
                    } else {
                        textViewRandom.setText("No movies to watch yet.");
                    }
                });
            });
        });

        // Set up ViewPager2 wrapped in a custom non-swipeable container.
        // Pager hosts the two fragments: "To Watch" and "Watched".
        NonSwipeableViewPager wrapper = findViewById(R.id.nonSwipePager);
        ViewPager2 viewPager = wrapper.getViewPager();
        viewPager.setAdapter(new MoviePagerAdapter(this));

        // Connect TabLayout with ViewPager2.
        // Tabs reflect the two lists managed by the ViewModel.
        TabLayout tabLayout = findViewById(R.id.tabLayout);
        new TabLayoutMediator(tabLayout, viewPager, (tab, position) -> {
            if (position == 0) {
                tab.setText("To Watch");
                tab.setContentDescription("To Watch tab");
            } else {
                tab.setText("Watched");
                tab.setContentDescription("Watched tab");
            }
        }).attach();

    }
}
