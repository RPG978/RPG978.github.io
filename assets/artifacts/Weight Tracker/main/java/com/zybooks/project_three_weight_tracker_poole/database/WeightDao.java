package com.zybooks.project_three_weight_tracker_poole.database;

import androidx.room.Dao;
import androidx.room.Delete;
import androidx.room.Insert;
import androidx.room.Query;
import java.util.List;

@Dao
public interface WeightDao {

    @Query("SELECT * FROM weight_entries WHERE username = :username ORDER BY date DESC")
    List<WeightEntry> getEntriesForUser(String username);

    @Insert
    void insert(WeightEntry entry);

    @Query("SELECT * FROM weight_entries WHERE id = :id LIMIT 1")
    WeightEntry getEntryById(int id);

    @Delete
    void deleteWeight(WeightEntry entry);

    @androidx.room.Update
    void updateWeight(WeightEntry entry);
}