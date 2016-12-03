/**
 * Created by Jianan on 11/22/2016.
 * This class is to do native transform including color filter and motion blur transform.
 */

package edu.asu.msrs.artcelerationlibrary;

import android.graphics.Bitmap;
import android.util.Log;



public class NativeTransform {
    static {
        System.loadLibrary("native-lib");
    }
    public native static String myStringFromJNI();
    public native static String stringFromJNI();
    private native void jniColorFilter(Bitmap img,int[] args, int size);
    private native boolean neonMotionBlur(Bitmap img,int[] intArgus);

    public NativeTransform(){}


    /*
    * This function is used to do call the native MotionBlur function
    * @param img  img is a bitmap to be transformed
    * @param r    r is the radius of the motion transform
    * */
    public void motionBlur(Bitmap img, int[] intArgus)
    {
        Log.d("native motion blur","dd");
        neonMotionBlur( img,intArgus);
    }

    /*
    * This function is used to do call the native ColorFIlter function
    * @param img    img is a bitmap to be transformed
    * @param args   args is int array with input value of the color filter
    * @return it returns a boolean value after transformation
    * */
    public boolean colorFilter(Bitmap img, int[] args)
    {
        Log.d("native color filter","dd");
        jniColorFilter(img, args, args.length);
        return true;
    }
}
