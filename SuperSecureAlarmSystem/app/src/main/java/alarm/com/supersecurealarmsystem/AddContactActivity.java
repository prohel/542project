package alarm.com.supersecurealarmsystem;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.StrictMode;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;


public class AddContactActivity extends Activity {

    EditText phoneNumber;
    EditText emailAddress;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_add_contact);

        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);

        phoneNumber = (EditText) findViewById(R.id.phone_input);
        emailAddress = (EditText) findViewById(R.id.email_input);
    }

    /**
     * Checks the phone number from the phone EditView to see if it is a valid
     * phone number.
     * If it is invalid, makes a descriptive toast and returns.
     * Else sends the phone request to the server.
     * @param v
     */
    public void phoneButtonListener(View v) {
        String responseMessage = "Please Enter a valid phone number: 12312341234";
        CharSequence number = phoneNumber.getText();


        if (!isValidPhone(number)) {
            makeToast(responseMessage);
            return;
        }

        //Do Something Else
    }

    /**
     * Checks the email address from the emailEditView to see if it is a valid email address.
     * If it is invalid, makes a descriptive toast and returns.
     * Else sends the email request to the server.
     * @param v
     */
    public void emailButtonListener(View v) {
        String responseMessage = "Please Enter a valid email address: foo@bar.com";
        CharSequence email = emailAddress.getText();

        if (!isValidEmail(email)) {
            makeToast(responseMessage);
            return;
        }

        HttpRequest newRequest = new HttpRequest("email", email.toString());
        try {
            newRequest.sendGet();
        } catch (Exception e) {
            Log.v("AddContactActivity", "error occurred");
            e.printStackTrace();
        }
    }

    /**
     * Makes and shows a Toast with the given string for Toast.LENGTH_LONG
     */
    public void makeToast(String s) {
        makeToast(s, Toast.LENGTH_LONG);
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


    /**
     * Returns true if the target is a valid e-mail address
     * @param target the string to check
     */
    public final  boolean isValidEmail(CharSequence target) {
        if (target == null) {
            return false;
        } else {
            return android.util.Patterns.EMAIL_ADDRESS.matcher(target).matches();
        }
    }

    /**
     * Returns true if the target is a valid phone number (10 digits)
     * @param target the string to check
     */
    public final boolean isValidPhone(CharSequence target) {
        if (target == null) {
            return false;
        }
        if (target.length() != 10) {
            return false;
        }

        for (int i = 0; i < target.length(); i++) {
            char c = target.charAt(i);

            if (!Character.isDigit(c))
                return false;
        }

        return true;
    }
}
