package edu.asu.msrs.artcelerationlibrary;

import android.os.AsyncTask;
import android.util.Log;

/**
 * Created by Jianan on 11/7/2016.
 */

public class ThreadManager {
    static private ThreadManager sInstance;

    static {
        sInstance = new ThreadManager();
    }

    private ThreadManager() {
        ThreadTask tt1 = new ThreadTask(1);
        tt1.start();
        ThreadTask tt2 = new ThreadTask(2);
        tt2.start();
        new AsyncTest().execute(1,2,3);


    }


    Object barrier = new Object();

    class ThreadTask extends Thread {
        int a;

        ThreadTask(int a) {
            this.a = a;
        }

        @Override
        public void run() {
            while (true)


            {
                synchronized (barrier) {


                    Log.d("TTask", "hey before " + a);
                    try {
                        Thread.sleep(a * 1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                Log.d("TTask", "hey after " + a);
                try {
                    Thread.sleep(20);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }


    //Params, progress, Result
    class AsyncTest extends AsyncTask<Integer, Float, Double> {
        //DONE IN BACKGROUND
        @Override
        protected Double doInBackground(Integer[] params) {
            double product = 1;
            for (int i = 0; i < params.length; i++) {
                Log.d("AsyncTest", "yo " + i);
                product *= i;
            }
            return product;
        }


        //ON UI THREAD
        protected void onPostExecute(Double result) {
            Log.d("AsyncTest", "AsyncTest finished" + result);
        }
    }


}
