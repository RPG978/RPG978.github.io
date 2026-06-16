package com.zybooks.project_three_weight_tracker_poole;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import androidx.room.Room;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import com.zybooks.project_three_weight_tracker_poole.database.AppDatabase;

public class LoginActivity extends AppCompatActivity {

    EditText usernameInput, passwordInput;
    Button loginButton, createAccountButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        usernameInput = findViewById(R.id.usernameInput);
        passwordInput = findViewById(R.id.passwordInput);
        loginButton = findViewById(R.id.loginButton);
        createAccountButton = findViewById(R.id.createAccountButton);

        AppDatabase db = Room.databaseBuilder(getApplicationContext(),
                        AppDatabase.class, "weight_tracker_db")
                        .fallbackToDestructiveMigration()
                        .allowMainThreadQueries()
                        .build();
        UserDao userDao = db.userDao();

        loginButton.setOnClickListener(v -> {
            String username = usernameInput.getText().toString().trim();
            String password = passwordInput.getText().toString();
            String hashedPassword = hashPassword(password);

            User user = userDao.getUser(username);

            if (user != null) {
                if (user.passwordHash.equals(hashedPassword)) {
                    SharedPreferences prefs = getSharedPreferences("user_session", MODE_PRIVATE);
                    prefs.edit().putString("logged_in_username", username).apply();

                    Intent intent = new Intent(LoginActivity.this, MainActivity.class);
                    startActivity(intent);
                    finish();
                }
                else {
                    passwordInput.setError("Incorrect password");
                }
            }
            else {
                usernameInput.setError("Username not found");
                Toast.makeText(this, "No account found for \"" + username + "\"", Toast.LENGTH_SHORT).show();
            }
        });

        createAccountButton.setOnClickListener(v -> {
            String username = usernameInput.getText().toString().trim();
            String password = passwordInput.getText().toString();
            String hashedPassword = hashPassword(password);

            if (userDao.getUser(username) != null) {
                usernameInput.setError("Username already exists");
            }
            else {
                User newUser = new User();
                newUser.username = username;
                newUser.passwordHash = hashedPassword;
                userDao.insertUser(newUser);

                // Save username to SharedPreferences
                SharedPreferences prefs = getSharedPreferences("user_session", MODE_PRIVATE);
                prefs.edit().putString("logged_in_username", username).apply();

                // Navigate to MainActivity
                Intent intent = new Intent(LoginActivity.this, MainActivity.class);
                startActivity(intent);
                finish();
            }
        });
    }

    private String hashPassword(String password) {
        try {
            MessageDigest digest = MessageDigest.getInstance("SHA-256");
            byte[] hash = digest.digest(password.getBytes(StandardCharsets.UTF_8));
            StringBuilder hexString = new StringBuilder();
            for (byte b : hash) {
                hexString.append(String.format("%02x", b));
            }
            return hexString.toString();
        }
        catch (NoSuchAlgorithmException e) {
            throw new RuntimeException(e);
        }
    }
}