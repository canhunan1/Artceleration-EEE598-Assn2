package edu.asu.msrs.artcelerationlibrary;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.util.Log;

public class TestService extends Service {
    public TestService() {
    }

    String TAG = "ArtTransformService";
    static final int MSG_HELLO = 0;
    static final int MSG_MULTI = 1;
    class ArtTransformHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Log.d(TAG, "handleMessage(msg)");
            switch(msg.what){
                case MSG_HELLO:
                    Log.d(TAG, "Hello!");
                        break;
                case MSG_MULTI:
                    int result = msg.arg1+msg.arg2;
                    Log.d(TAG, "Multi" + result);
                    break;
                default:
                    break;
            }
        }
    }

    final Messenger mMessenger = new Messenger(new ArtTransformHandler());

    @Override
    public IBinder onBind(Intent intent) {
        return mMessenger.getBinder();
    }
}
