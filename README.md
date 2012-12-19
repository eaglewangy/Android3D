Android3D
=========

Android3D is a almost pure native OpenGL ES 2.0 rending library using on Android platform.

How to configure eclipse to build.

1. install Android SDK, Android NDK and CDT.

2. add NDK_ROOT to your path environment.

3. run build_native.sh to get 3rd party library and use ndk-build to build native code.

If you use windows, you should set up ndk-build environment.


Use Adnroid NDK in eclipse

--Right click the Android3D project in eclipse, then Properties->C/C++ Build->Build Variables

 ->Add, the variable name is: NDK_ROOT, Value: (your NDK root path, for example:/usr/eaglewangy/android-ndk-r8b)
 
 <img src=http://s12.sinaimg.cn/large/6453bedegd12ec30916fb&690" alt="Snapshort" height="300" width="240">
 
