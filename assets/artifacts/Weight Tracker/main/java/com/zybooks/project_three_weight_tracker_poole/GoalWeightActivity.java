package com.zybooks.project_three_weight_tracker_poole;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import androidx.room.Room;
import com.zybooks.project_three_weight_tracker_poole.database.AppDatabase;

public class GoalWeightActivity extends AppCompatActivity {

    private EditText goalWeightInput;
    private UserDao userDao;
    private String username;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_goal_weight);

        goalWeightInput = findViewById(R.id.goalWeightInput);
        Button saveGoalWeightButton = findViewById(R.id.saveGoalWeightButton);

        // Get logged-in username
        SharedPreferences prefs = getSharedPreferences("user_session", MODE_PRIVATE);
        username = prefs.getString("logged_in_username", null);

        if (username == null) {
            Toast.makeText(this, "No user logged in", Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        // Get DAO
        AppDatabase db = Room.databaseBuilder(getApplicationContext(),
                        AppDatabase.class, "weight_tracker_db")
                .allowMainThreadQueries() // okay for now
                .fallbackToDestructiveMigration()
                .build();
        userDao = db.userDao();

        // Pre-fill if goal already exists
        Float existingGoal = userDao.getGoalWeight(username);
        if (existingGoal != null) {
            goalWeightInput.setText(String.valueOf(existingGoal));
        }

        saveGoalWeightButton.setOnClickListener(v -> {
            String input = goalWeightInput.getText().toString().trim();
            if (!input.isEmpty()) {
                try {
                    float goalWeight = Float.parseFloat(input);
                    if (goalWeight >= 10000f) {
                        goalWeightInput.setError("Weight must be below 10,000");
                        return;
                    }

                    userDao.setGoalWeight(username, goalWeight);
                    Toast.makeText(this, "Goal weight saved!", Toast.LENGTH_SHORT).show();
                    finish();
                }
                catch (NumberFormatException e) {
                    Toast.makeText(this, "Please enter a valid number", Toast.LENGTH_SHORT).show();
                }
            }
            else {
                Toast.makeText(this, "Goal weight cannot be empty", Toast.LENGTH_SHORT).show();
            }
        });

        Button backButton = findViewById(R.id.backButton);
        backButton.setOnClickListener(v -> {
            Toast.makeText(this, "Returning without saving", Toast.LENGTH_SHORT).show();
            finish(); // Navigates back to MainActivity
        });
    }
}