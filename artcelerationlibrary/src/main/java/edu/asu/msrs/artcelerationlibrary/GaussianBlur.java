package edu.asu.msrs.artcelerationlibrary;

import android.graphics.Bitmap;
import android.graphics.Color;

/**
 * Created by Wenhao on 11/24/2016.
 */

public class GaussianBlur {
    private Bitmap img;
    private int[] inputInt;
    private float[] inputFloat;
    private float[] weightVector;
    private int width;
    private int height;

    private int[][][] pixelValue;
    private float[][][] pixelValue_tmp;

    GaussianBlur(Bitmap img, int[] inputInt, float[] inputFloat) {
        this.img = img;
        this.inputInt = inputInt;
        this.inputFloat = inputFloat;

        this.width = img.getWidth();
        this.height = img.getHeight();
        this.pixelValue = new int[3][width][height]; //0 red, 1 green, 2 blue
        this.pixelValue_tmp = new float[3][width][height]; //0 red, 1 green, 2 blue
        this.weightVector=new float[2*inputInt[0]+1];// give a certain size for this array

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {

                int colorOfPixel = img.getPixel(x, y);
                pixelValue[0][x][y] = Color.red(colorOfPixel);
                pixelValue[1][x][y] = Color.green(colorOfPixel);
                pixelValue[2][x][y] = Color.blue(colorOfPixel);

            }
        }
    }

    private void weightInit(int r, float sigma) {

        for (int i = 0; i < 2 * r + 1; i++) {

            float part_1= (float) (1 / Math.sqrt(2 * 3.14 * sigma * sigma));
            float expInput = -(-r + i) * (-r + i) / (2 * sigma * sigma);

            this.weightVector[i] = (float)  (part_1* Math.exp(expInput));
            //this.weightVector[i]=1.0f;

        }
    }

    public Bitmap startTransform() {

        weightInit(this.inputInt[0], this.inputFloat[0]);

        for (int x = 0; x < width; x++) {

            for (int y = 0; y < height; y++) {

                pixelValue_tmp[0][x][y] = algo_GaussianBlue_first(this.weightVector, 0, x, y);
                pixelValue_tmp[1][x][y] = algo_GaussianBlue_first(this.weightVector, 1, x, y);
                pixelValue_tmp[2][x][y] = algo_GaussianBlue_first(this.weightVector, 2, x, y);

            }
        }

        for (int x = 0; x < width; x++) {

            for (int y = 0; y < height; y++) {

                int redPart = algo_GaussianBlue_second(this.weightVector, 0, x, y);
                int greenPart = algo_GaussianBlue_second(this.weightVector, 1, x, y);
                int bluePart = algo_GaussianBlue_second(this.weightVector, 2, x, y);

                int colorOfPixel = Color.argb(255, redPart, greenPart, bluePart);
                img.setPixel(x, y, colorOfPixel);

            }
        }

        return img;
    }

    private float algo_GaussianBlue_first(float[] weightVector, int color, int positionX, int positionY) {
        int weightLength = weightVector.length;

        float q = 0;

        for (int i = 0; i < weightLength; i++) {

            if (positionX - this.inputInt[0] + i >= 0 && positionX - this.inputInt[0] + i < this.width) {

                q += weightVector[i] * pixelValue[color][positionX - this.inputInt[0] + i][positionY];

            } else {
                q += 0;
            }
        }

        return q;
    }

    private int algo_GaussianBlue_second(float[] weightVector, int color, int positionX, int positionY) {
        int weightLength = weightVector.length;

        float p = 0;

        for (int i = 0; i < weightLength; i++) {

            if (positionY - this.inputInt[0] + i >= 0 && positionY - this.inputInt[0] + i < this.height) {

                p += weightVector[i] * pixelValue_tmp[color][positionX][positionY- this.inputInt[0] + i];

            } else {
                p += 0;
            }
        }

        return (int) p;

    }


}
