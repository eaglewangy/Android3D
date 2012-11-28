package com.peony.android3d.util;

import java.io.IOException;

import android.content.Context;

public class AssetsUtils {
	//Copy files from assets to internal storage
	public static void copyFiles(Context context){
		try {
			String[] files = context.getAssets().list("android3d");
			for (String file : files){
				
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
