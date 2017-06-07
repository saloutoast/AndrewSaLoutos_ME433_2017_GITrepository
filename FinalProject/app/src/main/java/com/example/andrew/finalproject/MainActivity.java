package com.example.andrew.finalproject;

import android.Manifest;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;

import com.hoho.android.usbserial.driver.CdcAcmSerialDriver;
import com.hoho.android.usbserial.driver.ProbeTable;
import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.util.SerialInputOutputManager;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import static android.graphics.Color.blue;
import static android.graphics.Color.green;
import static android.graphics.Color.red;
import static android.graphics.Color.rgb;
import static java.lang.Math.abs;

public class MainActivity extends AppCompatActivity implements TextureView.SurfaceTextureListener{
    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Bitmap bmp = Bitmap.createBitmap(640, 480, Bitmap.Config.ARGB_8888);
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();

    TextView mTextView;
    TextView mTextView2;

    //SeekBar myControl1;
    TextView myTextView;
    //SeekBar myControl2;
    TextView myTextView2;

    //Button button;
    //TextView myTextView3;
    //ScrollView myScrollView;
    TextView myTextView4;

    SeekBar myControl3;

    TextView FPStext;

    private UsbManager manager;
    private UsbSerialPort sPort;
    private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();
    private SerialInputOutputManager mSerialIoManager;

    static long prevtime = 0; // for FPS calculation

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // keeps the screen from turning off

        mTextView = (TextView) findViewById(R.id.top_textView);
        mTextView2 = (TextView) findViewById(R.id.mid_textView);

        myTextView = (TextView) findViewById(R.id.textView01);
        myTextView.setText("Motor A value");

        myTextView2 = (TextView) findViewById(R.id.textView02);
        myTextView2.setText("Motor B value");

        //myTextView3 = (TextView) findViewById(R.id.textView03);
        //myScrollView = (ScrollView) findViewById(R.id.ScrollView01);
        myTextView4 = (TextView) findViewById(R.id.textView04);
        //button = (Button) findViewById(R.id.button1);

        myControl3 = (SeekBar) findViewById(R.id.seekSensitivity);
        FPStext = (TextView) findViewById(R.id.framespersec);

        //setMyControl1Listener();
        //setMyControl2Listener();

