package com.zego.videotalk;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.pgyersdk.update.PgyUpdateManager;
import com.zego.videotalk.ui.activities.SettingsActivity;
import com.zego.videotalk.ui.activities.VideoTalkActivity;
import com.zego.videotalk.utils.SystemUtil;

public class MainActivity extends AppCompatActivity {

    static final private int OPEN_SETTINGS_CODE = 1;
    static final private int REQUEST_PERMISSION_CODE = 101;

    private Toolbar mToolbar;
    private EditText mSessionIDEditText;
    private Button mStartButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!isTaskRoot()) {
            /* If this is not the root activity */
            Intent intent = getIntent();
            String action = intent.getAction();
            if (intent.hasCategory(Intent.CATEGORY_LAUNCHER) && Intent.ACTION_MAIN.equals(action)) {
                finish();
                return;
            }
        }

        setContentView(R.layout.activity_main);
        mToolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(mToolbar);

        mStartButton = (Button) findViewById(R.id.start_button);
        mStartButton.setEnabled(false);

        mSessionIDEditText = (EditText) findViewById(R.id.sessionid_edittext);
        mSessionIDEditText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                if (s.length() > 0) {
                    mStartButton.setEnabled(true);
                } else {
                    mStartButton.setEnabled(false);
                }
            }
        });

        mStartButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean permissionGranted = checkOrRequestPermission();
                if (permissionGranted) {
                    gotoVideoTalk();
                }
            }
        });
        if (checkOrRequestPermission(1002) && !SystemUtil.isDebugVersion(this)) {

            /** 可选配置集成方式 **/
            new PgyUpdateManager.Builder()
                    .setForced(false)                //设置是否强制更新
                    .setUserCanRetry(false)         //失败后是否提示重新下载
                    .setDeleteHistroyApk(false)     // 检查更新前是否删除本地历史 Apk， 默认为true
                    .register();

        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    private static String[] PERMISSIONS_STORAGE = {
            "android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE", Manifest.permission.CAMERA, Manifest.permission.RECORD_AUDIO};

    private boolean checkOrRequestPermission(int code) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED
                    || ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED ||
                    ContextCompat.checkSelfPermission(this, "android.permission.READ_EXTERNAL_STORAGE") != PackageManager.PERMISSION_GRANTED ||
                    ContextCompat.checkSelfPermission(this, "android.permission.WRITE_EXTERNAL_STORAGE") != PackageManager.PERMISSION_GRANTED) {
                this.requestPermissions(PERMISSIONS_STORAGE, code);
                return false;
            }
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_settings:
                // open setting activity
                startActivityForResult(new Intent(MainActivity.this, SettingsActivity.class), OPEN_SETTINGS_CODE);
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case OPEN_SETTINGS_CODE:
                // TODO: 刷新配置信息
                break;
            default:
                super.onActivityResult(requestCode, resultCode, data);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        switch (requestCode) {
            case REQUEST_PERMISSION_CODE: {
                boolean allPermissionGranted = true;
                for (int i = 0; i < grantResults.length; i++) {
                    if (grantResults[i] != PackageManager.PERMISSION_GRANTED) {
                        allPermissionGranted = false;
                        Toast.makeText(this, getString(R.string.vt_toast_permission_denied, permissions[i]), Toast.LENGTH_LONG).show();
                    }
                }
                if (allPermissionGranted) {
                    gotoVideoTalk();
                } else {
                    Intent intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
                    intent.setData(Uri.parse("package:" + getPackageName()));
                    startActivity(intent);
                }
            }
            break;
        }
    }

    private boolean checkOrRequestPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED
                    || ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, new String[]{
                        Manifest.permission.CAMERA, Manifest.permission.RECORD_AUDIO}, REQUEST_PERMISSION_CODE);
                return false;
            }
        }
        return true;
    }

    private void gotoVideoTalk() {
        // 设置app朝向
        int currentOrientation = getWindowManager().getDefaultDisplay().getRotation();
        ZegoAppHelper.getLiveRoom().setAppOrientation(currentOrientation);

        String sessionID = mSessionIDEditText.getText().toString();
        if (sessionID.contains(" ")) {
            Toast.makeText(this, R.string.vt_input_session_id_no_allow_contain_white_space, Toast.LENGTH_LONG).show();
            return;
        }
        if (sessionID.length() > 0) {
            // 将sessionID传给下一个页面
            Intent intent = new Intent(MainActivity.this, VideoTalkActivity.class);
            intent.putExtra("sessionId", sessionID.trim());
            intent.putExtra("orientation", currentOrientation);
            startActivity(intent);
        } else {
            Toast.makeText(this, R.string.vt_hint_input_session_id, Toast.LENGTH_LONG).show();
        }
    }
}
