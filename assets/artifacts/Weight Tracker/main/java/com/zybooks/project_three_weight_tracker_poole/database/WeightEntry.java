package com.zybooks.project_three_weight_tracker_poole.database;

import androidx.room.ColumnInfo;
import androidx.room.Entity;
import androidx.room.PrimaryKey;

@Entity(tableName = "weight_entries")
public class WeightEntry {

    @PrimaryKey(autoGenerate = true)
    private int id;

    private String date;
    private float weight;

    // New field to associate entry with a user
    @ColumnInfo(name = "username")
    private String username;

    // Updated constructor
    public WeightEntry(String date, float weight, String username) {
        this.date = date;
        this.weight = weight;
        this.username = username;
    }

    // Getters and Setters
    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getDate() {
        return date;
    }

    public void setDate(String date) {
        this.date = date;
    }

    public float getWeight() {
        return weight;
    }

    public void setWeight(float weight) {
        this.weight = weight;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }
}
