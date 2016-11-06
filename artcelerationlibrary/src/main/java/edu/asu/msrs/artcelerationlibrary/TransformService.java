package edu.asu.msrs.artcelerationlibrary;

import android.app.Service;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.MemoryFile;
import android.os.Message;
import android.os.Messenger;
import android.os.ParcelFileDescriptor;
import android.os.RemoteException;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

public class TransformService extends Service {

    static {
        System.loadLibrary("my-native-lib");
    }

    static ArrayList<Messenger> mClients = new ArrayList<>();
    TransformHandler transformHandler;

    public void onCreate() {
        Log.v(TAG, "test");
    }

    public TransformService() {
        //Log.v("nativeInTransform",myStringFromJNI());

        //  Log.v("nativeInTransform",String.valueOf(JNI_OnLoad()));
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
                    Log.v("nativeInTransform", myStringFromJNI());
                    Bundle dataBundle = msg.getData();
                    ParcelFileDescriptor pfd = (ParcelFileDescriptor) dataBundle.get("pfd");
                    InputStream istream = new ParcelFileDescriptor.AutoCloseInputStream(pfd);
                    //convertInputStreamToBitmap
                    Bitmap img = BitmapFactory.decodeStream(istream);
                    Bitmap mutableBitmap = img.copy(Bitmap.Config.ARGB_8888, true);
                    int width = mutableBitmap.getWidth();
                    int height = mutableBitmap.getHeight();
                    for(int x = width/4; x < width/4*3; x++)
                    {
                        for(int y = height/4; y < height/4*3; y++)
                        {
                            mutableBitmap.setPixel(x, y, Color.YELLOW);

                        }
                    }
                    /*int[] pixels = new int[3];
                    mutableBitmap.getPixels(pixels,0,0,10,10,10,10);
                    mutableBitmap.eraseColor(255);*/
                    //do some transform here

                    //When transfrom finished, send the image back

                    mClients.add(msg.replyTo);
                    if (msg.replyTo == null) {
                        Log.d("mclient is ", "null");
                    }
                    try {
                        ByteArrayOutputStream stream = new ByteArrayOutputStream();
                        mutableBitmap.compress(Bitmap.CompressFormat.PNG, 100, stream);
                        byte[] byteArray = stream.toByteArray();
                        //Secondly, put the stream into the memory file.
                        MemoryFile memoryFile = new MemoryFile("someone", byteArray.length);
                        memoryFile.writeBytes(byteArray, 0, 0, byteArray.length);
                        pfd = MemoryFileUtil.getParcelFileDescriptor(memoryFile);

                        memoryFile.close();
                        dataBundle.putParcelable("pfd", pfd);
                        msg.setData(dataBundle);
                        msg.obtain(null,6, 2, 3);
                        mClients.get(0).send(msg);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    /*//for test
                    ByteArrayOutputStream stream = new ByteArrayOutputStream();
                    img.compress(Bitmap.CompressFormat.PNG, 100, stream);
                    byte[] byteArray = stream.toByteArray();
                    Log.v(TAG, byteArray.toString());*/

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

    public native static String myStringFromJNI();
    // public native static int JNI_OnLoad();

}
