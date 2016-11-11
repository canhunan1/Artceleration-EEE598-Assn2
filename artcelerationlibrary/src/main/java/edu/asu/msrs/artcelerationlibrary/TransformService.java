package edu.asu.msrs.artcelerationlibrary;

import android.app.Service;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.MemoryFile;
import android.os.Message;
import android.os.Messenger;
import android.os.ParcelFileDescriptor;
import android.os.RemoteException;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.ArrayList;

public class TransformService extends Service {

    static {
        System.loadLibrary("my-native-lib");
    }
    static final int COLOR_FILTER = 0;
    static final int MOTION_BLUR = 1;
    static final int GAUSSIAN_BLUR = 2;
    static final int TILT_SHIFT = 3;
    static final int NEON_EDGES = 4;
    static final int TEST_TRANS = 5;
    static final String TAG = "ArtTransformService";
    static ArrayList<Messenger> mClients = new ArrayList<>();
    static Messenger replyTo;


    public TransformService() {}



    class ArtTransformHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            //trasnformHelper(msg);
            replyTo = msg.replyTo;
            new TransformService.AsyncTest().execute(getBitmap(msg));
        }

    }
    /*
    * This class is used to process the message got from the activity
    * @param msg get the message from the handler
    * */
    private void trasnformHelper(Message msg) {
        switch (msg.what) {
            case COLOR_FILTER:
                break;
            case MOTION_BLUR:
                break;
            case GAUSSIAN_BLUR:
                break;
            case TILT_SHIFT:
                break;
            case NEON_EDGES:
                break;
            case TEST_TRANS:
            default:
                break;
        }
        Bitmap mutableBitmap = getBitmap(msg);
        mutableBitmap = testTransform(mutableBitmap);
        imageProcessed(mutableBitmap);
    }
    /*
    * This method is to get the bitmap from the message
    * @param msg
    * @return       the mutable bitmap so that it can be modified
    * */
    private Bitmap getBitmap(Message msg) {
        Bundle dataBundle = msg.getData();
        ParcelFileDescriptor pfd = (ParcelFileDescriptor) dataBundle.get("pfd");
        InputStream istream = new ParcelFileDescriptor.AutoCloseInputStream(pfd);
        //convertInputStreamToBitmap
        Bitmap img = BitmapFactory.decodeStream(istream);
        return img.copy(Bitmap.Config.ARGB_8888, true);
    }
    /*
    * This method is used to send the processed image back to the activity
    * @param msg
    * @param img    the image which has been processed
    * */
    private void imageProcessed(Bitmap img){
        Message msg = new Message();
        msg.replyTo = replyTo;
        int what = 0;
        //Message msg = Message.obtain(null, what);
        Bundle dataBundle = new Bundle();
        mClients.add(msg.replyTo);
        if (msg.replyTo == null) {
            Log.d("mclient is ", "null");
        }
        try {
            int bytes = img.getByteCount();
            ByteBuffer buffer = ByteBuffer.allocate(bytes); //Create a new buffer
            img.copyPixelsToBuffer(buffer); //Move the byte data to the buffer
            byte[] byteArray = buffer.array();

            /*ByteArrayOutputStream stream = new ByteArrayOutputStream();
            img.compress(Bitmap.CompressFormat.PNG, 100, stream);
            byte[] byteArray = stream.toByteArray();*/
            //Secondly, put the stream into the memory file.
            MemoryFile memoryFile = new MemoryFile("someone", byteArray.length);
            memoryFile.writeBytes(byteArray, 0, 0, byteArray.length);
            ParcelFileDescriptor pfd = MemoryFileUtil.getParcelFileDescriptor(memoryFile);
            memoryFile.close();
            dataBundle.putParcelable("pfd", pfd);
            msg.setData(dataBundle);
            //msg.obtain(null,6, 2, 3);
            mClients.get(0).send(msg);
        } catch (RemoteException | IOException e) {
            e.printStackTrace();
        }
    }


    /*
    * This method is the test transform which change part of the image to be yellow
    * @param img    img should be mutable bitmap
    * @return img   processed image
    * */
    private Bitmap testTransform(Bitmap img) {
        int width = img.getWidth();
        int height = img.getHeight();
        for(int x = width/4; x < width/4*3; x++)
        {
            for(int y = height/4; y < height/4*3; y++)
            {
                img.setPixel(x, y, Color.YELLOW);
            }
        }
        return img;
    }





    final Messenger mMessenger = new Messenger(new ArtTransformHandler());

    @Override
    public IBinder onBind(Intent intent) {
        return mMessenger.getBinder();
    }

    public native static String myStringFromJNI();
    // public native static int JNI_OnLoad();

    class AsyncTest extends AsyncTask<Bitmap, Float, Bitmap> {
        //DONE IN BACKGROUND

        @Override
        protected Bitmap doInBackground(Bitmap...img) {
            return testTransform(img[0]);
        }

        //ON UI THREAD
        protected void onPostExecute(Bitmap mutableBitmap) {

            imageProcessed(mutableBitmap);
            Log.d("AsyncTest", "AsyncTest finished" );
        }
    }

}
