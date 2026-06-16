package com.zybooks.project_three_weight_tracker_poole;

import android.app.DatePickerDialog;
import android.content.Context;
import android.widget.EditText;
import java.util.Calendar;
import java.util.Locale;

public class DatePickerHelper {

    public static void attachDatePicker(Context context, EditText dateField) {

        // Prevent manual typing
        dateField.setFocusable(false);
        dateField.setClickable(true);

        dateField.setOnClickListener(v -> {
            Calendar calendar = Calendar.getInstance();
            int year = calendar.get(Calendar.YEAR);
            int month = calendar.get(Calendar.MONTH);
            int day = calendar.get(Calendar.DAY_OF_MONTH);

            DatePickerDialog datePickerDialog = new DatePickerDialog(
                    context,
                    (view, selectedYear, selectedMonth, selectedDay) -> {
                        Calendar selectedCal = Calendar.getInstance();
                        selectedCal.set(selectedYear, selectedMonth, selectedDay, 0, 0, 0);
                        selectedCal.set(Calendar.MILLISECOND, 0);

                        Calendar today = Calendar.getInstance();
                        today.set(Calendar.HOUR_OF_DAY, 0);
                        today.set(Calendar.MINUTE, 0);
                        today.set(Calendar.SECOND, 0);
                        today.set(Calendar.MILLISECOND, 0);

                        if (selectedCal.after(today)) {
                            dateField.setError("Date cannot be in the future");
                        } else {
                            String selectedDate = String.format(Locale.getDefault(), "%02d-%02d-%04d", selectedMonth + 1, selectedDay, selectedYear);
                            dateField.setText(selectedDate);
                            dateField.setError(null);
                        }
                    },
                    year, month, day
            );
            datePickerDialog.show();
        });
    }
}