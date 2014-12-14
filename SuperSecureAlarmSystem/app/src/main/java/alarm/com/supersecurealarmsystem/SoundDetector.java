package alarm.com.supersecurealarmsystem;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Looper;
import android.util.Log;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;


public class SoundDetector extends Activity {
    private SoundMeter mRecorder = null;
    private Accelerometer mAccelerometer = null;
    private TextView tv = null;
    private TextView tv2 = null;
    protected static final String SERVER = "158.130.106.40";
    protected static final String PORT = "3000";
    private int connectionAttempts = 0;

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
                Looper.prepare();
                setUIText(tv, "started thread");
                while (keepRunning) {
                    if (mRecorder != null && mRecorder.isOn()) {
                        boolean response = true;
                        double amplitude = mRecorder.getAmplitude();
                        double accel = mAccelerometer.getAccel();
                        setUIText(tv, String.valueOf(amplitude));
                        setUIText(tv2, String.valueOf(accel));
                        if (amplitude > 20000)
                            response = sendMessage("audio", String.valueOf(amplitude));
                        if (Math.abs(accel) > 1)
                            response = sendMessage("accel", String.format("%.2f", accel));
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
}