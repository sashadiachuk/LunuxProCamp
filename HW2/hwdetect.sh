#!/bin/bash


echo "lsusb result"
echo
echo $(lsusb)

echo # shift beetwen outputs

echo "i2cdetect result"
echo
echo  $(i2cdetect -l)

echo # shift beetwen outputs 

echo "usb devices"
echo
echo $(sudo ls -l /dev/disk/by-id/usb*) #display plugged flash drives

echo # shift beetwen outputs

echo $(ls /dev | grep i2c) # i2c devices

echo # shift beetwen outputs


dmesg | grep tty #Display Detected System’s Serial Support Under Linux



