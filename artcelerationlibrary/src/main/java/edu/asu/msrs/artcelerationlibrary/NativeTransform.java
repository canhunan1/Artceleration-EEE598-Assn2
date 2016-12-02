package edu.asu.msrs.artcelerationlibrary;

import android.graphics.Bitmap;
import android.util.Log;

/**
 * Created by Jianan on 11/22/2016.
 */

public class NativeTransform {
    static {
        System.loadLibrary("native-lib");
    }
    public native static String myStringFromJNI();
    public native static String stringFromJNI();
    private native void jniColorFilter(Bitmap img,int[] args, int size);
    private native boolean neonMotionBlur(Bitmap img,float brightness);

    public NativeTransform(){}

    static public void NeonEdgeTransform(Bitmap bitmapOri, Bitmap bitmapProcessed, float  f1, float f2){
       // neonEdgeSum(bitmapOri, bitmapProcessed,   f1,  f2);
    }

    public void motionBlur(Bitmap img, float r)
    {
        Log.d("native motion blur","dd");
        neonMotionBlur( img,r);
    }

    public boolean colorFilter(Bitmap img, int[] args)
    {
        Log.d("native color filter","dd");
        jniColorFilter(img, args, args.length);
        return true;
    }
}
