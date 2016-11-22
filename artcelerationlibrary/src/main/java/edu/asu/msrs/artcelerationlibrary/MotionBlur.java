package edu.asu.msrs.artcelerationlibrary;

import android.graphics.Bitmap;
import android.graphics.Color;

/**
 * Created by Wenhao on 11/21/2016.
 */

public class MotionBlur {
    private Bitmap img;
    private int[] inputParams;
    private int width;
    private int height;

    private int[][][] pixelValue;


    MotionBlur(Bitmap img, int[] inputParams) {
        this.img = img;
        this.inputParams = inputParams;
        this.width = img.getWidth();
        this.height = img.getHeight();
        this.pixelValue = new int[3][width][height]; //0 red, 1 green, 2 blue

        for (int x = 0 * width / 4; x < width / 2 * 2; x++) {
            for (int y = 0 * height / 4; y < height / 2 * 2; y++) {

                int colorOfPixel = img.getPixel(x, y);
                pixelValue[0][x][y] = Color.red(colorOfPixel);
                pixelValue[1][x][y] = Color.green(colorOfPixel);
                pixelValue[2][x][y] = Color.blue(colorOfPixel);

            }
        }
    }


    public Bitmap startTransform() {


        for (int x = 0 * width / 4; x < width / 2 * 2; x++) {

            for (int y = 0 * height / 4; y < height / 2 * 2; y++) {

                int colorOfPixel = img.getPixel(x, y);
                int redPart = Color.red(colorOfPixel);
                int greenPart = Color.green(colorOfPixel);
                int bluePart = Color.blue(colorOfPixel);

                redPart = algo_MotionBlur(redPart, inputParams, 0, x, y);
                greenPart = algo_MotionBlur(greenPart, inputParams, 1, x, y);
                bluePart = algo_MotionBlur(bluePart, inputParams, 2, x, y);
                colorOfPixel = Color.argb(255, redPart, greenPart, bluePart);
                img.setPixel(x, y, colorOfPixel);

            }

        }

        return img;
    }

    private int algo_MotionBlur(int inputColor, int[] inputParams, int color, int positionX, int positionY) {

        if (inputParams[0] == 0) {
            int tmpColor = 0;

            for (int i = 0; i < 2*inputParams[1]+1; i++) {
                if ((positionX - inputParams[1] + i) < 0 || (positionX - inputParams[1] + i) > height) {
                    tmpColor += 0;
                } else {
                    tmpColor += pixelValue[color][positionX - inputParams[1] + i][positionY];
                }
                inputColor = tmpColor / (2*inputParams[1]+1);
            }

        } else if (inputParams[0] == 1) {

            int tmpColor = 0;

            for (int i = 0; i < 2*inputParams[1]+1; i++) {
                if ((positionY - inputParams[1] + i) < 0 || (positionY - inputParams[1] + i) > width) {
                    tmpColor += 0;
                } else {
                    tmpColor += pixelValue[color][positionX][positionY - inputParams[1] + i];
                }
                inputColor = tmpColor / (2*inputParams[1]+1);
            }

        } else {
            inputColor = 255;
        }

        return inputColor;
    }
}
