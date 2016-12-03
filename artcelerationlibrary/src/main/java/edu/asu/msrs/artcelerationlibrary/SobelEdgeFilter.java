package edu.asu.msrs.artcelerationlibrary;

import android.graphics.Bitmap;
import android.graphics.Color;

/**
 * Created by Wenhao on 11/22/2016.
 * This is the java class for SobelEdge image transform
 */

public class SobelEdgeFilter {

    private Bitmap img;
    private int inputParams;
    private int width;
    private int height;
    private int [][] qArray;

    //initialize the two SobelEdge filter
    int[][] Sx=new int[][]{{-1,0,1}, {-2,0,2},{-1,0,1}};
    int[][] Sy=new int[][]{{-1,-2,-1},{0,0,0},{1,2,1}};

    /**
     * This is the constructor of SobelEdgeFilter.class.
     * @param img        is the bitmap image to be transformed
     * @param inputParams is the input arguments for using specific type of SobelEdgeFilter
     */
    SobelEdgeFilter(Bitmap img, int inputParams) {
        this.img = img;
        this.inputParams = inputParams;
        this.width = img.getWidth();
        this.height = img.getHeight();
        this.qArray = new int[width][height];

        //Gray scale every pixels of the input bitmap image
        for (int x = 0 ; x < width ; x++) {
            for (int y = 0 ; y < height ; y++) {
                int colorOfPixel = img.getPixel(x, y);
                int q = (int) (0.2989 * (float)Color.red(colorOfPixel))+ (int) (0.5870 * (float)Color.green(colorOfPixel))+(int) (0.1140 * (float)Color.blue(colorOfPixel));
                qArray[x][y] = q;
            }
        }
    }

    /**
     * This function is used to start the SobelEdgeFilter transform, it takes no inputs and returns the transformed bitmap for displaying.
     */
    public Bitmap startTransform() {

        int width = img.getWidth();
        int height = img.getHeight();
        for (int x = 0; x < width ; x++) {
            for (int y = 0 ; y < height ; y++) {
                int q = algo_SobelEdgeFilter(inputParams,x,y);
                int colorOfPixel = Color.argb(255, q, q, q);
                img.setPixel(x, y, colorOfPixel);
            }
        }
        return img;
    }

    /**
     * This function defines the algorithm of SobelEdgeFilter, which returns the transformed color value as an int value.
     *
     * @param inputParams   determines what kind of SobelEdgeFilter transform to perform, Grx:0, Gry:1, Gr:2
     * @param positionX,    is the pixel's X coordinate
     * @param positionY,    is the pixel's Y coordinate
     */
    private int algo_SobelEdgeFilter(int inputParams,  int positionX, int positionY) {

        int outputColor;

        //BEGIN OF PIXEL MATRIX INTITIALIZATION
        int[][] colorMatrix= new int[3][3];
        for(int i = -1; i<2;i++) {
            for (int j = -1; j < 2; j++){
                if (positionX + i >= 0 && positionY + j >= 0 && positionX + i < width && positionY + j < height) {
                    colorMatrix[i+1][j+1] = qArray[positionX + i][positionY + j];
                } else {
                    colorMatrix[i+1][j+1] = 0;
                }
            }
        }
        //END OF PIXEL MATRIX INITIALIZATION

        //do the SobelEdgeFilter using the Sx matrix as filter
        if (inputParams == 0) {
            int Grx= getGrx(colorMatrix);
            if(Grx<0){
                outputColor=0;
            }else{
                outputColor=Grx;
            }
        }
        //do the SobelEdgeFilter using the Sy matrix as filter
        else if (inputParams == 1) {
            int Gry= getGry(colorMatrix);
            if(Gry<0){
                outputColor=0;
            }else{
                outputColor=Gry;
            }
        }
        //calculate Gr
        else if (inputParams == 2){
            int Grx= getGrx(colorMatrix);
            int Gry= getGry(colorMatrix);
            int Gr=(int)Math.sqrt(Grx*Grx+Gry*Gry);
            outputColor=Gr;
        } else {
            outputColor = 255;
        }
        return outputColor;
    }

    /**
     * This function does the actual mathematical calculation of Gry, which returns an int value.
     *
     * @param colorMatrix   is the input pixel matrix for calculation
     */
    private int getGry(int[][] colorMatrix) {
        return colorMatrix[0][0]*Sy[0][0]+colorMatrix[0][1]*Sy[0][1]+colorMatrix[0][2]*Sy[0][2]
                +colorMatrix[1][0]*Sy[1][0]+colorMatrix[1][1]*Sy[1][1]+colorMatrix[1][2]*Sy[1][2]
                +colorMatrix[2][0]*Sy[2][0]+colorMatrix[2][1]*Sy[2][1]+colorMatrix[2][2]*Sy[2][2];
    }

    /**
     * This function does the actual mathematical calculation of Grx, which returns an int value.
     *
     * @param colorMatrix   is the input pixel matrix for calculation
     */
    private int getGrx(int[][] colorMatrix) {
        return colorMatrix[0][0]*Sx[0][0]+colorMatrix[0][1]*Sx[0][1]+colorMatrix[0][2]*Sx[0][2]
                +colorMatrix[1][0]*Sx[1][0]+colorMatrix[1][1]*Sx[1][1]+colorMatrix[1][2]*Sx[1][2]
                +colorMatrix[2][0]*Sx[2][0]+colorMatrix[2][1]*Sx[2][1]+colorMatrix[2][2]*Sx[2][2];
    }
}
