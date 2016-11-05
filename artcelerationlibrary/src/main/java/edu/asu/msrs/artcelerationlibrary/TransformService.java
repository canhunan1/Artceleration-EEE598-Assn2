package edu.asu.msrs.artcelerationlibrary;

import android.app.Service;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.ParcelFileDescriptor;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;

public class TransformService extends Service {

    public void onCreate() {
        Log.v(TAG, "test");
    }

    public TransformService() {
    }

    static String TAG = "ArtTransformService";
    static final int MSG_HELLO = 0;
    static final int MSG_MULTI = 1;

    static class ArtTransformHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Log.d(TAG, "handleMessage(msg)");
            switch (msg.what) {
                case MSG_HELLO:
                    Log.d(TAG, "Hello!");
                    break;
                case MSG_MULTI:
                    Bundle dataBundle = msg.getData();
                    ParcelFileDescriptor pfd = (ParcelFileDescriptor) dataBundle.get("pfd");
                    InputStream istream = new ParcelFileDescriptor.AutoCloseInputStream(pfd);
                    //convertInputStreamToBitmap
                    Bitmap img = BitmapFactory.decodeStream(istream);
                    ByteArrayOutputStream stream = new ByteArrayOutputStream();

                    img.compress(Bitmap.CompressFormat.PNG, 100, stream);
                    byte[] byteArray = stream.toByteArray();
                    //Log.v(TAG, byteArray.toString());
                    int result = msg.arg1 + msg.arg2;
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