        // see if the app has permission to use the camera
        //ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CAMERA}, 1); // comment out this one
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED) {
            mSurfaceView = (SurfaceView) findViewById(R.id.surfaceview);
            mSurfaceHolder = mSurfaceView.getHolder();

            mTextureView = (TextureView) findViewById(R.id.textureview);
            mTextureView.setSurfaceTextureListener(this);

            // set the paintbrush for writing text on the image
            paint1.setColor(0xffff0000); // red
            paint1.setTextSize(24);

            mTextView.setText("started camera");
        } else {
            mTextView.setText("no camera permissions");
        }

        manager = (UsbManager) getSystemService(Context.USB_SERVICE);
    }

    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mCamera = Camera.open();
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setPreviewSize(640, 480);
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY); // no autofocusing
        parameters.setAutoExposureLock(true); // keep the white balance constant
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90); // rotate to portrait mode

        try {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        } catch (IOException ioe) {
            // Something bad happened
        }
    }

    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    // the important function
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // every time there is a new Camera preview frame
        mTextureView.getBitmap(bmp);


        final Canvas c = mSurfaceHolder.lockCanvas();
        int thresh = myControl3.getProgress(); // comparison value from progress of slider
        if (c != null) {

            int com_far = 0;
            int far_pix = 0;
            int com_near = 0;
            int near_pix = 0;
            for (int j = (3*bmp.getHeight()/8); j < (5*bmp.getHeight()/8); j+=5) {// index through every five rows
                int[] pixels = new int[bmp.getWidth()]; // pixels[] is the RGBA data
                int startY = j; // which row in the bitmap to analyze to read
                bmp.getPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1);


                int com_local = 0;
                int num_pix = 0;
                // in the row, see if row is gray //there is more green than red and blue
                for (int i = 0; i < bmp.getWidth(); i++) {
                    if ((green(pixels[i]) + blue(pixels[i]) + red(pixels[i])) < 720) { // try to filter out white points...assume they are bad, not assume they are good
                        if ((abs(green(pixels[i]) - red(pixels[i])) < thresh) && (abs(blue(pixels[i]) - red(pixels[i])) < thresh) && (abs(blue(pixels[i]) - green(pixels[i])) < thresh)) {
                            // if ((red(pixels[i] - green(pixels[i]) < thresh) { // detect gray by finding if red is greater than green
                            pixels[i] = rgb(0, 255, 0); // over write the pixel with pure green
                            com_local = com_local + i; // add to center of mass
                            num_pix = num_pix + 1; // track number of points in com
                        }
                    }
                }

                if (num_pix > 0) {com_local = com_local/num_pix;}// find center of mass of green pieces
                if (com_local > 0) {
                    pixels[com_local] = rgb(255, 0, 0); // target at center of mass
                    //canvas.drawCircle(com_local, j, 5, paint1);
                }

                if (j < (bmp.getHeight()/2) && com_local!=0) { // update far, near centers of mass
                    com_far = com_far + com_local;
                    far_pix = far_pix + 1;
                } else if (j>=(bmp.getHeight()/2) && com_local!=0) {
                    com_near = com_near + com_local;
                    near_pix = near_pix + 1;
                }

                // update the row
                bmp.setPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1);
            }

            //mTextView2.setText(Integer.toString(bmp.getWidth())+" by "+Integer.toString(bmp.getHeight()));

            if (far_pix > 0) {com_far = com_far / far_pix; }// determine 2 CoMs
            if (near_pix > 0) {com_near = com_near / near_pix; }

            //myTextView2.setText("com_far: "+Integer.toString(com_far)+", far_pix: "+Integer.toString(far_pix));
            //myTextView3.setText("com_near: "+Integer.toString(com_near)+", near_pix: "+Integer.toString(near_pix));

            canvas.drawCircle(com_far, bmp.getHeight()/4, 10, paint1);
            canvas.drawCircle(com_near, 3*(bmp.getHeight()/4), 10, paint1);

            // Send motor control based on two com values
            int base_spd = 40;
            int err_bound_near = 2000;
            int err_bound_far = 10;
            int err_near = com_near - (bmp.getWidth()/2);
            int err_far = com_far - (bmp.getWidth()/2);
            double gain_near = 0.2;
            double gain_far = 0.14;
            double correction_near = abs(err_near*gain_near)/2;
            double correction_far = abs(err_far*gain_far)/2;

//            if (err_near > err_bound_near) {
//                String sendString = String.valueOf(base_spd-correction_near/) + ',' + String.valueOf(base_spd+correction_near) + '\n';
//                try {
//                    sPort.write(sendString.getBytes(), 10); // 10 is the timeout
//                } catch (IOException e) { }
//            } else if (err_near < -err_bound_near) {
//                String sendString = String.valueOf(base_spd+correction_near) + ',' + String.valueOf(base_spd-correction_near) + '\n';
//                try {
//                    sPort.write(sendString.getBytes(), 10); // 10 is the timeout
//                } catch (IOException e) { }
//            } else
            if (err_far > err_bound_far) {
                String sendString = String.valueOf(base_spd-((int)correction_far)) + ',' + String.valueOf(base_spd+((int)correction_far)) + '\n';
                try {
                    sPort.write(sendString.getBytes(), 10); // 10 is the timeout
                } catch (IOException e) { }
            } else if (err_far < -err_bound_far) {
                String sendString = String.valueOf(base_spd+ ((int)correction_far)) + ',' + String.valueOf(base_spd-((int)correction_far)) + '\n';
                try {
                    sPort.write(sendString.getBytes(), 10); // 10 is the timeout
                } catch (IOException e) { }
            } else {// within bounds, keep going straight
                String sendString = String.valueOf(base_spd) + ',' + String.valueOf(base_spd) + '\n';
                try {
                    sPort.write(sendString.getBytes(), 10); // 10 is the timeout
                } catch (IOException e) {
                }
            }
        }

        // draw a circle at some position
        //int pos = 0;
        //canvas.drawCircle(pos, 0, 5, paint1); // x position, y position, diameter, color

        // write the thresh as text
        canvas.drawText("thresh = "+thresh, 10, 30, paint1);
        c.drawBitmap(bmp, 0, 0, null);
        mSurfaceHolder.unlockCanvasAndPost(c);

        // calculate the FPS to see how fast the code is running
        long nowtime = System.currentTimeMillis();
        long diff = nowtime - prevtime;
        FPStext.setText("FPS " + 1000 / diff);
        prevtime = nowtime;
    }

