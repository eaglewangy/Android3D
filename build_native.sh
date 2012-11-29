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

mkdir libpng
cp libpng-android/jni/*.c libpng/
cp libpng-android/jni/*.h libpng/
rm -rf libpng-android/

popd

cd android/

ndk-build
