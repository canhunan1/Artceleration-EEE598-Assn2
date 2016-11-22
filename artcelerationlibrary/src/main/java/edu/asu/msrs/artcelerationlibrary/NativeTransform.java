package edu.asu.msrs.artcelerationlibrary;

import android.util.Log;

/**
 * Created by Jianan on 11/22/2016.
 */

public class NativeTransform {
    static {
        System.loadLibrary("native-lib");
    }
    public void nativeTest(){
        Log.d("Success",myStringFromJNI());
    }

    //Log.d(TAG,myStringFromJNI());
    public native static String myStringFromJNI();
}
