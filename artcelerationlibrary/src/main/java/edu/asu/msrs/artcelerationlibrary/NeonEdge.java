package edu.asu.msrs.artcelerationlibrary;

import android.graphics.Bitmap;
import android.graphics.Color;

/**
 * Created by Jianan on 11/30/2016.
 */

public class NeonEdge {
    NeonEdge(){

    }

    public static Bitmap NeonEdgeTransForm(Bitmap img, float argsF[]){

        Bitmap original = Bitmap.createBitmap(img);
        SobelEdgeFilter sobelEdgeFilter=new SobelEdgeFilter(img,2);
        img =  sobelEdgeFilter.startTransform();
        int[] r = {(int)(argsF[0]*6)};
        float[] f = {argsF[0]};
        GaussianBlur gaussianBlur=new GaussianBlur(img, r,f);
        img = gaussianBlur.startTransform();

        for(int i = 0;i < img.getWidth();i++){
            for(int j = 0; j < img.getHeight();j++){
                int pixel = img.getPixel(i,j);
                int red = Color.red(pixel);
                int green = Color.green(pixel);
                int blue = Color.blue(pixel);
                int originalPixel = original.getPixel(i,j);
                int redOri = Color.red(originalPixel);
                int greenOri = Color.green(originalPixel);
                int blueOri = Color.blue(originalPixel);
                int Pred = (int)(red*argsF[1] + redOri*argsF[2]);
                int PGreen = (int)(green*argsF[1] + greenOri*argsF[2]);
                int PBlue = (int)(blue*argsF[1] + blueOri*argsF[2]);
                int color = Color.argb(255, Pred, PGreen,PBlue);
                img.setPixel(i,j,color);

            }
        }
        return img;
    }

}
