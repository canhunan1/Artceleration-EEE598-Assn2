# Artceleration

## Author
By Jianan Yang and Wenhao Chen

## Goal
Artceleration is an Android library framework / service which enables user application to implement artistic transforms for images. In current version, it includes 5 different image tranformation functionality for app developer, i.e.  COLOR_FILTER, MOTION_BLUR, GAUSSIAN_BLUR, TILT_SHIFT and NEON_EDGES.

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
A complete AugGraffitiDev consists 5 different screens, i.e. Login, Map, Place, Collect and Gallery. As of now, users can only interact with Login and Map screen. The other three screens will be developed and added to this app in next step.

Step 1: To open the app, click the icon named "MyApplication". Once it is opened succeffully, the Login screen pops up.

![alt tag](https://cloud.githubusercontent.com/assets/21367763/18692829/e482ac4c-7f51-11e6-8ffd-627f12f5cba6.JPG) Login screen

Step 2: Use your "Google Account" to login by clikcing "Sigin in with Google" button. If the login succeeds, you will be directed to Map screen. In this screen, a "P" tag located in the center represnets your current location. This "P" button also allows you to place a tag on current location (this function is currently unavailable and will be implemented in next step). By tapping the "SIGN OUT" button on top of the screen, you will be directed back to Login screen.   

![alt tag](https://cloud.githubusercontent.com/assets/21367763/18692831/e67f0f54-7f51-11e6-817e-e51446127084.JPG) Map screen

Step 3: Walking around ASU campus and find tags! Tags within 50m of your current location will appear on your Map screen. This is shown in picture below in which the "C" are tags placed by other Users. Further steps regarding how to place tags and collect tags will be added here in next step.

![alt tag](https://cloud.githubusercontent.com/assets/21367763/18692833/e7e49de6-7f51-11e6-9b28-32f08f4c4463.JPG) Map screen with tags

Step 4: This screen is invoked when clicking the "P" marker in the google map screen. Once the screen is opened, the camera is opened, and you can draw anything you like by simply tapping and dragging on screen. After drawing, you can send your draw (with the camera background) to sever by clicking the "COMMIT" button located at the bottom.

![alt tag](https://cloud.githubusercontent.com/assets/21367763/19371421/b761fd94-9167-11e6-9a13-476f001ebe97.png) Placing tag screen

Step 5: This Collecting Tag screen is invoked when you tap the "C" tag on google map screen. In this screen, the up-right shows the tag image drawn by somebody else, by changing the distance, orientation of the device's screen, this image will change correspondingly. The image will be saved in gallery when you tap the "COLLECT" button on the bottom. Unfortunately, the image's background is black, and we couldn't figure out how to use real picture as the image's background.

![alt tag](https://cloud.githubusercontent.com/assets/21367763/19371424/baa7c1a0-9167-11e6-87f6-009c14226ed8.png) Collecting tag screen

Step 6: The two picture below show the gallary. When you click the "GALLERY" button on google map screen. You will be directed to the "Gallery screen-1", it lists all the tags you have collected by listing the urls. By clicking any url, you will be directed to "Gallery screen-2" which shows the actual tag image.

![alt tag](https://cloud.githubusercontent.com/assets/21367763/19371425/bcc8576a-9167-11e6-90d8-63aeecc023b1.png) Gallery screen-1

![alt tag](https://cloud.githubusercontent.com/assets/21367763/19371426/be4e61f6-9167-11e6-83e6-18324d97940f.png) Gallery screen-2


## Design (coding part)
This app is developed in Android Studio. The entire codes is composed of two major parts - .xml and .java file. The .xml files define the layouts of user interface while the .java files form the backbone of this application and enable the functionalies.

The .xml files are stored in ```app\src\main\res\layout``` folder:

- ```activity_main.xml``` - decribes the Login screen, in which the "Sign in with Google" is defined as below
```
<com.google.android.gms.common.SignInButton
    android:id="@+id/login"
    android:layout_width="match_parent"
    android:layout_height="wrap_content"
    android:layout_gravity="bottom">
</com.google.android.gms.common.SignInButton>

```

- ```activity_google_map.xml``` - decribes the Map screen, in which the "Googe Map" is contained in a ```fragment``` view
```
<fragment
   android:layout_width="match_parent"
   android:layout_height="422dp"
   android:name="com.google.android.gms.maps.MapFragment"
   android:id="@+id/mapFragment"
   android:layout_alignParentTop="true"
   android:layout_weight="1.21" />
```

- ```activity_place.xml``` - decribes the tag placing screen, in which the camera and drawing is implemented in two seperated fullscreen views

```
<com.example.jianan.auggraffiti.CameraPreview
        android:id="@+id/camera_preview"
        android:layout_width="fill_parent"
        android:layout_height="fill_parent"
        android:layout_alignParentRight="true"
        android:layout_alignParentEnd="true"
        android:layout_alignParentBottom="true" />
```
```
 <com.example.jianan.auggraffiti.Graphique
        android:id="@+id/graph"
        android:layout_width="fill_parent"
        android:layout_height="fill_parent"
        android:layout_alignParentLeft="true"
        android:layout_alignParentStart="true"
        android:layout_alignBottom="@+id/button_save"
        android:layout_alignParentRight="true"
        android:layout_alignParentEnd="true" />
```

- ```activity_collect.xml``` - decribes the tag collecting screen, in which the camera and tagImage is implemented in fullscreen view and ImageView, respectively.

```
<com.example.jianan.auggraffiti.CameraPreview
        android:id="@+id/camera_preview"
        android:layout_width="fill_parent"
        android:layout_height="fill_parent"
        android:layout_alignParentRight="true"
        android:layout_alignParentEnd="true"
        android:layout_alignParentBottom="true" />

    <ImageView
        android:id="@+id/tag_image"
        android:layout_width="match_parent"
        android:layout_height="match_parent"
        android:layout_alignParentBottom="true"
        android:layout_alignParentLeft="true"
        android:layout_alignParentStart="true"
        android:layout_marginBottom="300dp" />
```

- ```activity_gallery.xml``` - decribes the gallery screen, in which collected images are stored and shown in a ```ListView```.

```
<ListView
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:id="@+id/list"
        android:layout_alignParentTop="true"
        android:layout_alignParentLeft="true"
        android:layout_alignParentStart="true"
        android:choiceMode="singleChoice" />
```
