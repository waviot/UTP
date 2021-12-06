#!/bin/bash
rm -rf build>/dev/null
rm -rf content>/dev/null
mkdir content
mkdir build && cd build
mkdir client
mkdir server

cd server 
cmake ../../UTP
make 

cd ../client
cmake ../../UTPClient
make 

cd ../../


