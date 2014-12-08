package alarm.com.supersecurealarmsystem;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;


public class HomePageActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home_page);

    }


    public void startButtonListener(View v) {
        Intent intent = new Intent(getApplicationContext(), SoundDetector.class);
        startActivity(intent);
    }

    public void addContactButton(View v) {
        Intent intent = new Intent(getApplicationContext(), AddContactActivity.class);
        startActivity(intent);
    }
}
