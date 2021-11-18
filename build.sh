#!/bin/bash
rm -rf build>/dev/null
mkdir build && cd build
mkdir UTP
mkdir UTPClient

cd UTP 
cmake ../../UTP
make 

cd ../UTPClient
cmake ../../UTPClient
make 

cd ../../


