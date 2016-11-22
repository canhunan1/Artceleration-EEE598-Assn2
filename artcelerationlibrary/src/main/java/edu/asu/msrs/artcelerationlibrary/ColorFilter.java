package edu.asu.msrs.artcelerationlibrary;

import android.graphics.Bitmap;
import android.graphics.Color;

/**
 * Created by Wenhao on 11/21/2016.
 */

public class ColorFilter {
    private Bitmap img;
    private int[] inputParams;


    ColorFilter(Bitmap img, int[] inputParams){
        this.img=img;
        this.inputParams=inputParams;
    }


    public Bitmap startTransform() {

        int width = img.getWidth();
        int height = img.getHeight();
        int[] inputParamsRed=new int[8];
        for(int i=0;i<8;i++){inputParamsRed[i]=inputParams[i];}
        int[] inputParamsGreen=new int[8];
        for(int i=0;i<8;i++){inputParamsGreen[i]=inputParams[i+8];}
        int[] inputParamsBlue=new int[8];
        for(int i=0;i<8;i++){inputParamsBlue[i]=inputParams[i+16];}


        for (int x = 0 * width / 4; x < width / 2 * 2; x++) {

            for (int y = 0 * height / 4; y < height / 2 * 2; y++) {

                int colorOfPixel = img.getPixel(x, y);
                int redPart = Color.red(colorOfPixel);
                redPart = algo_ColorFilter(redPart, inputParamsRed);
                int greenPart = Color.green(colorOfPixel);
                greenPart = algo_ColorFilter(greenPart, inputParamsGreen);
                int bluePart = Color.blue(colorOfPixel);
                bluePart = algo_ColorFilter(bluePart, inputParamsBlue);
                colorOfPixel = Color.argb(255, redPart, greenPart, bluePart);
                img.setPixel(x, y, colorOfPixel);

            }

        }
        return img;
    }


    private int algo_ColorFilter(int inputColor, int[] inputParams) {
        if (inputParams[0] != 0 && inputParams[6] != 255) {
            if (inputColor < inputParams[0]) {

                inputColor = inputParams[1] / inputParams[0] * inputColor;

            } else if (inputColor >= inputParams[0] && inputColor < inputParams[2]) {

                inputColor = (inputParams[3] - inputParams[1]) / (inputParams[2] - inputParams[0]) * (inputColor - inputParams[0]) + inputParams[1];

            } else if (inputColor >= inputParams[2] && inputColor < inputParams[4]) {

                inputColor = (inputParams[5] - inputParams[3]) / (inputParams[4] - inputParams[2]) * (inputColor - inputParams[2]) + inputParams[3];

            } else if (inputColor >= inputParams[4] && inputColor < inputParams[6]) {

                inputColor = (inputParams[7] - inputParams[5]) / (inputParams[6] - inputParams[4]) * (inputColor - inputParams[4]) + inputParams[5];

            } else {

                inputColor = (255 - inputParams[7]) / (255 - inputParams[6]) * (inputColor - inputParams[6]) + inputParams[7];
            }

        } else if (inputParams[0] == 0 && inputParams[6] != 255) {

            if (inputColor >= inputParams[0] && inputColor < inputParams[2]) {

                inputColor = (inputParams[3] - inputParams[1]) / (inputParams[2] - inputParams[0]) * (inputColor - inputParams[0]) + inputParams[1];

            } else if (inputColor >= inputParams[2] && inputColor < inputParams[4]) {

                inputColor = (inputParams[5] - inputParams[3]) / (inputParams[4] - inputParams[2]) * (inputColor - inputParams[2]) + inputParams[3];

            } else if (inputColor >= inputParams[4] && inputColor < inputParams[6]) {

                inputColor = (inputParams[7] - inputParams[5]) / (inputParams[6] - inputParams[4]) * (inputColor - inputParams[4]) + inputParams[5];

            } else {

                inputColor = (255 - inputParams[7]) / (255 - inputParams[6]) * (inputColor - inputParams[6]) + inputParams[7];
            }

        } else if (inputParams[0] != 0 ) {

            if (inputColor < inputParams[0]) {

                inputColor = inputParams[1] / inputParams[0] * inputColor;

            } else if (inputColor >= inputParams[0] && inputColor < inputParams[2]) {

                inputColor = (inputParams[3] - inputParams[1]) / (inputParams[2] - inputParams[0]) * (inputColor - inputParams[0]) + inputParams[1];

            } else if (inputColor >= inputParams[2] && inputColor < inputParams[4]) {

                inputColor = (inputParams[5] - inputParams[3]) / (inputParams[4] - inputParams[2]) * (inputColor - inputParams[2]) + inputParams[3];

            } else if (inputColor >= inputParams[4] && inputColor <= 255) {

                inputColor = (inputParams[7] - inputParams[5]) / (255 - inputParams[4]) * (inputColor - inputParams[4]) + inputParams[5];

            }

        } else {

            if (inputColor >= inputParams[0] && inputColor < inputParams[2]) {

                inputColor = (inputParams[3] - inputParams[1]) / (inputParams[2] - inputParams[0]) * (inputColor - inputParams[0]) + inputParams[1];

            } else if (inputColor >= inputParams[2] && inputColor < inputParams[4]) {

                inputColor = (inputParams[5] - inputParams[3]) / (inputParams[4] - inputParams[2]) * (inputColor - inputParams[2]) + inputParams[3];

            } else if (inputColor >= inputParams[4] && inputColor < 255) {

                inputColor = (inputParams[7] - inputParams[5]) / (255 - inputParams[4]) * (inputColor - inputParams[4]) + inputParams[5];

            }

        }

        return inputColor;
    }
}
