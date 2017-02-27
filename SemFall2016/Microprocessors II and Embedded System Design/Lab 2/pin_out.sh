#!/bin/sh


echo -n "$1" > /sys/class/gpio/export
echo -n "out" > /sys/class/gpio/gpio$1/direction
echo -n "$2" > /sys/class/gpio/export 2>>error
echo -n "out" > /sys/class/gpio/gpio$2/direction
echo -n "$3" > /sys/class/gpio/gpio$2/value

echo -n "$2" > /sys/class/gpio/unexport 
echo -n "$1" > /sys/class/gpio/unexport
