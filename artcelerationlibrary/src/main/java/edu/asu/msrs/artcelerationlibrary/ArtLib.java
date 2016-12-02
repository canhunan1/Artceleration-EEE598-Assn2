package edu.asu.msrs.artcelerationlibrary;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.MemoryFile;
import android.os.Message;
import android.os.Messenger;
import android.os.ParcelFileDescriptor;
import android.os.RemoteException;
import android.util.Log;

import org.apache.commons.io.IOUtils;

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
                byte[] byteArray = new byte[0];
                try {
                    byteArray = IOUtils.toByteArray(istream);
                } catch (IOException e) {
                    e.printStackTrace();
                }

                Bitmap.Config configBmp = Bitmap.Config.valueOf("ARGB_8888");
                Bitmap img = Bitmap.createBitmap(msg.arg1, msg.arg2, configBmp);
                ByteBuffer buffer = ByteBuffer.wrap(byteArray);
                img.copyPixelsFromBuffer(buffer);
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
        String[] transforms = {"Color Filter", "Motion Blur", "Gaussian Blur","Sobel edges", "Neon edges" };
        return transforms;
    }

    public TransformTest[] getTestsArray() {
        TransformTest[] transforms = new TransformTest[5];
        transforms[0] = new TransformTest(0, new int[]{0, 0, 30, 30, 100, 100, 255, 255, 0, 0, 30, 50, 60, 150, 90, 250,100, 100, 130, 130, 200, 200, 255, 255}, new float[]{0.1f, 0.2f, 0.3f});
        transforms[1] = new TransformTest(1, new int[]{0, 1}, new float[]{0.3f, 0.2f, 0.3f});
        transforms[2] = new TransformTest(2, new int[]{20}, new float[]{5f});
        transforms[3] = new TransformTest(3, new int[]{2}, new float[]{0.5f, 0.6f, 0.3f});
        transforms[4] = new TransformTest(4, new int[]{51, 42, 33}, new float[]{1.5f, 0.6f, 0.4f});

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
        if(!argumentValidation(index, intArgs, floatArgs))
            return false;
        try {
            //Write the image to the memory file
            //Firstly,convert bitmap to byte array
            //Without using compress, to speed up.
            int width = img.getWidth();
            int height = img.getHeight();
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
            Message msg = Message.obtain(null, what,width,height);
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
    private boolean argumentValidation(int index, int[] intArgs, float[] floatArgs){
        int intLength = intArgs.length;
        float floatLength = floatArgs.length;
        switch (index) {
            case TransformService.COLOR_FILTER:
                if(intLength!=24){
                    return false;
                }
                int pre = intArgs[0];
                for(int i = 0; i<intLength;i++){
                    if(intArgs[i]<0 || intArgs[i]>255)
                        return false;
                    if(i%2 == 0 && i%8 != 0) {
                        if(intArgs[i] <= pre)
                            return false;
                        else {
                            if(i+2<intLength)
                                pre = intArgs[i];
                        }
                    }
                    if(i%8 == 0)
                        pre = intArgs[i];
                }
                break;
            case TransformService.MOTION_BLUR:
                if(intLength != 2)
                    return false;
                if(intArgs[0] != 0 && intArgs[0] != 1)
                    return false;
                if(intArgs[1] <= 0)
                    return false;
                break;
            case TransformService.GAUSSIAN_BLUR:
                if(intLength != 1 || floatLength != 1)
                    return false;
                if(intArgs[0]<=0||floatArgs[0]<=0)
                    return false;
                break;
            case TransformService.SOBEL_EDGE:
                if( intLength != 1 )
                    return false;
                if(intArgs[0] != 0 && intArgs[0] != 1 && intArgs[0] != 2)
                    return false;
                break;
            case TransformService.NEON_EDGES:
                if(floatLength != 3)
                    return false;
                if(floatArgs[0]<=0)
                    return false;
                //if()
                break;
            default:
                return false;

        }
        return true;
    }
}
