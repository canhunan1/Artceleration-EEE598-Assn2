package edu.asu.msrs.artcelerationlibrary;

import android.graphics.Bitmap;
import android.util.Log;

import java.nio.ByteBuffer;

/**
 * Created by Jianan on 11/22/2016.
 */

public class NativeTransform {
    static {
        System.loadLibrary("native-lib");
    }
    public void nativeTest(){
        Log.d("Success",stringFromJNI());
    }

    //Log.d(TAG,myStringFromJNI());
    public native static String myStringFromJNI();
    public native static String stringFromJNI();

    ByteBuffer _handler =null;

    private native ByteBuffer jniStoreBitmapData(Bitmap bitmap);

    private native Bitmap jniGetBitmapFromStoredBitmapData(ByteBuffer handler);

    private native void jniFreeBitmapData(ByteBuffer handler);

    private native void jniRotateBitmapCcw90(ByteBuffer handler);

    private native void jniCropBitmap(ByteBuffer handler,final int left,final int top,final int right,final int bottom);

    //private native void jniColorFilter(ByteBuffer handler,int[] args, int size);
    private native void jniColorFilter(Bitmap img,int[] args, int size);

    private native boolean brightness(ByteBuffer handler,float brightness);
    private native boolean neonBrightness(Bitmap img,float brightness);
    private native boolean neonMotionBlur(Bitmap img,float brightness);
   // static private native void neonEdgeSum(Bitmap bitmapOri, Bitmap bitmapProcessed, float  f1, float f2) ;



        public NativeTransform(){

    }

    public NativeTransform(final Bitmap bitmap)
    {
        storeBitmap(bitmap);
    }
    public void storeBitmap(final Bitmap bitmap)
    {
        if(_handler!=null)
            freeBitmap();
        _handler=jniStoreBitmapData(bitmap);
    }

    public void rotateBitmapCcw90()
    {
        if(_handler==null)
            return;
        jniRotateBitmapCcw90(_handler);
    }

    public void brightness(float brightness)
    {
        if(_handler==null)
            return;
        brightness(_handler, brightness);

    }
    public void bright(Bitmap img, float brightness)
    {
        Log.d("native bright","dd");

        neonBrightness(img, brightness);
    }

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

       /* if(!colorFilterValidateArgs(args)){
            return false;
        }*/
       // nativeTest();
        Log.d("native color filter","dd");
        jniColorFilter(img, args, args.length);
        return true;
    }
    /*private boolean colorFilterValidateArgs(int args[]){
        if(args == null || args.length!=24 )
            return false;
        for(int i = 0; i<args.length; i++){
            if(Math.floor(i/4)%2==0&&(i+1)%4!=0){
                if(args[i]>args[i+1])
                    return false;
            }
            if( args[i]<0 )
                return false;
        }
        return true;
    }*/


    public void cropBitmap(final int left,final int top,final int right,final int bottom)
    {
        if(_handler==null)
            return;
        jniCropBitmap(_handler,left,top,right,bottom);
    }

    public Bitmap getBitmap()
    {
        if(_handler==null)
            return null;
        return jniGetBitmapFromStoredBitmapData(_handler);
    }

    public Bitmap getBitmapAndFree()
    {
        final Bitmap bitmap=getBitmap();
        freeBitmap();
        return bitmap;
    }

    public void freeBitmap()
    {
        if(_handler==null)
            return;
        jniFreeBitmapData(_handler);
        _handler=null;
    }

    @Override
    protected void finalize() throws Throwable
    {
        super.finalize();
        if(_handler==null)
            return;
        Log.w("DEBUG","JNI bitmap wasn't freed nicely.please rememeber to free the bitmap as soon as you can");
        freeBitmap();
    }
}
