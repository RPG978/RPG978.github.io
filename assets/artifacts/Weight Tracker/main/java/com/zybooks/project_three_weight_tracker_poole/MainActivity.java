package com.zybooks.project_three_weight_tracker_poole;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageButton;
import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.room.Room;
import java.util.List;
import com.zybooks.project_three_weight_tracker_poole.database.AppDatabase;
import com.zybooks.project_three_weight_tracker_poole.database.WeightDao;
import com.zybooks.project_three_weight_tracker_poole.database.WeightEntry;
import com.google.android.material.floatingactionbutton.FloatingActionButton;

public class MainActivity extends AppCompatActivity {
    private WeightEntryAdapter adapter;
    private WeightDao weightDao;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);

        FloatingActionButton addWeightButton = findViewById(R.id.addWeightButton);
        addWeightButton.setOnClickListener(v -> {
            Intent intent = new Intent(MainActivity.this, AddWeightActivity.class);
            startActivity(intent);
        });

        ImageButton smsSetupButton = findViewById(R.id.smsSetupButton);
        smsSetupButton.setOnClickListener(v -> {
            Intent intent = new Intent(MainActivity.this, SmsSetupActivity.class);
            startActivity(intent);
        });

        Button setGoalWeightButton = findViewById(R.id.setGoalWeightButton);
        setGoalWeightButton.setOnClickListener(v -> {
            Intent intent = new Intent(MainActivity.this, GoalWeightActivity.class);
            startActivity(intent);
        });

        RecyclerView weightRecyclerView = findViewById(R.id.weightRecyclerView);
        weightRecyclerView.setLayoutManager(new LinearLayoutManager(this));

        AppDatabase db = Room.databaseBuilder(getApplicationContext(),
                        AppDatabase.class, "weight_tracker_db")
                .fallbackToDestructiveMigration()
                .allowMainThreadQueries()
                .build();

        weightDao = db.weightDao();

        SharedPreferences prefs = getSharedPreferences("user_session", MODE_PRIVATE);
        SharedPreferences.Editor editor = prefs.edit();
        editor.apply();
        String username = prefs.getString("logged_in_username", null);
        Log.d("MainActivity", "Logged in as: " + username);

        if (username == null) {
            // Optional: redirect to login or show error
            Log.e("MainActivity", "No username found in session");
            return;
        }

        List<WeightEntry> weightEntries = weightDao.getEntriesForUser(username);
        Log.d("MainActivity", "Loaded " + weightEntries.size() + " entries for user: " + username);

        adapter = new WeightEntryAdapter(weightEntries, weightDao);
        weightRecyclerView.setAdapter(adapter);
    }

    @Override
    protected void onResume() {
        super.onResume();

        SharedPreferences prefs = getSharedPreferences("user_session", MODE_PRIVATE);
        String username = prefs.getString("logged_in_username", null);

        if (username != null) {
            List<WeightEntry> updatedEntries = weightDao.getEntriesForUser(username);
            adapter.updateData(updatedEntries);
        }
    }
}