/**
 * Created by Jianan on 11/30/2016.
 * This file is created to do neon edges transform.
 * Firstly do the sobel edge transform, secondly do the gaussian transform
 * Thirdly do the linear sum.
 */

package edu.asu.msrs.artcelerationlibrary;

import android.graphics.Bitmap;
import android.graphics.Color;

public class NeonEdge {
    NeonEdge() {
    }

    /*
    * This function is used to do NeonEdgeTransform
    * @param img    img is a bitmap to be transformed
    * @param argsF   argsF is float array with input value for NeonEdgeTransform
    * */
    public static Bitmap NeonEdgeTransForm(Bitmap img, float argsF[]) {

        Bitmap original = Bitmap.createBitmap(img);
        SobelEdgeFilter sobelEdgeFilter = new SobelEdgeFilter(img, 2);
        img = sobelEdgeFilter.startTransform();
        int[] r = {(int) (argsF[0] * 6)};
        float[] f = {argsF[0]};
        GaussianBlur gaussianBlur = new GaussianBlur(img, r, f);
        img = gaussianBlur.startTransform();
        //NativeTransform.neonNeonEdges(original, img, argsF[1], argsF[2]);
        neonEdgeLinearSum(img, argsF, original);
        return img;
    }


    /*
    * This function is used to do LinearSum which is the third step of the transform
    * @param img        img is a bitmap after processed the first and second step
    * @param argsF      argsF is float array with input value for NeonEdgeTransform
    * @param original   original is a bitmap to be transformed
    * */
    private static void neonEdgeLinearSum(Bitmap img, float[] argsF, Bitmap original) {
        for (int i = 0; i < img.getWidth(); i++) {
            for (int j = 0; j < img.getHeight(); j++) {
                int pixel = img.getPixel(i, j);
                int red = Color.red(pixel);
                int green = Color.green(pixel);
                int blue = Color.blue(pixel);
                int originalPixel = original.getPixel(i, j);
                int redOri = Color.red(originalPixel);
                int greenOri = Color.green(originalPixel);
                int blueOri = Color.blue(originalPixel);
                int Pred = (int) (red * argsF[1] + redOri * argsF[2]);
                int PGreen = (int) (green * argsF[1] + greenOri * argsF[2]);
                int PBlue = (int) (blue * argsF[1] + blueOri * argsF[2]);
                int color = Color.argb(255, Pred, PGreen, PBlue);
                img.setPixel(i, j, color);
            }
        }
    }

}
