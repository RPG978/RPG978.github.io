package com.zybooks.project_three_weight_tracker_poole;

import androidx.room.ColumnInfo;
import androidx.room.Entity;
import androidx.room.PrimaryKey;

@Entity
public class User {

    @PrimaryKey(autoGenerate = true)
    public int id;

    public String username;
    @ColumnInfo(name = "goal_weight")
    public Float goalWeight;
    public String passwordHash;
}
