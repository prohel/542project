package alarm.com.supersecurealarmsystem;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.widget.TextView;


public class SoundDetector extends Activity {
    private SoundMeter mRecorder = null;
    private Accelerometer mAccelerometer = null;
    private TextView tv = null;
    private TextView tv2 = null;
    protected static final String SERVER = "158.130.106.198";
    protected static final String PORT = "3000";
    private int count = 0;

    boolean keepRunning;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sound_detector);
    }

    @Override
    protected void onResume() {
        super.onResume();
        tv = (TextView)findViewById(R.id.tv);
        tv2 = (TextView) findViewById(R.id.tv2);
        mRecorder = new SoundMeter();
        keepRunning = true;
        mAccelerometer = new Accelerometer(getApplicationContext());
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                setUIText(tv, "started thread");
                while (keepRunning) {
                    if (mRecorder != null && mRecorder.isOn()) {
                        double amplitude = mRecorder.getAmplitude();
                        double accel = mAccelerometer.getAccel();
                        setUIText(tv, String.valueOf(amplitude));
                        setUIText(tv2, String.valueOf(accel));
                        if (amplitude > 20000)
                            sendMessage("audio", String.valueOf(amplitude));
                    }
                    else {
                        setUIText(tv, "no recorder");
                    }
                    try {Thread.sleep(5000);} catch (InterruptedException e) {}
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
    }

    private void sendMessage(String type, String value) {
        HttpRequest newRequest = new HttpRequest(type, value);
        try {
            newRequest.sendGet();
            count += 1;
            Log.v("SoundDetector", "" + count);
        } catch (Exception e) {
            e.printStackTrace();
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
}