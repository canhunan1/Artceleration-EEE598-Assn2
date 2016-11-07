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
The general idea of this library/service is to realize image transformation for app developers so they don't have to worry about building their own image process algorithm, instead they can just pick can use. Below is a sample application which uses our Artceleration library framework/service.

![alt tag](https://cloud.githubusercontent.com/assets/21367763/18692829/e482ac4c-7f51-11e6-8ffd-627f12f5cba6.JPG) Login screen

## Framework/Service Design
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
