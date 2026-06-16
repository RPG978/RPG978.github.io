package com.rpg978.reelchoice;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.viewpager2.adapter.FragmentStateAdapter;

/**
 * MoviePagerAdapter
 *
 * Adapter for ViewPager2 that supplies the two main fragments:
 *   - Position 0 -> ToWatchFragment
 *   - Position 1 -> WatchedFragment
 *
 * Responsibilities:
 *   - Create the correct fragment for each page.
 *   - Report the number of pages (2 total).
 *
 * Notes:
 *   - FragmentStateAdapter automatically handles fragment lifecycle,
 *     saving/restoring state as the user switches tabs.
 *   - New fragment instances are created on demand; ViewPager2 manages caching.
 */
public class MoviePagerAdapter extends FragmentStateAdapter {

    /**
     * Constructor receives the hosting FragmentActivity.
     * ViewPager2 requires a lifecycle owner to manage fragment state.
     */
    public MoviePagerAdapter(@NonNull FragmentActivity fa) {
        super(fa);
    }

    /**
     * Returns the fragment for the given page index.
     *
     * Position mapping:
     *   0 -> ToWatchFragment
     *   1 -> WatchedFragment
     *
     * This keeps the tab layout simple and predictable.
     */
    @NonNull
    @Override
    public Fragment createFragment(int position) {
        return position == 0 ? new ToWatchFragment() : new WatchedFragment();
    }

    /**
     * Total number of pages in the ViewPager.
     * Currently fixed at 2, matching the two movie lists.
     */
    @Override
    public int getItemCount() {
        return 2;
    }
}
