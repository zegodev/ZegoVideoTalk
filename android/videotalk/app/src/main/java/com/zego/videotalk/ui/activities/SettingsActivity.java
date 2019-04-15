package com.zego.videotalk.ui.activities;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.AppCompatSeekBar;
import android.support.v7.widget.Toolbar;
import android.text.TextUtils;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.alibaba.fastjson.JSONObject;
import com.dommy.qrcode.util.Constant;
import com.google.zxing.activity.CaptureActivity;
import com.zego.videotalk.R;
import com.zego.videotalk.VideoTalkApplication;
import com.zego.videotalk.ZegoAppHelper;
import com.zego.videotalk.entity.AppConfig;
import com.zego.videotalk.ui.widgets.CustomSeekBar;
import com.zego.videotalk.utils.ByteSizeUnit;
import com.zego.videotalk.utils.PrefUtil;
import com.zego.videotalk.utils.SystemUtil;
import com.zego.zegoliveroom.ZegoLiveRoom;
import com.zego.zegoliveroom.constants.ZegoAvConfig;


public class SettingsActivity extends AppCompatActivity {

    private EditText mUserNameView;
    private Spinner mAppFlavorView;
    private Spinner mLayeredCoding;
    private EditText mAppIdView;
    private EditText mAppSignKeyView;
    private LinearLayout mAppSignKeyLayout;

    private Spinner mLiveQualityView;
    private TextView mResolutionDescView;
    private CustomSeekBar mResolutionView;

    private TextView mFPSDescView;
    private CustomSeekBar mFPSView;

    private TextView mBitrateDescView;
    private CustomSeekBar mBitrateView;
    private TextView userIdView;
    private CheckBox mHardwareEncodeView;
    private CheckBox mHardwareDecodeView;
    private CheckBox mTestEncodeView;
    private TextView tvDemoVersion;

    private String[] mResolutionText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        // enable the up button
        ActionBar ab = getSupportActionBar();
        ab.setDisplayHomeAsUpEnabled(true);

        mResolutionText = getResources().getStringArray(R.array.zg_resolutions);

