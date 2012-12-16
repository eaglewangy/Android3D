#!/bin/bash

#crate 3rd party directory
mkdir -p src/3rdParty

pushd ./
cd src/3rdParty

#get 3rd party source code from github
#get glm 
git clone https://github.com/g-truc/glm.git
#get libpng
git clone  https://github.com/julienr/libpng-android.git
#get libjpeg
git clone https://github.com/eaglewangy/libjpeg-android.git

mkdir libpng
cp libpng-android/jni/*.c libpng/
cp libpng-android/jni/*.h libpng/
#rm -rf libpng-android/

mv libjpeg-android/ libjpeg/

#get freetype
git clone https://github.com/cdave1/freetype2-android.git
mv freetype2-android/src/raster/raster.c  freetype2-android/src/raster/raster.c_
mv freetype2-android/src/sfnt/ttsbit0.c  freetype2-android/src/sfnt/ttsbit0.c_
mv freetype2-android/src/smooth/smooth.c  freetype2-android/src/smooth/smooth.c_

popd

cd android/

ndk-build
