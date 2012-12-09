package com.peony.android3d.util;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.content.Context;
import android.content.res.AssetManager;

public class AssetsUtils {
	//Copy files from assets to internal storage
	public static void copyFiles(Context context){
		try {
			AssetManager assetManager = context.getAssets();
			File filesRoot = context.getFilesDir();
			String fullPath = filesRoot.getAbsolutePath();
			File rootDir = new File(fullPath);
			if (!rootDir.exists()){
				rootDir.mkdirs();
			}
			String[] files = assetManager.list("android3d");
			for (String file : files){
				File f = new File(file);
				if (f.exists())
					continue;
				InputStream input = assetManager.open("android3d/" + file);
				OutputStream output = new FileOutputStream(fullPath + "/" + file);
				copy(input, output);
				input.close();
				output.close();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}finally{
		}
	}

	/**
	 * copy between InputStream & OutputStream 
	 * @param input
	 * @param output
	 * @throws IOException
	 */
	public static void copy(InputStream input, OutputStream output) throws IOException {
		byte[] bytes = new byte[1024];

		do {
			int n = input.read(bytes);
			if(n <= 0) {
				break;
			}
			output.write(bytes, 0, n);
		}while(true);
	}
}
