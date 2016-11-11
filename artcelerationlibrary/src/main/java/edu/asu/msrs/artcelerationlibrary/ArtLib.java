package edu.asu.msrs.artcelerationlibrary;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
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

/**
 * Created by rlikamwa on 10/2/2016.
 * This class is art library class.
 */

public class ArtLib {
    static private TransformHandler artlistener;
    private Activity mActivity;
    private Messenger mMessenger;
    private boolean mBound;

    public ArtLib(Activity activity) {
        mActivity = activity;
        init();
    }
    //This handler is to handle the message from the service.
    static private class ImageProcessedHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {//Called when get the message from the service. Usually mean that a transform is finised.
            Bundle dataBundle = msg.getData();
            ParcelFileDescriptor pfd = (ParcelFileDescriptor) dataBundle.get("pfd");
            if(pfd == null){
                Log.d("pfd","null");
            }else {
                Log.d("image ","has been sent back to the client");
                InputStream istream = new ParcelFileDescriptor.AutoCloseInputStream(pfd);
                //convertInputStreamToBitmap
                Bitmap img = BitmapFactory.decodeStream(istream);
                if (artlistener != null) {//triger the listener to send back the processed image to the activity
                    artlistener.onTransformProcessed(img);
                }
            }
        }
    }

    final Messenger inMessenger = new Messenger(new ImageProcessedHandler());

    ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mMessenger = new Messenger(service);
            mBound = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            mMessenger = null;
            mBound = false;
        }
    };

    public void init() {
        mActivity.bindService(new Intent(mActivity, TransformService.class), mServiceConnection, Context.BIND_AUTO_CREATE);
    }

    public String[] getTransformsArray() {
        //String[] transforms = {"Color Filter", "Motion Blur","Gaussian Blur", "Tilt Shift", "Neon Edges", "Test Transform"};
        String[] transforms = {"Gaussian Blur", "Neon edges", "Color Filter"};
        return transforms;
    }

    public TransformTest[] getTestsArray() {
        TransformTest[] transforms = new TransformTest[3];
        transforms[0] = new TransformTest(0, new int[]{1, 2, 3}, new float[]{0.1f, 0.2f, 0.3f});
        transforms[1] = new TransformTest(1, new int[]{11, 22, 33}, new float[]{0.3f, 0.2f, 0.3f});
        transforms[2] = new TransformTest(2, new int[]{51, 42, 33}, new float[]{0.5f, 0.6f, 0.3f});

        return transforms;
    }

    public void registerHandler(TransformHandler artlistener) {
        this.artlistener = artlistener;
    }

    /*
    * This method is used by the activity to request a transform.
    * @param img        the image to be processed
    * @param index      the type of the transform
    * @param intArgs    the integer arguments
    * @param floatArgs  the float arguments
    * @return           true if the transform is started successfully, false otherwise
    * */
    public boolean requestTransform(Bitmap img, int index, int[] intArgs, float[] floatArgs) {

        try {
            //Write the image to the memory file
            //Firstly,convert bitmap to byte array
            int  bytes = img.getByteCount();
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
            Bundle dataBundle = new Bundle();
            //put the image in the bundle, sharing the memory with ashmen
            dataBundle.putParcelable("pfd", pfd);
            dataBundle.putIntArray("intArgs", intArgs);
            dataBundle.putFloatArray("floatArgs", floatArgs);
            //index means the type of the transform.
            int what = index;
            Message msg = Message.obtain(null, what);
            msg.setData(dataBundle);
            msg.replyTo = inMessenger;
            try {
                mMessenger.send(msg);
            } catch (RemoteException e) {
                e.printStackTrace();
                return false;
            }
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }
}
