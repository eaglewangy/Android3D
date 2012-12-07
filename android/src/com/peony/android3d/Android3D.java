/*
 * Copyright 2012
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *  Created on: Nov 25, 2012
 *  Author:     wyylling
 *  Email:      wyylling@gmail.com
 */

package com.peony.android3d;

import com.peony.android3d.util.AssetsUtils;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.View.OnClickListener;
import android.util.Log;

public class Android3D extends Activity implements SurfaceHolder.Callback {
    private static String TAG = "android3d";
    
    private Surface mSurface;
    private boolean mNeedRestart = false;

    @Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);

    	Log.i(TAG, "onCreate()");

    	nativeOnCreate();

    	setContentView(R.layout.main);
    	SurfaceView surfaceView = (SurfaceView)findViewById(R.id.surfaceview);
    	surfaceView.getHolder().addCallback(this);
    	surfaceView.setOnClickListener(new OnClickListener() {
    		public void onClick(View view) {
    			/*Toast toast = Toast.makeText(Android3D.this, "", Toast.LENGTH_LONG);
                    toast.show();*/
    	}});
    	
    	AssetsUtils.copyFiles(this);
    	mNeedRestart = false;
    }

    @Override
    protected void onResume() {
    	super.onResume();

    	Log.i(TAG, "onResume()");
    	if (mNeedRestart){
    		nativeOnCreate();
    	}

    	nativeOnResume();
    }
    
    @Override
    protected void onPause() {
        super.onPause();
        Log.i(TAG, "onPause()");
        nativeOnPause();
        
        mNeedRestart = true;
    }

    @Override
    protected void onStop() {
        super.onDestroy();
        Log.i(TAG, "onStop()");
        nativeOnStop();
    }
    
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
    	super.onConfigurationChanged(newConfig);
    	Log.e(TAG, "Orientition has changed.");
    	Intent intent = new Intent(this, Android3D.class);
    	startActivity(intent);
    	this.finish();
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
    	mSurface = holder.getSurface();
    	Log.v(TAG, "Surface changes to width: " + String.valueOf(w) + ", height: " + String.valueOf(h));
        nativeSetSurface(mSurface);
    }

    public void surfaceCreated(SurfaceHolder holder) {
    	Log.v(TAG, "Surface created.");
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        nativeSetSurface(null);
    }

    public static native void nativeOnCreate();
    public static native void nativeOnResume();
    public static native void nativeOnPause();
    public static native void nativeOnStop();
    public static native void nativeSetSurface(Surface surface);

    static {
        System.loadLibrary("android3d_jni");
    }

}