//    private void setMyControl1Listener() {
////        myControl1.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
////
////            int progressChanged = 0;
////
////            @Override
////            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
////                progressChanged = progress;
////
////                myTextView.setText("The value is: "+((progress*2)-100));
////            }
////
////            @Override
////            public void onStartTrackingTouch(SeekBar seekBar) {
////            }
////
////            @Override
////            public void onStopTrackingTouch(SeekBar seekBar) {
////
////            }
////        });
//    }

//    private void setMyControl2Listener() {
//        myControl2.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
//
//            int progressChanged = 0;
//
//            @Override
//            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
//                progressChanged = progress;
//
//                myTextView2.setText("The value is: "+((progress*2)-100));
//            }
//
//            @Override
//            public void onStartTrackingTouch(SeekBar seekBar) {
//            }
//
//            @Override
//            public void onStopTrackingTouch(SeekBar seekBar) {
//
//            }
//        });
//    }

    private final SerialInputOutputManager.Listener mListener =
            new SerialInputOutputManager.Listener() {
                @Override
                public void onRunError(Exception e) {

                }

                @Override
                public void onNewData(final byte[] data) {
                    MainActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            MainActivity.this.updateReceivedData(data);
                        }
                    });
                }
            };

    @Override
    protected void onPause(){
        super.onPause();
        stopIoManager();
        if(sPort != null){
            try{
                sPort.close();
            } catch (IOException e){ }
            sPort = null;
        }
        finish();
    }

    @Override
    protected void onResume() {
        super.onResume();

        ProbeTable customTable = new ProbeTable();
        customTable.addProduct(0x04D8,0x000A, CdcAcmSerialDriver.class);
        UsbSerialProber prober = new UsbSerialProber(customTable);

        final List<UsbSerialDriver> availableDrivers = prober.findAllDrivers(manager);

        if(availableDrivers.isEmpty()) {
            //check
            return;
        }

        UsbSerialDriver driver = availableDrivers.get(0);
        sPort = driver.getPorts().get(0);

        if (sPort == null){
            //check
        }else{
            final UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
            UsbDeviceConnection connection = usbManager.openDevice(driver.getDevice());
            if (connection == null){
                //check
                PendingIntent pi = PendingIntent.getBroadcast(this, 0, new Intent("com.android.example.USB_PERMISSION"), 0);
                usbManager.requestPermission(driver.getDevice(), pi);
                return;
            }

            try {
                sPort.open(connection);
                sPort.setParameters(9600, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);

            }catch (IOException e) {
                //check
                try{
                    sPort.close();
                } catch (IOException e1) { }
                sPort = null;
                return;
            }
        }
        onDeviceStateChange();
    }

    private void stopIoManager(){
        if(mSerialIoManager != null) {
            mSerialIoManager.stop();
            mSerialIoManager = null;
        }
    }

    private void startIoManager() {
        if(sPort != null){
            mSerialIoManager = new SerialInputOutputManager(sPort, mListener);
            mExecutor.submit(mSerialIoManager);
        }
    }

    private void onDeviceStateChange(){
        stopIoManager();
        startIoManager();
    }

    private void updateReceivedData(byte[] data) {
        //do something with received data

        //for displaying:
        String motorVals = null;
        try {
            motorVals = new String(data, "UTF-8"); // put the data you got into a string
            if (motorVals.length() > 1 ) {
                myTextView4.setText(motorVals);

            }
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    }
}

