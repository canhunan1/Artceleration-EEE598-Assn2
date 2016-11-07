# Artceleration Library and Service

## Author
By Jianan Yang and Wenhao Chen

## Goal
Artceleration is an Android library framework / service which enables user application to implement artistic transforms for images. In current version, it includes 5 different image tranformation functions for app developer to use - COLOR_FILTER, MOTION_BLUR, GAUSSIAN_BLUR, TILT_SHIFT and NEON_EDGES.

## Install
To be determined...

##Developement

To be determined...
To develope, you need to confingure the build.gradle (app) firstly. You need to change directory ```storeFile file('D:/asu/asu/EEE598/AugGraffitiDev/keystore.jks')``` to your local directory
```
debug {
            keyAlias 'AugGraffiti'
            keyPassword '940205'
            storeFile file('D:/asu/asu/EEE598/AugGraffitiDev/keystore.jks')
            storePassword '940205'
        }
```
## Design
The general idea of this library/service is to realize image transformation for app developers so they don't have to worry about building their own image process algorithm, instead they can just pick can use. Below is a sample application which uses our Artceleration library framework/service. In this app, user can specify the image transformation type from the drop-down located on top of screen, the transformed image will be showing in the yellow region which is a dummy image transformation we implement for this checkout point.

![alt tag](https://cloud.githubusercontent.com/assets/21367763/20068194/133eafc4-a4d5-11e6-8d02-bb0ff2de6e1f.png) Sample app

## Framework/Service Design
In summary, the client's requests are sent to service for image processing, once processing is done, the processed image is sent back to client and shown on app's UI.

In the first step, the user must create a libaray object ```artlib``` through ```artlib =  new ArtLib(this)```. During the library object creation, the activity is binded to service using ```init()``` method, the binding request is sent through the ```Intent()``` object, in which the inputs are an activity object and service object.

```
public void init() {
        mActivity.bindService(new Intent(mActivity, TransformService.class), mServiceConnection, Context.BIND_AUTO_CREATE);
}
```
In the service class, the the ```onBind()``` callback function is trigured by ```bindService()``` method and a ```IBinder``` is returned by ```onBind()``` method as the binder corresponding to a messenger ```mMessenger``` which will be used to hanlde message transferring.

```
@Override
    public IBinder onBind(Intent intent) {
        return mMessenger.getBinder();
    }
```
If the service is connected succefully, the ```onServiceConnected()``` callback function is triggered, one of the input is the ```IBinder``` replied from the ```onBind()``` method and this ```IBinder``` object is connected to another ```Messenger``` object ```mMessenger```in library object to communiate with the ```Messenger``` in service object.

```
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mMessenger = new Messenger(service);
            mBound = true;
        }
```
As of here, the communication `bridge` - ```IBinder-Messenger``` between client and service is setup via ```init()``` method in libaray.

Next, the user must register a ```TransformHandler()``` interface oject into the libaray. This is done by calling ```registerHandler()``` method with an ```TransformHandler``` oject as input, here we are using anonymous inner class. The ```onTransformProcessed``` method is over written and will be called once the transformation is done, this callback funciton has a ```Bitmap``` object as input argument and it is the processed image. The transformed image is shown on UI by using ```setTransBmp()``` method.

```
artlib.registerHandler(new TransformHandler() {
            @Override
            public void onTransformProcessed(Bitmap img_out) {
                Log.d("In the mainviewr","img_out");
                artview.setTransBmp(img_out);
            }
        });
```
























