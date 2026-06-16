package com.zybooks.project_three_weight_tracker_poole;

import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;
import java.util.ArrayList;
import java.util.List;
import com.zybooks.project_three_weight_tracker_poole.database.WeightDao;
import com.zybooks.project_three_weight_tracker_poole.database.WeightEntry;

public class WeightEntryAdapter extends RecyclerView.Adapter<WeightEntryAdapter.WeightViewHolder> {

    private final List<WeightEntry> weightEntryList;
    private final WeightDao weightDao;

    public WeightEntryAdapter(List<WeightEntry> weightEntryList, WeightDao weightDao) {
        this.weightEntryList = new ArrayList<>(weightEntryList); // Make it mutable
        this.weightDao = weightDao;
    }

    @NonNull
    @Override
    public WeightViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.weight_entry_item, parent, false);
        return new WeightViewHolder(itemView);
    }

    @Override
    public void onBindViewHolder(@NonNull WeightViewHolder holder, int position) {
        WeightEntry entry = weightEntryList.get(position);

        // Format date: "MM-dd-yyyy" → "MM-dd\nyyyy"
        String rawDate = entry.getDate(); // e.g., "08-14-2025"
        String[] parts = rawDate.split("-");
        if (parts.length == 3) {
            String formattedDate = parts[0] + "-" + parts[1] + "\n" + parts[2];
            holder.dateText.setText(formattedDate);
        }
        else {
            holder.dateText.setText(rawDate); // fallback
        }

        // Format weight
        String weightText = holder.itemView.getContext()
                .getString(R.string.weight_display, entry.getWeight());
        holder.weightText.setText(weightText);

        // Delete button logic
        holder.deleteButton.setOnClickListener(v -> {
            weightDao.deleteWeight(entry);
            weightEntryList.remove(position);
            notifyItemRemoved(position);
        });

        // Edit button logic
        holder.editButton.setOnClickListener(v -> {
            Intent intent = new Intent(v.getContext(), EditWeightEntryActivity.class);
            intent.putExtra("entry_id", entry.getId());
            v.getContext().startActivity(intent);
        });
    }

    @Override
    public int getItemCount() {
        return weightEntryList.size();
    }

    // method to refresh the list
    public void updateData(List<WeightEntry> newEntries) {
        weightEntryList.clear();
        weightEntryList.addAll(newEntries);
        notifyDataSetChanged();
    }

    public static class WeightViewHolder extends RecyclerView.ViewHolder {
        TextView dateText, weightText;
        Button deleteButton;
        Button editButton;

        public WeightViewHolder(@NonNull View itemView) {
            super(itemView);
            dateText = itemView.findViewById(R.id.dateText);
            weightText = itemView.findViewById(R.id.weightText);
            deleteButton = itemView.findViewById(R.id.deleteButton);
            editButton = itemView.findViewById(R.id.editButton);
        }
    }
}