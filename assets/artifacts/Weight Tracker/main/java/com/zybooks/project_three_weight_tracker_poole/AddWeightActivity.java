package com.zybooks.project_three_weight_tracker_poole;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.telephony.SmsManager;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.room.Room;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import com.zybooks.project_three_weight_tracker_poole.database.AppDatabase;
import com.zybooks.project_three_weight_tracker_poole.database.WeightDao;
import com.zybooks.project_three_weight_tracker_poole.database.WeightEntry;

public class AddWeightActivity extends AppCompatActivity {

    private EditText dateInput, weightInput;
    private WeightDao weightDao;
    private UserDao userDao;
    private String username;

    private static final int SMS_PERMISSION_REQUEST_CODE = 1001;

    private void requestSmsPermissionIfNeeded(float currentWeight, float goalWeight) {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.SEND_SMS)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.SEND_SMS},
                    SMS_PERMISSION_REQUEST_CODE);
            // Store weight info temporarily if needed
        }
        else {
            triggerGoalReachedSMS(this, username, currentWeight, goalWeight);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_add_weight);

        dateInput = findViewById(R.id.dateInput);
        weightInput = findViewById(R.id.weightInput);
        Button addButton = findViewById(R.id.confirmAddButton);
        Button backButton = findViewById(R.id.backButton);

        // Autofill current date
        String currentDate = new SimpleDateFormat("MM-dd-yyyy", Locale.getDefault()).format(new Date());
        dateInput.setText(currentDate);

        // Date picker
        DatePickerHelper.attachDatePicker(this, dateInput);

        // DB setup
        AppDatabase db = Room.databaseBuilder(getApplicationContext(),
                        AppDatabase.class, "weight_tracker_db")
                .allowMainThreadQueries()
                .fallbackToDestructiveMigration()
                .build();
        weightDao = db.weightDao();
        userDao = db.userDao();

        // Get logged-in user
        SharedPreferences prefs = getSharedPreferences("user_session", MODE_PRIVATE);
        username = prefs.getString("logged_in_username", null);

        if (username == null) {
            Toast.makeText(this, "No user logged in", Toast.LENGTH_SHORT).show();
            finish();
            return;
        }
        addButton.setOnClickListener(v -> handleAddWeight());
        backButton.setOnClickListener(v -> finish());
    }

    private void handleAddWeight() {
        String date = dateInput.getText().toString().trim();
        String weightStr = weightInput.getText().toString().trim();

        if (weightStr.isEmpty()) {
            weightInput.setError("Please enter a weight");
            return;
        }

        float weight;

        try {
            weight = Float.parseFloat(weightStr);
        }
        catch (NumberFormatException e) {
            weightInput.setError("Invalid weight format");
            return;
        }

        if (weight >= 10000) {
            weightInput.setError("Weight must be less than 10,000 lbs");
            return;
        }

        WeightEntry entry = new WeightEntry(date, weight, username);

        new Thread(() -> {
            weightDao.insert(entry);
            Float goalWeight = userDao.getGoalWeight(username);

            runOnUiThread(() -> {
                Toast.makeText(this, "Weight saved!", Toast.LENGTH_SHORT).show();

                if (goalWeight != null && weight <= goalWeight) {
                    requestSmsPermissionIfNeeded(weight, goalWeight);
                    // Continue with dialog as usual
                    new AlertDialog.Builder(this)
                            .setTitle("Goal reached!")
                            .setMessage("You hit your goal of " + goalWeight + " lbs.")
                            .setPositiveButton("OK", (dialog, which) -> {
                                startActivity(new Intent(AddWeightActivity.this, MainActivity.class));
                                finish();
                            })
                            .setCancelable(false)
                            .show();
                }
                else {
                    // No dialog needed, proceed immediately
                    startActivity(new Intent(AddWeightActivity.this, MainActivity.class));
                    finish();
                }
            });
        }).start();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (requestCode == SMS_PERMISSION_REQUEST_CODE) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // You may need to store weight info to recall here
                Float goalWeight = userDao.getGoalWeight(username);
                String weightStr = weightInput.getText().toString().trim();
                float weight = Float.parseFloat(weightStr);
                triggerGoalReachedSMS(this, username, weight, goalWeight);
            }
            else {
                Log.d("SMS", "Permission denied — SMS will not be sent");
            }
        }
    }

    private void triggerGoalReachedSMS(Context context, String username, float currentWeight, float goalWeight)
    {
        SharedPreferences prefs = getSharedPreferences("user_session", MODE_PRIVATE);
        String phoneNumber = prefs.getString("user_phone_" + username, null);

        SharedPreferences userPrefs = getSharedPreferences("user_prefs", MODE_PRIVATE);
        boolean smsEnabled = userPrefs.getBoolean("sms_enabled", false);

        if (!smsEnabled) {
            Log.d("SMS", "SMS disabled — skipping");
            return;
        }

        if (phoneNumber == null || phoneNumber.isEmpty()) {
            Log.d("SMS", "No phone number found for user: " + username);
            return;
        }

        String message = "Goal reached!\nCurrent: " + currentWeight + " lbs\nGoal: " + goalWeight + " lbs";

        // Emulator-friendly confirmation using AlertDialog
        if (context instanceof Activity) {
            Activity activity = (Activity) context;

            activity.runOnUiThread(() -> {
                AlertDialog.Builder builder = new AlertDialog.Builder(activity)
                        .setTitle("SMS Preview")
                        .setMessage("SMS would be sent to " + phoneNumber + ":\n\n" + message)
                        .setCancelable(false);

                AlertDialog dialog = builder.create();
                dialog.show();

                new Handler(Looper.getMainLooper()).postDelayed(() -> {
                    if (dialog.isShowing()) {
                        dialog.dismiss();
                        Log.d("SMS", "Dialog dismissed after delay");
                    }
                }, 3000);
            });
        }
        else {
            Log.e("SMS", "Invalid context — cannot show dialog");
        }
        Log.d("SMS", "[TEST MODE] Would send SMS to " + phoneNumber + ": " + message);

        // Testing on real device
    if (ActivityCompat.checkSelfPermission(this, Manifest.permission.SEND_SMS) == PackageManager.PERMISSION_GRANTED) {
        SmsManager.getDefault().sendTextMessage(phoneNumber, null, message, null, null);
        Log.d("SMS", "SMS sent to " + phoneNumber);
    }
    else {
        Log.d("SMS", "SMS permission not granted");
    }
    }
}