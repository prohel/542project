package alarm.com.supersecurealarmsystem;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;
import android.os.Bundle;
import android.os.Looper;
import android.util.Log;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Date;


public class SoundDetector extends Activity {
    protected static final String SERVER = "158.130.106.40";
    protected static final String PORT = "3000";
    protected static final float timeLimit = 3600;

    private SoundMeter mRecorder = null;
    private Accelerometer mAccelerometer = null;
    private TextView tv = null;
    private TextView tv2 = null;
    private int connectionAttempts = 0;
    IntentFilter ifilter;
    Intent batteryStatus;
    float startBatteryLevel = 0;
    long lStartTime;
    long lCurTime;
    float avgConsumption;
    boolean isBatteryDying = false;
    boolean hasDeathBeenSent = false;

    boolean keepRunning;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sound_detector);
    }

    @Override
    protected void onResume() {
        super.onResume();
        lStartTime = new Date().getTime();
        tv = (TextView)findViewById(R.id.tv);
        tv2 = (TextView) findViewById(R.id.tv2);
        ifilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        batteryStatus = getApplicationContext().registerReceiver(null, ifilter);
        mRecorder = new SoundMeter();
        keepRunning = true;
        mAccelerometer = new Accelerometer(getApplicationContext());
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                Looper.prepare();
                setUIText(tv, "started thread");
                while (keepRunning) {
                    if (mRecorder != null && mRecorder.isOn()) {
                        boolean response = true;
                        double amplitude = mRecorder.getAmplitude();
                        double accel = mAccelerometer.getAccel();
                        setUIText(tv, String.valueOf(amplitude));
                        setUIText(tv2, String.valueOf(accel));
                        getBatteryLevel();
                        if (amplitude > 20000)
                            response = sendMessage("audio", String.valueOf(amplitude));
                        else if (Math.abs(accel) > 1)
                            response = sendMessage("accel", String.format("%.2f", accel));
                        else if (isBatteryDying && !hasDeathBeenSent) {
                            response = sendMessage("death", "warning");
                            hasDeathBeenSent = true;
                        }
                        else
                            response = sendMessage("foo", String.format("%.2f", accel));
                        if (response == false)
                            connectionAttempts += 1;
                        else
                            connectionAttempts = 0;

                        if (connectionAttempts > 10) {
                            makeToast("Cannot connect to server");
                            connectionAttempts = 0;
                        }

                    }
                    else {
                        setUIText(tv, "no recorder");
                    }
                    try {Thread.sleep(500);} catch (InterruptedException e) {}
                }
            }
        });
        mRecorder.start();
        mAccelerometer.start();
        thread.start();
    }

    @Override

    protected void onPause() {
        super.onPause();
        keepRunning = false;
        mAccelerometer.stop();
        mRecorder.stop();
    }

    private boolean sendMessage(String type, String value) {
        HttpRequest newRequest = new HttpRequest(type, value);
        try {
            return newRequest.sendGet();
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    public void setUIText(final TextView view, final String text) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                view.setText(text);
            }
        });
    }

    public void makeToast(final String s) {
       runOnUiThread(new Runnable(){
        @Override
        public void run() {
            makeToast(s, Toast.LENGTH_LONG);
        }
       });
    }

    /**
     * Makes and shows a Toast with the given string for for the
     * specified duration
     */
    public void makeToast(String s, int duration) {
        Context context = getApplicationContext();
        Toast toast = Toast.makeText(context, s, duration);
        toast.show();
    }

    public float getBatteryLevel() {
        int level = batteryStatus.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
        int scale = batteryStatus.getIntExtra(BatteryManager.EXTRA_SCALE, -1);

        float batteryPct = level / (float)scale;
        Log.v("SoundDectector", "battery scale: " + scale);
        Log.v("SoundDectector", "battery level: " + level);
        if (startBatteryLevel == 0) {
            startBatteryLevel = level;
            return batteryPct;
        } else {
            lCurTime = new Date().getTime();
            long difference = (long) ((lCurTime - lStartTime) / 1000.0);;
            avgConsumption = (level - startBatteryLevel)/difference;
        }
        if (avgConsumption * timeLimit > scale)
            isBatteryDying = true;
        return batteryPct;
    }
}