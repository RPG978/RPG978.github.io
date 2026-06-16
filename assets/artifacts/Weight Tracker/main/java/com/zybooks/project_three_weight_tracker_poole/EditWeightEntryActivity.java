package com.zybooks.project_three_weight_tracker_poole;

import android.app.AlertDialog;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import com.zybooks.project_three_weight_tracker_poole.database.AppDatabase;
import com.zybooks.project_three_weight_tracker_poole.database.WeightEntry;
import com.zybooks.project_three_weight_tracker_poole.database.WeightDao;

public class EditWeightEntryActivity extends AppCompatActivity {
    private EditText editDate;
    private EditText editWeight;
    private WeightDao weightEntryDao;
    private int entryId;
    private UserDao userDao;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_edit_weight_entry);

        editDate = findViewById(R.id.editDateInput);
        DatePickerHelper.attachDatePicker(this, editDate);

        editWeight = findViewById(R.id.editWeightInput);
        Button saveButton = findViewById(R.id.saveChangesButton);
        Button backButton = findViewById(R.id.backButton);

        weightEntryDao = AppDatabase.getInstance(this).weightDao();
        entryId = getIntent().getIntExtra("entry_id", -1);

        userDao = AppDatabase.getInstance(this).userDao();

        if (entryId == -1) {
            Toast.makeText(this, "Invalid entry ID", Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        loadEntry(entryId);

        saveButton.setOnClickListener(v -> saveChanges());
        backButton.setOnClickListener(v -> finish());
    }

    private void loadEntry(int id) {
        new Thread(() -> {
            WeightEntry entry = weightEntryDao.getEntryById(id);

            if (entry != null) {
                runOnUiThread(() -> {
                    editDate.setText(entry.getDate());
                    editWeight.setText(String.valueOf(entry.getWeight()));
                });
            }
        }).start();
    }

    private void saveChanges() {
        String newDate = editDate.getText().toString().trim();
        String weightStr = editWeight.getText().toString().trim();

        if (newDate.isEmpty() || weightStr.isEmpty()) {
            Toast.makeText(this, "Please fill out all fields", Toast.LENGTH_SHORT).show();
            return;
        }

        float newWeight;
        try {
            newWeight = Float.parseFloat(weightStr);
        }
        catch (NumberFormatException e) {
            Toast.makeText(this, "Invalid weight format", Toast.LENGTH_SHORT).show();
            return;
        }

        if (newWeight >= 10000) {
            editWeight.setError("Weight must be less than 10,000 lbs");
            return;
        }

        SharedPreferences prefs = getSharedPreferences("user_session", MODE_PRIVATE);
        String username = prefs.getString("logged_in_username", null);

        if (username == null) {
            Toast.makeText(this, "No user logged in", Toast.LENGTH_SHORT).show();
            return;
        }

        WeightEntry updatedEntry = new WeightEntry(newDate, newWeight, username);
        updatedEntry.setId(entryId);

        new Thread(() -> {
            weightEntryDao.updateWeight(updatedEntry);
            Float goalWeight = userDao.getGoalWeight(username);

            runOnUiThread(() -> {
                if (goalWeight != null && newWeight <= goalWeight) {
                    new AlertDialog.Builder(this)
                            .setTitle("Goal reached!")
                            .setMessage("You hit your goal of " + goalWeight + " lbs.")
                            .setPositiveButton("OK", (dialog, which) -> {
                                setResult(RESULT_OK);
                                finish();
                            })
                            .setCancelable(false)
                            .show();
                } else {
                    Toast.makeText(this, "Entry updated", Toast.LENGTH_SHORT).show();
                    setResult(RESULT_OK);
                    finish();
                }
            });
        }).start();
    }
}