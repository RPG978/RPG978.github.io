package com.zybooks.project_three_weight_tracker_poole;

import android.Manifest;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

public class SmsSetupActivity extends AppCompatActivity {

    private static final int SMS_PERMISSION_CODE = 101;
    private EditText phoneNumberInput;
    private String username;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sms_permission);

        phoneNumberInput = findViewById(R.id.phoneNumberInput);
        Button requestSmsPermissionButton = findViewById(R.id.requestSmsPermissionButton);
        Button continueWithoutSmsButton = findViewById(R.id.continueWithoutSmsButton);

        SharedPreferences sessionPrefs = getSharedPreferences("user_session", MODE_PRIVATE);
        username = sessionPrefs.getString("logged_in_username", null);

        requestSmsPermissionButton.setOnClickListener(v -> {
            String phone = phoneNumberInput.getText().toString().trim();
            if (phone.isEmpty()) {
                Toast.makeText(this, "Please enter a phone number", Toast.LENGTH_SHORT).show();
                return;
            }

            // Save phone number scoped by username
            sessionPrefs.edit().putString("user_phone_" + username, phone).apply();

            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.SEND_SMS},
                    SMS_PERMISSION_CODE);
        });

        continueWithoutSmsButton.setOnClickListener(v -> {
            Toast.makeText(this, "Continuing without SMS notifications", Toast.LENGTH_SHORT).show();

            SharedPreferences prefs = getSharedPreferences("user_prefs", MODE_PRIVATE);
            prefs.edit().putBoolean("sms_enabled", false).apply(); // Explicit opt-out
            finish();
        });
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (requestCode == SMS_PERMISSION_CODE) {
            boolean smsPermissionGranted = grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED;
            if (smsPermissionGranted) {
                Toast.makeText(this, "SMS permission granted!", Toast.LENGTH_SHORT).show();

                SharedPreferences prefs = getSharedPreferences("user_prefs", MODE_PRIVATE);
                prefs.edit().putBoolean("sms_enabled", true).apply();

                finish();
            }
            else {
                Toast.makeText(this, "SMS permission denied", Toast.LENGTH_SHORT).show();
            }
        }
    }
}