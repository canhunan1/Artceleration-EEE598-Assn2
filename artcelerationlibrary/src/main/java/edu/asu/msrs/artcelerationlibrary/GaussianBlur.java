package edu.asu.msrs.artcelerationlibrary;

import android.graphics.Bitmap;
import android.graphics.Color;

/**
 * Created by Wenhao on 11/24/2016.
 * This is the java class for Gaussian Blur image transform.
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

    /**
     * This is the constructor of GassianBlur.class.
     * This function is used to instantiate objects, values and arrays for later usage.
     *
     * @param img        is the bitmap image to be transformed
     * @param inputInt   is the radius value 'r' for mathematical calculation
     * @param inputFloat is the sigma value for mathematical calculation
     */
    GaussianBlur(Bitmap img, int[] inputInt, float[] inputFloat) {
        this.img = img;
        this.inputInt = inputInt;
        this.inputFloat = inputFloat;

        this.width = img.getWidth();
        this.height = img.getHeight();
        this.pixelValue = new int[3][width][height]; //initiate an empty 3D array to store the pixel value of all three color
        this.pixelValue_tmp = new float[3][width][height]; //initiate an empty 3D array to temporarily store the pixel value of all three color
        this.weightVector = new float[2 * inputInt[0] + 1];// initiate an empty 1D array for later use to store the Gaussian weights

        //red out all pixel values and save them to the 3D array initiated above
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {

                int colorOfPixel = img.getPixel(x, y);
                pixelValue[0][x][y] = Color.red(colorOfPixel); // 0 is red
                pixelValue[1][x][y] = Color.green(colorOfPixel); // 1 is green
                pixelValue[2][x][y] = Color.blue(colorOfPixel); // 2 is blue

            }
        }
    }

    /**
     * This function is used to initiate the Gaussian weights vector
     *
     * @param r     is the Gaussian radius
     * @param sigma is the Gaussian sigma which determines the importance of the values around target pixel
     */
    private void weightInit(int r, float sigma) {

        for (int i = 0; i < 2 * r + 1; i++) {

            float part_1 = (float) (1 / Math.sqrt(2 * 3.14 * sigma * sigma));
            float expInput = -(-r + i) * (-r + i) / (2 * sigma * sigma);

            this.weightVector[i] = (float) (part_1 * Math.exp(expInput));
        }
    }

    /**
     * This function is used to start the Gaussian Blur transform, it takes no inputs and returns the transformed bitmap.
     */
    public Bitmap startTransform() {
        //initiate Gaussian weight vector
        weightInit(this.inputInt[0], this.inputFloat[0]);

        //do the first half of Gaussian transform, that is for all pixels aligned in 'X-axis'
        for (int x = 0; x < width; x++) {

            for (int y = 0; y < height; y++) {
                pixelValue_tmp[0][x][y] = algo_GaussianBlue_first(this.weightVector, 0, x, y);
                pixelValue_tmp[1][x][y] = algo_GaussianBlue_first(this.weightVector, 1, x, y);
                pixelValue_tmp[2][x][y] = algo_GaussianBlue_first(this.weightVector, 2, x, y);
            }
        }

        //do the second half of Gaussian transform, that is for all pixels aligned in 'Y-axis'
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

    /**
     * This function is used to perform the first half of Gaussian Blur transform, it returns the calculated int value q.
     *
     * @param weightVector, is the Gassian weight vector
     * @param color         is the color to be transformed with red:0, green:1, blue:2
     * @param positionX,    is the pixel's X coordinate
     * @param positionY,    is the pixel's Y coordinate
     */
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

    /**
     * This function is used to perform the second half of Gaussian Blur transform, it returns the calculated int value p.
     *
     * @param weightVector, is the Gassian weight vector
     * @param color         is the color to be transformed with red:0, green:1, blue:2
     * @param positionX,    is the pixel's X coordinate
     * @param positionY,    is the pixel's Y coordinate
     */
    private int algo_GaussianBlue_second(float[] weightVector, int color, int positionX, int positionY) {
        int weightLength = weightVector.length;

        float p = 0;

        for (int i = 0; i < weightLength; i++) {

            if (positionY - this.inputInt[0] + i >= 0 && positionY - this.inputInt[0] + i < this.height) {

                p += weightVector[i] * pixelValue_tmp[color][positionX][positionY - this.inputInt[0] + i];

            } else {
                p += 0;
            }
        }

        return (int) p;

    }


}
