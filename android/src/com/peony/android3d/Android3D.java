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
import android.util.Log;

public class Android3D extends Activity {
    private static String TAG = "android3d";
    
    private Android3DView mSurfaceView;
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
    	
    	AssetsUtils.copyFiles(this);
    }

    @Override 
    protected void onPause() {
        super.onPause();
        mSurfaceView.onPause();
    }

    @Override 
    protected void onResume() {
        super.onResume();
        mSurfaceView = new Android3DView(getApplication());
    	setContentView(mSurfaceView);
        mSurfaceView.onResume();
    }
    
    /*@Override 
    public void onDestroy(){
    	super.onDestroy();
    	Android3DLib.destroy();
    }*/
}
