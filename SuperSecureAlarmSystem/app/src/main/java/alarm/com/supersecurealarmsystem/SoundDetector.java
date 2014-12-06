package alarm.com.supersecurealarmsystem;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.widget.TextView;

public class SoundDetector extends Activity {
    private SoundMeter mRecorder = null;
    private TextView tv = null;
    protected static final String SERVER = "158.130.110.209";
    protected static final String PORT = "3000";
    private int count = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sound_detector);

        tv = (TextView)findViewById(R.id.tv);
        mRecorder = new SoundMeter();
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                    setUIText(tv, "started thread");
                while (true) {
                    if (mRecorder != null && mRecorder.isOn()) {
                        double amplitude = mRecorder.getAmplitude();
                        setUIText(tv, String.valueOf(amplitude));
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
        thread.start();
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