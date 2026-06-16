package com.rpg978.reelchoice;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.viewpager2.widget.ViewPager2;

/**
 * NonSwipeableViewPager
 *
 * A custom container that hosts a ViewPager2 instance while disabling all
 * user‑initiated swipe gestures. This allows the app to control page changes
 * exclusively through TabLayout or programmatic navigation.
 *
 * Why this exists:
 *   - ViewPager2 does not provide a built‑in "disable swipe" flag.
 *   - Simply disabling touch on ViewPager2 breaks internal gesture handling.
 *   - Wrapping it in a parent view allows touch events to be intercepted cleanly.
 *
 * Behavior:
 *   - Creates its own internal ViewPager2 instance during initialization.
 *   - Disables horizontal swiping via setUserInputEnabled(false).
 *   - Exposes the ViewPager2 through getViewPager() so MainActivity can attach
 *     adapters, TabLayoutMediator, and other configuration.
 *
 * Usage:
 *   - Declared in activity_main.xml as a standalone custom view.
 *   - MainActivity retrieves the internal ViewPager2 via getViewPager().
 *   - MoviePagerAdapter supplies the two fragments ("To Watch" and "Watched").
 */
public class NonSwipeableViewPager extends FrameLayout {

    // Internal ViewPager2 instance managed entirely by this wrapper.
    private ViewPager2 viewPager;

    /**
     * Constructor used when creating the view programmatically.
     */
    public NonSwipeableViewPager(@NonNull Context context) {
        super(context);
        init(context);
    }

    /**
     * Constructor used when inflating from XML.
     */
    public NonSwipeableViewPager(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    /**
     * Constructor used when inflating from XML with a style attribute.
     */
    public NonSwipeableViewPager(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    /**
     * Initializes the internal ViewPager2 instance.
     * This method is guaranteed to run regardless of which constructor
     * Android calls during inflation.
     */
    private void init(Context context) {
        viewPager = new ViewPager2(context);
        viewPager.setUserInputEnabled(false); // disable swiping
        addView(viewPager);
    }

    /**
     * Provides access to the internal ViewPager2 so callers can attach
     * adapters, mediators, and other configuration.
     */
    public ViewPager2 getViewPager() {
        return viewPager;
    }
}
