package com.zybooks.project_three_weight_tracker_poole;

import androidx.room.Dao;
import androidx.room.Insert;
import androidx.room.Query;

@Dao
public interface UserDao {
    @Insert
    void insertUser(User user);

    @Query("SELECT * FROM User WHERE username = :username LIMIT 1")
    User getUser(String username);

    @Query("UPDATE User SET goal_weight = :goalWeight WHERE username = :username")
    void setGoalWeight(String username, float goalWeight);

    @Query("SELECT goal_weight FROM User WHERE username = :username LIMIT 1")
    Float getGoalWeight(String username);

}
