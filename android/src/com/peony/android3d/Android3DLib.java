package com.peony.android3d;

public class Android3DLib {

	static {
        System.loadLibrary("android3d_jni");
    }

   /**
    * @param width the current view width
    * @param height the current view height
    */
    public static native void init(int width, int height);
    public static native void step();
    //public static native void destroy();
}