        initCtrls();
    }

    private final static int REQ_CODE = 1028;

    public void scan(View view) {
        if (checkOrRequestPermission(REQUEST_PERMISSION_CODE)) {
            startActivityForResult(new Intent(this, CaptureActivity.class), REQ_CODE);
        }
    }

    private final int REQUEST_PERMISSION_CODE = 0506;

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

    AppConfig appConfig = null;

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQ_CODE && data != null && resultCode == RESULT_OK) {
            Bundle bundle = data.getExtras();
            String result = bundle.getString(Constant.INTENT_EXTRA_KEY_QR_SCAN);
            if (result != null) {
                try {
                    JSONObject jsonObject = JSONObject.parseObject(result);
                    if ("ac".equals(jsonObject.getString("type"))) {
                        appConfig = jsonObject.getObject("data", AppConfig.class);
                        PrefUtil.getInstance().setAppId(appConfig.appid);
                        PrefUtil.getInstance().setAppSignKey(appConfig.appkey);
                        PrefUtil.getInstance().setTestEncode(appConfig.isTestenv());
                        PrefUtil.getInstance().setBusinessType(appConfig.getBusinesstype());
                        PrefUtil.getInstance().setInternational(appConfig.isI18n());
                        PrefUtil.getInstance().setAppFlavor(3);
                        setResult(RESULT_OK);
                        Toast.makeText(this, getString(R.string.zg_toast_config_successful), Toast.LENGTH_LONG).show();
                        ((VideoTalkApplication) getApplication()).reInitZegoSDK();
                        finish();
                    }
                } catch (Exception e) {
                    Toast.makeText(this, getString(R.string.zg_toast_config_failure), Toast.LENGTH_LONG).show();
                }
            }
        }
    }


    /**
     * Take care of popping the fragment back stack or finishing the activity
     * as appropriate.
     */
    @Override
    public void onBackPressed() {
        goBack();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                goBack();
                return true;
        }
        return super.onOptionsItemSelected(item);
    }


    public void shareLog(View view) {
        ZegoLogShareActivity.actionStart(this);

    }

    boolean shouldReInitSDK = false;

    private void goBack() {


        String newUserName = mUserNameView.getText().toString().trim();
        if (TextUtils.isEmpty(newUserName)) {
            mUserNameView.requestFocus();
            Toast.makeText(this, R.string.zg_toast_username_format_error, Toast.LENGTH_LONG).show();
            return;
        } else if (!TextUtils.equals(newUserName, PrefUtil.getInstance().getUserName())) {
            PrefUtil.getInstance().setUserName(newUserName);
            shouldReInitSDK = true;
        }
        String userId = userIdView.getText().toString().trim();
        if (TextUtils.isEmpty(userId)) {
            Toast.makeText(this, R.string.zg_toast_user_id_format_error, Toast.LENGTH_LONG).show();
            return;
        } else if (!TextUtils.equals(userId, PrefUtil.getInstance().getUserId())) {
            PrefUtil.getInstance().setUserId(userId);
            shouldReInitSDK = true;
        }

        String strAppId = mAppIdView.getText().toString().trim();
        if (TextUtils.isEmpty(strAppId) || !TextUtils.isDigitsOnly(strAppId)) {
            mAppIdView.requestFocus();
            Toast.makeText(this, R.string.zg_toast_app_id_format_error, Toast.LENGTH_LONG).show();
            return;
        }
        String strSignKey = mAppSignKeyView.getText().toString().trim();
        if (TextUtils.isEmpty(strSignKey)) {
            mAppSignKeyView.requestFocus();
            Toast.makeText(this, R.string.zg_toast_app_sign_key_format_error, Toast.LENGTH_LONG).show();
            return;
        }

        boolean mTest = mTestEncodeView.isChecked();
        if (mTest != PrefUtil.getInstance().getTestEncode()) {
            shouldReInitSDK = true;
        }


        if (PrefUtil.getInstance().getCurrentAppFlavor() != mAppFlavorView.getSelectedItemPosition()) {
            shouldReInitSDK = true;
        }

        try {
            ZegoAppHelper.parseSignKeyFromString(strSignKey);   // 测试 signKey 是否合法
        } catch (NumberFormatException e) {
            mAppSignKeyView.requestFocus();
            Toast.makeText(this, R.string.zg_toast_app_sign_key_format_error, Toast.LENGTH_LONG).show();
            return;
        }

        PrefUtil prefUtil = PrefUtil.getInstance();
        final long newAppId = Long.valueOf(strAppId);
        if (newAppId != prefUtil.getAppId() && !TextUtils.equals(strSignKey, prefUtil.getAppSignKey())) {
            if (mAppFlavorView.getSelectedItemPosition() == 3) {
                prefUtil.setAppId(newAppId);
                prefUtil.setAppSignKey(strSignKey);
            }
            shouldReInitSDK = true;
        }

        prefUtil.setAppFlavor(mAppFlavorView.getSelectedItemPosition());

        prefUtil.setLayeredCoding(mLayeredCoding.getSelectedItemPosition());

        prefUtil.setLiveQuality(mLiveQualityView.getSelectedItemPosition());

        prefUtil.setLiveQualityResolution(mResolutionView.getProgress());
        prefUtil.setLiveQualityFps(mFPSView.getProgress());
        prefUtil.setLiveQualityBitrate(mBitrateView.getProgress());

        prefUtil.setHardwareEncode(mHardwareEncodeView.isChecked());
        prefUtil.setHardwareDecode(mHardwareDecodeView.isChecked());
        prefUtil.setTestEncode(mTestEncodeView.isChecked());

        if (shouldReInitSDK) {

            ((VideoTalkApplication) getApplication()).reInitZegoSDK();

        } else {


            boolean useHardwareEncode = mHardwareEncodeView.isChecked();
            ZegoLiveRoom.requireHardwareEncoder(useHardwareEncode);
            ZegoLiveRoom.requireHardwareDecoder(mHardwareDecodeView.isChecked());

            ZegoLiveRoom liveRoom = ZegoAppHelper.getLiveRoom();

            if (useHardwareEncode) {
                liveRoom.enableRateControl(false);
            } else {
                liveRoom.enableRateControl(true);
            }

            ZegoAvConfig config;
            if (mLiveQualityView.getSelectedItemPosition() > ZegoAvConfig.Level.SuperHigh) {
                config = new ZegoAvConfig(ZegoAvConfig.Level.High);
                config.setVideoBitrate(mBitrateView.getProgress());
                config.setVideoFPS(mFPSView.getProgress());
                int resolutionLevel = mResolutionView.getProgress();

                String resolutionText = getResources().getStringArray(R.array.zg_resolutions)[resolutionLevel];
                String[] strWidthHeight = resolutionText.split("x");

                int height = Integer.parseInt(strWidthHeight[0].trim());
                int width = Integer.parseInt(strWidthHeight[1].trim());
                config.setVideoEncodeResolution(width, height);
                config.setVideoCaptureResolution(width, height);
            } else {
                config = new ZegoAvConfig(mLiveQualityView.getSelectedItemPosition());
            }
            liveRoom.setAVConfig(config);
        }

        setResult(RESULT_OK);
        finish();
    }

    private void initCtrls() {
        ZegoLiveRoom liveRoom = ZegoAppHelper.getLiveRoom();

        userIdView = (TextView) findViewById(R.id.tv_user_id);
        userIdView.setText(PrefUtil.getInstance().getUserId());

        mUserNameView = (EditText) findViewById(R.id.tv_user_name);
        mUserNameView.setText(PrefUtil.getInstance().getUserName());

        mAppFlavorView = (Spinner) findViewById(R.id.sp_app_flavor);
        mAppFlavorView.setSelection(PrefUtil.getInstance().getCurrentAppFlavor());
        mAppFlavorView.setOnItemSelectedListener(mItemSelectedListener);

        mAppIdView = (EditText) findViewById(R.id.et_app_id);
        mAppIdView.setText(String.valueOf(PrefUtil.getInstance().getAppId()));

        mAppSignKeyView = (EditText) findViewById(R.id.et_app_key);
        mAppSignKeyView.setText(PrefUtil.getInstance().getAppSignKey());

        mAppSignKeyLayout = (LinearLayout) findViewById(R.id.ll_app_key);

        // 默认设置级别为"高"
        int defaultLevel = PrefUtil.getInstance().getLiveQuality();

        mLiveQualityView = (Spinner) findViewById(R.id.sp_resolutions);
        mLiveQualityView.setSelection(defaultLevel, false);

        mLiveQualityView.setOnItemSelectedListener(mItemSelectedListener);
        mResolutionDescView = (TextView) findViewById(R.id.tv_resolution);
        mResolutionView = (CustomSeekBar) findViewById(R.id.sb_resolution);
        mResolutionView.setMax(ZegoAvConfig.VIDEO_BITRATES.length - 1);
        mResolutionView.setOnSeekBarChangeListener(mSeekBarChangeListener);
        mResolutionView.setProgress(PrefUtil.getInstance().getLiveQualityResolution());

        int defaultLayeredCoding = PrefUtil.getInstance().getLayeredCoding();
        mLayeredCoding = (Spinner) findViewById(R.id.sp_layered_coding);
        mLayeredCoding.setSelection(defaultLayeredCoding);


        mResolutionDescView.setText(getString(R.string.resolution_prefix, mResolutionText[PrefUtil.getInstance().getLiveQualityResolution()]));

        int defaultFPS = PrefUtil.getInstance().getLiveQualityFps();
        mFPSView = (CustomSeekBar) findViewById(R.id.sb_fps);
        mFPSView.setMax(30);
        mFPSView.setProgress(defaultFPS);
        mFPSView.setOnSeekBarChangeListener(mSeekBarChangeListener);
        mFPSDescView = (TextView) findViewById(R.id.tv_fps);
        mFPSDescView.setText(getString(R.string.fps_prefix, String.valueOf(defaultFPS)));


        int defaultBitrate = PrefUtil.getInstance().getLiveQualityBitrate();
        mBitrateView = (CustomSeekBar) findViewById(R.id.sb_bitrate);
        mBitrateView.setMax(ZegoAvConfig.VIDEO_BITRATES[ZegoAvConfig.VIDEO_BITRATES.length - 1] + 1000 * 1000);
        mBitrateView.setProgress(defaultBitrate);
        mBitrateView.setOnSeekBarChangeListener(mSeekBarChangeListener);
        mBitrateDescView = (TextView) findViewById(R.id.tv_bitrate);
        mBitrateDescView.setText(getString(R.string.bitrate_prefix, ByteSizeUnit.toHumanString(defaultBitrate, ByteSizeUnit.RADIX_TYPE.K, 2)) + "ps");

        mHardwareEncodeView = (CheckBox) findViewById(R.id.zg_checkbox_user_hardware_encode);
        mHardwareEncodeView.setChecked(PrefUtil.getInstance().getHardwareEncode());

        mHardwareDecodeView = (CheckBox) findViewById(R.id.zg_checkbox_hardware_decode);
        mHardwareDecodeView.setChecked(PrefUtil.getInstance().getHardwareDecode());

        mTestEncodeView = (CheckBox) findViewById(R.id.zg_checkbox_test);
        mTestEncodeView.setChecked(PrefUtil.getInstance().getTestEncode());
        tvDemoVersion = (TextView) findViewById(R.id.tv_demo_version);
        tvDemoVersion.setText(SystemUtil.getAppVersionName(this));

        if (defaultLevel == 6) {
            mResolutionView.setEnabled(true);
            mFPSView.setEnabled(true);
            mBitrateView.setEnabled(true);
        } else {
            mResolutionView.setEnabled(false);
            mFPSView.setEnabled(false);
            mBitrateView.setEnabled(false);
        }

        TextView sdkVersionView = (TextView) findViewById(R.id.tv_version);
        sdkVersionView.setText(liveRoom.version());

        TextView veVersionView = (TextView) findViewById(R.id.tv_version2);
        veVersionView.setText(liveRoom.version2());
    }

    private void changeAppFlavor(int index) {
        shouldReInitSDK = true;
        PrefUtil.getInstance().setAppWebRtc(false);
        if (index == 3) {
            mAppIdView.setText(String.valueOf(PrefUtil.getInstance().getAppId()));
            mAppIdView.setEnabled(true);

            mAppSignKeyView.setText(String.valueOf(PrefUtil.getInstance().getAppSignKey()));
            mAppSignKeyLayout.setVisibility(View.VISIBLE);
        } else {
            long appId = 0;
            switch (index) {
                case 0:
                    appId = ZegoAppHelper.UDP_APP_ID;
                    break;
                case 1:
                    appId = ZegoAppHelper.INTERNATIONAL_APP_ID;
                    break;
                case 2:
                    appId = ZegoAppHelper.UDP_APP_ID;
                    PrefUtil.getInstance().setAppWebRtc(true);
                    break;
            }
            mAppIdView.setEnabled(false);
            mAppIdView.setText(String.valueOf(appId));

            byte[] signKey = ZegoAppHelper.requestSignKey(appId);
            mAppSignKeyView.setText(ZegoAppHelper.convertSignKey2String(signKey));
            mAppSignKeyLayout.setVisibility(View.GONE);
        }
    }

    private void changeResolution(int index) {

        if (index < ZegoAvConfig.VIDEO_BITRATES.length) {
            int level = index;
            mResolutionView.setProgress(level);
            // 预设级别中,帧率固定为"15"
            mFPSView.setProgress(15);
            mBitrateView.setProgress(ZegoAvConfig.VIDEO_BITRATES[level]);
            if (level == 5) {
                enableHardwareCodec();
            }
            mResolutionView.setEnabled(false);
            mFPSView.setEnabled(false);
            mBitrateView.setEnabled(false);
        } else {
            mResolutionView.setEnabled(true);
            mFPSView.setEnabled(true);
            mBitrateView.setEnabled(true);
        }
    }

    private void enableHardwareCodec(){
        if (!mHardwareDecodeView.isChecked() || !mHardwareEncodeView.isChecked()) {
            mHardwareEncodeView.setChecked(true);
            mHardwareDecodeView.setChecked(true);
            Toast.makeText(this, getString(R.string.tips_auto_open_hardware_codec), Toast.LENGTH_LONG).show();

            PrefUtil.getInstance().setHardwareEncode(true);

            PrefUtil.getInstance().setHardwareDecode(true);
        }
    }

    private AdapterView.OnItemSelectedListener mItemSelectedListener = new AdapterView.OnItemSelectedListener() {

        /**
         * <p>Callback method to be invoked when an item in this view has been
         * selected. This callback is invoked only when the newly selected
         * position is different from the previously selected position or if
         * there was no selected item.</p>
         * <p>
         * Impelmenters can call getItemAtPosition(position) if they need to access the
         * data associated with the selected item.
         *
         * @param parent   The AdapterView where the selection happened
         * @param view     The view within the AdapterView that was clicked
         * @param position The position of the view in the adapter
         * @param id       The row id of the item that is selected
         */
        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            switch (parent.getId()) {
                case R.id.sp_app_flavor:
                    changeAppFlavor(position);
                    break;

                case R.id.sp_resolutions:
                    changeResolution(position);
                    break;
            }
        }

        /**
         * Callback method to be invoked when the selection disappears from this
         * view. The selection can disappear for instance when touch is activated
         * or when the adapter becomes empty.
         *
         * @param parent The AdapterView that now contains no selected item.
         */
        @Override
        public void onNothingSelected(AdapterView<?> parent) {

        }
    };

    private AppCompatSeekBar.OnSeekBarChangeListener mSeekBarChangeListener = new AppCompatSeekBar.OnSeekBarChangeListener() {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            switch (seekBar.getId()) {
                case R.id.sb_resolution:
                    mResolutionDescView.setText(getString(R.string.resolution_prefix, mResolutionText[progress]));
                    if (seekBar.isPressed() && progress == 5) {
                        enableHardwareCodec();
                    }
                    break;

                case R.id.sb_fps:
                    mFPSDescView.setText(getString(R.string.fps_prefix, progress + ""));
                    break;

                case R.id.sb_bitrate:
                    mBitrateDescView.setText(getString(R.string.bitrate_prefix, ByteSizeUnit.toHumanString(progress, ByteSizeUnit.RADIX_TYPE.K, 2)) + "ps");
                    break;
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {

        }
    };
}
