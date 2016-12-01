package edu.asu.msrs.artcelerationlibrary;

import android.graphics.Bitmap;
import android.graphics.Color;

/**
 * Created by Wenhao on 11/22/2016.
 */

public class SobelEdgeFilter {

    private Bitmap img;
    private int inputParams;
    private int width;
    private int height;

    private int[][][] pixelValue;

    int[][] Sx=new int[][]{{-1,0,1}, {-2,0,2},{-1,0,1}};
    int[][] Sy=new int[][]{{-1,-2,-1},{0,0,0},{1,2,1}};


    SobelEdgeFilter(Bitmap img, int inputParams) {
        this.img = img;
        this.inputParams = inputParams;
        this.width = img.getWidth();
        this.height = img.getHeight();
        this.pixelValue = new int[3][width][height]; //0 red, 1 green, 2 blue

        for (int x = 0 * width / 4; x < width / 2 * 2; x++) {
            for (int y = 0 * height / 4; y < height / 2 * 2; y++) {

                int colorOfPixel = img.getPixel(x, y);

                pixelValue[0][x][y] = (int) (0.2989 * Color.red(colorOfPixel));
                pixelValue[1][x][y] = (int) (0.5870 * Color.green(colorOfPixel));
                pixelValue[2][x][y] = (int) (0.1140 * Color.blue(colorOfPixel));
            }
        }
    }


    public Bitmap startTransform() {

        int width = img.getWidth();
        int height = img.getHeight();
        //int[] inputParamsRed = new int[8];

        for (int x = 0 * width / 4; x < width / 2 * 2; x++) {

            for (int y = 0 * height / 4; y < height / 2 * 2; y++) {

                int redPart = algo_SobelEdgeFilter(inputParams,0,x,y);
                int greenPart = algo_SobelEdgeFilter(inputParams,1,x,y);
                int bluePart = algo_SobelEdgeFilter(inputParams,2,x,y);
                int colorOfPixel = Color.argb(255, redPart, greenPart, bluePart);

                img.setPixel(x, y, colorOfPixel);

            }

        }
        return img;
    }

    private int algo_SobelEdgeFilter(int inputParams, int color, int positionX, int positionY) {

        int outputColor=0;

        //BEGIN OF PIXEL MATRIX INTITIALIZATION
        int[][] colorMatrix= new int[3][3];

        if(positionX-1>=0 && positionY-1>=0){
            colorMatrix[0][0]=pixelValue[color][positionX-1][positionY-1];
        }else{
            colorMatrix[0][0]=0;
        }

        if(positionX-1>=0){
            colorMatrix[1][0]=pixelValue[color][positionX-1][positionY];
        }else{
            colorMatrix[1][0]=0;
        }

        if(positionX-1>=0 && positionY+1<height){
            colorMatrix[2][0]=pixelValue[color][positionX-1][positionY+1];
        }else{
            colorMatrix[2][0]=0;
        }

        if(positionY-1>=0){
            colorMatrix[0][1]=pixelValue[color][positionX][positionY-1];
        }else{
            colorMatrix[0][1]=0;
        }

        colorMatrix[1][1]=pixelValue[color][positionX][positionY];

        if(positionY+1<height){
            colorMatrix[2][1]=pixelValue[color][positionX][positionY+1];
        }else{
            colorMatrix[2][1]=0;
        }

        if(positionX+1<width && positionY-1>=0){
            colorMatrix[0][2]=pixelValue[color][positionX+1][positionY-1];
        }else{
            colorMatrix[0][2]=0;
        }

        if(positionX+1<width){
            colorMatrix[1][2]=pixelValue[color][positionX+1][positionY];
        }else{
            colorMatrix[1][2]=0;
        }

        if(positionX+1<width && positionY+1<height){
            colorMatrix[2][2]=pixelValue[color][positionX+1][positionY+1];
        }else{
            colorMatrix[2][2]=0;
        }
        //END OF PIXEL MATRIX INITIALIZATION

        if (inputParams == 0) {

            int Grx=colorMatrix[0][0]*Sx[0][0]+colorMatrix[0][1]*Sx[0][1]+colorMatrix[0][2]*Sx[0][2]
                    +colorMatrix[1][0]*Sx[1][0]+colorMatrix[1][1]*Sx[1][1]+colorMatrix[1][2]*Sx[1][2]
                    +colorMatrix[2][0]*Sx[2][0]+colorMatrix[2][1]*Sx[2][1]+colorMatrix[2][2]*Sx[2][2];
            if(Grx<0){
                outputColor=0;
            }else{
                outputColor=Grx;
            }


        } else if (inputParams == 1) {

            int Gry=colorMatrix[0][0]*Sy[0][0]+colorMatrix[0][1]*Sy[0][1]+colorMatrix[0][2]*Sy[0][2]
                    +colorMatrix[1][0]*Sy[1][0]+colorMatrix[1][1]*Sy[1][1]+colorMatrix[1][2]*Sy[1][2]
                    +colorMatrix[2][0]*Sy[2][0]+colorMatrix[2][1]*Sy[2][1]+colorMatrix[2][2]*Sy[2][2];
            if(Gry<0){
                outputColor=0;
            }else{
                outputColor=Gry;
            }


        }else if (inputParams == 3){

            int Grx=colorMatrix[0][0]*Sx[0][0]+colorMatrix[0][1]*Sx[0][1]+colorMatrix[0][2]*Sx[0][2]
                    +colorMatrix[1][0]*Sx[1][0]+colorMatrix[1][1]*Sx[1][1]+colorMatrix[1][2]*Sx[1][2]
                    +colorMatrix[2][0]*Sx[2][0]+colorMatrix[2][1]*Sx[2][1]+colorMatrix[2][2]*Sx[2][2];

            int Gry=colorMatrix[0][0]*Sy[0][0]+colorMatrix[0][1]*Sy[0][1]+colorMatrix[0][2]*Sy[0][2]
                    +colorMatrix[1][0]*Sy[1][0]+colorMatrix[1][1]*Sy[1][1]+colorMatrix[1][2]*Sy[1][2]
                    +colorMatrix[2][0]*Sy[2][0]+colorMatrix[2][1]*Sy[2][1]+colorMatrix[2][2]*Sy[2][2];
            int Gr=(int)Math.sqrt(Grx*Grx+Gry*Gry);

            outputColor=Gr;



        } else {
            outputColor = 255;
        }

        return outputColor;
    }
}
