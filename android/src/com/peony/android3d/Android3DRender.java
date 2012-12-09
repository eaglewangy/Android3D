package com.peony.android3d;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.util.Log;

public class Android3DRender implements GLSurfaceView.Renderer{
	private String TAG = Android3DRender.class.getSimpleName();
	
	public void onDrawFrame(GL10 gl) {
        Android3DLib.step();
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
    	Android3DLib.init(width, height);
    	Log.e(TAG, "Render surface changed.");
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	
    }
}
