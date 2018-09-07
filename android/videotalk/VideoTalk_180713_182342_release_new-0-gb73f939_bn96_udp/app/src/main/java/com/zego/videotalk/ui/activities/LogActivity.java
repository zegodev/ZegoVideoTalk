package com.zego.videotalk.ui.activities;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.zego.videotalk.R;
import com.zego.videotalk.adapter.LogListAdapter;
import com.zego.videotalk.utils.AppLogger;
import com.zego.videotalk.utils.PrefUtil;
import com.zego.zegoliveroom.ZegoLiveRoom;

public class LogActivity extends AppCompatActivity {

    private PrefUtil.OnLogChangeListener mLogDataChangeListener;
    private Button button;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_log);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        // enable the up button
        ActionBar ab = getSupportActionBar();
        ab.setDisplayHomeAsUpEnabled(true);

        final RecyclerView logView = (RecyclerView) findViewById(R.id.log_view_area);
        logView.setLayoutManager(new LinearLayoutManager(this));

        LogListAdapter adapter = new LogListAdapter(this);
        logView.setAdapter(adapter);
        adapter.setData(AppLogger.getInstance().getAllLog());

        mLogDataChangeListener = new PrefUtil.OnLogChangeListener() {
            @Override
            public void onLogDataChanged() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        LogListAdapter adapter = (LogListAdapter) logView.getAdapter();
                        adapter.setData(AppLogger.getInstance().getAllLog());
                    }
                });
            }
        };
        PrefUtil.getInstance().registerLogChangeListener(mLogDataChangeListener);

        button = (Button) findViewById(R.id.log_update);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ZegoLiveRoom.uploadLog();
                Toast.makeText(LogActivity.this, getString(R.string.zg_update_log_success), Toast.LENGTH_LONG).show();
            }
        });
    }

    /**
     * Take care of popping the fragment back stack or finishing the activity
     * as appropriate.
     */
    @Override
    public void onBackPressed() {
        goBack();
    }

    /**
     * Initialize the contents of the Activity's standard options menu.  You
     * should place your menu items in to <var>menu</var>.
     * <p>
     * <p>This is only called once, the first time the options menu is
     * displayed.  To update the menu every time it is displayed, see
     * {@link #onPrepareOptionsMenu}.
     * <p>
     * <p>The default implementation populates the menu with standard system
     * menu items.  These are placed in the {@link Menu#CATEGORY_SYSTEM} group so that
     * they will be correctly ordered with application-defined menu items.
     * Deriving classes should always call through to the base implementation.
     * <p>
     * <p>You can safely hold on to <var>menu</var> (and any items created
     * from it), making modifications to it as desired, until the next
     * time onCreateOptionsMenu() is called.
     * <p>
     * <p>When you add items to the menu, you can implement the Activity's
     * {@link #onOptionsItemSelected} method to handle them there.
     *
     * @param menu The options menu in which you place your items.
     * @return You must return true for the menu to be displayed;
     * if you return false it will not be shown.
     * @see #onPrepareOptionsMenu
     * @see #onOptionsItemSelected
     */
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.log, menu);
        return true;
    }

    /**
     * This hook is called whenever an item in your options menu is selected.
     * The default implementation simply returns false to have the normal
     * processing happen (calling the item's Runnable or sending a message to
     * its Handler as appropriate).  You can use this method for any items
     * for which you would like to do processing without those other
     * facilities.
     * <p>
     * <p>Derived classes should call through to the base class for it to
     * perform the default menu handling.</p>
     *
     * @param item The menu item that was selected.
     * @return boolean Return false to allow normal menu processing to
     * proceed, true to consume it here.
     * @see #onCreateOptionsMenu
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                goBack();
                return true;

            case R.id.action_clean:
                AppLogger.getInstance().clearLog();
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void goBack() {
        if (mLogDataChangeListener != null) {
            PrefUtil.getInstance().unregisterLogChangeListener(mLogDataChangeListener);
        }
        finish();
    }
}
