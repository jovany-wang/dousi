#!/usr/bin/env bash

set -x
# Cause the script to exit if a single command fails.
set -e

# We build the C++ part first.
mkdir build
cd build
cmake ../..
make -j 4

# Then copy the native library file to the destination directory.
mkdir -p ../rpc-client/native_dependencies
cp ./libdousi_jni_client_lib.* ../rpc-client/native_dependencies

echo "Native libraries were all installed successfully."
