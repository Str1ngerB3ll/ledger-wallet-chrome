#!/bin/sh

update_nacl_sdk() {
    ./nacl_sdk/naclsdk update
}

if [ -d ./nacl_sdk/ ];
then
   update_nacl_sdk
else
   curl -O "http://storage.googleapis.com/nativeclient-mirror/nacl/nacl_sdk/nacl_sdk.zip" && unzip nacl_sdk.zip && rm nacl_sdk.zip && update_nacl_sdk
fi