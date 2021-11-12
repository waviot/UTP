#!/bin/bash

rm ./content/output1.jpg
rm ./content/output2.jpg
rm ./content/output3.jpg
echo ""
./build/UTPClient/UTP_Client -ea

echo ""
./build/UTPClient/UTP_Client -i

echo ""
./build/UTPClient/UTP_Client -l ./content/input.jpg -uid 1

echo ""
./build/UTPClient/UTP_Client -l ./content/input.jpg -uid 1

echo ""
./build/UTPClient/UTP_Client -l ./content/input.jpg -uid 2

echo ""
./build/UTPClient/UTP_Client -l ./content/input.jpg -uid 3

echo ""
./build/UTPClient/UTP_Client -i

echo ""
echo "Request reading by UID 3"
./build/UTPClient/UTP_Client -r 3 -f ./content/output3.jpg -s 2028

echo ""
./build/UTPClient/UTP_Client -r 2 -f ./content/output2.jpg -s 2028

echo ""
./build/UTPClient/UTP_Client -r 1 -f ./content/output1.jpg -s 2028

echo ""
./build/UTPClient/UTP_Client -eu 10

echo ""
./build/UTPClient/UTP_Client -eu 2

echo ""
./build/UTPClient/UTP_Client -i

echo ""
./build/UTPClient/UTP_Client -eu 1

echo ""
./build/UTPClient/UTP_Client -eu 1

echo ""
./build/UTPClient/UTP_Client -i


